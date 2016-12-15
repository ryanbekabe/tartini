/***************************************************************************
                          viewwidget.cpp  -  description
                             -------------------
    begin                : 2003
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "viewwidget.h"
#include "mainwindow.h"

//------------------------------------------------------------------------------
ViewWidget::ViewWidget( int p_view_ID, QWidget * /*p_parent*/, Qt::WFlags p_flags ):
  QWidget( NULL, Qt::WDestructiveClose | p_flags ),
  m_view_ID(p_view_ID)
{
  setWindowTitle(viewData[m_view_ID].title);
  setAutoFillBackground(true);
}

// EOF
