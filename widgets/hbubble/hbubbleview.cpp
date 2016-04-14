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
#include "hbubbleview.h"
#include "hbubblewidget.h"
#include "channel.h" 
#include "analysisdata.h"
#include "useful.h" 
#include "myscrollbar.h"

#include <QLayout>
#include <QPushButton>
#include <QSizeGrip>
#include <QSplitter>
#include <QToolTip>
#include <QFrame>
#include <qwt_wheel.h>
#include <QCursor>
#include <QComboBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

//------------------------------------------------------------------------------
HBubbleView::HBubbleView( int viewID_, QWidget *parent):
  ViewWidget( viewID_, parent)
{
  setCaption("Harmonic Bubbles");

  QHBoxLayout *mainLayout = new QHBoxLayout(this);

  QVBoxLayout *leftLayout = new QVBoxLayout(mainLayout);
  QVBoxLayout *rightLayout = new QVBoxLayout(mainLayout);
 
  QFrame *waveFrame = new QFrame(this);
  waveFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  leftLayout->addWidget(waveFrame);

  hBubbleWidget = new HBubbleWidget(waveFrame);

  QHBoxLayout *bottomLayout = new QHBoxLayout(leftLayout);

  QwtWheel* harmonicsWheel = new QwtWheel(this);
  harmonicsWheel->setOrientation(Qt::Vertical);
  harmonicsWheel->setWheelWidth(14);
  harmonicsWheel->setRange(1, 40, 1, 1);
  harmonicsWheel->setValue(15);
  hBubbleWidget->setNumHarmonics(15);
  harmonicsWheel->setToolTip("Change number of harmonics shown");
  rightLayout->addWidget(harmonicsWheel);
  rightLayout->addStretch(2);
 
  QwtWheel* windowSizeWheel = new QwtWheel(this);
  windowSizeWheel->setOrientation(Qt::Horizontal);
  windowSizeWheel->setWheelWidth(14);
  windowSizeWheel->setRange(32, 1024, 2, 1);
  windowSizeWheel->setValue(128);
  hBubbleWidget->setHistoryChunks(128);
  windowSizeWheel->setToolTip("Change the window size");
  bottomLayout->addWidget(windowSizeWheel);
  bottomLayout->addStretch(2);
  
  connect(harmonicsWheel, SIGNAL(valueChanged(double)), hBubbleWidget, SLOT(setNumHarmonics(double)));
  connect(harmonicsWheel, SIGNAL(valueChanged(double)), hBubbleWidget, SLOT(update()));

  connect(windowSizeWheel, SIGNAL(valueChanged(double)), hBubbleWidget, SLOT(setHistoryChunks(double)));
  connect(windowSizeWheel, SIGNAL(valueChanged(double)), hBubbleWidget, SLOT(update()));

}

//------------------------------------------------------------------------------
HBubbleView::~HBubbleView(void)
{
  delete hBubbleWidget;
}

//------------------------------------------------------------------------------
QSize HBubbleView::sizeHint(void) const
{
  return QSize(300, 200);
}
// EOF
