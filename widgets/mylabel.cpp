/***************************************************************************
                          mylabel.cpp  -  description
                             -------------------
    begin                : 29/6/2005
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

#include "mylabel.h"
#include <QPixmap>
#include <QPaintEvent>

//------------------------------------------------------------------------------
MyLabel::MyLabel(const QString &text_, QWidget *parent, const char * name):
  DrawWidget(parent),
  _text(text_)
{
  QFontMetrics fm = QFontMetrics(get_painter().font());
  _fontHeight = fm.height();
  _textWidth = fm.width(_text);
}

//------------------------------------------------------------------------------
void MyLabel::paintEvent( QPaintEvent * )
{
  beginDrawing(false);
  QPalette l_palette;
  fillBackground(l_palette.color(QPalette::Window));
  get_painter().drawText(4, _fontHeight - 2, _text);
  endDrawing();
}

// EOF
