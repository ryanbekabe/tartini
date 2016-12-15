/***************************************************************************
                          pitchcompassdrawwidget.cpp  -  description
                             -------------------
    begin                : Wed Dec 15 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "pitchcompassdrawwidget.h"
#include "channel.h"
#include "gdata.h"
#include "musicnotes.h"

#include <QMap>
#include <QString>

#include <qwt_compass.h>
#include <qwt_dial_needle.h>
#include <QResizeEvent>

#define INTERVAL 90

//------------------------------------------------------------------------------
PitchCompassDrawWidget::PitchCompassDrawWidget(QWidget *parent, const char* name, int mode)
  : QWidget(parent, name, Qt::WDestructiveClose)
{
    this->mode = mode;

    compass = new QwtCompass(this);
    compass->setLineWidth(4);
    compass->setFrameShadow(QwtCompass::Sunken);
    
    if (mode == 0)
      {
	compass->setMode(QwtCompass::RotateNeedle);
	compass->setScale(36, 5, 0);
      }
    else if (mode == 1)
      {
	compass->setMode(QwtCompass::RotateScale);
	compass->setScale(360, 0);
      }
    else if (mode == 2)
      {
	QMap< double, QString > notes;
	compass->setMode(QwtCompass::RotateNeedle);
	compass->setScale(11, 2, 30);
	for (int i = 0; i < 12; i++)
	  {
	    notes[i * 30] = noteName(i);
	  }
	compass->setLabelMap(notes);
      }

    compass->setScaleTicks(1, 1, 3);

    compass->setNeedle(new QwtCompassMagnetNeedle(QwtCompassMagnetNeedle::ThinStyle));
    compass->setValue(0.0);

    // Force a blank update
    blankCount = 9;
    blank();

    compass->setReadOnly(true);
    compass->show();

}

//------------------------------------------------------------------------------
PitchCompassDrawWidget::~PitchCompassDrawWidget(void)
{
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::resizeEvent(QResizeEvent *)
{
  compass->resize(size());
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::updateCompass(double time)
{
  Channel *active = gdata->getActiveChannel();
  if (active == NULL)
    {
      blank();
      return;
    }
  
  const AnalysisData *data = active->dataAtTime(time);

  if(data && data->getCorrelation() >= 0.9)
    {
      double pitch = data->getPitch();
  
      if (mode == 0)
	{
	  QMap< double, QString > notes;
	  double zeroVal = myround(pitch);
  
	  double value = (pitch - zeroVal) * INTERVAL;
	  compass->setValue(value);
  
	  notes[INTERVAL * 3] = noteName(toInt(zeroVal));
	  notes[0] = noteName(toInt(zeroVal += 2));
	  notes[INTERVAL] = noteName(toInt(zeroVal));
	  compass->setLabelMap(notes);
	}
      else if (mode == 1)
	{
	  QMap< double, QString > notes;
	  double closePitch = myround(pitch);
	  double start = toInt((closePitch - pitch) * INTERVAL);
  
	  if (start < 0)
	    {
	      start += 360;
	    }
	  if (start > 360)
	    {
	      start = fmod(start, 360.0);
	    }
      
	  notes[start] = noteName(toInt(closePitch));
	  notes[start - INTERVAL] = noteName(toInt(closePitch - 1));
	  notes[start + INTERVAL] = noteName(toInt(closePitch + 1));
	  compass->setLabelMap(notes);
	}
      else
	{
	  // mode == 2
	  double value = pitch * 30;
	  compass->setValue(value);
	}

      compass->setValid(true);
    }
  else
    {
      blank();
    }
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::blank(void)
{
  if (++blankCount % 10 == 0)
    {
      if (mode != 2)
	{
	  QMap< double, QString > notes;
	  compass->setLabelMap(notes);
	}
      compass->setValid(false);
      blankCount = 1;
    }
}
// EOF
