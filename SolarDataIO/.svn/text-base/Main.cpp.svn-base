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
#include "osg/Node"
#include "osgDB/ReadFile"
#include "osgViewer/ViewerEventHandlers"
#include "osgEarthUtil\EarthManipulator"
#include "osgGA\StateSetManipulator"
#include "osg\Switch"
#include "osgGA\TrackballManipulator"
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
#include <osgEarthDrivers/arcgis/ArcGISOptions>
#include "osgEarthDrivers/feature_ogr/OGRFeatureOptions"
#include "SolarDataIO.h"
#include "osgEarth/Registry"

using namespace osgEarth;
using namespace osgEarth::Drivers;
using namespace osgEarth::Util;

//************************************
// Method:    getSolarFileNames
// FullName:  getSolarFileNames
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: std::string name (prefix)
// Parameter: std::string & solarFile (file that stores radiation data)
// Parameter: std::string & posFile (file that stores 3D position data)
// Parameter: std::string & normalFile (file that stores normal vector data)
//************************************
void getSolarFileNames(std::string name, std::string& solarFile,std::string& posFile,std::string& normalFile)
{
	normalFile = name + "_normal" + ".surf";
	posFile =  name + "_pos" + ".surf";
	solarFile = name+ ".solar";
}


//************************************
// Method:    accessSurface
// FullName:  accessSurface
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: std::string dir
// Parameter: int id (index of the building footprint feature)
// Parameter: osg::Vec3d & translation (a reference position has been subtracted from the stored positions to avoid precision issue)
//************************************
void accessSurface(std::string dir,int id,osg::Vec3d& translation)
{
	std::stringstream name;
	name << dir << id;
	std::string solarFile,posFile,normalFile;
	getSolarFileNames(name.str(),solarFile,posFile,normalFile);

	if(!QFileInfo(solarFile.data()).exists() || !QFileInfo(posFile.data()).exists() || !QFileInfo(normalFile.data()).exists())
	{
		return;
	}
	float* positions;
	float* normals;
	float* solarValues;
	unsigned int width = 0;
	unsigned int height = 0;
	Stats stats[4];
	//radiation data
	SolarDataIO::readSolarFile(solarFile.data(),solarValues,width,height,stats);
	//3D position data
	SolarDataIO::readFloat4Texture(posFile.data(),positions,width,height);
	//normal vector
	SolarDataIO::readFloat4Texture(normalFile.data(),normals,width,height);

	osg::Vec4* pPos= (osg::Vec4*)positions;
	osg::Vec4* pNormal = (osg::Vec4*)normals;
	SolarRadiation* pSolar = (SolarRadiation*)solarValues;
	float minH = 100000;
	float maxH = -minH;
	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			//invalid cell;
			if(pNormal->w() < -1)
			{
				pNormal++;
				pPos++;
				pSolar++;
				continue;
			}
			osg::Vec3d pos = osg::Vec3d(pPos->x(),pPos->y(),pPos->z()) + translation;
			printf("position:%f,%f,%f;normal vector:%f,%f,%f;beam:%f;diffuse:%f;global:%f;\n",pos.x(),pos.y(),pos.z(),pNormal->x(),pNormal->y(),pNormal->z(),pSolar->beam,pSolar->diffuse,pSolar->global);
			pNormal++;
			pPos++;
			pSolar++;
		}

	}
	delete[] positions;
	delete[] normals;
	delete[] solarValues;
	

}
int main(int argc, char** argv)
{
	std::string shapeFile = "./data/boston_buildings_utm19.shp";
	OGRFeatureOptions feature_opt;
	feature_opt.url() = shapeFile;
	feature_opt.buildSpatialIndex() = true;
	osg::ref_ptr<FeatureSource>	features = FeatureSourceFactory::create( feature_opt );
	features->initialize();
	features->getFeatureProfile();
	std::string solarDataDir = "./data/boston_output/";
	osg::Vec3d translation = features->getFeatureProfile()->getExtent().bounds().center();
	translation.z() = 0;
	//traverse the features in the shape file
	for (int i=0;i<features->getFeatureCount();i++)
	{	
		osgEarth::Features::Feature* fea = features->getFeature(i);
		int roofIndex = i*2;// roof index
		int wallIndex = i*2+1;// wall index
	    
		//access roof data
		accessSurface(solarDataDir,roofIndex,translation);
		//access wall data
		accessSurface(solarDataDir,wallIndex,translation);



	}


}
