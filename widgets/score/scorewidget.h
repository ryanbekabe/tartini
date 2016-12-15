/***************************************************************************
                          scorewidget.h  -  description
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
#ifndef SCOREWIDGET_H
#define SCOREWIDGET_H

#include "drawwidget.h"
#include <QFont>
#include <QMouseEvent>
#include <QPaintEvent>


class ScoreWidget : public DrawWidget
{
  Q_OBJECT

  enum StemType { StemNone, StemUp, StemDown };
  enum FillType { FilledNote, EmptyNote };
  enum NoteType { DemiSemiQuaver, SemiQuaver, Quaver, Crotchet, Minum, SemiBreve };

 public:
  ScoreWidget(QWidget * parent);
  virtual ~ScoreWidget(void);

  /**
   * Draw a crotchet at x, y
   * @param x The lefthand-side x pixel
   * @param y The center y pixel
   * @param stemUp true if the stem is to be drawn upwards, else its drawn downwards
   */
  void drawNote(int x, int y, StemType stepType, FillType fillType, int numFlicks);

  /**
     Draw a single note
     @param x The x pos in pixels
     @param y The y pos of the center of the scale (Middle C)
     @param pitch The midi number of the note (60 == Middle C)
     @param noteLength in seconds
  */
  void drawNoteAtPitch(int x, int y, int pitch, double noteLength, double volume=1.0);

  /** Draws a segment of the musical score
   * @param ch The channel to get the notes from.
   * @param leftX In pixels (even though a doule)
   * @param lineCenterY In pixels
   * @param leftTime The time is seconds at which is at the lefthand side of the scoreSegment to draw
   */
  void drawScoreSegment(Channel * ch, double leftX, int lineCenterY, double leftTime, double rightTime);

  /** Draw a segment of stave lines
   * @param leftX The pixel at the left of the segment
   * @param lineCenterY The y pixel to put middle C on
   * @param widthX The width of the stave segment
   */
  void drawStaveSegment(int leftX, int lineCenterY, int widthX);
  void paintEvent(QPaintEvent * );
  inline int getStaveHeight(void);
  inline int getStaveCenterY(void);
  inline QSize sizeHint(void) const;
  NoteType getNoteType(double noteLength);

  inline double scaleX(void);
  inline double scaleY(void);

  public slots:
  inline void setScaleX(double x);
  inline void setScaleY(double y);
  inline void setSharpsMode(int mode);
  inline void setNotesMode(int mode);
  inline void setClefMode(int mode);
  inline void setOpaqueMode(int mode);
  inline void setTransposeLevel(int index);
  inline void setShowAllMode(int mode);

 private:
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

  //data goes here
  //num. pixels between lines
  double _scaleY;
  //num. pixels per second
  double _scaleX;
  double _boarderX;
  double _boarderY;
  bool _showBaseClef;
  bool _showNotes;
  bool _showOpaqueNotes;
  bool _useFlats;
  bool _showAllMode;
  int _pitchOffset;
  int sharpsLookup[129];
  int flatsLookup[129];
  int _fontHeight;
  int _fontWidth;
  QFont _font;
  //in fractions of a page
  double _lookAhead;
  //in fractions of a page
  double _lookAheadGap;
  bool _mouseDown;
  friend class ScoreSegmentIterator;
};

#include "scorewidget.hpp"

#endif // SCOREWIDGET_H
// EOF
