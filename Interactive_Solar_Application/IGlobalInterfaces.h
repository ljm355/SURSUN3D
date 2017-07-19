#pragma once
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QStatusBar>
#include <osgEarth/ElevationQuery>
#include <osgViewer/Viewer>
//
#include "GrassSolar.h"
enum SpatialQueryMode
{
	Point = 0,
	Circle = 1,
	Disabled = 2
};


enum SolarAnalysisMode
{
	Point_based = 0,
	Surface_based = 1
};


class SolarAnalysisParams
{
public:
	osgViewer::Viewer* Viewer;
	QMainWindow* MainWin;
	//rectangle selection
	float XMin,YMin,XMax,YMax;
	//circle selection
	float Radius;
	//osg::Vec3d CircleCenter;
	//mouse selection
	float GeoX,GeoY;
	osg::Vec3d IntersectionPoint;
	osg::Vec3d IntersectionNormal;
	osg::Vec3d WorldIntersectionPoint;
	osg::Vec3d ModelTranslation;
	float Lon;
	float Lat;
	float Elevation;
	bool IsInstantaneous;
	SolarTime Time;
	//r.sun,Day
	int Year, Day;
	float Linke,Coefbh,Coefdh,Step;
    int FirstDay,LastDay;

	SolarAnalysisMode AnalysisMode;
	SpatialQueryMode QueryMode;

	bool RooftopIncluded;
	bool FacadeIncluded;
	bool SolarPanelIncluded;
	std::vector<SolarRadiation> DailySeries;
	std::vector<SolarRadiation> MonthlySeries;
	osg::Vec4 getBound()
	{
		return osg::Vec4(XMin,YMin,XMax,YMax);
	}
	std::vector<int> getDays()
	{
        std::vector<int> days;
		for (int i=FirstDay;i<=LastDay;i++)
		{
            days.push_back(i);
		}
		return days;
	}


	SolarAnalysisParams()
	{
		Day = 1;
		RooftopIncluded = true;
		FacadeIncluded = true;
		SolarPanelIncluded = false;
		AnalysisMode = SolarAnalysisMode::Surface_based;
		QueryMode = SpatialQueryMode::Circle;
		IsInstantaneous = false;
	}


};
class IGlobalInterfaces
{
public:
	IGlobalInterfaces(void);
	~IGlobalInterfaces(void);
     static IGlobalInterfaces* getInstance();
	 static void setInstance(IGlobalInterfaces* pInstance);
	 virtual void setMessage(QString message);
	 osgEarth::ElevationQuery* getElevationQuery();
     SolarAnalysisParams* getSolarAnalysisParams();
	 virtual void execute();
	 virtual void export();
	 virtual void setProgress(int min,int max,int val);
	 virtual void updateSolarPanel()=0;
protected:
	 osgEarth::ElevationQuery* g_pElevationQuery;
	 SolarAnalysisParams m_pAnalysisParams;
};
