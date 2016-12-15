/***************************************************************************
                          savedialog.cpp  -  description
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

#include "savedialog.h"
#include <QCheckBox>
#include <QLayout>
#include <QLabel>
#include <QVBoxLayout>
#include "gdata.h"
#include <assert.h>

//------------------------------------------------------------------------------
SaveDialog::SaveDialog(QWidget * p_parent):
  QFileDialog(p_parent,tr("Save file"),QDir::convertSeparators(gdata->getSettingsValue("Dialogs/saveFilesFolder", QDir::currentPath())),tr("Wave files (*.wav)"))
{
  setWindowTitle(tr("Choose a filename to save under"));
  setAcceptMode(QFileDialog::AcceptSave);
  setFileMode(QFileDialog::AnyFile);
  setModal(true);
  setOption(QFileDialog::DontUseNativeDialog,true);
  QLayout * l_layout = this->layout();

  QVBoxLayout * l_base_layout = new QVBoxLayout();
  l_layout->addItem(l_base_layout);
  m_append_wav_check_box = new QCheckBox(tr("Append .wav extension if needed"), this);
  m_append_wav_check_box->setChecked(gdata->getSettingsValue("Dialogs/appendWav", true));

  m_remember_folder_check_box = new QCheckBox(tr("Remember current folder"), this);
  m_remember_folder_check_box->setChecked(gdata->getSettingsValue("Dialogs/rememberSaveFolder", true));

  l_base_layout->addSpacing(10);
  l_base_layout->addWidget(m_append_wav_check_box);
  l_base_layout->addWidget(m_remember_folder_check_box);
}

//------------------------------------------------------------------------------
SaveDialog::~SaveDialog(void)
{
}

//------------------------------------------------------------------------------
void SaveDialog::accept(void)
{
  bool l_remember = m_remember_folder_check_box->isChecked();
  gdata->setSettingsValue("Dialogs/rememberSaveFolder", l_remember);
  if(l_remember == true)
    {
      QDir l_current_dir = directory();
      gdata->setSettingsValue("Dialogs/saveFilesFolder", l_current_dir.absolutePath());
    }
  bool l_append_wav = m_append_wav_check_box->isChecked();
  gdata->setSettingsValue("Dialogs/appendWav", l_append_wav);
  if(l_append_wav == true)
    {
      QStringList l_selected_files = selectedFiles();
      assert(1==l_selected_files.size());
      QString l_selected_file = l_selected_files[0];
      if(!l_selected_file.toLower().endsWith(".wav"))
	{
	  l_selected_file += ".wav";
	}
      selectFile(l_selected_file);
    }
  QFileDialog::accept();
}

//------------------------------------------------------------------------------
QString SaveDialog::getSaveWavFileName(QWidget *p_parent)
{
  SaveDialog l_dialog(p_parent);
  if(l_dialog.exec() != QDialog::Accepted)
    {
      return QString();
    }
  QStringList l_selected_files = l_dialog.selectedFiles();
  assert(1==l_selected_files.size());
  QString l_selected_file = l_selected_files[0];

  return l_selected_file;
}

// EOF
