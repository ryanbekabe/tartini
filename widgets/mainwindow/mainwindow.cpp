/***************************************************************************
                          mainwindow.cpp  -  description
                             -------------------
    begin                : May 2002
    copyright            : (C) 2002-2007 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include <QStatusBar>
#include <QMenuBar>
#include <QSplitter>
#include <QLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QWorkspace>
#include <QToolButton>
#include <QLabel>
#include <QSpinBox>
#include <QPixmap>
#include <QTimer>
#include <QMetaObject>
#include <QSizeGrip>
#include <QFileInfo>
#include <QLabel>
#include <QToolBar>
#include <QToolTip>
#include <QMimeData>
#include <QTextEdit>
#include <QTextStream>
#include <QTextBrowser>

#include <QAction>
#include <QKeyEvent>
#include <QEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCustomEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QDesktopServices>

#include "gdata.h"
#include "mainwindow.h"

#include "freqpair.h"
#include "soundfile.h"
#include "channel.h"
#include "myscrollbar.h"
#include "mylabel.h"

// Views
#include "openfiles.h"
#include "freqview.h"
#include "summaryview.h"
#include "pitchcompassview.h"
#include "volumemeterview.h"
#include "tunerview.h"
#include "hblockview.h"
#include "hstackview.h"
#include "hbubbleview.h"
#include "hcircleview.h"
#include "waveview.h"
#include "pianoview.h"
#include "htrackview.h"
#include "correlationview.h"
#include "fftview.h"
#include "cepstrumview.h"
#include "debugview.h"
#include "scoreview.h"
#include "vibratoview.h"
#include "musicnotes.h"

#include "savedialog.h"
#include "opendialog.h"
#include "tartinisettingsdialog.h"
#include <QLineEdit>
#include <QMessageBox>
#include <qwt_slider.h>
#include <QLabel>
#include <QWhatsThis>

#include "wave_stream.h"
#ifdef USE_OGG
#include "ogg_stream.h"
#endif // USE_OGG
#ifdef USE_SOX
#include "sox_stream.h"
#endif // USE_SOX

#include <math.h>
#include "useful.h"
#include "mystring.h"
#include <string.h>

//include the icon .xpm pictures into the program directly
#include "pics/record32x32.xpm"
#include "pics/open.xpm"
#include "pics/close32x32.xpm"
#include "pics/save32x32.xpm"

#include "pics/beginning32x32.xpm"
#include "pics/rewind32x32.xpm"
#include "pics/play32x32.xpm"
#include "pics/playrecord32x32.xpm"
#include "pics/stop32x32.xpm"
#include "pics/fastforward32x32.xpm"
#include "pics/end32x32.xpm"

#include "pics/autofollow32x32.xpm"
#include "pics/shadingon32x32.xpm"
#include "pics/shadingoff32x32.xpm"

#include "myglfonts.h"

#ifdef MACX
#include "main.h"
#endif // MACX

#include "myassert.h"

MainWindow *mainWindow;
MyGLFont *mygl_font;

ViewData viewData[NUM_VIEWS] = 
  {
    //ViewData(title,                          menuName,                           className,           menu type);
    ViewData(QObject::tr("File List"),         QObject::tr("&File List"),          "OpenFiles",         0),
    ViewData(QObject::tr("Pitch Contour"),     QObject::tr("&Pitch Contour"),      "FreqView",          0),
    ViewData(QObject::tr("Chromatic Tuner"),   QObject::tr("&Chromatic Tuner"),    "TunerView",         0),
    ViewData(QObject::tr("Harmonic Track"),    QObject::tr("3D Harmonic &Track"),  "HTrackView",        0),
    ViewData(QObject::tr("Vibrato View"),      QObject::tr("V&ibrato View"),       "VibratoView",       0),
    ViewData(QObject::tr("Musical Score"),     QObject::tr("&Musical Score"),      "ScoreView",         0),
    ViewData(QObject::tr("Oscilloscope"),      QObject::tr("&Oscilloscope"),       "WaveView",          1),
    ViewData(QObject::tr("Correlation View"),  QObject::tr("Corre&lation View"),   "CorrelationView",   1),
    ViewData(QObject::tr("FFT View"),          QObject::tr("FF&T View"),           "FFTView",           1),
    ViewData(QObject::tr("Cepstrum View"),     QObject::tr("C&epstrum View"),      "CepstrumView",      1),
    ViewData(QObject::tr("Debug View"),        QObject::tr("&Debug View"),         "DebugView",         1),
    ViewData(QObject::tr("Harmonic Block"),    QObject::tr("Harmonic &Block"),     "HBlockView",        2),
    ViewData(QObject::tr("Harmonic Stack"),    QObject::tr("&Harmonic Stack"),     "HStackView",        2),
    ViewData(QObject::tr("Harmonic Bubbles"),  QObject::tr("H&armonic Bubbles"),   "HBubbleView",       2),
    ViewData(QObject::tr("Harmonic Circle"),   QObject::tr("Ha&rmonic Circle"),    "HCircleView",       2),
    ViewData(QObject::tr("Pitch Compass"),     QObject::tr("Pitch &Compass"),      "PitchCompassView",  2),
    ViewData(QObject::tr("Piano Keyboard"),    QObject::tr("2D Piano &Keyboard"),  "PianoView",         3),
    ViewData(QObject::tr("Summary View"),      QObject::tr("&Summary View"),       "SummaryView",       3),
    ViewData(QObject::tr("Volume Meter"),      QObject::tr("&Volume Meter"),       "VolumeMeterView",   3)
  };

//------------------------------------------------------------------------------
MainWindow::MainWindow(void):
  QMainWindow(NULL)
{
  setAttribute(Qt::WA_DeleteOnClose);
  createSignalMapper = new QSignalMapper(this);
  connect(createSignalMapper, SIGNAL(mapped(int)), SLOT(openView(int)));

  QFont myFont = QFont();
  myFont.setPointSize(9);
  myFont.setStyleStrategy(QFont::PreferAntialias);
  mygl_font = new MyGLFont(myFont);

#ifndef WINDOWS
  //for running multi-threaded profiling
  getitimer(ITIMER_PROF, &profiler_value);
#endif // WINDOWS
  //6 pixel boarder
  resize(1024 - 6, 768 - 6);

#ifdef MYDEBUG
  setWindowTitle("Tartini (Debug)");
#else // MYDEBUG
  setWindowTitle("Tartini");
#endif // MYDEBUG

  //Create the main Workspace for the view widgets to go in (for MDI)
  theWorkspace = new QWorkspace(this);
  theWorkspace->setObjectName("TheWorkspace");
  setCentralWidget( theWorkspace );
  
  //Create the file Toolbar
  QToolBar * fileToolBar = new QToolBar("File Actions", this);
  addToolBar(Qt::TopToolBarArea, fileToolBar);
  fileToolBar->setIconSize(QSize(32, 32));

  QAction * openAction = new QAction(QIcon(iconOpen), tr("&Open"), this);
  openAction->setShortcut(tr("Ctrl+O"));
  openAction->setWhatsThis(tr("Open a sound from file and process it using the current preferences"));
  fileToolBar->addAction(openAction);
  connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

  QAction * saveAction = new QAction(QIcon(save32x32_xpm), tr("&Save"), this);
  saveAction->setShortcut(tr("Ctrl+S"));
  saveAction->setWhatsThis(tr("Save the active sound to a file"));
  fileToolBar->addAction(saveAction);
  connect(saveAction, SIGNAL(triggered()), gdata, SLOT(saveActiveFile()));
  
  QAction * closeAction = new QAction(QIcon(close32x32_xpm), tr("&Close"), this);
  closeAction->setShortcut(tr("Ctrl+W"));
  closeAction->setWhatsThis(tr("Close the active sound. If unsaved will ask to save. Note: Asking can be disabled in the preferences"));
  fileToolBar->addAction(closeAction);
  connect(closeAction, SIGNAL(triggered()), gdata, SLOT(closeActiveFile()));
  
  QAction * closeAllAction = new QAction(tr("Close All"), this);
  closeAllAction->setWhatsThis(tr("Close all the sounds. If any sounds are unsaved, it will ask to save. Note: Asking can be disabled in the preferences"));
  connect(closeAllAction, SIGNAL(triggered()), gdata, SLOT(closeAllFiles()));
  
  QAction * printAction = new QAction(tr("Print"), this);
  printAction->setShortcut(tr("Ctrl+P"));
  printAction->setWhatsThis(tr("Print the Pitch Contour, fitting the its current view onto a page"));
  connect(printAction, SIGNAL(triggered()), this, SLOT(printPitch()));

  //Create the sound Toolbar
  QToolBar * soundToolBar = new QToolBar("Sound Actions", this);
  addToolBar(Qt::BottomToolBarArea, soundToolBar);
  soundToolBar->setIconSize(QSize(32, 32));
  
  QAction * l_beginning_action = new QAction(QIcon(beginning32x32_xpm),tr("Beginning"),soundToolBar);
  l_beginning_action->setToolTip(tr("Rewind to the beginning"));
  l_beginning_action->setWhatsThis(tr("Jump to the beginning of the sound"));
  soundToolBar->addAction(l_beginning_action);
  connect(l_beginning_action, SIGNAL(triggered()), gdata, SLOT(beginning()));
  
  rewindTimer = new QTimer(this);
  connect(rewindTimer, SIGNAL(timeout()), gdata, SLOT(rewind()));
  QAction * l_rewind_action = new QAction(QIcon(rewind32x32_xpm), tr("Rewind"), soundToolBar);
  l_rewind_action->setToolTip(tr("Rewind"));
  l_rewind_action->setWhatsThis(tr("Rewind the sound"));
  QToolButton * rewindButton = new QToolButton(soundToolBar);
  rewindButton->setDefaultAction(l_rewind_action);
  soundToolBar->addWidget(rewindButton);
  connect(rewindButton, SIGNAL(pressed()), this, SLOT(rewindPressed()));
  connect(rewindButton, SIGNAL(released()), this, SLOT(rewindReleased()));
  
  playIconSet = new QIcon(play32x32_xpm);
  playRecordIconSet = new QIcon(playrecord32x32_xpm);
  stopIconSet = new QIcon(stop32x32_xpm);
  playStopAction = new QAction(*playIconSet, "&Play", this);
  playStopAction->setShortcut(tr("Space"));
  playStopAction->setWhatsThis(tr("Play/Stop the active sound"));
  soundToolBar->addAction(playStopAction);
  connect(playStopAction, SIGNAL(triggered()), this, SLOT(playStopClicked()));

  fastforwardTimer = new QTimer(this);
  connect(fastforwardTimer, SIGNAL(timeout()), gdata, SLOT(fastforward()));
  QAction * l_fast_forward_action = new QAction(QIcon(fastforward32x32_xpm), tr("Fast-forward"), soundToolBar);
  l_fast_forward_action->setToolTip(tr("Fast-forward"));
  l_fast_forward_action->setWhatsThis(tr("Fastfoward the sound"));
  QToolButton * fastforwardButton = new QToolButton(soundToolBar);
  fastforwardButton->setDefaultAction(l_fast_forward_action);
  soundToolBar->addWidget(fastforwardButton);
  connect(fastforwardButton, SIGNAL(pressed()), this, SLOT(fastforwardPressed()));
  connect(fastforwardButton, SIGNAL(released()), this, SLOT(fastforwardReleased()));

  QAction * endAction = new QAction(QIcon(end32x32_xpm), tr("&End"), this);
  endAction->setWhatsThis(tr("Jump to the end of the sound"));
  endAction->setShortcut(tr("Ctrl+E"));
  connect(endAction, SIGNAL(triggered()), gdata, SLOT(end()));
  soundToolBar->addAction(endAction);

  //Create the Actions, to be used in Menus and Toolbars
  recordIconSet = new QIcon(record32x32_xpm);
  recordAction = new QAction(*recordIconSet, tr("&Record"), this);
  recordAction->setShortcut(tr("Return"));
  recordAction->setWhatsThis(tr("Record a new sound, using the input device and settings selected in the preferences"));
  fileToolBar->addAction(recordAction);
  connect(recordAction, SIGNAL(triggered()), this, SLOT(openRecord()));

  playRecordAction = new QAction(*playRecordIconSet, tr("Play and Record"), this);
  playRecordAction->setShortcut(tr("Shift+Return"));
  playRecordAction->setWhatsThis(tr("Play the active sound and record a new one at the same time!"));
  fileToolBar->addAction(playRecordAction);
  connect(playRecordAction, SIGNAL(triggered()), this, SLOT(openPlayRecord()));

  QAction * quit = new QAction(tr("&Quit"), this);
  quit->setShortcut(tr("Ctrl+Q"));
  quit->setWhatsThis(tr("Quit the Tartini application"));
  connect(quit, SIGNAL(triggered()), this, SLOT( close() ));

  //Create the File Menu
  QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAction);
  fileMenu->addAction(saveAction);
  fileMenu->addAction(closeAction);
  fileMenu->addAction(closeAllAction);
  fileMenu->addSeparator();
  fileMenu->addAction(playStopAction);
  fileMenu->addAction(recordAction);
  fileMenu->addAction(playRecordAction);
  fileMenu->addSeparator();
  fileMenu->addAction(printAction);
  fileMenu->addSeparator();
  fileMenu->addAction(quit);

  //Channel Menu
  QAction * exportAction1 = new QAction(tr("&Export to plain text"), this);
  connect(exportAction1, SIGNAL(triggered()), this, SLOT(exportChannelPlainText()));
  QAction * exportAction2 = new QAction(tr("&Export to matlab"), this);
  connect(exportAction2, SIGNAL(triggered()), this, SLOT(exportChannelMatlab()));

  QMenu * channelMenu = menuBar()->addMenu(tr("C&hannel"));
  channelMenu->addAction(exportAction1);
  channelMenu->addAction(exportAction2);

  // Create actions for the new view menu
  newViewMenu = menuBar()->addMenu(tr("&Create"));
  connect(newViewMenu, SIGNAL(aboutToShow()), this, SLOT(newViewAboutToShow()));

  //Create the Window Menu
  windowMenu = menuBar()->addMenu(tr("&Windows"));
  connect( windowMenu, SIGNAL( aboutToShow() ),
	     this, SLOT( windowMenuAboutToShow() ) );

  //Create the Options Menu
  optionsMenu = menuBar()->addMenu(tr("&Options"));
  optionsMenu->addAction(tr("&Preferences"), this, SLOT(menuPreferences()));

  QAction * whatsThis = QWhatsThis::createAction(this);
  whatsThis->setToolTip(tr("What's this?"));
  whatsThis->setWhatsThis(tr("Click this button, then click something to learn more about it"));

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(whatsThis);
  helpMenu->addSeparator();
  helpMenu->addAction(tr("Documentation"), this, SLOT(showDocumentation()));
  helpMenu->addSeparator();
  helpMenu->addAction(tr("About Tartini"), this, SLOT(aboutTartini()));
  helpMenu->addAction(tr("About Qt"), this, SLOT(aboutQt()));
  
  //Create the other toolbar, which contains some option stuff
  QToolBar * analysisToolBar = new QToolBar(tr("Analysis Toolbar"), this);
  addToolBar(analysisToolBar);
  analysisToolBar->setIconSize(QSize(32, 32));

  QIcon * autoFollowIconSet = new QIcon();
  autoFollowIconSet->addPixmap(QPixmap(autofollow32x32_xpm), QIcon::Normal);
  QAction * l_auto_follow_action = new QAction(*autoFollowIconSet, tr("Auto Follow"),analysisToolBar);
  l_auto_follow_action->setToolTip(tr("Moves the view up and down automaticlly with the active channel when playing or recording"));
  l_auto_follow_action->setCheckable(true);
  l_auto_follow_action->setChecked(gdata->getView().autoFollow());
  QToolButton * autoFollowButton = new QToolButton(analysisToolBar);
  autoFollowButton->setDefaultAction(l_auto_follow_action);
  autoFollowButton->setAutoRaise(true);
  autoFollowButton->setWhatsThis(tr("Scrolls the Pitch Contour view up and down automaticlly with the active channel when playing or recording. Note: Manual scrolling (vertically) will be disabled during this time."));
  analysisToolBar->addWidget(autoFollowButton);
  connect(l_auto_follow_action, SIGNAL(toggled(bool)), &(gdata->getView()), SLOT(setAutoFollow(bool)));

  QIcon * backgroundShadingIconSet = new QIcon();
  backgroundShadingIconSet->addPixmap(QPixmap(shadingon32x32_xpm), QIcon::Normal, QIcon::On);
  backgroundShadingIconSet->addPixmap(QPixmap(shadingoff32x32_xpm), QIcon::Normal, QIcon::Off);

  QAction * l_background_shading_action = new QAction(*backgroundShadingIconSet, "Background Shading",analysisToolBar);
  l_background_shading_action->setToolTip("Toggle background shading on/off");
  l_background_shading_action->setCheckable(true);
  l_background_shading_action->setChecked(gdata->getView().backgroundShading());
  QToolButton * backgroundShadingButton = new QToolButton(analysisToolBar);
  backgroundShadingButton->setDefaultAction(l_background_shading_action);
  backgroundShadingButton->setAutoRaise(true);
  backgroundShadingButton->setWhatsThis(tr("Draw solid color underneath the Pitch Contour, to help you find the line"));
  analysisToolBar->addWidget(backgroundShadingButton);
  connect(l_background_shading_action, SIGNAL(toggled(bool)), &(gdata->getView()), SLOT(setBackgroundShading(bool)));

  analysisToolBar->addAction(whatsThis);

  View & view = gdata->getView();
  QToolBar * timeBarDock = new QToolBar(tr("Time-axis Slider"), this);
  addToolBar(Qt::BottomToolBarArea, timeBarDock);
  timeBarDock->setIconSize(QSize(32, 32));

#if QWT_VERSION == 0x050000
  timeSlider = new QwtSlider(timeBarDock, Qt::Horizontal, QwtSlider::None, QwtSlider::BgBoth);
#else // QWT_VERSION == 0x050000
  timeSlider = new QwtSlider(timeBarDock, Qt::Horizontal, QwtSlider::NoScale, QwtSlider::BgBoth);
#endif // QWT_VERSION == 0x050000
  timeSlider->setRange(gdata->leftTime(), gdata->rightTime(), 1.0 / 10000.0, 1000);
  timeSlider->setValue(view.currentTime());
  timeSlider->setTracking(true);
  timeSlider->setThumbWidth(20);
  timeSlider->setThumbLength(60);
  timeSlider->setBorderWidth(4);
  timeSlider->setMargins(2, 2);
  timeSlider->setMinimumWidth(200);
  timeSlider->setWhatsThis("Drag the time slider to move back and forward through the sound file");
  connect(timeSlider, SIGNAL(sliderMoved(double)), gdata, SLOT(updateActiveChunkTime(double)));
  connect(timeSlider, SIGNAL(sliderMoved(double)), &view, SLOT(doSlowUpdate()));
  connect(&view, SIGNAL(onSlowUpdate(double)), timeSlider, SLOT(setValue(double)));
  connect(gdata, SIGNAL(timeRangeChanged(double, double)), this, SLOT(setTimeRange(double, double)));
  timeBarDock->addWidget(timeSlider);
  
  QToolBar * volumeMeterToolBar = new QToolBar(tr("Volume Meter"), this);
  addToolBar(volumeMeterToolBar);
  volumeMeterToolBar->setIconSize(QSize(32, 32));
  VolumeMeterView * volumeMeterView = new VolumeMeterView(VIEW_VOLUME_METER, volumeMeterToolBar);
  volumeMeterView->setWhatsThis(tr("Shows the volume (in dB) of the left and right channels of the active sound. Note: If a mono sound the both are the same"));
  volumeMeterToolBar->addWidget(volumeMeterView);


  QToolBar * keyToolBar = new QToolBar(tr("Key Toolbar"), this);
  keyToolBar->setWhatsThis(tr("Used to set which reference lines are drawn in the Pitch Contour View."));
  addToolBar(keyToolBar);
  QLabel * keyLabel = new QLabel(tr("Key"), keyToolBar);
  keyToolBar->addWidget(keyLabel);

  QComboBox * keyComboBox = new QComboBox(keyToolBar);
  keyComboBox->setWindowTitle(tr("Key"));
  QStringList s;
  for(int j = 0; j < NUM_MUSIC_KEYS; j++)
    {
      s << gMusicKeyName[j];
    }
  keyComboBox->addItems(s);
  keyComboBox->setCurrentIndex(gdata->musicKey());
  keyToolBar->addWidget(keyComboBox);
  connect(keyComboBox, SIGNAL(activated(int)), gdata, SLOT(setMusicKey(int)));
  connect(gdata, SIGNAL(musicKeyChanged(int)), keyComboBox, SLOT(setCurrentIndex(int)));
  connect(gdata, SIGNAL(musicKeyChanged(int)), &(gdata->getView()), SLOT(doUpdate()));

  keyTypeComboBox = new QComboBox(keyToolBar);
  keyTypeComboBox->setWindowTitle(tr("Scale type"));
  s.clear();
  for(uint j = 0; j < gMusicScales.size(); j++)
    {
      s << gMusicScales[j].name();
    }
  keyTypeComboBox->addItems(s);
  keyTypeComboBox->setCurrentIndex(gdata->musicKeyType());
  keyToolBar->addWidget(keyTypeComboBox);
  connect(keyTypeComboBox, SIGNAL(activated(int)), gdata, SLOT(setMusicKeyType(int)));
  connect(gdata, SIGNAL(musicKeyTypeChanged(int)), keyTypeComboBox, SLOT(setCurrentIndex(int)));
  connect(gdata, SIGNAL(musicKeyTypeChanged(int)), &(gdata->getView()), SLOT(doUpdate()));

  QComboBox * temperedComboBox = new QComboBox(keyToolBar);
  temperedComboBox->setWindowTitle(tr("Tempered type"));
  s.clear();
  for(uint j = 0; j < gMusicKeys.size(); j++)
    {
      s << gMusicKeys[j].name();
    }
  temperedComboBox->addItems(s);
  temperedComboBox->setCurrentIndex(gdata->temperedType());
  keyToolBar->addWidget(temperedComboBox);
  connect(temperedComboBox, SIGNAL(activated(int)), gdata, SLOT(setTemperedType(int)));
  connect(gdata, SIGNAL(temperedTypeChanged(int)), temperedComboBox, SLOT(setCurrentIndex(int)));
  connect(gdata, SIGNAL(temperedTypeChanged(int)), &(gdata->getView()), SLOT(doUpdate()));

  QToolBar * freqAToolBar = new QToolBar(tr("Frequency Offset Toolbar"), this);
  freqAToolBar->setWhatsThis(tr("The frequency of an even-tempered 'A' used for reference lines in the Pitch Contour View. Default 440 Hz."
    "Note: For other scales the root note is chosen from the even-tempered scale with that 'A'."));
  addToolBar(freqAToolBar);

  QSpinBox * freqASpinBox = new QSpinBox(freqAToolBar);
  freqASpinBox->setRange(400,600);
  freqASpinBox->setSingleStep(1);
  freqASpinBox->setPrefix("A=");
  freqASpinBox->setSuffix(" Hz");
  freqASpinBox->setFocusPolicy(Qt::NoFocus);
  freqASpinBox->setValue(toInt(gdata->freqA()));
  freqAToolBar->addWidget(freqASpinBox);
  connect(freqASpinBox, SIGNAL(valueChanged(int)), gdata, SLOT(setFreqA(int)));
  connect(freqASpinBox, SIGNAL(valueChanged(int)), &(gdata->getView()), SLOT(doUpdate()));
  QFont font("Courier", 12, QFont::Bold);
  
  noteLabel = new MyLabel("Note: 9999", statusBar(), "notelabel");
  statusBar()->addPermanentWidget(noteLabel, 0);
  setNoteLabel();
  noteLabel->setToolTip("The current note number in the active file");
  connect(&(gdata->getView()), SIGNAL(onSlowUpdate(double)), this, SLOT(setNoteLabel()));

  
  chunkLabel = new MyLabel("Chunk: 999999", statusBar(), "chunklabel");
  statusBar()->addPermanentWidget(chunkLabel, 0);
  setChunkLabel();
  chunkLabel->setToolTip("The current chunk number in the active file");
  connect(&(gdata->getView()), SIGNAL(onSlowUpdate(double)), this, SLOT(setChunkLabel()));

  timeLabel = new MyLabel("Time: -00:00.000", statusBar(), "timelabel");
  statusBar()->addPermanentWidget(timeLabel, 0);
  setTimeLabel(0);
  timeLabel->setToolTip(tr("The current time positon for all files (mins:sec)"));
  connect(&(gdata->getView()), SIGNAL(onSlowUpdate(double)), this, SLOT(setTimeLabel(double)));

  statusBar()->showMessage( "Ready", 2000 );

  connect(gdata, SIGNAL(activeChannelChanged(Channel*)), this, SLOT(setTitle(Channel*)));
}

//------------------------------------------------------------------------------
MainWindow::~MainWindow(void)
{
  delete rewindTimer;
  delete fastforwardTimer;
  fprintf(stderr, "Has pending events = %s\n", (qApp->hasPendingEvents()) ? "Yes" : "No");
}

//------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
  if(gdata->closeAllFiles())
    {
      saveViewGeometry();
      event->accept();
    }
  else
    {
      event->ignore();
    }
}

//------------------------------------------------------------------------------
bool MainWindow::event(QEvent * e)
{
  if(e->type() == UPDATE_FAST || e->type() == UPDATE_SLOW)
    {
      SoundFile * soundFile = gdata->getAudioThread().curSoundFile();
      if(soundFile)
	{
	  soundFile->lock();
	  gdata->getView().setCurrentTime(soundFile->timeAtCurrentChunk());
	  soundFile->unlock();
	}
      gdata->updateViewLeftRightTimes();
      gdata->setNeedUpdate(false);
      return true;
    }
  else if(e->type() == SOUND_STARTED)
    {
      playStopAction->setIcon(*stopIconSet);
      playStopAction->setText("Stop");
      recordAction->setIcon(*stopIconSet);
      recordAction->setText("Stop");
      playRecordAction->setIcon(*stopIconSet);
      playRecordAction->setText("Stop");
    }
  else if(e->type() == SOUND_STOPPED)
    {
      playStopAction->setIcon(*playIconSet);
      playStopAction->setText("Play");
      recordAction->setIcon(*recordIconSet);
      recordAction->setText("Record");
      playRecordAction->setIcon(*playRecordIconSet);
      playRecordAction->setText("Play and Record");
    }
  else if(e->type() == SETTINGS_CHANGED)
    {
      gdata->updateQuickRefSettings();
    }
  return QMainWindow::event(e);
}

//------------------------------------------------------------------------------
void MainWindow::keyPressEvent (QKeyEvent * e)
{
  View & view = gdata->getView();
  
  double speed = 1;
  if(e->QInputEvent::modifiers() & Qt::ShiftModifier)
    {
      speed *= 4;
    }
  if(e->QInputEvent::modifiers() & Qt::ControlModifier)
    {
      speed *= 10;
    }
  double newTime;
  
  switch(e->key())
    {
#ifdef MYDEBUG
    case Qt::Key_Escape:
      gdata->stopAndWaitAudioThread();
      qApp->closeAllWindows();
      break;
#endif // MYDEBUG
    case Qt::Key_Left:
      if(gdata->getRunning() == STREAM_FORWARD)
	{
	  gdata->rewind();
	}
      else
	{
	  if(gdata->getActiveChannel())
	    {
	      newTime = view.currentTime() - gdata->getActiveChannel()->timePerChunk() * speed;
	    }
	  else
	    {
	      //move 1/5th of a second back
	      newTime = view.currentTime() - 0.10 * speed;
	    }
	  gdata->updateActiveChunkTime(newTime);
	}
      break;
    case Qt::Key_Right:
      if(gdata->getRunning() == STREAM_FORWARD)
	{
	  gdata->fastforward();
	}
      else
	{
	  if(gdata->getActiveChannel())
	    {
	      newTime = view.currentTime() + gdata->getActiveChannel()->timePerChunk()*speed;
	    }
	  else
	    {
	      //move 1/5th of a second forward
	      newTime = view.currentTime() + 0.10 * speed;
	    }
	  gdata->updateActiveChunkTime(newTime);
	}
      break;
    case Qt::Key_Up:
      //move 1/5 of a semi-tone
      view.setViewBottom(view.viewBottom() + 0.2 * speed);
      gdata->getView().doSlowUpdate();
      break;
    case Qt::Key_Down:
      //move 1/5 of a semi-tone
      view.setViewBottom(view.viewBottom() - 0.2 * speed);
      gdata->getView().doSlowUpdate();
      break;
    case Qt::Key_PageUp:
      //move 1/5 of a semi-tone
      view.setViewBottom(view.viewBottom() + 1.0 * speed);
      gdata->getView().doSlowUpdate();
      break;
    case Qt::Key_PageDown:
      //move 1/5 of a semi-tone
      view.setViewBottom(view.viewBottom() - 1.0 * speed);
      gdata->getView().doSlowUpdate();
      break;
    case Qt::Key_Home:
      gdata->updateActiveChunkTime(gdata->leftTime());
      break;
    case Qt::Key_End:
      gdata->updateActiveChunkTime(gdata->rightTime());
      break;
    case Qt::Key_Comma:
      view.setViewOffset(view.viewOffset() - view.viewWidth()/20.0);
      view.doSlowUpdate();
      break;
    case Qt::Key_Period:
      view.setViewOffset(view.viewOffset() + view.viewWidth()/20.0);
      view.doSlowUpdate();
      break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
      emit zoomInPressed();
      break;
    case Qt::Key_Minus:
      emit zoomOutPressed();
      break;
    default:
      e->ignore();
    }
}

//------------------------------------------------------------------------------
void MainWindow::openFile(void)
{
  QString lastFolder = QDir::convertSeparators(gdata->getSettingsValue("Dialogs/openFilesFolder", QDir::currentPath()));
  QString s = QFileDialog::getOpenFileName(this, "Open File", lastFolder, "Sounds (*.wav)");
  if(s.isEmpty())
    {
      return;
    }
  s = QDir::convertSeparators(s);
  gdata->setSettingsValue("Dialogs/openFilesFolder", s);
  openFile(s.toStdString().c_str());
}

//------------------------------------------------------------------------------
void MainWindow::openFile(const char *filename)
{
  SoundFile * newSoundFile = new SoundFile();
  if(!newSoundFile->openRead(filename))
    {
      fprintf(stderr, "Error opening %s\n", filename);
      delete newSoundFile;
      return;
    }

  newSoundFile->preProcess();
  gdata->updateViewLeftRightTimes();

  gdata->addFileToList(newSoundFile);
  gdata->setActiveChannel(&(newSoundFile->getChannel(0)));
  QApplication::postEvent(mainWindow, new QEvent(static_cast<QEvent::Type>(UPDATE_SLOW)));
  gdata->getView().doUpdate();
}

//------------------------------------------------------------------------------
void MainWindow::openRecord(void)
{
  openRecord(false);
}

//------------------------------------------------------------------------------
void MainWindow::openPlayRecord(void)
{
  openRecord(true);
}

//------------------------------------------------------------------------------
void MainWindow::openRecord(bool andPlay)
{
  if(gdata->getRunning())
    {
      gdata->stop();
      return;
    }

  SoundFile * playSoundFile = (andPlay) ? gdata->getActiveSoundFile() : NULL;

  int rate = gdata->getSettingsValue("Sound/sampleRate", 44100);
  QString numberOfChannels = gdata->getSettingsValue("Sound/numberOfChannels", QString("mono"));
  int channels;
  if(numberOfChannels.toLower() == "mono")
    {
      channels = 1;
    }
  else
    {
      channels = 2;
    }
  int bits = gdata->getSettingsValue("Sound/bitsPerSample", 16);
  int windowSize = gdata->getAnalysisBufferSize(rate);
  int stepSize = gdata->getAnalysisStepSize(rate);

  //If playing and recording then overide the record settings to the same as the playing file
  if(playSoundFile)
    {
      rate = playSoundFile->rate();
      channels = playSoundFile->numChannels();
      bits = playSoundFile->bits();
      windowSize = playSoundFile->bufferSize();
      stepSize = playSoundFile->bufferSize()/2;
    }

  QString tempFileFolder = gdata->getSettingsValue("General/tempFilesFolder", QDir::convertSeparators(QDir::currentPath()));
  QDir dir = QDir(tempFileFolder);
  QFileInfo fileInfo;
  QString fileName;
  bool fileExists;
  do
    {
      fileExists = false;
      fileName = gdata->getFilenameString();
      fileInfo.setFile(dir, fileName);
      if(fileInfo.exists())
	{
	  fileExists = true;
	  int fileGeneratingNumber = gdata->getSettingsValue("General/fileGeneratingNumber", 1);
	  gdata->setSettingsValue("General/fileGeneratingNumber", fileGeneratingNumber + 1);
	}
    }
  while(fileExists);
  
  SoundFile *newSoundFile = new SoundFile();
  QString newFilename(fileInfo.absoluteFilePath());
  newFilename = QDir::convertSeparators(newFilename);
  if(!newSoundFile->openWrite(newFilename.toStdString().c_str(), rate, channels, bits, windowSize, stepSize))
    {
      delete newSoundFile; newSoundFile = NULL;
      return;
    }
  
  if(!gdata->openPlayRecord(newSoundFile, playSoundFile))
    {
      gdata->closeFile(newSoundFile, GData::NEVER_SAVE);
      QMessageBox::warning(mainWindow, "Error", QString("Error opening sound device for recording"), QMessageBox::Ok, QMessageBox::NoButton);
      return;
    }
  gdata->addFileToList(newSoundFile);
  gdata->getView().setCurrentTime(0.0);
  gdata->setActiveChannel(&(newSoundFile->getChannel(0)));

  int fileGeneratingNumber = gdata->getSettingsValue("General/fileGeneratingNumber", 1);
  gdata->setSettingsValue("General/fileGeneratingNumber", fileGeneratingNumber + 1);
}

//------------------------------------------------------------------------------
void MainWindow::windowMenuAboutToShow(void)
{
  windowMenu->clear();

  QWidgetList windows = theWorkspace->windowList();
  for(int i = 0; i < int(windows.count()); ++i )
    {
      QAction * l_action = windowMenu->addAction(windows.at(i)->windowTitle(),this, SLOT( windowMenuActivated() ) );
      l_action->setData(i);
      l_action->setChecked(theWorkspace->activeWindow() == windows.at(i));
    }
  
  windowMenu->addSeparator();
  QAction * cascade = windowMenu->addAction( "&Cascade", theWorkspace, SLOT( cascade() ));
  QAction * close = windowMenu->addAction( "Close &All", this, SLOT( closeAllWidgets() ));

  if (windows.isEmpty())
    {
      cascade->setEnabled(false);
      close->setEnabled(false);
    }
}

//------------------------------------------------------------------------------
void MainWindow::windowMenuActivated(void)
{
  int id = static_cast<QAction*>(sender())->data().toInt();
  std::cout << "windowMenuActivated " << id << std::endl ;
    QWidget* w = theWorkspace->windowList().at( id );
    if ( w )
      {
    	w->showNormal();
	w->setFocus();
      }
}

//------------------------------------------------------------------------------
void MainWindow::message(QString s, int msec)
{
  QStatusBar *sb = statusBar();
  if(sb)
    {
      sb->showMessage(s, msec);
    }
}

//------------------------------------------------------------------------------
void MainWindow::closeAllWidgets(void)
{
  QWidgetList opened = theWorkspace->windowList();
  for(QWidgetList::iterator it = opened.begin(); it < opened.end(); it++)
    {
      (*it)->close();
    }
}

//------------------------------------------------------------------------------
void MainWindow::menuPreferences(void)
{
  TartiniSettingsDialog *settings = new TartiniSettingsDialog(this);
  settings->show();
}


//------------------------------------------------------------------------------
QWidget * MainWindow::openView(int viewID)
{
  QWidget * w = NULL;
  int useTopLevelWidgets = gdata->getSettingsValue("Display/useTopLevelWidgets", false);
  QWidget * parent = (useTopLevelWidgets) ? NULL : theWorkspace;

  switch(viewID)
    {
    case VIEW_OPEN_FILES:
      w = new OpenFiles(viewID, parent);
      break;
    case VIEW_FREQ:
      {
        FreqView * freqView = new FreqView(viewID, parent);
        connect(this, SIGNAL(zoomInPressed()), freqView, SLOT(zoomIn()));
        connect(this, SIGNAL(zoomOutPressed()), freqView, SLOT(zoomOut()));
        w = freqView;
      }
      break;
    case VIEW_SUMMARY:
      w = new SummaryView( viewID, parent);
      break;
    case VIEW_PITCH_COMPASS:
      w = new PitchCompassView(viewID, parent);
      break;
    case VIEW_VOLUME_METER:
      w = new VolumeMeterView(viewID, parent);
      break;
    case VIEW_TUNER:
      w = new TunerView(viewID, parent);
      break;
    case VIEW_HBLOCK:
      w = new HBlockView(viewID, parent);
      break;
    case VIEW_HSTACK:
      w = new HStackView(viewID, parent);
      break;
    case VIEW_HBUBBLE:
      w = new HBubbleView(viewID, parent);
      break;
    case VIEW_HCIRCLE:
      w = new HCircleView(viewID, parent);
      break;
    case VIEW_WAVE:
      w = new WaveView(viewID, parent);
      break;
    case VIEW_PIANO:
      w = new PianoView(viewID, parent);
      break;
    case VIEW_HTRACK:
      w = new HTrackView(viewID, parent);
      break;
    case VIEW_CORRELATION:
      w = new CorrelationView(viewID, parent);
      break;
    case VIEW_FFT:
      w = new FFTView(viewID, parent);
      break;
    case VIEW_CEPSTRUM:
      w = new CepstrumView(viewID, parent);
      break;
    case VIEW_DEBUGVIEW:
      w = new DebugView(viewID, parent);
      break;
    case VIEW_SCORE:
      w = new ScoreView(viewID, parent);
      break;
    case VIEW_VIBRATO:
      w = new VibratoView(viewID, parent);
      break;
    }
  if(w)
    {
      if(parent)
	{
	  theWorkspace->addWindow(w);
	}
      w->show();
    }
  return w;
}

//------------------------------------------------------------------------------
void MainWindow::newViewAboutToShow(void)
{
  newViewMenu->clear();

  QMenu * technicalMenu = new QMenu("Technical");
  QMenu * experimentalMenu = new QMenu("Experimental");
  QMenu * otherMenu = new QMenu("Other");

  QWidgetList opened = theWorkspace->windowList();

  for(int j = 0; j < NUM_VIEWS; j++)
    {
      QAction *action;
      if(viewData[j].menuType == 0)
	{
	  action = newViewMenu->addAction(viewData[j].menuName);
	}
      else if(viewData[j].menuType == 1)
	{
	  action = technicalMenu->addAction(viewData[j].menuName);
	}
      else if(viewData[j].menuType == 2)
	{
	  action = experimentalMenu->addAction(viewData[j].menuName);
	}
      else if(viewData[j].menuType == 3)
	{
	  action = otherMenu->addAction(viewData[j].menuName);
	}
      else
	{
	  continue;
	}
      
      connect(action, SIGNAL(triggered()), createSignalMapper, SLOT(map()));
      createSignalMapper->setMapping(action, j);
      for(QWidgetList::iterator it=opened.begin(); it<opened.end(); it++)
	{
	  if(QString((*it)->metaObject()->className()) == viewData[j].className)
	    {
	      action->setEnabled(false);
	      break;
	    }
	}
    }
  newViewMenu->addSeparator();
  newViewMenu->addMenu(technicalMenu);
  newViewMenu->addMenu(experimentalMenu);
  newViewMenu->addMenu(otherMenu);
}

//------------------------------------------------------------------------------
void MainWindow::setTimeLabel(double t)
{
  char temp[128];
  if(timeLabel)
    {
      int sign = (t < 0) ? -1 : 1;
      t = fabs(t);
      int mins = int(floor(t)) / 60;
      int secs = int(floor(t - double(mins * 60)));
      int milliSecs = int(floor((t - double(mins * 60) - double(secs))*1000.0));
      sprintf(temp, "Time: %s%02d:%02d.%03d", (sign == -1) ? "-" : " ", mins, secs, milliSecs);
      timeLabel->setText(QString(temp));
    }
}

//------------------------------------------------------------------------------
void MainWindow::setChunkLabel(void)
{
  char temp[128];
  Channel *active = gdata->getActiveChannel();
  if(active)
    {
      sprintf(temp, "Chunk: %d", active->currentChunk());
    }
  else
    {
      sprintf(temp, "Chunk: 0");
    }
  chunkLabel->setText(QString(temp));
}

//------------------------------------------------------------------------------
void MainWindow::setNoteLabel(void)
{
  char temp[128];
  Channel *active = gdata->getActiveChannel();
  if(active)
    {
      active->lock();
      AnalysisData *data = active->dataAtCurrentChunk();
      if(data && active->isVisibleNote(data->getNoteIndex()) && active->isLabelNote(data->getNoteIndex()))
	{
	  sprintf(temp, "Note: %d", data->getNoteIndex());
	}
      else
	{
	  sprintf(temp, "Note:    ");
	}
      active->unlock();
    }
  else
    {
      sprintf(temp, "Note:    ");
    }
  noteLabel->setText(QString(temp));
}

//------------------------------------------------------------------------------
void MainWindow::setTimeRange(double min_, double max_)
{
  if(timeSlider)
    {
      timeSlider->setRange(min_, max_, timeSlider->step(), 1000);
    }
}

//------------------------------------------------------------------------------
void MainWindow::rewindPressed(void)
{
  gdata->rewind();
  //every 0.2 seconds
  rewindTimer->start(gdata->fastUpdateSpeed());
}

//------------------------------------------------------------------------------
void MainWindow::rewindReleased(void)
{
  //every 0.2 seconds
  rewindTimer->stop();
}

//------------------------------------------------------------------------------
void MainWindow::playStopClicked(void)
{
  if(gdata->getRunning())
    {
      gdata->stop();
    }
  else
    {
      gdata->play();
    }  
}

//------------------------------------------------------------------------------
void MainWindow::fastforwardPressed(void)
{
  gdata->fastforward();
  //every 0.2 seconds
  fastforwardTimer->start(gdata->fastUpdateSpeed());
}

//------------------------------------------------------------------------------
void MainWindow::fastforwardReleased(void)
{
  //every 0.2 seconds
  fastforwardTimer->stop();
}

//------------------------------------------------------------------------------
void MainWindow::setTitle(Channel *ch)
{
#ifdef MYDEBUG
  if(ch)
    {
      setWindowTitle(QString("Tartini (debug) - ") + ch->getUniqueFilename());
    }
  else
    {
      setWindowTitle(QString("Tartini (debug)"));
    }
#else
  if(ch)
    {
      setWindowTitle(QString("Tartini - ") + ch->getUniqueFilename());
    }
  else
    {
      setWindowTitle(QString("Tartini"));
    }
#endif
}

//------------------------------------------------------------------------------
void MainWindow::aboutTartini(void)
{
  TartiniDialog *tartiniDialog = new TartiniDialog(this);
  tartiniDialog->exec();
  delete tartiniDialog;
}

//------------------------------------------------------------------------------
void MainWindow::aboutGPL(void)
{
  GPLDialog *gplDialog = new GPLDialog(this);
  gplDialog->exec();
  delete gplDialog;
}

//------------------------------------------------------------------------------
void MainWindow::aboutQt(void)
{
  QMessageBox::aboutQt(this, "About Qt");
}

//------------------------------------------------------------------------------
void MainWindow::showDocumentation(void)
{
  QDesktopServices::openUrl(QUrl("http://www.tartini.net/doc"));
}

//------------------------------------------------------------------------------
TartiniDialog::TartiniDialog(QWidget *parent):
  QDialog(parent)
{
  setModal(true);
  setWindowTitle("About Tartini - Version " TARTINI_VERSION_STR);
  setBackgroundRole(QPalette::Shadow);
  QVBoxLayout * layout = new QVBoxLayout(this);
  layout->setContentsMargins(6,6,6,6);
  layout->setSpacing(6);

  QHBoxLayout * topLayout = new QHBoxLayout();
  layout->addLayout(topLayout);
  QLabel * tartiniPicture = new QLabel(this);
  tartiniPicture->setObjectName("TartiniPicture");
  QPixmap tartiniPixmap(":/res/tartinilogo.jpg", "JPG");
  tartiniPicture->setPixmap(tartiniPixmap);
  tartiniPicture->setBackgroundRole(QPalette::Shadow);
  topLayout->addStretch(10);
  topLayout->addWidget(tartiniPicture);
  topLayout->addStretch(10);

  QTextBrowser * tartiniTextEdit = new QTextBrowser(this);
  tartiniTextEdit->setObjectName("TartiniTextEdit");
  tartiniTextEdit->setOpenExternalLinks(true);

  QColor linkColor(Qt::cyan);
  QString sheet = QString::fromLatin1("a { text-decoration: underline; color: %1 }").arg(linkColor.name());
  tartiniTextEdit->document()->setDefaultStyleSheet(sheet);

  tartiniTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  tartiniTextEdit->setHtml(
    "Tartini is a graphical tool for analysing the music of solo instruments.<br>"
    "This program was created by Philip McLeod as part of PhD work at the University of Otago, New Zealand<br>"
    "You can find the latest info about Tartini at <a href=\"http://www.tartini.net\">http://www.tartini.net</a><br><br>"
    "Copyright 2002-2007 Philip McLeod (pmcleod@cs.otago.ac.nz).<br><br>"

    "New features in version 1.2 include:<br>"
    "- The ability to set an offset frequency. i.e. not fixed to A=440 Hz<br>"
    "- Vertical reference lines<br>"
    "- Fixed a bug in note detection which caused a number of pitch detection problems<br><br>"

    "New features in version 1.1 include:<br>"
    "- A vibrato analysis tool<br>"
    "- Full duplex sound, allowing play and record at the same time<br>"
    "- Choice of musical scales<br>"
    "- Some basic printing abilities (pitch view only)<br>"
    "- Some basic exporting abilities (plain text or matlab code)<br>"
    "- A refined pitch algorithm<br>"
    "- A musical score generator (still in very early stages of development)<br><br>"

    "<i>Thanks to:-</i><br>"
    "Prof. Geoff Wyvill - PhD Supervisor (Computer Science)<br>"
    "Dr. Donald Warrington - PhD Supervisor (Physics)<br>"
    "Rob Ebbers - Programming<br>"
    "Maarten van Sambeek - Programming<br>"
    "Stuart Miller - Programming<br>"
    "Mr Kevin Lefohn - Support (Violin)<br>"
    "Miss Judy Bellingham - Support (Voice)<br>"
    "Jean-Philippe Grenier - Logo design<br>"
    "People at my lab, especially Alexis Angelidis, Sui-Ling Ming-Wong, Brendan McCane and Damon Simpson<br><br>"
    "Tartini version " TARTINI_VERSION_STR " is released under the GPL license <a href=\"http://www.gnu.org/licenses/gpl.html\">http://www.gnu.org/licenses/gpl.html</a>.<br><br>"

    "<i>Libraries used by tartini include:</i><br>"
    "Qt - <a href=\"http://www.trolltech.com/qt\">http://www.trolltech.com/qt</a><br>"
    "FFTW - <a href=\"http://www.fftw.org\">http://www.fftw.org</a><br>"
    "Qwt - <a href=\"http://qwt.sourceforge.net\">http://qwt.sourceforge.net</a><br>"
    "RtAudio - <a href=\"http://www.music.mcgill.ca/~gary/rtaudio/index.html\">http://www.music.mcgill.ca/~gary/rtaudio/index.html</a><br>"
    "<br>"
    "This program is free software; you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation; either version 2 of the License, or "
    "(at your option) any later version.<br><br>"
    "This program is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
    "GNU General Public License for more details.<br><br>"
    "You should have received a copy of the GNU General Public License "
    "along with this program; if not, write to the Free Software "
    "Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.<br><br>"
    "Please click 'Read License' or see LICENSE.txt for details.<br>");

  QPalette pal;
  pal = palette();
  pal.setColor(QPalette::Base, Qt::black);
  pal.setColor(QPalette::Text, Qt::white);
  tartiniTextEdit->setPalette(pal);
  
  layout->addWidget(tartiniTextEdit);
  
  QHBoxLayout * bottomLayout = new QHBoxLayout();
  layout->addLayout(bottomLayout);
  QPushButton * okButton = new QPushButton("&Ok", this);
  QPushButton * GPLButton = new QPushButton("&Read License", this);
  bottomLayout->addStretch(10);
  bottomLayout->addWidget(okButton);
  bottomLayout->addStretch(10);
  bottomLayout->addWidget(GPLButton);
  bottomLayout->addStretch(10);
  setFocusProxy(okButton);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(GPLButton, SIGNAL(clicked()), mainWindow, SLOT(aboutGPL()));
}

//------------------------------------------------------------------------------
GPLDialog::GPLDialog(QWidget *parent):
  QDialog(parent)
{
  setModal(true);
  setWindowTitle("GPL Licence");
  QVBoxLayout * layout = new QVBoxLayout(this);
  layout->setContentsMargins(6,6,6,6);
  layout->setSpacing(6);
  QTextEdit * GPLTextEdit = new QTextEdit(this);
  GPLTextEdit->setObjectName("GPLTextEdit");
  GPLTextEdit->setReadOnly(true);
  GPLTextEdit->setAcceptRichText(false);
  QString theText;
  {
    QFile theFile(":res/LICENSE.txt");
    theFile.open(QIODevice::ReadOnly);
    QTextStream textStream(&theFile);
    theText = textStream.readAll();
    theFile.close();
  }
  GPLTextEdit->setText(theText);
  
  layout->addWidget(GPLTextEdit);
  
  QHBoxLayout * bottomLayout = new QHBoxLayout();
  layout->addLayout(bottomLayout);
  QPushButton * okButton = new QPushButton("&Ok", this);
  bottomLayout->addStretch(10);
  bottomLayout->addWidget(okButton);
  bottomLayout->addStretch(10);
  setFocusProxy(okButton);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

#include "freqdrawwidget.h"
#include "drawwidget.h"

//------------------------------------------------------------------------------
void MainWindow::printPitch(void)
{
  QPrinter printer(QPrinter::HighResolution);
  printer.setOrientation(QPrinter::Landscape);
  QPrintDialog printDialog(&printer, this);
  if(printDialog.exec() == QDialog::Accepted)
    {
      View & view = gdata->getView();
      QPainter p;
      p.begin(&printer);
      int w = printer.width();
      int h = printer.height();
      double leftTime = view.viewLeft();
      double rightTime = view.viewRight();
      double viewBottom = view.viewBottom();
      double viewTop = view.viewTop();
      double zoomX = (rightTime-leftTime) / double(w);
      double zoomY = (viewTop-viewBottom) / double(h);
      FreqDrawWidget::drawReferenceLines(printer, p, 0.0, zoomX, viewBottom, zoomY, DRAW_VIEW_PRINT);

      double dotsPerLineStepY = 1.0 / zoomY;
      double dotsPerMM = double(printer.height()) / double(printer.heightMM());
      //1mm thick line
      DrawWidget::setLineWidth(toInt(std::min(dotsPerMM * 1.0, dotsPerLineStepY * 0.2)));
      //draw all the visible channels
      for (uint i = 0; i < gdata->getChannelsSize(); i++)
	{
	  Channel *ch = gdata->getChannelAt(i);
	  if(!ch->isVisible())
	    {
	      continue;
	    }
	  DrawWidget::drawChannel(printer, ch, p, leftTime, 0.0, zoomX, viewBottom, zoomY, DRAW_VIEW_PRINT);
	  if(ch == gdata->getActiveChannel())
	    {
	      p.setPen(Qt::black);
	      QString s = ch->getUniqueFilename();
	      QFont f = p.font();
	      f.setPointSize(18);
	      p.setFont(f);
	      p.drawText(QRect(0, 0, printer.width(), printer.height()), Qt::AlignHCenter | Qt::AlignTop, s);
	    }
	}
      p.end();
    }
}

//------------------------------------------------------------------------------
void MainWindow::exportChannel(int type, QString typeString)
{
  Channel *ch = gdata->getActiveChannel();
  if(ch)
    {
      ch->exportChannel(type, typeString);
    }
  else
    {
      fprintf(stderr, "No channel to export.\n");
    }
}

//------------------------------------------------------------------------------
void MainWindow::exportChannelPlainText(void)
{
  exportChannel(0, "Text (*.txt)");
}

//------------------------------------------------------------------------------
void MainWindow::exportChannelMatlab(void)
{
  exportChannel(1, "Matlab code (*.m)");
}

//------------------------------------------------------------------------------
bool MainWindow::loadViewGeometry(void)
{
  QPoint pos;
  QSize size;
  int counter = 0;

  for(int j = 0; j < NUM_VIEWS; j++)
    {
      QString base = QString("geometry/") + viewData[j].className;
      QString l_key = base+"/visible";
      if(gdata->settingsContains(l_key) && gdata->getSettingsValue(l_key, false) == true)
	{
	  pos = gdata->getSettingsValue(base+"/pos", QPoint(0, 0));
	  size = gdata->getSettingsValue(base+"/size", QSize(100, 100));
	  QWidget * w = openView(j);
	  //get the subwindow frame
	  QWidget * p = (QWidget*)(w->parent());
      if(p)
	{
	  p->resize(size);
	  p->move(pos);
	}
      counter++;
	}
    }
  if(counter == 0)
    {
      openView(VIEW_OPEN_FILES);
      openView(VIEW_FREQ);
      openView(VIEW_TUNER);
      openView(VIEW_VIBRATO);
      return false;
    }
  return true;
}

//------------------------------------------------------------------------------
void MainWindow::saveViewGeometry(void)
{
  QWidgetList opened = theWorkspace->windowList();

  for(int j = 0; j < NUM_VIEWS; j++)
    {
      QString base = QString("geometry/") + viewData[j].className;
      bool found = false;
      for(QWidgetList::iterator it = opened.begin(); it < opened.end(); it++)
	{
	  if(QString((*it)->metaObject()->className()) == viewData[j].className)
	    {
	      //get the subwindow frame
	      QWidget * p = (QWidget*)((*it)->parent());
	      if(!p)
		{
		  break;
		}
	      gdata->setSettingsValue(base + "/visible", true);
	      gdata->setSettingsValue(base + "/pos", p->pos());
	      gdata->setSettingsValue(base + "/size", p->size());
	      found = true;
	      break;
	    }
	}
      if(!found)
	{
	  gdata->setSettingsValue(base+"/visible", false);
	}
    }
}

//------------------------------------------------------------------------------
QSize TartiniDialog::sizeHint(void) const
{
  return QSize(600, 600);
}

//------------------------------------------------------------------------------
QSize GPLDialog::sizeHint(void) const
{
  return QSize(600, 480);
}
// EOF
