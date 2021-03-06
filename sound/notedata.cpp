/***************************************************************************
                          notedata.cpp  -  description
                             -------------------
    begin                : Mon May 23 2005
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

#include <vector>

#include "notedata.h"
#include "gdata.h"
#include "channel.h"
#include "useful.h"
#include "musicnotes.h"

//------------------------------------------------------------------------------
NoteData::NoteData(const Channel & p_channel):
  maxima(new Array1d<int>()),
  minima(new Array1d<int>()),
  channel(&p_channel),
  maxLogRMS(gdata->dBFloor()),
  _numPeriods(0),
  _periodOctaveEstimate(1.0f)
{
}

//------------------------------------------------------------------------------
NoteData::NoteData(const Channel & p_channel, int startChunk_, const AnalysisData & p_analysis_data):
  nsdfAggregateRoof(0.0),
  firstNsdfPeriod(0.0f),
  currentNsdfPeriod(0.0f),
  maxima(new Array1d<int>()),
  minima(new Array1d<int>()),
  channel(&p_channel),
  _startChunk(startChunk_),
  _endChunk(startChunk_ + 1),
  maxLogRMS(p_analysis_data.getLogRms()),
  maxIntensityDB(p_analysis_data.getMaxIntensityDB()),
  maxCorrelation(p_analysis_data.getCorrelation()),
  maxPurity(p_analysis_data.getVolumeValue(*gdata)),
  _numPeriods(0.0f), //periods;
  _periodOctaveEstimate(1.0f),
  _volume(0.0f),
  _avgPitch(0.0f),
  loopStep(channel->rate() / 1000),  // stepsize = 0.001 seconds
  loopStart(_startChunk * channel->framesPerChunk() + loopStep),
  prevExtremumTime(-1),
  prevExtremumPitch(-1),
  prevExtremum(NONE)
{
  nsdfAggregateData.resize(p_channel.get_nsdf_data().size(), 0.0f);
  nsdfAggregateDataScaled.resize(p_channel.get_nsdf_data().size(), 0.0f);
}

//------------------------------------------------------------------------------
NoteData::~NoteData(void)
{
}

//------------------------------------------------------------------------------
void NoteData::resetData(void)
{
  _numPeriods = 0;
}

//------------------------------------------------------------------------------
void NoteData::addData(const AnalysisData & p_analysis_data, float periods)
{
  maxLogRMS = MAX(maxLogRMS, p_analysis_data.getLogRms());
  maxIntensityDB = MAX(maxIntensityDB, p_analysis_data.getMaxIntensityDB());
  maxCorrelation = MAX(maxCorrelation, p_analysis_data.getCorrelation());
  maxPurity = MAX(maxPurity, p_analysis_data.getVolumeValue(*gdata));
  _volume = MAX(_volume, dB2Normalised(p_analysis_data.getLogRms(),*gdata));
  _numPeriods += periods; //sum up the periods
  _avgPitch = bound(freq2pitch(avgFreq()), 0.0, gdata->topPitch());
}

//------------------------------------------------------------------------------
double NoteData::noteLength(void) const
{
  return double(numChunks()) * double(channel->framesPerChunk()) / double(channel->rate());
}

//------------------------------------------------------------------------------
float NoteData::avgPitch(void) const
{
  return _avgPitch;
}

//------------------------------------------------------------------------------
void NoteData::addVibratoData(int chunk)
{
  if ((channel->doingDetailedPitch()) && (channel->get_pitch_lookup_smoothed().size() > 0))
    {
      // Detailed pitch information available, calculate maxima and minima
      int loopLimit = ((chunk + 1) * channel->framesPerChunk()) - loopStep;
      for (int currentTime = loopStart; currentTime < loopLimit; currentTime += loopStep)
	{
	  myassert(currentTime + loopStep < (int)channel->get_pitch_lookup_smoothed().size());
	  myassert(currentTime - loopStep >= 0);
	  float prevPitch = channel->get_pitch_lookup_smoothed().at(currentTime - loopStep);
	  float currentPitch = channel->get_pitch_lookup_smoothed().at(currentTime);
	  float nextPitch = channel->get_pitch_lookup_smoothed().at(currentTime + loopStep);

	  if ((prevPitch < currentPitch) && (currentPitch >= nextPitch))
	    {
	      // Maximum
	      if (prevExtremum == NONE)
		{
		  maxima->push_back(currentTime);
		  prevExtremumTime = currentTime;
		  prevExtremumPitch = currentPitch;
		  prevExtremum = FIRST_MAXIMUM;
		  continue;
		}
	      if ((prevExtremum == FIRST_MAXIMUM) || (prevExtremum == MAXIMUM))
		{
		  if (currentPitch >= prevExtremumPitch)
		    {
		      myassert(!maxima->isEmpty());
		      maxima->pop_back();
		      maxima->push_back(currentTime);
		      prevExtremumTime = currentTime;
		      prevExtremumPitch = currentPitch;
		    }
		  continue;
		}
	      if ((fabs(currentPitch - prevExtremumPitch) > 0.04) &&
		  (currentTime - prevExtremumTime > 42 * loopStep))
		{
		  if (prevExtremum == MINIMUM)
		    {
		      maxima->push_back(currentTime);
		      prevExtremumTime = currentTime;
		      prevExtremumPitch = currentPitch;
		      prevExtremum = MAXIMUM;
		      continue;
		    }
		  else
		    {
		      if (currentTime - minima->at(0) < loopStep * 500)
			{
			  // Good
			  maxima->push_back(currentTime);
			  prevExtremumTime = currentTime;
			  prevExtremumPitch = currentPitch;
			  prevExtremum = MAXIMUM;
			  continue;
			}
		      else
			{
			  // Not good
			  myassert(!minima->isEmpty());
			  minima->pop_back();
			  maxima->push_back(currentTime);
			  prevExtremumTime = currentTime;
			  prevExtremumPitch = currentPitch;
			  prevExtremum = FIRST_MAXIMUM;
			  continue;
			}
		    }
		}
	    }
	  else if ((prevPitch > currentPitch) && (currentPitch <= nextPitch))
	    {
	      // Minimum
	      if (prevExtremum == NONE)
		{
		  minima->push_back(currentTime);
		  prevExtremumTime = currentTime;
		  prevExtremumPitch = currentPitch;
		  prevExtremum = FIRST_MINIMUM;
		  continue;
		}
	      if ((prevExtremum == FIRST_MINIMUM) || (prevExtremum == MINIMUM))
		{
		  if (currentPitch <= prevExtremumPitch)
		    {
		      myassert(!minima->isEmpty());
		      minima->pop_back();
		      minima->push_back(currentTime);
		      prevExtremumTime = currentTime;
		      prevExtremumPitch = currentPitch;
		    }
		  continue;
		}
	      if ((fabs(currentPitch - prevExtremumPitch) > 0.04) &&
		  (currentTime - prevExtremumTime > 42 * loopStep))
		{
		  if (prevExtremum == MAXIMUM)
		    {
		      minima->push_back(currentTime);
		      prevExtremumTime = currentTime;
		      prevExtremumPitch = currentPitch;
		      prevExtremum = MINIMUM;
		      continue;
		    }
		  else
		    {
		      if (currentTime - maxima->at(0) < loopStep * 500)
			{
			  // Good
			  minima->push_back(currentTime);
			  prevExtremumTime = currentTime;
			  prevExtremumPitch = currentPitch;
			  prevExtremum = MINIMUM;
			  continue;
			}
		      else
			{
			  // Not good
			  myassert(!maxima->isEmpty());
			  maxima->pop_back();
			  minima->push_back(currentTime);
			  prevExtremumTime = currentTime;
			  prevExtremumPitch = currentPitch;
			  prevExtremum = FIRST_MINIMUM;
			  continue;
			}
		    }
		}
	    }
	}
      // Calculate start of next loop
      loopStart = loopStart + ((loopLimit - loopStart) / loopStep) * loopStep + loopStep;
    }
}

//------------------------------------------------------------------------------
void NoteData::recalcAvgPitch(void)
{
  _numPeriods = 0.0f;
  for(int j = startChunk(); j < endChunk(); j++)
    {
      _numPeriods += float(channel->framesPerChunk()) / float(channel->dataAtChunk(j)->getPeriod());
    }
  _avgPitch = bound(freq2pitch(avgFreq()), 0.0, gdata->topPitch());
}

// EOF
