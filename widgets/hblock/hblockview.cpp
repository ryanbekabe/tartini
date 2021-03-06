/***************************************************************************
                          hblockview.cpp  -  description
                             -------------------
    begin                : Mon Jan 10 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "hblockview.h"
#include "hblockwidget.h"
#include "gdata.h"
#include <QResizeEvent>

//------------------------------------------------------------------------------
HBlockView::HBlockView( int viewID_, QWidget *parent )
 : ViewWidget( viewID_, parent)
{
  hBlockWidget = new HBlockWidget(this);
  hBlockWidget->show();

  //make the widget get updated when the view changes
  connect(&(gdata->getView()), SIGNAL(onFastUpdate(double)), hBlockWidget, SLOT(update()));
}

//------------------------------------------------------------------------------
HBlockView::~HBlockView(void)
{
  delete hBlockWidget;
}

//------------------------------------------------------------------------------
void HBlockView::resizeEvent(QResizeEvent *)
{
  hBlockWidget->resize(size());
}

//------------------------------------------------------------------------------
QSize HBlockView::sizeHint(void) const
{
  return QSize(200, 180);
}

// EOF
