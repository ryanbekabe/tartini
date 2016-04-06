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

#include <qmap.h>
#include <qstring.h>

#include <qwt_compass.h>
#include <qwt_dial_needle.h>
#include <QResizeEvent>

#include "assert.h"

#define INTERVAL 90

//------------------------------------------------------------------------------
PitchCompassDrawWidget::PitchCompassDrawWidget(QWidget *parent, const char* name, int mode):
  QWidget(parent, name, Qt::WDestructiveClose)
{
    this->mode = mode;

    compass = new QwtCompass(this);
    compass->setLineWidth(4);
    compass->setFrameShadow(QwtCompass::Sunken);
    
#if QWT_VERSION >= 0x060000
    QwtCompassScaleDraw *l_scale_draw = new QwtCompassScaleDraw();
#endif // QWT_VERSION >= 0x060000

    if (mode == 0)
      {
	compass->setMode(QwtCompass::RotateNeedle);
#if QWT_VERSION >= 0x060000
	compass->setScale(36, 5);
	// Stepping is now defined by qwt_abstract_slider
	compass->setSingleSteps(0);
#else
	compass->setScale(36, 5, 0);
#endif // QWT_VERSION >= 0x060000
      }
    else if (mode == 1)
      {
	compass->setMode(QwtCompass::RotateScale);
	compass->setScale(360, 0);
      }
    else if (mode == 2)
      {
	compass->setMode(QwtCompass::RotateNeedle);
	QMap< double, QString > notes;
#if QWT_VERSION >= 0x060000
	compass->setScale(11, 2);
	// Stepping is now defined by qwt_abstract_slider
	compass->setSingleSteps(30);
	for (int i = 0; i < 12; i++)
	  {
	    notes[i] = noteName(i);
	  }
	l_scale_draw->setLabelMap(notes);
#else
	compass->setScale(11, 2, 30);
	for (int i = 0; i < 12; i++)
	  {
	    notes[i * 30] = noteName(i);
	  }
	compass->setLabelMap(notes);
#endif // QWT_VERSION >= 0x060000
      }

#if QWT_VERSION >= 0x060000
    // I assume that y defualt Ticks Labels and backbone where displayed with Qwt 5.x
    l_scale_draw->enableComponent( QwtAbstractScaleDraw::Ticks, true );
    l_scale_draw->enableComponent( QwtAbstractScaleDraw::Labels, true );
    l_scale_draw->enableComponent( QwtAbstractScaleDraw::Backbone, true );

    // setScaleTicks method is now replaced by setTickLegnth of QwtAbstractScaleDraw
    l_scale_draw->setTickLength( QwtScaleDiv::MinorTick,10);
    l_scale_draw->setTickLength( QwtScaleDiv::MediumTick,20);
    l_scale_draw->setTickLength( QwtScaleDiv::MajorTick,30);
    compass->setScaleDraw(l_scale_draw);
#else
    compass->setScaleTicks(1, 1, 3);
#endif // QWT_VERSION >= 0x060000

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
  
#if QWT_VERSION >= 0x060000
	  // With Qwt 6.x the map values should match the scale
	  // In mode 0 scale is 36 isntead of 360 so need to divide keys by 10
	  unsigned int l_div = 10;
#else
	  unsigned int l_div = 1;
#endif // QWT_VERSION >= 0x060000

	  notes[(INTERVAL * 3 ) / l_div] = noteName(toInt(zeroVal));
	  notes[0 / l_div] = noteName(toInt(zeroVal += 2));
	  notes[INTERVAL / l_div] = noteName(toInt(zeroVal));

#if QWT_VERSION >= 0x060000
	  QwtCompassScaleDraw * l_scale_draw = dynamic_cast<QwtCompassScaleDraw*>(compass->scaleDraw());
	  assert(l_scale_draw);
	  l_scale_draw->setLabelMap(notes);
#else
	  compass->setLabelMap(notes);
#endif // QWT_VERSION >= 0x060000

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

#if QWT_VERSION >= 0x060000
	  QwtCompassScaleDraw * l_scale_draw = dynamic_cast<QwtCompassScaleDraw*>(compass->scaleDraw());
	  assert(l_scale_draw);
	  l_scale_draw->setLabelMap(notes);
#else
	  compass->setLabelMap(notes);
#endif // QWT_VERSION >= 0x060000
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
#if QWT_VERSION >= 0x060000
	  QwtCompassScaleDraw * l_scale_draw = dynamic_cast<QwtCompassScaleDraw*>(compass->scaleDraw());
	  assert(l_scale_draw);
	  l_scale_draw->setLabelMap(notes);
#else
	  compass->setLabelMap(notes);
#endif // QWT_VERSION >= 0x060000
	}
      compass->setValid(false);
      blankCount = 1;
    }
}
// EOF
