/***************************************************************************
                          scorewidget.cpp  -  description
                             -------------------
    begin                : Mar 26 2006
    copyright            : (C) 2006 by Philip McLeod
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
#include <QMouseEvent>
#include <QPaintEvent>

#include "scorewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "qfontmetrics.h"
#include "qpen.h"
#include "scoresegmentiterator.h"
#include "musicnotes.h"

//------------------------------------------------------------------------------
ScoreWidget::ScoreWidget(QWidget *parent):
  DrawWidget(parent)
{
  _scaleY = 10;
  _scaleX = 40;
  _boarderX = 15;
  _boarderY = 15;
  _showBaseClef = true;
  _useFlats = false;
  _showNotes = true;
  _showOpaqueNotes = false;
  _showAllMode = false;
  _fontHeight = 14;
  _pitchOffset = 0; //-12; //display 1 octave down
  _font = QFont(tr("AnyStyle"), _fontHeight);
  _fontWidth = QFontMetrics(_font).width('#');
  //half a page look ahead
  _lookAhead = 0.5;
  _lookAheadGap = 0.2;
  _mouseDown = false;

  int counter = 0;
  for(int j = 0; j <= 128; j++)
    {
      flatsLookup[j] = counter;
      if(!isBlackNote(j))
	{
	  counter++;
	}
      sharpsLookup[j] = counter;
    }
  //make relative to Middle C
  int sharpsMiddleC = sharpsLookup[60];
  int flatsMiddleC = flatsLookup[60];
  for(int j = 0; j <= 128; j++)
    {
      sharpsLookup[j] -= sharpsMiddleC;
      flatsLookup[j] -= flatsMiddleC;
    }
}

//------------------------------------------------------------------------------
ScoreWidget::~ScoreWidget(void)
{
}

//------------------------------------------------------------------------------
void ScoreWidget::drawNote(int x, int y, StemType stemType, FillType fillType, int numFlicks)
{
  double noteWidth = _scaleY;
  double noteHeight = _scaleY;
  //draw the left hand side of the note on the point
  if(gdata->polish())
    {
      get_painter().setRenderHint(QPainter::Antialiasing, true);
    }
  if(fillType == FilledNote)
    {
      get_painter().setBrush(get_painter().pen().color());
    }
  else
    {
      get_painter().setBrush(Qt::NoBrush);
    }
  get_painter().drawEllipse(x, y - toInt(noteHeight / 2), toInt(noteWidth), toInt(noteHeight));
  if(gdata->polish())
    {
      get_painter().setRenderHint(QPainter::Antialiasing, false);
    }
  if(stemType == StemUp)
    {
      int stemX = x + toInt(noteWidth); 
      get_painter().drawLine(stemX, y - toInt(3 * _scaleY), stemX, y);
      for(int j = 0; j < numFlicks; j++)
	{
	  get_painter().drawLine(stemX, y - toInt((3.0 - 0.5 * j) * _scaleY), stemX + toInt(noteWidth * 0.75), y - toInt((2.5 - 0.5 * j) * _scaleY));
	}
    }
  else if(stemType == StemDown)
    {
      int stemX = x; 
      get_painter().drawLine(stemX, y + toInt(3 * _scaleY), stemX, y);
      for(int j = 0; j < numFlicks; j++)
	{
	  get_painter().drawLine(stemX, y + toInt((3.0 - 0.5 * j ) * _scaleY), stemX + toInt(noteWidth * 0.75), y + toInt((2.5 - 0.5 * j) * _scaleY));
	}
    }
}

//------------------------------------------------------------------------------
ScoreWidget::NoteType ScoreWidget::getNoteType(double noteLength)
{
  double crotchetLength = 0.4;
  noteLength /= crotchetLength;

  if(noteLength < 0.25 - 0.1  )
    {
      return DemiSemiQuaver;
    }
  else if(noteLength < 0.5 - 0.125)
    {
      return SemiQuaver;
    }
  else if(noteLength < 1.0 - 0.25 )
    {
      return Quaver;
    }
  else if(noteLength < 2.0 - 0.5  )
    {
      return Crotchet;
    }
  else if(noteLength < 4.0 - 1.0  )
    {
      return Minum;
    }
  else
    {
      return SemiBreve;
    }
}

//------------------------------------------------------------------------------
void ScoreWidget::drawNoteAtPitch(int x, int y, int pitch, double noteLength, double volume)
{
  pitch = bound(pitch + _pitchOffset, 0, 128);
  if(!_showAllMode)
    {
      if(noteLength < 0.2)
	{
	  //Don't show really short notes
	  return;
	}
    if(pitch > 84)
      {
	//Don't show extreame notes
	return;
      }
    if(_showBaseClef)
      {
	if(pitch < 36)
	  {
	    return;
	  }
      }
    else
      {
	if(pitch < 57)
	  {
	    return;
	  }
      }
    }
  double noteWidth = _scaleY;
  double noteHeight = _scaleY;
  int yOffset;
  int ySteps;
  int accidentalOffsetX = (_showNotes) ? 2 : 0;
  int j;
  if(!_showNotes)
    {
      get_painter().setPen(QPen(Qt::red, 1));
    }
  if(_useFlats)
    {
      ySteps = flatsLookup[pitch];
      yOffset = toInt(_scaleY * (double)ySteps * 0.5);
      if(isBlackNote(pitch))
	{
	  get_painter().drawText(x - accidentalOffsetX - _fontWidth, y - yOffset + _fontHeight / 2, "b");
	}
    }
  else
    {
      ySteps = sharpsLookup[pitch];
      yOffset = toInt(_scaleY * (double)ySteps * 0.5);
      if(isBlackNote(pitch))
	{
	  get_painter().drawText(x - accidentalOffsetX - _fontWidth, y - yOffset + _fontHeight/2, "#");
	}
    }
  //draw any ledger lines
  get_painter().setPen(QPen(Qt::black, 1));
  int legerLeftX = x - toInt(noteWidth / 2);
  int legerRightX = x + toInt(noteWidth + noteWidth / 2);
  if(ySteps == 0)
    {
      //Middle C ledger line
      get_painter().drawLine(legerLeftX, y, legerRightX, y);
    }
  for(j = 6; j <= ySteps / 2; j++)
    {
      //top ledger lines
      get_painter().drawLine(legerLeftX, y - toInt(_scaleY*j), legerRightX, y - toInt(_scaleY*j));
    }
  for(j = (_showBaseClef) ? -6 : 0; j >= ySteps / 2; j--)
    {
      //bottom ledger lines
      get_painter().drawLine(legerLeftX, y - toInt(_scaleY*j), legerRightX, y - toInt(_scaleY*j));
    }

  NoteType noteType = getNoteType(noteLength);
  QColor noteColor(Qt::black);
  if(!_showOpaqueNotes)
    {
      noteColor.setAlphaF(volume);
    }
  get_painter().setPen(QPen(noteColor, 2));
  if(_showNotes)
    {
      StemType stemType = StemNone;
      if(noteType <= Minum)
	{
	  stemType = (ySteps < 7) ? StemUp : StemDown;
	}
      int numFlicks = 0;
      if(noteType <= Quaver)
	{
	  numFlicks = 1 + (Quaver - noteType);
	}
      drawNote(x, y-yOffset, stemType, (noteType<=Crotchet) ? FilledNote : EmptyNote, numFlicks);
    }
  else
    {
      get_painter().drawRoundRect(x, y - yOffset - toInt(noteHeight/2), toInt(noteLength * _scaleX)-1, toInt(noteHeight), 3, 3);
    }
}

//------------------------------------------------------------------------------
void ScoreWidget::drawStaveSegment(int leftX, int lineCenterY, int widthX)
{
  int j;
  get_painter().setPen(QPen(Qt::blue, 1));
  for(j = 1; j <= 5; j++)
    {
      get_painter().drawLine(leftX, lineCenterY - toInt(j*_scaleY), leftX+widthX, lineCenterY - toInt(j*_scaleY));
    }

  if(_showBaseClef)
    {
      get_painter().drawLine(leftX, lineCenterY - toInt(5*_scaleY), leftX, lineCenterY+toInt(5*_scaleY));
      for(j = 1; j <= 5; j++) 
	{
	  get_painter().drawLine(leftX, lineCenterY + toInt(j*_scaleY), leftX+widthX, lineCenterY + toInt(j*_scaleY));
	}
    }
  else
    {
      get_painter().drawLine(leftX, lineCenterY - toInt(5*_scaleY), leftX, lineCenterY-toInt(1*_scaleY));
    }
}

//------------------------------------------------------------------------------
void ScoreWidget::drawScoreSegment(Channel *ch, double leftX, int lineCenterY, double leftTime, double rightTime)
{
  myassert(ch);
  uint j;

  drawStaveSegment(toInt(leftX), lineCenterY, toInt((rightTime - leftTime) * _scaleX));

  if(ch)
    {
      get_painter().setPen(QPen(Qt::black, 2));
      ChannelLocker channelLocker(ch);
      //draw the notes
      for(j = 0; j < ch->get_note_data().size(); j++)
	{
	  if(ch->isVisibleNote(j) && ch->isLabelNote(j))
	    {
	      double noteTime = ch->timeAtChunk(ch->get_note_data()[j].startChunk());
	      if(between(noteTime, leftTime, rightTime))
		{
		  //FIXME: avgPitch is quite slow to calc
		  drawNoteAtPitch(toInt(leftX + (noteTime - leftTime) * _scaleX),
				  lineCenterY,
				  toInt(ch->get_note_data()[j].avgPitch()),
				  ch->get_note_data()[j].noteLength(),
				  ch->get_note_data()[j].volume()
				  );
		}
	    }
	}
    }
}

//------------------------------------------------------------------------------
void ScoreWidget::paintEvent(QPaintEvent *)
{
  Channel * ch = gdata->getActiveChannel();

  beginDrawing();
  get_painter().setFont(_font);
  get_painter().setBrush(Qt::black);

  ScoreSegmentIterator si(this, ch);
  if(ch)
    {
      QString pageString;
      get_painter().drawText(_fontWidth, height() - 4, pageString.sprintf(tr("Page %d of %d"), si.curPage() + 1, si.numPages()));
      while(si.next())
	{
	  drawScoreSegment(ch, si.leftX(), si.lineCenterY(), si.leftTime(), si.rightTime());
	  if(si.contains(si.curTime()))
	    {
	      int pixelX = toInt(si.leftX() + (si.curTime() - si.leftTime()) * _scaleX);
	      get_painter().setPen(QPen(Qt::black));
	      get_painter().drawLine(pixelX, si.staveTop(), pixelX, si.staveBottom());
	    }
	}
    }
  else
    {
      while(si.next())
	{
	  drawStaveSegment(toInt(si.leftX()), si.lineCenterY(), toInt(si.widthX()));
	}
    }
  endDrawing();
}

//------------------------------------------------------------------------------
void ScoreWidget::mousePressEvent(QMouseEvent *e)
{
  _mouseDown = true;
  Channel * ch = gdata->getActiveChannel();

  ScoreSegmentIterator si(this, ch);
  if(ch)
    {
      while(si.next())
	{
	  if(between(e->x(), (int)si.leftX(), (int)(si.leftX()+si.widthX())) &&
	     between(e->y(), si.staveTop(), si.staveBottom()))
	    {
	      double t = si.leftTime() + ((double)e->x() - si.leftX()) / _scaleX;

	      //Find the last note played at time t
	      int chunk = ch->chunkAtTime(t);
	      AnalysisData * data = ch->dataAtChunk(chunk);
	      while (data && data->getNoteIndex() < 0 && chunk > 0)
		{
		  data = ch->dataAtChunk(--chunk);
		}
	      if(data && data->getNoteIndex() >= 0)
		{
		  int startChunk = ch->get_note_data()[data->getNoteIndex()].startChunk();
		  gdata->updateActiveChunkTime(ch->timeAtChunk(startChunk));
		  if(gdata->getRunning() == STREAM_STOP)
		    {
		      gdata->playSound(ch->getParent());
		    }
		}
	    }
	}
    }
}

//------------------------------------------------------------------------------
void ScoreWidget::mouseMoveEvent(QMouseEvent *)
{
}

//------------------------------------------------------------------------------
void ScoreWidget::mouseReleaseEvent(QMouseEvent *)
{
  _mouseDown = false;
}
// EOF
