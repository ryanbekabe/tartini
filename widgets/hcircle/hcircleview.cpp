/***************************************************************************
                          hstackview.cpp  -  description
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
#include "hcircleview.h"
#include "hcirclewidget.h"
#include "channel.h" 
#include "analysisdata.h"
#include "useful.h" 
#include "myscrollbar.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qsizegrip.h>
#include <qsplitter.h>
#include <qtooltip.h>
#include <q3grid.h>
#include <qwt_wheel.h>
#include <qcursor.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <Q3Frame>

//------------------------------------------------------------------------------
HCircleView::HCircleView( int viewID_, QWidget *parent):
  ViewWidget( viewID_, parent)
{
  setCaption("Harmonic Circle");

  Q3BoxLayout * mainLayout = new Q3HBoxLayout(this);

  Q3BoxLayout * leftLayout = new Q3VBoxLayout(mainLayout);
  Q3BoxLayout * rightLayout = new Q3VBoxLayout(mainLayout);
 
  Q3Grid * waveFrame = new Q3Grid(1, this);
  waveFrame->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
  leftLayout->addWidget(waveFrame);

  hCircleWidget = new HCircleWidget(waveFrame);

  Q3BoxLayout * bottomLayout = new Q3HBoxLayout(leftLayout);
 
  QwtWheel * ZoomWheel = new QwtWheel(this);
  ZoomWheel->setOrientation(Qt::Vertical);
  ZoomWheel->setWheelWidth(14);

#if QWT_VERSION >= 0x060000
  ZoomWheel->setRange(0.001, 0.1);
  ZoomWheel->setSingleStep(0.001);
  // Multiplicator value is 1000 = 1 / 0.001
  ZoomWheel->setPageStepCount(1000);
#else
  ZoomWheel->setRange(0.001, 0.1, 0.001, 1);
#endif // QWT_VERSION >= 0x060000

  ZoomWheel->setValue(0.007);
  hCircleWidget->setZoom(0.007);
  QToolTip::add(ZoomWheel, "Zoom in or out");
  rightLayout->addWidget(ZoomWheel);
  
  QwtWheel * lowestValueWheel = new QwtWheel(this);
  lowestValueWheel->setOrientation(Qt::Vertical);
  lowestValueWheel->setWheelWidth(14);

#if QWT_VERSION >= 0x060000
  lowestValueWheel->setRange(-160, 10);
  lowestValueWheel->setSingleStep(0.01);
  // Multiplicator value is 100 = 1 / 0.01
  lowestValueWheel->setPageStepCount(100);
#else
  lowestValueWheel->setRange(-160, 10, 0.01, 1);
#endif // QWT_VERSION >= 0x060000

  lowestValueWheel->setValue(-100);
  hCircleWidget->setLowestValue(-100);
  QToolTip::add(lowestValueWheel, "Change the lowest value");
  rightLayout->addWidget(lowestValueWheel);
  rightLayout->addStretch(2);
 
  QwtWheel* thresholdWheel = new QwtWheel(this);
  thresholdWheel->setOrientation(Qt::Horizontal);
  thresholdWheel->setWheelWidth(14);

#if QWT_VERSION >= 0x060000
  thresholdWheel->setRange(-160, 10);
  thresholdWheel->setSingleStep(0.01);
  // Multiplicator value is 100 = 1 / 0.01
  thresholdWheel->setPageStepCount(100);
#else
  thresholdWheel->setRange(-160, 10, 0.01, 1);
#endif // QWT_VERSION >= 0x060000

  thresholdWheel->setValue(-100);
  hCircleWidget->setThreshold(-100);
  QToolTip::add(thresholdWheel, "Change the harmonic threshold");
  bottomLayout->addWidget(thresholdWheel);
  bottomLayout->addStretch(2);

  connect(ZoomWheel, SIGNAL(valueChanged(double)), hCircleWidget, SLOT(setZoom(double)));
  connect(ZoomWheel, SIGNAL(valueChanged(double)), hCircleWidget, SLOT(update()));

  connect(lowestValueWheel, SIGNAL(valueChanged(double)), hCircleWidget, SLOT(setLowestValue(double)));
  connect(lowestValueWheel, SIGNAL(valueChanged(double)), hCircleWidget, SLOT(update()));

  connect(thresholdWheel, SIGNAL(valueChanged(double)), hCircleWidget, SLOT(setThreshold(double)));
  connect(thresholdWheel, SIGNAL(valueChanged(double)), hCircleWidget, SLOT(update()));
}

//------------------------------------------------------------------------------
HCircleView::~HCircleView(void)
{
  delete hCircleWidget;
}

//------------------------------------------------------------------------------
QSize HCircleView::sizeHint(void) const
{
  return QSize(300, 200);
}

// EOF
