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
float HTrackWidget::peakThreshold(void)const
{
  return _peakThreshold;
}

//------------------------------------------------------------------------------
const double & HTrackWidget::viewAngleHorizontal(void)const
{
  return _viewAngleHorizontal;
}

//------------------------------------------------------------------------------
const double & HTrackWidget::viewAngleVertical(void)const
{
  return _viewAngleVertical;
}

//------------------------------------------------------------------------------
void HTrackWidget::setPeakThreshold(float peakThreshold)
{
  _peakThreshold = peakThreshold;
}

//------------------------------------------------------------------------------
void HTrackWidget::setViewAngleHorizontal(const double & angle)
{
  if(angle != _viewAngleHorizontal)
    {
      _viewAngleHorizontal = angle;
      emit viewAngleHorizontalChanged(angle);
    }
}

//------------------------------------------------------------------------------
void HTrackWidget::setViewAngleVertical(const double & angle)
{
  if(angle != _viewAngleVertical)
    {
      _viewAngleVertical = angle;
      emit viewAngleVerticalChanged(angle);
    }
}

//------------------------------------------------------------------------------
void HTrackWidget::setDistanceAway(const double & distance)
{
  if(distance != _distanceAway)
    {
      _distanceAway = distance;
      emit distanceAwayChanged(distance);
    }
}

//EOF
