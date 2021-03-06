/***************************************************************************
                          pianoview.cpp  -  description
                             -------------------
    begin                : 24 Mar 2005
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
#include <QResizeEvent>

#include "pianoview.h"
#include "pianowidget.h"
#include "gdata.h"
#include "channel.h"
#include "musicnotes.h"

//------------------------------------------------------------------------------
PianoView::PianoView(int viewID_, QWidget * parent):
  ViewWidget( viewID_, parent)
{
  pianoWidget = new PianoWidget(this);
  pianoWidget->show();

  //make the widget get updated when the view changes
  connect(&(gdata->getView()), SIGNAL(onFastUpdate(double)), this, SLOT(changeKey()));
}

//------------------------------------------------------------------------------
PianoView::~PianoView(void)
{
  delete pianoWidget;
}

//------------------------------------------------------------------------------
void PianoView::resizeEvent(QResizeEvent *)
{
  pianoWidget->resize(size());
}

//------------------------------------------------------------------------------
void PianoView::changeKey(void)
{
  Channel *active = gdata->getActiveChannel();
  if(active)
    {
      AnalysisData * data = active->dataAtCurrentChunk();
      if(data && active->isVisibleChunk(data))
	{
	  float pitch = data->getPitch();
	  pianoWidget->setCurrentNote(noteValue(pitch), data->getCorrelation());
	}
      else
	{
	  pianoWidget->setNoNote();
	}
    }
  else
    {
      pianoWidget->setNoNote();
    }
  pianoWidget->update();
}

//------------------------------------------------------------------------------
QSize PianoView::sizeHint(void) const
{
  return QSize(200, 100);
}

// EOF
