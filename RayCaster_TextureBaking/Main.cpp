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

int
main(int argc, char** argv)
{

    osgViewer::Viewer viewer;
    viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	//viewer.getCamera()->setNearFarRatio(0.00002);
    viewer.getDatabasePager()->setDoPreCompile( true );
	viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgViewer::WindowSizeHandler());
	viewer.addEventHandler(new osgViewer::ThreadingHandler());
	viewer.addEventHandler(new osgViewer::LODScaleHandler());
	viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));
	//osg::ref_ptr<osg::Node> oldNode = osgDB::readNodeFile("D:/Projects/Solar_Radiation_Model/data/shadow/shadow.osg");
	osg::ref_ptr<osg::Node> oldNode = osgDB::readNodeFile("./scene01/scene01.osg");
	
	TextureBaker baker;
	baker.setBakeSlot(0);
	baker.setDiffusSlot(-1);
    baker.loadFromOSGNode(oldNode);

	QDir dir("./out_texture_dir");
	if(!dir.exists())
		dir.mkdir(".");
	baker.bakeTexture("./out_texture_dir",osg::Vec3(1,0,0.7));
	osg::ref_ptr<osg::Node> newNode = baker.toOSG("./out_texture_dir");
	viewer.setSceneData( newNode.get() );
	viewer.setUpViewInWindow(100,100,1024,768);
	return viewer.run();
}
