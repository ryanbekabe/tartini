/***************************************************************************
                          scoreview.cpp  -  description
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

#include "scoreview.h"
#include "scorewidget.h"
#include "gdata.h"
#include "qwt_wheel.h"
#include "qlayout.h"
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <QResizeEvent>
#include <QToolTip>
#include <QComboBox>

//------------------------------------------------------------------------------
ScoreView::ScoreView(int viewID_, QWidget * parent):
  ViewWidget( viewID_, parent)
{

  Q3BoxLayout * mainLayout = new Q3HBoxLayout(this);

  Q3BoxLayout * leftLayout = new Q3VBoxLayout(mainLayout);
  Q3BoxLayout * rightLayout = new Q3VBoxLayout(mainLayout);

  //create a drawing object
  scoreWidget = new ScoreWidget(this);
  leftLayout->addWidget(scoreWidget);
  Q3BoxLayout *bottomLayout = new Q3HBoxLayout(leftLayout);

  //vertical scroll wheel
  QwtWheel *scaleWheelY = new QwtWheel(this);
  scaleWheelY->setOrientation(Qt::Vertical);
  scaleWheelY->setWheelWidth(14);

#if QWT_VERSION >= 0x060000
  scaleWheelY->setRange(1.0, 30.0);
  scaleWheelY->setSingleStep(0.1);
  // Multiplicator value is 100 = 1 / 0.01
  scaleWheelY->setPageStepCount(10);
#else
  scaleWheelY->setRange(1.0, 30.0, 0.1, 1);
#endif // QWT_VERSION >= 0x060000

  scaleWheelY->setValue(scoreWidget->scaleY());
  QToolTip::add(scaleWheelY, tr("Zoom vertically"));
  rightLayout->addWidget(scaleWheelY, 1);
  rightLayout->addStretch(4);
  connect(scaleWheelY, SIGNAL(sliderMoved(double)), scoreWidget, SLOT(setScaleY(double)));
  connect(scaleWheelY, SIGNAL(sliderMoved(double)), scoreWidget, SLOT(update()));

  //horizontal scroll wheel
  QwtWheel * scaleWheelX = new QwtWheel(this);
  scaleWheelX->setOrientation(Qt::Horizontal);
  scaleWheelX->setWheelWidth(16);

#if QWT_VERSION >= 0x060000
  scaleWheelX->setRange(1.0, 100.0);
  scaleWheelX->setSingleStep(1.1);
  scaleWheelX->setPageStepCount(1);
#else
  scaleWheelX->setRange(1.0, 100.0, 1.1, 1);
#endif // QWT_VERSION >= 0x060000

  scaleWheelX->setValue(scoreWidget->scaleX());
  QToolTip::add(scaleWheelX, tr("Zoom horizontally"));
  connect(scaleWheelX, SIGNAL(sliderMoved(double)), scoreWidget, SLOT(setScaleX(double)));
  connect(scaleWheelX, SIGNAL(sliderMoved(double)), scoreWidget, SLOT(update()));

  QComboBox * sharpsModeComboBox = new QComboBox(this, tr("sharpsModeComboBox"));
  QStringList s;
  s << tr("Sharps") << tr("Flats");
  sharpsModeComboBox->addItems(s);
  connect(sharpsModeComboBox, SIGNAL(activated(int)), scoreWidget, SLOT(setSharpsMode(int)));
  connect(sharpsModeComboBox, SIGNAL(activated(int)), scoreWidget, SLOT(update()));

  QComboBox * notesModeComboBox = new QComboBox(this, tr("notesModeComboBox"));
  s.clear();
  s << tr("Notes") << tr("MIDI blocks");
  notesModeComboBox->addItems(s);
  connect(notesModeComboBox, SIGNAL(activated(int)), scoreWidget, SLOT(setNotesMode(int)));
  connect(notesModeComboBox, SIGNAL(activated(int)), scoreWidget, SLOT(update()));

  QComboBox * clefModeComboBox = new QComboBox(this, tr("clefModeComboBox"));
  s.clear();
  s << tr("Both clefs") << tr("Treble clef");
  clefModeComboBox->addItems(s);
  connect(clefModeComboBox, SIGNAL(activated(int)), scoreWidget, SLOT(setClefMode(int)));
  connect(clefModeComboBox, SIGNAL(activated(int)), scoreWidget, SLOT(update()));

  QComboBox * opaqueModeComboBox = new QComboBox(this, tr("opaqueModeComboBox"));
  s.clear();
  s << tr("See-through notes") << tr("Opaque notes");
  opaqueModeComboBox->addItems(s);
  connect(opaqueModeComboBox, SIGNAL(activated(int)), scoreWidget, SLOT(setOpaqueMode(int)));
  connect(opaqueModeComboBox, SIGNAL(activated(int)), scoreWidget, SLOT(update()));

  QComboBox * transposeComboBox = new QComboBox(this, tr("transposeComboBox"));
  s.clear();
  s << tr("Up 2 octaves") << tr("Up 1 octave") << tr("As played") << tr("Down 1 octave") << tr("Down 2 octaves");
  transposeComboBox->addItems(s);
  transposeComboBox->setCurrentIndex(2);
  connect(transposeComboBox, SIGNAL(activated(int)), scoreWidget, SLOT(setTransposeLevel(int)));
  connect(transposeComboBox, SIGNAL(activated(int)), scoreWidget, SLOT(update()));

  QComboBox * showAllModeComboBox = new QComboBox(this, "showAllModeComboBox");
  s.clear();
  s << tr("Hide extream notes") << tr("Show all notes");
  showAllModeComboBox->addItems(s);
  connect(showAllModeComboBox, SIGNAL(activated(int)), scoreWidget, SLOT(setShowAllMode(int)));
  connect(showAllModeComboBox, SIGNAL(activated(int)), scoreWidget, SLOT(update()));

  bottomLayout->addWidget(scaleWheelX, 0);
  bottomLayout->addStretch(1);
  bottomLayout->addWidget(sharpsModeComboBox, 0);
  bottomLayout->addStretch(1);
  bottomLayout->addWidget(notesModeComboBox, 0);
  bottomLayout->addStretch(1);
  bottomLayout->addWidget(clefModeComboBox, 0);
  bottomLayout->addStretch(1);
  bottomLayout->addWidget(opaqueModeComboBox, 0);
  bottomLayout->addStretch(1);
  bottomLayout->addWidget(transposeComboBox, 0);
  bottomLayout->addStretch(1);
  bottomLayout->addWidget(showAllModeComboBox, 0);
  bottomLayout->addStretch(4);

  //make any connections
  connect(&(gdata->getView()), SIGNAL(onFastUpdate(double)), scoreWidget, SLOT(update()));
}

//------------------------------------------------------------------------------
ScoreView::~ScoreView(void)
{
  delete scoreWidget;
}

//------------------------------------------------------------------------------
void ScoreView::resizeEvent(QResizeEvent *)
{
}

//------------------------------------------------------------------------------
QSize ScoreView::sizeHint(void) const
{
  return QSize(800, 240);
}
// EOF
