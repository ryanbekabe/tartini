/***************************************************************************
                          opendialog.h  -  description
                             -------------------
    begin                : Feb 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef OPENDIALOG_H
#define OPENDIALOG_H

#include <QFileDialog>

class QCheckBox;

class OpenDialog : public QFileDialog
{
  Q_OBJECT
public:
  OpenDialog(QWidget * parent);
  ~OpenDialog(void);
  void accept(void);
  
  static QString getOpenWavFileName(QWidget *parent = 0);
  
  QCheckBox *rememberFolderCheckBox;
};

#endif // OPENDIALOG_H
// EOF
