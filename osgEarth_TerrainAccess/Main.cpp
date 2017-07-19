/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
* Copyright 2008-2012 Pelican Mapping
* http://osgearth.org
*
* osgEarth is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

//------------------------------------------------------------------
#include "osg\Node"
#include "osgDB\ReadFile"
#include <osgEarth\MapNode>
#include <osgEarth\ElevationQuery>
#include <osgGA/GUIEventHandler>
#include <osgGA/StateSetManipulator>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgEarth/MapNode>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/Controls>
#include <osgEarthSymbology/Color>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/gdal/GDALOptions>
#include "GrassSolar.h"
using namespace osgEarth;
using namespace osgEarth::Drivers;
using namespace osgEarth::Util;



osgEarth::MapNode* createMapNode()
{
	// create the map.
	Map* map = new Map();

	// add a TMS imager layer:
	TMSOptions imagery;
	imagery.url() = "http://readymaps.org/readymap/tiles/1.0.0/7/";
	map->addImageLayer( new ImageLayer("Imagery", imagery) );

	// add a TMS elevation layer:
	//read SRTM data from tms service at http://readymap.org/readymap/tiles/1.0.0/9/
	//internet connection required
	TMSOptions elevation;
	elevation.url() = "http://readymap.org/readymap/tiles/1.0.0/9/";
	map->addElevationLayer( new ElevationLayer("Elevation", elevation) );

	//load multiple .tif DEM files from a directory
	GDALOptions gdal;
	gdal.extensions() = "tif";
	gdal.url() = "./data/terrain";

	////load single DEM files
	//GDALOptions gdal;
	//gdal.url() = "./data/terrain/mt_everest_90m.tif";
	
	// make the map scene graph:
	MapNode* node = new MapNode( map );

	return node;
}

int main(int argc, char** argv)
{


	osgEarth::MapNode* mapNode = createMapNode();
	osgEarth::Map* map = mapNode->getMap();
	osgEarth::ElevationQuery*  elevationQuery = new osgEarth::ElevationQuery(map);
	osgEarth::GeoPoint geop(map->getSRS(), 91.1000,29.6500);
	double elevation = 0;

	SolarParam params;
	params.time_step = 1;
	params.lon = 110;
	params.lat = 30;
	params.linke = 3;
	params.aspect = 0;
	params.slope = 0;
	params.isShadowed = false;
	params.linke = 10;
	SolarRadiation rad = calculateSolarRadiation(params);

	//get elevation at a single point
	elevationQuery->getElevation(osgEarth::GeoPoint(map->getSRS(),params.lon,params.lat),elevation,0.0008333);
	params.elev = elevation;
	rad = calculateSolarRadiation(params);

	//get an array of elevation
	double north = 53.55;
	double west = 73.66;
	double resolution = 0.8333;
	int nrows = 0;
	int ncols = 0;
	double south = north;
	double east = west;
	std::vector<osg::Vec3d> ele_points;
	//spatial extent for Mainland China, latitude 3.86~53.55£¬longitude 73.66~135.05
	while(south > 18)
	{
		ncols = 0;
		while(east < 135.05)
		{
			ele_points.push_back(osg::Vec3d(east,south,0));
	        ncols++;
			east+=resolution;
		}
		east = west;
		nrows++;
		south-=resolution;
	}
	std::vector<double> elevations;
	
	elevationQuery->getElevations(ele_points,map->getSRS(),elevations,resolution);

	return 0;
}
