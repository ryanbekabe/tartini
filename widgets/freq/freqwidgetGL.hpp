/*    This file is part of Tartini
      Copyright (C) 2016  Julien Thevenon ( julien_thevenon at yahoo.fr )
      Copyright (C) 2004 by Philip McLeod ( pmcleod@cs.otago.ac.nz )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

//------------------------------------------------------------------------------
double FreqWidgetGL::leftTime(void) const
{
  return gdata->getView().viewLeft();
}

//------------------------------------------------------------------------------
double FreqWidgetGL::rightTime(void) const
{
  return gdata->getView().viewRight();
}

//------------------------------------------------------------------------------
double FreqWidgetGL::timeWidth(void) const
{
  return gdata->getView().viewTotalTime();
}

//------------------------------------------------------------------------------
QSize FreqWidgetGL::sizeHint(void) const
{
  return QSize(400, 350);
}
//EOF
