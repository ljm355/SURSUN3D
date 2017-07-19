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
#include "GeometryWrapper.h"
#include "Utils.h"
#include "osg\Node"
#include "osgDB\ReadFile"
#include "osgViewer\ViewerEventHandlers"
#include "osgEarthUtil\EarthManipulator"
#include "osgGA\StateSetManipulator"
#include "osg\Switch"
#include "osgGA\TrackballManipulator"
#include "TextureBaker.h"
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
#include <osgEarthDrivers/cache_filesystem/FileSystemCache>
#include <osgEarthUtil/ObjectLocator>
#include "osgDB/WriteFile"
#include "osgEarthDrivers/arcgis/ArcGISOptions"
using namespace osgEarth;
using namespace osgEarth::Drivers;
using namespace osgEarth::Util;



osgEarth::MapNode* createMapNode()
{
	// create the map.
	Map* map = new Map();

	// add a TMS imager layer:
	//TMSOptions imagery;
	//imagery.url() = "http://readymaps.org/readymap/tiles/1.0.0/7/";
	//map->addImageLayer( new ImageLayer("Imagery", imagery) );

	//ArcGISOptions imagery;
	//imagery.url() = "http://services.arcgisonline.com/ArcGIS/rest/services/World_Street_Map/MapServer";
	//map->addImageLayer( new ImageLayer("Imagery", imagery) );

	ArcGISOptions imagery;
	imagery.url() = "http://services.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer";
	map->addImageLayer( new ImageLayer("Imagery", imagery) );

		
	// add a TMS elevation layer:
	TMSOptions elevation;
	elevation.url() = "http://readymap.org/readymap/tiles/1.0.0/9/";
	map->addElevationLayer( new ElevationLayer("Elevation", elevation) );
	// make the map scene graph:
	MapNode* node = new MapNode( map );

   return node;
}

int main(int argc, char** argv)
{

    osgViewer::Viewer viewer;
	EarthManipulator* manip = new EarthManipulator;

	//viewer.getCamera()->setNearFarRatio(0.00002);
    viewer.getDatabasePager()->setDoPreCompile( true );
	viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgViewer::WindowSizeHandler());
	viewer.addEventHandler(new osgViewer::ThreadingHandler());
	viewer.addEventHandler(new osgViewer::LODScaleHandler());
	viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));

	viewer.getCamera()->setNearFarRatio(0.00002);
	viewer.getCamera()->setSmallFeatureCullingPixelSize(-1.0f);
    viewer.setCameraManipulator(manip);
	//osg::ref_ptr<osg::Node> node = osgDB::readNodeFile( "./SURSUN3D.earth" );//load MapNode from config file
	osg::ref_ptr<osg::Node> node = createMapNode();//create MapNode using API

	osgEarth::MapNode* mapNode = osgEarth::MapNode::findMapNode(node);
	osgEarth::Map* map = mapNode->getMap();
	osgEarth::ElevationQuery*  elevationQuery = new osgEarth::ElevationQuery(map);
	osgEarth::GeoPoint geop(map->getSRS(), 91.1000,29.6500);
	double elevation = 0;
	//elevationQuery->getElevation(geop,elevation,0.0008333);
	//geop = osgEarth::GeoPoint(map->getSRS(),80,10);
	//elevationQuery->getElevation(geop,elevation,0.0008333);
	//osg::ref_ptr<osg::Node> oldNode = osgDB::readNodeFile("D:/Projects/Solar_Radiation_Model/data/shadow/shadow.osg");
	
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

	//read SRTM data from tms service at http://readymap.org/readymap/tiles/1.0.0/9/
	//internet connection is required
	elevationQuery->getElevation(osgEarth::GeoPoint(map->getSRS(),params.lon,params.lat),elevation,0.0008333);
	params.elev = elevation;
	rad = calculateSolarRadiation(params);
    osg::ref_ptr<osg::Node> oldNode = osgDB::readNodeFile("./scene01/scene.osg"); 

	TextureBaker baker;
	baker.setBakeSlot(0);
	baker.setDiffusSlot(-1);
	baker.loadFromOSGNode(oldNode);
	//osg::MatrixTransform* mat = new osg::MatrixTransform;
	//osg::Matrix mx = osg::Matrix::identity();
	//osg::Vec3d center = baker.Extent().center();
	//center.z() = 0;
	//mx *= osg::Matrix::translate(-center);
	//mx *= osg::Matrix::scale(0.1,0.1,0.1);
	//mat->addChild(oldNode.get());
	//mat->setMatrix(mx);
	//osgDB::writeNodeFile(*mat,"./scene01/scene.osg");
	//return 0;

	//osgDB::writeNodeFile(*oldNode,"scene.osg");
	//baker.bakeTexture("./out_texture_dir",params,1,1,0.01);
	baker.symbolize("./out_texture_dir");
	osg::ref_ptr<osg::Node> newNode = baker.toOSG("./out_texture_dir");
	osgEarth::Util::ObjectLocator locator(mapNode->getMap());
	locator.setPosition(osg::Vec3d(params.lon,params.lat,elevation + 1));
	osg::ref_ptr<osg::MatrixTransform> transNode = new osg::MatrixTransform;
	osg::Matrix matrix;
	locator.getLocatorMatrix(matrix);
	transNode->setMatrix(matrix);
	transNode->addChild(newNode.get());

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(node.get());
	root->addChild(transNode.get());
	viewer.setSceneData( root.get() );
	viewer.setUpViewInWindow(100,100,515,512);
	double heading = 0;
	double pitch = -89;
	double range = 500;
	osgEarth::Viewpoint vp(osg::Vec3d(params.lon, params.lat, 0),heading,pitch,range);

	//manip->setHomeViewpoint(vp, 0.0);
    manip->setViewpoint( vp, 0.0 );
	return viewer.run();
}
