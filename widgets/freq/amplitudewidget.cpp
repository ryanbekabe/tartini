/***************************************************************************
                          amplitudewidget.cpp  -  description
                             -------------------
    begin                : 19 Mar 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/


#include "mygl.h"
#include "amplitudewidget.h"
#include "gdata.h"
#include "channel.h"
#include "zoomlookup.h"
#include "qcursor.h"
#include <QMouseEvent>
#include <Q3PointArray>
#include <QWheelEvent>
#include <QPaintEvent>
#include "analysisdata.h"
#include "conversions.h"

#include <qpixmap.h>

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//------------------------------------------------------------------------------
AmplitudeWidget::AmplitudeWidget(QWidget * /*parent*/, const char* /*name*/)
// Replace call to setRange(0.8); and setOffset(0.0) by final value assignations
// to avoid issue with non initialised values. We don't care about signal emissions
// because at construction time there are still not bound to slots
  :
dragMode(DragNone),
mouseX(0),
mouseY(0),
downTime(0.0),
downOffset(0.0),
thresholdIndex(0),
_range(0.8),
_offset(0.0),
_offsetInv(1.0 - _range),
lineWidth(0.0),
halfLineWidth(0.0)
{
  setMouseTracking(true);
  setAttribute(Qt::WA_OpaquePaintEvent);
}

//------------------------------------------------------------------------------
void AmplitudeWidget::initializeGL(void)
{
  qglClearColor(gdata->backgroundColor());
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnableClientState(GL_VERTEX_ARRAY);
}

//------------------------------------------------------------------------------
void AmplitudeWidget::resizeGL(int w, int h)
{
  mygl_resize2d(w, h);
}

//------------------------------------------------------------------------------
AmplitudeWidget::~AmplitudeWidget(void)
{
}

//------------------------------------------------------------------------------
void AmplitudeWidget::setRange(double newRange)
{
  if(_range != newRange)
    {
      _range = bound(newRange, 0.0, 1.0);
      setOffset(offset());
      emit rangeChanged(_range);
    }
}

//------------------------------------------------------------------------------
void AmplitudeWidget::setOffset(double newOffset)
{
  newOffset = bound(newOffset, 0.0, maxOffset());
  _offset = newOffset;
  _offsetInv = maxOffset() - _offset;
  emit offsetChanged(_offset);
  emit offsetInvChanged(offsetInv());
}

//------------------------------------------------------------------------------
void AmplitudeWidget::paintGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT);

  setLineWidth(3.0);
  glEnable(GL_LINE_SMOOTH);

  View & view = gdata->getView();

  //draw the red/blue background color shading if needed
  if(view.backgroundShading() && gdata->getActiveChannel())
    {
      drawChannelAmplitudeFilledGL(gdata->getActiveChannel());
    }

  setLineWidth(1.0);
  glDisable(GL_LINE_SMOOTH);

  drawVerticalRefLines();

  setLineWidth(3.0);
  glEnable(GL_LINE_SMOOTH);

  //draw all the visible channels
  for (uint i = 0; i < gdata->getChannelsSize(); i++)
    {
      Channel *ch = gdata->getChannelAt(i);
      if(!ch->isVisible())
	{
	  continue;
	}
      drawChannelAmplitudeGL(ch);
    }

  // Draw the current time line
  glDisable(GL_LINE_SMOOTH);
  qglColor(colorGroup().foreground());
  glLineWidth(1.0);
  double curScreenTime = (view.currentTime() - view.viewLeft()) / view.zoomX();
  mygl_line(curScreenTime, 0, curScreenTime, height() - 1);

  // Draw a horizontal line at the current threshold.
  int y;
  double heightRatio = double(height()) / range();

  qglColor(Qt::black);
  y = height() - 1 - toInt((getCurrentThreshold(0) - offsetInv()) * heightRatio);
  mygl_line(0, y, width(), y);

  qglColor(Qt::red);
  y = height() - 1 - toInt((getCurrentThreshold(1) - offsetInv()) * heightRatio);
  mygl_line(0, y, width(), y);

  qglColor(Qt::black);
  renderText(2, height() - 3, getCurrentThresholdString());
}

//------------------------------------------------------------------------------
void AmplitudeWidget::drawVerticalRefLines(void)
{
  //Draw the vertical reference lines
  //time per 150 pixels
  double timeStep = timeWidth() / double(width()) * 150.0;
  //round down to the nearest power of 10
  double timeScaleBase = pow10(floor(log10(timeStep)));

  //choose a timeScaleStep which is a multiple of 1, 2 or 5 of timeScaleBase
  int largeFreq;
  if(timeScaleBase * 5.0 < timeStep)
    {
      largeFreq = 5;
    }
  else if (timeScaleBase * 2.0 < timeStep)
    {
      largeFreq = 2;
    }
  else
    {
      largeFreq = 2;
      timeScaleBase /= 2;
    }

  //calc the first one just off the left of the screen
  double timePos = floor(leftTime() / (timeScaleBase * largeFreq)) * (timeScaleBase * largeFreq);
  int x, largeCounter = -1;
  double ratio = double(width()) / timeWidth();
  double lTime = leftTime();

  for(; timePos <= rightTime(); timePos += timeScaleBase)
    {
      if(++largeCounter == largeFreq)
	{
	  largeCounter = 0;
	  //draw the darker lines
	  glColor4ub(25, 125, 170, 128);
	}
      else
	{
	  //draw the lighter lines
	  glColor4ub(25, 125, 170, 64);
	}
      x = toInt((timePos-lTime) * ratio);
      mygl_line(x, 0, x, height() - 1);
    }
}

//------------------------------------------------------------------------------
bool AmplitudeWidget::calcZoomElement(ZoomElement &ze, Channel *ch, int baseElement, double baseX)
{
  int startChunk = int(floor(double(baseElement) * baseX));
  myassert(startChunk <= ch->totalChunks());
  int finishChunk = int(floor(double(baseElement + 1) * baseX)) + 1;
  myassert(finishChunk <= ch->totalChunks());
  if(startChunk == finishChunk)
    {
      return false;
    }

  myassert(startChunk < finishChunk);

  int mode = gdata->amplitudeMode();
  std::pair<large_vector<AnalysisData>::iterator, large_vector<AnalysisData>::iterator> a =
    minMaxElement(ch->dataIteratorAtChunk(startChunk), ch->dataIteratorAtChunk(finishChunk), lessValue(mode));
  myassert(a.second != ch->dataIteratorAtChunk(finishChunk));
  float low = (*amp_mode_func[mode])(a.first->getValue(mode), *gdata);
  float high = (*amp_mode_func[mode])(a.second->getValue(mode), *gdata);
  
  ze.set(low, high, 0, ch->get_color(), NO_NOTE, (startChunk + finishChunk) / 2);
  return true;
}

//------------------------------------------------------------------------------
double AmplitudeWidget::calculateElement(AnalysisData *data)
{
  double val = (*amp_mode_func[gdata->amplitudeMode()])(data->getValue(gdata->amplitudeMode()), *gdata);
  return val;
}

//------------------------------------------------------------------------------
double AmplitudeWidget::getCurrentThreshold(int index)
{
  return (*amp_mode_func[gdata->amplitudeMode()])(gdata->ampThreshold(gdata->amplitudeMode(), index), *gdata);
}

//------------------------------------------------------------------------------
void AmplitudeWidget::setCurrentThreshold(double newThreshold, int index)
{
  newThreshold = bound(newThreshold, 0.0, 1.0);
  if(newThreshold < offsetInv())
    {
      setOffset(maxOffset() - newThreshold);
    }
  else if(newThreshold > offsetInv() + range())
    {
      setOffset(maxOffset() - (newThreshold - range()));
    }

  gdata->setAmpThreshold(gdata->amplitudeMode(), index, (*amp_mode_inv_func[gdata->amplitudeMode()])(newThreshold, *gdata));
}

//------------------------------------------------------------------------------
QString AmplitudeWidget::getCurrentThresholdString(void)const
{
  QString thresholdStr;
  thresholdStr.sprintf(amp_display_string[gdata->amplitudeMode()], gdata->ampThreshold(gdata->amplitudeMode(), 0), gdata->ampThreshold(gdata->amplitudeMode(), 1));
  return thresholdStr;
}

//------------------------------------------------------------------------------
void AmplitudeWidget::setLineWidth(float width)
{
  lineWidth = width;
  halfLineWidth = width / 2;
  glLineWidth(lineWidth);
}

//------------------------------------------------------------------------------
void AmplitudeWidget::drawChannelAmplitudeGL(Channel *ch)
{
  View & view = gdata->getView();
  
  ChannelLocker channelLocker(ch);
  ZoomLookup *z = &ch->get_amplitude_zoom_lookup();
  
  // baseX is the no. of chunks a pixel must represent.
  double baseX = view.zoomX() / ch->timePerChunk();

  z->setZoomLevel(baseX);
  
  double currentChunk = ch->chunkFractionAtTime(view.currentTime());
  double leftFrameTime = currentChunk - ((view.currentTime() - view.viewLeft()) / ch->timePerChunk());
  int n = 0;
  int baseElement = int(floor(leftFrameTime / baseX));
  if(baseElement < 0)
    {
      n -= baseElement;
      baseElement = 0;
    }
  int lastBaseElement = int(floor(double(ch->totalChunks()) / baseX));
  double heightRatio = double(height()) / range();
  
  Array1d<MyGLfloat2d> vertexArray(width() * 2);
  int pointIndex = 0;

  if (baseX > 1)
    {
      // More samples than pixels
      int theWidth = width();
      if(lastBaseElement > z->size())
	{
	  z->setSize(lastBaseElement);
	}
      for(; n < theWidth && baseElement < lastBaseElement; n++, baseElement++)
	{
	  myassert(baseElement >= 0);
	  ZoomElement &ze = z->at(baseElement);
	  if(!ze.isValid())
	    {
	      if(!calcZoomElement(ze, ch, baseElement, baseX))
		{
		  continue;
		}
	    }
      
	  vertexArray[pointIndex++] = MyGLfloat2d(n, height() - 1 - ((ze.high() - offsetInv()) * heightRatio) - halfLineWidth);
	  vertexArray[pointIndex++] = MyGLfloat2d(n, height() - 1 - ((ze.low() - offsetInv()) * heightRatio) + halfLineWidth);
	}
      myassert(pointIndex <= width() * 2);
      qglColor(ch->get_color());
      glLineWidth(1.0f);
      glVertexPointer(2, GL_FLOAT, 0, vertexArray.begin());
      glDrawArrays(GL_LINES, 0, pointIndex);
    }
  else
    {
      //baseX <= 1
      float val = 0.0;
      // Integer version of frame time
      int intChunk = (int) floor(leftFrameTime);
      // So we skip some pixels
      double stepSize = 1.0 / baseX;
      float x = 0.0f, y;

      double start = (double(intChunk) - leftFrameTime) * stepSize;
      double stop = width() + (2 * stepSize);

      double dn = start;
      int totalChunks = ch->totalChunks();
      if(intChunk < 0)
	{
	  dn += stepSize * -intChunk;
	  intChunk = 0;
	}
      for(; dn < stop && intChunk < totalChunks; dn += stepSize, intChunk++)
	{
	  AnalysisData *data = ch->dataAtChunk(intChunk);
	  myassert(data);
      
	  if(!data)
	    {
	      continue;
	    }

	  val = calculateElement(data);

	  x = dn;
	  y = height() - 1 - ((val - offsetInv()) * heightRatio);
	  vertexArray[pointIndex++] = MyGLfloat2d(x, y);
	}
      myassert(pointIndex <= width() * 2);
      qglColor(ch->get_color());
      glLineWidth(3.0f);
      glVertexPointer(2, GL_FLOAT, 0, vertexArray.begin());
      glDrawArrays(GL_LINE_STRIP, 0, pointIndex);
    }
}

//------------------------------------------------------------------------------
void AmplitudeWidget::drawChannelAmplitudeFilledGL(Channel *ch)
{
  View & view = gdata->getView();
  
  ChannelLocker channelLocker(ch);
  ZoomLookup *z = &ch->get_amplitude_zoom_lookup();
  
  // baseX is the no. of chunks a pixel must represent.
  double baseX = view.zoomX() / ch->timePerChunk();

  z->setZoomLevel(baseX);
  
  double currentChunk = ch->chunkFractionAtTime(view.currentTime());
  double leftFrameTime = currentChunk - ((view.currentTime() - view.viewLeft()) / ch->timePerChunk());
  int n = 0;
  int baseElement = int(floor(leftFrameTime / baseX));
  if(baseElement < 0)
    {
      n -= baseElement;
      baseElement = 0;
    }
  int lastBaseElement = int(floor(double(ch->totalChunks()) / baseX));
  double heightRatio = double(height()) / range();
  
  Array1d<MyGLfloat2d> vertexArray(width() * 2);
  int pointIndex = 0;

  if (baseX > 1)
    {
      // More samples than pixels
      int theWidth = width();
      if(lastBaseElement > z->size())
	{
	  z->setSize(lastBaseElement);
	}
      for(; n < theWidth && baseElement < lastBaseElement; n++, baseElement++)
	{
	  myassert(baseElement >= 0);
	  ZoomElement &ze = z->at(baseElement);
	  if(!ze.isValid())
	    {
	      if(!calcZoomElement(ze, ch, baseElement, baseX))
		{
		  continue;
		}
	    }
      
	  int y = height() - 1 - toInt((ze.high() - offsetInv()) * heightRatio);
	  vertexArray[pointIndex++] = MyGLfloat2d(n, y);
	  vertexArray[pointIndex++] = MyGLfloat2d(n, height());
	}
      qglColor(gdata->shading2Color());
      glVertexPointer(2, GL_FLOAT, 0, vertexArray.begin());
      glDrawArrays(GL_QUAD_STRIP, 0, pointIndex);
    }
  else
    {
      //baseX <= 1
      float val = 0.0;
      // Integer version of frame time
      int intChunk = (int) floor(leftFrameTime);
      // So we skip some pixels
      double stepSize = 1.0 / baseX;
      float x = 0.0f, y;

      double start = (double(intChunk) - leftFrameTime) * stepSize;
      double stop = width() + (2 * stepSize);

      double dn = start;
      int totalChunks = ch->totalChunks();
      if(intChunk < 0)
	{
	  dn += stepSize * -intChunk;
	  intChunk = 0;
	}
    
      for(; dn < stop && intChunk < totalChunks; dn += stepSize, intChunk++)
	{
	  AnalysisData *data = ch->dataAtChunk(intChunk);
	  myassert(data);
      
	  if(!data)
	    {
	      continue;
	    }

	  val = calculateElement(data);

	  x = dn;
	  y = height() - 1 - ((val - offsetInv()) * heightRatio);
	  vertexArray[pointIndex++] = MyGLfloat2d(x, y);
	  vertexArray[pointIndex++] = MyGLfloat2d(x, height());
	}
      myassert(pointIndex <= width() * 2);
      qglColor(gdata->shading2Color());
      glVertexPointer(2, GL_FLOAT, 0, vertexArray.begin());
      glDrawArrays(GL_QUAD_STRIP, 0, pointIndex);
    }
}

//------------------------------------------------------------------------------
void AmplitudeWidget::mousePressEvent(QMouseEvent * e)
{
  View & view = gdata->getView();
  int timeX = toInt(view.viewOffset() / view.zoomX());
  int pixelAtCurrentNoiseThresholdY;
  dragMode = DragNone;
  
  //Check if user clicked on center bar, to drag it
  if(within(4, e->x(), timeX))
    {
      dragMode = DragTimeBar;
      mouseX = e->x();
      return;
    }
  //Check if user clicked on a threshold bar
  for(int j = 0; j < 2; j++)
    {
      pixelAtCurrentNoiseThresholdY = height() - 1 - toInt((getCurrentThreshold(j) - offsetInv()) / range() * double(height()));
      if(within(4, e->y(), pixelAtCurrentNoiseThresholdY))
	{
	  dragMode = DragNoiseThreshold;
	  //remember which thresholdIndex the user clicked
	  thresholdIndex = j;
	  mouseY = e->y();
	  return;
	}
    }
  //Otherwise user has clicked on background
  {
    mouseX = e->x();
    mouseY = e->y();
    dragMode = DragBackground;
    downTime = view.currentTime();
    downOffset = offset();
  }
}

//------------------------------------------------------------------------------
void AmplitudeWidget::mouseMoveEvent(QMouseEvent *e)
{
  View & view = gdata->getView();
  int pixelAtCurrentTimeX = toInt(view.viewOffset() / view.zoomX());
  int pixelAtCurrentNoiseThresholdY;
  
  switch(dragMode)
    {
    case DragTimeBar:
      {
	int newX = pixelAtCurrentTimeX + (e->x() - mouseX);
	view.setViewOffset(double(newX) * view.zoomX());
	mouseX = e->x();
	view.doSlowUpdate();
      }
      break;
    case DragNoiseThreshold:
      {
	int newY = e->y();
	setCurrentThreshold(double(height() - 1 - newY) / double(height()) * range() + offsetInv(), thresholdIndex);
	mouseY = e->y();
	gdata->getView().doSlowUpdate();
      }
      break;
    case DragBackground:
      gdata->updateActiveChunkTime(downTime - (e->x() - mouseX) * view.zoomX());
      setOffset(downOffset - (double(e->y() - mouseY) / double(height()) * range()));
      view.doSlowUpdate();
      break;
    case DragNone:
      if(within(4, e->x(), pixelAtCurrentTimeX))
	{
  	  setCursor(QCursor(Qt::SplitHCursor));
	}
      else
	{
	  bool overThreshold = false;
	  for(int j = 0; j < 2; j++)
	    {
	      pixelAtCurrentNoiseThresholdY = height() - 1 - toInt((getCurrentThreshold(j) - offsetInv()) / range() * double(height()));
	      if(within(4, e->y(), pixelAtCurrentNoiseThresholdY))
		{
		  overThreshold = true;
		}
	    }
	  if(overThreshold)
	    {
	      setCursor(QCursor(Qt::SplitVCursor));
	    }
	  else
	    {
	      unsetCursor();
	    }
	}
    }
}

//------------------------------------------------------------------------------
void AmplitudeWidget::mouseReleaseEvent(QMouseEvent *)
{
  dragMode = DragNone;
}

//------------------------------------------------------------------------------
void AmplitudeWidget::wheelEvent(QWheelEvent * e)
{
  View & view = gdata->getView();
    if (!(e->state() & (Qt::ControlModifier | Qt::ShiftModifier)))
      {
	if(gdata->getRunning() == STREAM_FORWARD)
	  {
	    view.setZoomFactorX(view.logZoomX() + double(e->delta() / WHEEL_DELTA) * 0.3);
	  }
	else
	  {
	    if(e->delta() < 0)
	      {
		view.setZoomFactorX(view.logZoomX() + double(e->delta() / WHEEL_DELTA) * 0.3, width() / 2);
	      }
	    else
	      {
		view.setZoomFactorX(view.logZoomX() + double(e->delta() / WHEEL_DELTA) * 0.3, e->x());
	      }
	  }
	view.doSlowUpdate();
      }
    e->accept();
}
// EOF
