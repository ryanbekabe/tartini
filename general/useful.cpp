/***************************************************************************
                          useful.cpp  -  Generic useful functions
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
//#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "useful.h"
#include <stdio.h>
#include <errno.h>
#include "myassert.h"

//------------------------------------------------------------------------------
//return a pointer to a 2d array with each element of size size
void **malloc2d(const int row, const int col, const int size)
{
  void **ptr = (void **)malloc(sizeof(void *) * row);
  for(int j=0; j<row; j++)
    {
      ptr[j] = (void *)malloc(size * col);
    }
  return ptr;
}

//------------------------------------------------------------------------------
//like malloc2d but changes an existing 2d array
void **realloc2d(void **ptr, const int row, const int col, const int old_row, const int old_col, const int size)
{
  if(ptr)
    {
      int j;
      //shrink
      if(row < old_row)
	{
	  for(j=row; j<old_row; j++)
	    {
	      free(ptr[j]);
	    }
	  ptr = (void **)realloc(ptr, sizeof(void *) * row);
	  if(col != old_col)
	    {
	      for(j=0; j<row; j++)
		{
		  ptr[j] = (void *)realloc(ptr[j], size * col);
		}
	    }
	}
      //grow
      else if(row > old_row)
	{
	  ptr = (void **)realloc(ptr, sizeof(void *) * row);
	  if(col != old_col)
	    {
	      for(j=0; j<old_row; j++)
		{
		  ptr[j] = (void *)realloc(ptr[j], size * col);
		}
	    }
	  for(j=old_row; j<row; j++)
	    {
	      ptr[j] = (void *)malloc(size * col);
	    }
	}
      else
	{ //same
	  if(col != old_col)
	    {
	      for(j=0; j<row; j++)
		{
		  ptr[j] = (void *)realloc(ptr[j], size * col);
		}
	    }
	}
      return ptr;
    }
  else
    {
      return malloc2d(row, col, size);
    }
}

//------------------------------------------------------------------------------
void free2d(void **ptr, const int row)
{
  for(int j=0; j<row; j++)
    {
      free(ptr[j]);
    }
  free(ptr);
}

//------------------------------------------------------------------------------
double powi(const double x, const int y)
{
    if(y > 1)
      {
	if(y & 0x01)
	  {
	    return x * powi(x * x, y >> 1);
	  }
	else
	  {
	    return powi(x * x, y >> 1);
	  }
      }
    else if(y == 1)
      {
	return x;
      }
    else if(y == 0)
      {
	return 1;
      }
    else 
      {
	return 1.0 / powi(x, -y);
      }
}

//------------------------------------------------------------------------------
double nearestPowerOf2(double x)
{
  return pow2(myround(log2(x)));
}

//------------------------------------------------------------------------------
//sets max_pos to the position with the max value in the array
//sets max_value to the max value in the array
//max_pos and/or max_value can be NULL
void max_array(int n, float *data, int *max_pos, float *max_value)
{
  int pos = 0;
  float value = data[0];
  for(int j=1; j<n; j++)
    {
      if(data[j] > value)
	{
	  value=data[j];
	  pos=j;
	}
    }
  if(max_pos)
    {
      *max_pos = pos;
    }
  if(max_value)
    {
      *max_value = value;
    }
}

//------------------------------------------------------------------------------
MinMax minMax(float *begin, float *end)
{
  if (begin == end)
    {
      return MinMax();
    }
  
  MinMax toReturn(*begin, *begin);

  for (float *p = begin + 1; p < end; p++)
    {
      toReturn.min = MIN(toReturn.min, *p);
      toReturn.max = MAX(toReturn.max, *p);
    }
  return toReturn;
}

//------------------------------------------------------------------------------
MinMax minMax(float *begin, float *end, float lowBound, float highBound)
{
  if (begin == end)
    {
      return MinMax();
    }

  float def = (*begin > lowBound && *begin < highBound) ? *begin : 0;
  MinMax toReturn(def, def);

  for (float *p = begin + 1; p < end; p++)
    {
      if (*p >= lowBound && *p <= highBound)
	{
	  toReturn.min = MIN(toReturn.min, *p);
	  toReturn.max = MAX(toReturn.max, *p);
	}
    }
  return toReturn;
}

//------------------------------------------------------------------------------
float average(float *begin, float *end)
{
  if (begin == end)
    {
      return 0;
    }

  float total = *begin;

  int count = 1;
  for (float *p = begin + 1; p < end; p++, count++)
    {
      total += *p;
    }
  return (total / count);
}

//------------------------------------------------------------------------------
float average(float *begin, float *end, float lowBound, float highBound)
{
  if (begin == end)
    {
      return *begin;
    }

  float total = (*begin > lowBound && *begin < highBound) ? *begin : 0;

  int count = 1;
  for (float *p = begin + 1; p < end; p++, count++)
    {
      if (*p >= lowBound && *p <= highBound)
	{
	  total += *p;
	}
    }
  return (total / count);
}

//------------------------------------------------------------------------------
int calcIndex(const double & frameTime, const double & baseX, int size)
{
  int toReturn = (int) myround((floor(frameTime / baseX) * baseX));
  if (toReturn < 0)
    {
      return 0;
    }
  if (toReturn > size)
    {
      return size;
    }
  return toReturn;
}

//------------------------------------------------------------------------------
bool copyFile(const char *src, const char *dest)
{
  char buffer[4096];
  size_t bytesRead;
  
  FILE *inputFile = fopen(src, "rb");
  if(!inputFile)
    {
      fprintf(stderr, "copyFile: Error opening %s for reading\n", src);
      return false;
    }
  
  FILE *outputFile = fopen(dest, "wb");
  if(!outputFile)
    {
      fclose(inputFile);
      fprintf(stderr, "copyFile: Error opening %s for writing\n", dest);
      return false;
  }
  
  while((bytesRead = fread(buffer, 1, 4096, inputFile)) != 0)
    {
      if(fwrite(buffer, 1, bytesRead, outputFile) < bytesRead)
	{
	  fprintf(stderr, "copyFile: Error writing to %s. Possibly out of disk\n", dest);
	  fclose(inputFile);
	  fclose(outputFile);
	  return false;
	}
    }

  fclose(inputFile);
  fclose(outputFile);
  return true; //success
}

//------------------------------------------------------------------------------
bool moveFile(const char *src, const char *dest)
{
  if(rename(src, dest) == 0)
    {
      return true;
    }
  if(errno == EXDEV)
    { //on different file system
      if(copyFile(src, dest))
	{
	  if(remove(src))
	    {
	      fprintf(stderr, "moveFile: Copy to %s Successful. Error removing old file %s\n", dest, src);
	      return false;
	    }
	  else
	    {
	      return true; //success
	    }
	}
    }
  else
    {
      fprintf(stderr, "moveFile: Error moving '%s' to '%s'\n", src, dest);
    }
  return false;
}

//------------------------------------------------------------------------------
int nextPowerOf2(int x)
{
  myassert(x > 1<<31);
  int y = 1;
  while(y < x)
    {
      y <<= 1;
    }
  return y;
}

//EOF
