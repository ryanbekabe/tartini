/***************************************************************************
                          scorsegmentiterator.cpp  -  description
                             -------------------
    begin                : Mar 29 2006
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

#include "scorewidget.h"
#include "scoresegmentiterator.h"

//------------------------------------------------------------------------------
double ScoreSegmentIterator::widthX(void)
{
  return (_rightTime - _leftTime) * sw->_scaleX;
}

//------------------------------------------------------------------------------
void ScoreSegmentIterator::reset(ScoreWidget * scoreWidget, Channel * channel)
{
  sw = scoreWidget;
  ch = channel;
  staveHeight = sw->getStaveHeight();
  halfStaveHeight = sw->getStaveCenterY();
  numRows = std::max((sw->height() - toInt(sw->_boarderY * 2)) / staveHeight, 1);
  totalRowTime = (double)(sw->width() - sw->_boarderX * 2) / sw->_scaleX;
  totalPageTime = totalRowTime * numRows;
  if(ch)
    {
      _curTime = ch->timeAtChunk(ch->currentChunk());
      _curPage = (int)floor(_curTime / totalPageTime);
      lookAheadGapTime = sw->_lookAheadGap * totalPageTime;
      lookAheadTime = _curTime + sw->_lookAhead * totalPageTime;
      lookBehindTime = lookAheadTime - totalPageTime;
      startOfPageTime = floor(_curTime / totalPageTime) * totalPageTime;
      curRelPageTime = _curTime - startOfPageTime;
      endTime = ch->totalTime();
      _numPages = (int)ceil(endTime / totalPageTime);
      endOfLastPageTime = _numPages * totalPageTime;
      lookAheadTime2 = (_curPage == _numPages - 1) ? std::min(endTime, lookAheadTime) : lookAheadTime;
      lookAheadTime2 = (_curPage == 0) ? endOfLastPageTime : lookAheadTime2;
      lookBehindTime2 = std::min(lookBehindTime, endTime - totalPageTime);
      //FIXME: The ending page isn't drawn correctly
      lookBehindTime2 = (_curPage == 0) ? lookBehindTime + totalPageTime : lookBehindTime2;
      lookBehindTime3 = std::min(lookBehindTime, endTime - totalPageTime);
      curRow = (int)floor(curRelPageTime / totalRowTime);
    }
  rowCounter = 0;
  subRowCounter = -1;
  _leftTime = _rightTime = _leftX = 0.0;
  _lineCenterY = 0;
  _isValid = false;
}

//------------------------------------------------------------------------------
bool ScoreSegmentIterator::next(void)
{
  if(ch)
    {
      while(rowCounter < numRows)
	{
	  int j = rowCounter;
	  double startOfRowTime = startOfPageTime + j * totalRowTime;
	  double endOfRowTime = startOfRowTime + totalRowTime;
	  _lineCenterY = toInt(sw->_boarderY) + halfStaveHeight + staveHeight * j;
	  while(++subRowCounter < 4)
	    {
	      switch(subRowCounter)
		{
		case 1:
		  if(startOfRowTime < lookBehindTime3)
		    {
		      //draw any parts of the next page
		      _leftTime = startOfRowTime + totalPageTime;
		      _rightTime = std::min(endOfRowTime, lookBehindTime3) + totalPageTime;
		      _leftX = (double)sw->_boarderX;
		      return (_isValid = true);
		    }
		  break;
		case 2:
		  if(endOfRowTime > lookBehindTime3+lookAheadGapTime && startOfRowTime < lookAheadTime2)
		    {
		      //normal case
		      _leftTime = std::max(startOfRowTime, lookBehindTime3 + lookAheadGapTime);
		      _rightTime = std::min(startOfRowTime + totalRowTime, lookAheadTime2);
		      _leftX = (double)sw->_boarderX + (_leftTime-startOfRowTime) * sw->_scaleX;
		      return (_isValid = true);
		    }
		  break;
		case 3:
		  if(endOfRowTime - totalPageTime > lookBehindTime2 + lookAheadGapTime)
		    {
		      _leftTime = std::max(startOfRowTime - totalPageTime, lookBehindTime2 + lookAheadGapTime);
		      _leftTime = std::min(_leftTime, endOfRowTime - totalPageTime);
		      _rightTime = endOfRowTime - totalPageTime;
		      _leftX = (double)sw->_boarderX + (_leftTime -(startOfRowTime - totalPageTime)) * sw->_scaleX;
		      return (_isValid = true);
		    }
		}
	    }
	  rowCounter++;
	  subRowCounter = -1;
	}
    }
  else
    {
      while(rowCounter < numRows)
	{
	  double startOfRowTime = startOfPageTime + rowCounter*totalRowTime;
	  double endOfRowTime = startOfRowTime + totalRowTime;
	  _lineCenterY = toInt(sw->_boarderY) + halfStaveHeight + staveHeight*rowCounter;
	  _leftX = sw->_boarderX;
	  _leftTime = startOfRowTime;
	  _rightTime = endOfRowTime;
	  rowCounter++;
	  return (_isValid = true);
	}
    }
  return (_isValid = false);
}

// EOF
