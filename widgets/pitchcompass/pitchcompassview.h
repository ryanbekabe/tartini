/***************************************************************************
                          pitchcompassview.h  -  description
                             -------------------
    begin                : Wed Dec 15 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef PITCHCOMPASSVIEW_H
#define PITCHCOMPASSVIEW_H

#include "viewwidget.h"
#include <QResizeEvent>

class PitchCompassDrawWidget;

class PitchCompassView: public ViewWidget
{
  Q_OBJECT

  public:
  PitchCompassView(int viewID_, QWidget * parent = NULL, int mode = 2);
  virtual ~PitchCompassView(void);

  QSize sizeHint(void) const;

  void resizeEvent(QResizeEvent *);


 private:
  PitchCompassDrawWidget * pitchCompassDrawWidget;
  void changeMode(int mode);

};

#endif // PITCHCOMPASSVIEW_H
// EOF
