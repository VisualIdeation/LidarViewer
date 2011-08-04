/***********************************************************************
LidarExporter - Utility to export points from LiDAR files to ASCII files
in xyzrgb format.
Copyright (c) 2010 Oliver Kreylos

This file is part of the LiDAR processing and analysis package.

The LiDAR processing and analysis package is free software; you can
redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

The LiDAR processing and analysis package is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the LiDAR processing and analysis package; if not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA
***********************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <Misc/File.h>

#include "LidarTypes.h"
#include "LidarProcessOctree.h"

class PointSaver
	{
	/* Elements: */
	private:
	Misc::File resultFile;
	size_t numPoints;
	
	/* Constructors and destructors: */
	public:
	PointSaver(const char* resultFileName)
		:resultFile(resultFileName,"wb"),
		 numPoints(0)
		{
		}
	
	/* Methods: */
	void operator()(const LidarPoint& point)
		{
		fprintf(resultFile.getFilePtr(),"%.6f %.6f %.6f %03d %03d %03d\n",point[0],point[1],point[2],point.value[0],point.value[1],point.value[2]);
		++numPoints;
		}
	size_t getNumPoints(void) const
		{
		return numPoints;
		}
	};

int main(int argc,char* argv[])
	{
	/* Process command line: */
	const char* lidarFile=0;
	const char* asciiFile=0;
	int cacheSize=512;
	Box box=Box::full;
	for(int i=1;i<argc;++i)
		{
		if(argv[i][0]=='-')
			{
			if(strcasecmp(argv[i]+1,"cache")==0)
				{
				++i;
				cacheSize=atoi(argv[i]);
				}
			else if(strcasecmp(argv[i]+1,"box")==0)
				{
				Box::Point min,max;
				for(int j=0;j<3;++j)
					{
					++i;
					min[j]=Box::Scalar(atof(argv[i]));
					}
				for(int j=0;j<3;++j)
					{
					++i;
					max[j]=Box::Scalar(atof(argv[i]));
					}
				box=Box(min,max);
				}
			else
				std::cerr<<"Ignoring command line option "<<argv[i]<<std::endl;
			}
		else if(lidarFile==0)
			lidarFile=argv[i];
		else if(asciiFile==0)
			asciiFile=argv[i];
		else
			std::cerr<<"Ignoring command line argument "<<argv[i]<<std::endl;
		}
	if(lidarFile==0)
		{
		std::cerr<<"No input LiDAR file name provided"<<std::endl;
		return 1;
		}
	if(asciiFile==0)
		{
		std::cerr<<"No output ASCII file name provided"<<std::endl;
		return 1;
		}
	
	/* Open the input and output files: */
	LidarProcessOctree lpo(lidarFile,size_t(cacheSize)*1024*1024);
	PointSaver ps(asciiFile);
	
	/* Process the LiDAR file: */
	lpo.processPointsInBox(box,ps);
	
	/* Print statistics: */
	std::cout<<ps.getNumPoints()<<" points saved"<<std::endl;
	return 0;
	}
