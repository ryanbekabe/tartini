/***************************************************************************
                          hstackwidget.cpp  -  description
                             -------------------
    begin                : Mon Jan 10 2005
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
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>

#include "hcirclewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

//------------------------------------------------------------------------------
HCircleWidget::HCircleWidget(QWidget *parent):
  DrawWidget(parent)
{
  //make the widget get updated when the view changes
  zoom = 0.001;
  threshold = -100;
  lowestValue = -110;
  connect(&(gdata->getView()), SIGNAL(onFastUpdate(double)), this, SLOT(update()));
}

//------------------------------------------------------------------------------
HCircleWidget::~HCircleWidget(void)
{
} 

//------------------------------------------------------------------------------
void HCircleWidget::setZoom(double num)
{
  if (zoom != num)
    {
      zoom = num;
    }
}

//------------------------------------------------------------------------------
void HCircleWidget::setThreshold(double num)
{
  if (threshold != num)
    {
      threshold = num;
    }
}

//------------------------------------------------------------------------------
void HCircleWidget::setLowestValue(double num)
{
  if (lowestValue != num)
    {
      lowestValue = num;
    }
}

//------------------------------------------------------------------------------
void HCircleWidget::paintEvent( QPaintEvent * )
{
  Channel * active = gdata->getActiveChannel();
  int numHarmonics = 40;

  beginDrawing();

  double scale;

  QFont * f = new QFont(get_painter().font());
  f->setPointSize(6);
  get_painter().setFont(*f);
  QString s;

  if (threshold > lowestValue)
    {
      get_painter().setPen(QPen(colorBetween(gdata->backgroundColor(), qRgb(128,128,128), 0.3), 2));
      int radius = toInt((double)height() * zoom * (threshold - lowestValue));
      get_painter().drawEllipse(width() / 2 - radius, height() / 2 - radius, 2 * radius, 2 * radius);
      get_painter().drawText(width() / 2 - radius + 5, height() / 2, s.sprintf("Threshold"));
    }

  get_painter().setPen(QPen(QColor(128,128,128),1));

  double step = 10;  

  for (scale = 0; scale > -160; scale -= step)
    {
      if (scale > lowestValue)
	{
	  int radius = toInt((double)height() * zoom * (scale - lowestValue));
	  get_painter().drawEllipse(width() / 2 - radius,height() / 2 - radius, 2 * radius, 2 * radius);
	  
	  get_painter().drawText(width() / 2 + radius, height() / 2, s.sprintf("%1.1f", scale));
  	}
    }

  
  f = new QFont(get_painter().font());
  f->setPointSize(8);
  get_painter().setFont(*f);

  if (active)
    {
      AnalysisData *data = active->dataAtCurrentChunk();

      if (data)
	{
	  int i;
	  double octave = data->getFundamentalFreq();
	  double nextoctave = octave * 2;	
	  int dotSize = 6;
	  int halfDotSize = dotSize / 2;
	  get_painter().setPen(QPen(Qt::black, 2));
	  int m = MIN(data->getHarmonicAmpNoCutOffSize(), (unsigned) numHarmonics);
	  assert(data->getHarmonicAmpNoCutOffSize() == data->getHarmonicFreqSize());
	  for (i = 0; i < m; i++)
	    {
	      if (data->getHarmonicFreqAt(i) > nextoctave)
		{
		  octave = nextoctave;
		  nextoctave = octave * 2;
		}
	      if (data->getHarmonicAmpNoCutOffAt(i) > MAX(threshold, lowestValue))			
		{
		  double angle = (data->getHarmonicFreqAt(i) - octave) / (nextoctave - octave) * 2 * PI;
		  double size = height() * zoom * (data->getHarmonicAmpNoCutOffAt(i) - lowestValue);
		  double size1 = height() * zoom * (MAX(threshold, lowestValue) - lowestValue);
		  int x1 = toInt(sin(angle) * size1);
		  int y1 = toInt(-cos(angle) * size1);
		  int x = toInt(sin(angle) * size); 
		  int y = toInt(-cos(angle) * size);
				
		  get_painter().drawLine(width() / 2 + x1, height() / 2 + y1, width() / 2 + x, height() / 2 + y);
		  get_painter().drawEllipse(width() / 2 + x - halfDotSize, height() / 2 + y - halfDotSize, dotSize, dotSize);
		  QString s;
		  get_painter().drawText(width() / 2 + x + 5, height() / 2 + y, s.sprintf("%d", i + 1));
		}
	    }
	}
    }
  endDrawing();
}

//------------------------------------------------------------------------------
QSize HCircleWidget::sizeHint(void) const
{
  return QSize(300, 200);
}
// EOF
