/***************************************************************************
                          htrackview.cpp  -  description
                             -------------------
    begin                : 29 Mar 2005
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
#include "htrackview.h"
#include "htrackwidget.h"
#include "gdata.h"

#include <QLayout>
#include <QSlider>
#include <qwt_wheel.h>
#include <QSizeGrip>
#include <QToolTip>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QResizeEvent>

//------------------------------------------------------------------------------
HTrackView::HTrackView( int viewID_, QWidget *parent ):
  ViewWidget( viewID_, parent)
{
  QGridLayout * mainLayout = new QGridLayout(this);
  mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
  QVBoxLayout * rightLayout = new QVBoxLayout();
  QHBoxLayout * bottomLayout = new QHBoxLayout();

  QFrame * frame = new QFrame(this);
  frame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  QWidget * aWidget = new QWidget(frame);
  hTrackWidget = new HTrackWidget(aWidget);
  hTrackWidget->setWhatsThis("Shows a 3D keyboard with the current note coloured. "
    "Vertical columns (or tracks), each representing a harmonic (or component frequency), protrude from the back, and move further away over time. "
    "The height of each track is related to how much energy is at that frequency. "
    "Tracks alternate in colour for better visibility. It can be seen how the hamonics in a note fit into the musical scale.");

  peakThresholdSlider = new QSlider(Qt::Vertical, this);
  peakThresholdSlider->setRange(0, 100);
  peakThresholdSlider->setPageStep(10);
  peakThresholdSlider->setValue(5);
  peakThresholdSlider->setToolTip("Thresholding of harmonics");
  
  rotateXWheel = new QwtWheel(this);
  rotateXWheel->setWheelWidth(20);
  rotateXWheel->setRange(-180, 180, 0.1, 1);
  rotateXWheel->setToolTip("Rotate piano horizonally");
  
  rotateYWheel = new QwtWheel(this);
  rotateYWheel->setOrientation(Qt::Vertical);
  rotateYWheel->setWheelWidth(20);
  rotateYWheel->setRange(-90, 0, 0.1, 1);
  rotateYWheel->setToolTip("Rotate piano vertically");
  
  distanceWheel = new QwtWheel(this);
  distanceWheel->setOrientation(Qt::Vertical);
  distanceWheel->setRange(100, 5000, 10, 20);
  distanceWheel->setTotalAngle(20*360);
  distanceWheel->setToolTip("Move towards/away from piano");
  
  QPushButton * homeButton = new QPushButton("Reset", this);
  homeButton->setObjectName("homebutton");
  homeButton->setToolTip("Return to the original view");
  
  QSizeGrip * sizeGrip = new QSizeGrip(this);
  
  mainLayout->addWidget(frame, 0, 0);
  mainLayout->addLayout(bottomLayout, 1, 0);
  mainLayout->addLayout(rightLayout, 0, 1);
  rightLayout->addStretch(2);
  rightLayout->addWidget(peakThresholdSlider);
  rightLayout->addStretch(4);
  rightLayout->addWidget(rotateYWheel);
  rightLayout->addSpacing(14);
  rightLayout->addWidget(distanceWheel);
  bottomLayout->addStretch(0);
  bottomLayout->addWidget(homeButton);
  bottomLayout->addSpacing(14);
  bottomLayout->addWidget(rotateXWheel);
  mainLayout->addWidget(sizeGrip, 1, 1);

  //make the widget get updated when the view changes
  connect(&(gdata->getView()), SIGNAL(onSlowUpdate(double)), hTrackWidget, SLOT(update()));
  connect(peakThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(setPeakThreshold(int)));
  connect(rotateYWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(setViewAngleVertical(double)));
  connect(rotateYWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(update()));
  connect(hTrackWidget, SIGNAL(viewAngleVerticalChanged(double)), rotateYWheel, SLOT(setValue(double)));
  connect(distanceWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(setDistanceAway(double)));
  connect(distanceWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(update()));
  connect(hTrackWidget, SIGNAL(distanceAwayChanged(double)), distanceWheel, SLOT(setValue(double)));
  connect(rotateXWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(setViewAngleHorizontal(double)));
  connect(rotateXWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(update()));
  connect(hTrackWidget, SIGNAL(viewAngleHorizontalChanged(double)), rotateXWheel, SLOT(setValue(double)));
  connect(homeButton, SIGNAL(clicked()), hTrackWidget, SLOT(home()));
}

//------------------------------------------------------------------------------
HTrackView::~HTrackView(void)
{
  delete hTrackWidget;
}

//------------------------------------------------------------------------------
void HTrackView::resizeEvent(QResizeEvent *)
{
  hTrackWidget->resize(size());
}

//------------------------------------------------------------------------------
void HTrackView::setPeakThreshold(int value)
{
  hTrackWidget->setPeakThreshold(float(value) / 100.0);
  hTrackWidget->update();
}

//------------------------------------------------------------------------------
QSize HTrackView::sizeHint(void) const
{
  return QSize(500, 400);
}

// EOF
