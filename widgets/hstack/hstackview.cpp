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
#include "hstackview.h"
#include "hstackwidget.h"
#include "channel.h" 
#include "analysisdata.h"
#include "useful.h" 
#include "myscrollbar.h"

#include <QLayout>
#include <QPushButton>
#include <QSizeGrip>
#include <QSplitter>
#include <QToolTip>
#include <qwt_wheel.h>
#include <QCursor>
#include <QComboBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

//------------------------------------------------------------------------------
HStackView::HStackView( int viewID_, QWidget * parent):
  ViewWidget( viewID_, parent)
{
  setCaption("Harmonic Stack");

  QHBoxLayout * mainLayout = new QHBoxLayout(this);

  QVBoxLayout * leftLayout = new QVBoxLayout(mainLayout);
  QVBoxLayout * rightLayout = new QVBoxLayout(mainLayout);

  QFrame * waveFrame = new QFrame(this);
  waveFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  leftLayout->addWidget(waveFrame);
  hStackWidget = new HStackWidget(waveFrame);
  
  QHBoxLayout * bottomLayout = new QHBoxLayout(leftLayout);

  QwtWheel * dbRangeWheel = new QwtWheel(this);
  dbRangeWheel->setOrientation(Qt::Vertical);
  dbRangeWheel->setWheelWidth(14);
  dbRangeWheel->setRange(5, 160.0, 0.1, 100);
  dbRangeWheel->setValue(100);
  QToolTip::add(dbRangeWheel, "Zoom dB range vertically");
  rightLayout->addWidget(dbRangeWheel, 0);
  rightLayout->addStretch(2);

  QwtWheel * windowSizeWheel = new QwtWheel(this);
  windowSizeWheel->setOrientation(Qt::Horizontal);
  windowSizeWheel->setWheelWidth(14);
  windowSizeWheel->setRange(32, 1024, 2, 1);
  windowSizeWheel->setValue(128);
  QToolTip::add(windowSizeWheel, "Zoom windowsize horizontally");
  bottomLayout->addWidget(windowSizeWheel, 0);
  bottomLayout->addStretch(2); 

  connect(dbRangeWheel, SIGNAL(valueChanged(double)), hStackWidget, SLOT(setDBRange(double)));
  connect(dbRangeWheel, SIGNAL(valueChanged(double)), hStackWidget, SLOT(update()));

  connect(windowSizeWheel, SIGNAL(valueChanged(double)), hStackWidget, SLOT(setWindowSize(double)));
  connect(windowSizeWheel, SIGNAL(valueChanged(double)), hStackWidget, SLOT(update()));
}

//------------------------------------------------------------------------------
HStackView::~HStackView(void)
{
  delete hStackWidget;
}

//------------------------------------------------------------------------------
QSize HStackView::sizeHint(void) const
{
  return QSize(300, 200);
}

// EOF
