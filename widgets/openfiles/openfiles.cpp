/***************************************************************************
                          openfiles.cpp  -  description
                             -------------------
    begin                : Thu Dec 16 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/



#include <QApplication>
#include <QTableWidget>
#include <QHeaderView>
#include <QPixmap>
#include <QResizeEvent>
#include <QCheckBox>

#include "openfiles.h"
#include "gdata.h"
#include "channel.h"
#include "mainwindow.h"
#include "soundfile.h"
#include <assert.h>

//------------------------------------------------------------------------------
OpenFiles::OpenFiles(int id, QWidget *parent):
  ViewWidget(id, parent)
{
  m_table = new QTableWidget(0,2,this);
  QStringList l_header_list;
  l_header_list << tr("Filename(Channel)") << tr("A");
  m_table->setHorizontalHeaderLabels(l_header_list);

  m_table->setWhatsThis("A list of all open channels in all open sounds. "
			"The current active channel is marked with an 'A' beside it. "
			"The tick specifies if a channel should be visible or not in the multi-channel views");
  m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_table->setSelectionMode(QAbstractItemView::SingleSelection);
  m_table->setFocusPolicy(Qt::NoFocus);
  m_table->setSortingEnabled(false);
  m_table->verticalHeader()->setVisible(false);
  connect(gdata, SIGNAL(channelsChanged()), this, SLOT(refreshChannelList()));
  connect(gdata, SIGNAL(activeChannelChanged(Channel*)), this, SLOT(slotActiveChannelChanged(Channel *)));
  connect(m_table, SIGNAL(cellClicked(int,int)), this, SLOT(slotCurrentChanged(int)));

  refreshChannelList();
}

//------------------------------------------------------------------------------
OpenFiles::~OpenFiles(void)
{
}
  
//------------------------------------------------------------------------------
void OpenFiles::refreshChannelList(void)
{
  m_table->clearContents();

  m_table->setRowCount(gdata->getChannelsSize());
  int l_index = 0;
  //put in any channel items that already exist
  unsigned int l_channel_index = 0;
  for(l_channel_index = 0 ; l_channel_index < gdata->getChannelsSize() ; ++l_channel_index)
    {
      QString l_file_name = gdata->getChannelAt(l_channel_index)->getUniqueFilename();
      QCheckBox *l_checkbox = new QCheckBox(l_file_name,m_table);
      l_checkbox->setCheckState(gdata->getChannelAt(l_channel_index)->isVisible() ? Qt::Checked : Qt::Unchecked);
      m_table->setCellWidget(l_index,0,l_checkbox);
      connect(l_checkbox, SIGNAL(stateChanged(int)), this,SLOT(listViewChanged()));
      if(gdata->getChannelAt(l_channel_index) == gdata->getActiveChannel())
	{
	  m_table->setItem(l_index,1,new QTableWidgetItem("A"));
	}
      ++l_index;
    }

  m_table->resizeColumnsToContents();
}

//TODO: Tidy this method up
//------------------------------------------------------------------------------
void OpenFiles::slotActiveChannelChanged(Channel *active)
{
  // Find the index of the active channel
  unsigned int l_index = 0;
  bool l_found = false;
  for(l_index = 0 ; !l_found && l_index < gdata->getChannelsSize() ; ++l_index)
    {
      l_found = gdata->getChannelAt(l_index) == active;
    }
  if(l_found)
    {
      for(int l_row_index = 0 ; l_row_index < m_table->rowCount() ; ++l_row_index)
	{
	  m_table->setItem(l_row_index,1,new QTableWidgetItem(l_row_index == (int)l_index ? "A" : ""));
	}
    }
}

//------------------------------------------------------------------------------
void OpenFiles::listViewChanged(void)
{
  bool l_found = false;
  for(int l_row = 0 ; !l_found && l_row < m_table->rowCount() ; ++l_row)
    {
      QCheckBox * l_check_box = static_cast<QCheckBox *>(m_table->cellWidget(l_row,0));
      bool l_state = Qt::Checked == l_check_box->checkState();
      myassert(l_row < (int)gdata->getChannelsSize());
      if(gdata->getChannelAt(l_row)->isVisible() != l_state)
	{
	  l_found = true;
	  gdata->getChannelAt(l_row)->setVisible(l_state);
	  gdata->getView().doUpdate();
	}
    }
}

//------------------------------------------------------------------------------
void OpenFiles::slotCurrentChanged(int p_row)
{
  int l_selected_row = p_row;
  myassert(0 <= l_selected_row && l_selected_row < int(gdata->getChannelsSize()));
  gdata->setActiveChannel(gdata->getChannelAt(l_selected_row));
  refreshChannelList();
}

//------------------------------------------------------------------------------
void OpenFiles::resizeEvent(QResizeEvent *)
{
  m_table->resize(size());
}

//------------------------------------------------------------------------------
QSize OpenFiles::sizeHint(void) const
{
  return QSize(200, 300);
}

// EOF
