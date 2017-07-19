#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QMainWindow>
#include "ui_mymainwindow.h"
//#include "ILayerManager.h"
//#include "ObstacleManager.h"
//#include "FlightBoundManager.h"
//#include "RouteManager.h"
#include "IGlobalInterfaces.h"
#include "DlgBuildingParams.h"
#include "qevent.h"
#include "osgEarth\Map"
#include "osgEarth\ElevationQuery"
#include "osgEarth\Terrain"
#include "osgEarth\MapNode"
#include <osgEarthQt\ViewerWidget>
#include "DlgBuildingParams.h"
#include "DlgParams.h"
#include "City.h"
#include "osgUtil\LineSegmentIntersector"
#include "osgUtil\IntersectionVisitor"
#include "osg\Point"
#include "osg\PointSprite"
#include <osgUtil\LineSegmentIntersector>
#include "NorthArrow.h"
#include "TemporalSolarWidget.h"
#include "Utils.h"
#include "QCustomPlot/QCustomPlot.h"
#include "osgEarthQt/MapCatalogWidget"
#include "osgEarthQt/DataManager"
#include "osgEarthSymbology/Style"
#include "osgEarthAnnotation/PlaceNode"
#include "osgDB/WriteFile"
#include "VGEViewerWidget.h"
//------------------------------------------------------------------
#include "GeometryWrapper.h"
#include "Utils.h"
#include "ViewerWidget2.h"
#include "osgEarth\Map"
#include "osgEarth\MapNode"
#include "osg\Node"
#include "osgDB\ReadFile"
#include "osgViewer\ViewerEventHandlers"
#include "osgEarthUtil\EarthManipulator"
#include "osgGA\StateSetManipulator"
#include "osg\Switch"
#include "ViewerWidget2.h"
#include "osgEarth\Registry"
#include "osgEarthDrivers/arcgis/ArcGISOptions"
#include "osgEarthDrivers/feature_ogr/OGRFeatureOptions"
#include "osgEarthSymbology/ExtrusionSymbol"
#include "osgEarthSymbology/Skins"
#include "osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions"
#include "ExtrudeGeometryFilter2.h"
#include "VGEViewerWidget.h"
using namespace osgEarth;
using namespace osgEarth::Drivers;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;



class CMainWindow : public QMainWindow, public IGlobalInterfaces,public osgGA::GUIEventHandler
{
	Q_OBJECT

public:
	CMainWindow();
	~CMainWindow();	
	  virtual void setMessage(QString message){g_pTextBox->setText(message);} 
	  virtual void saveFile(const QString& filename);
	  virtual void openFile(const QString& filename,bool read = false);
	  virtual void openShapeFile( const QString& filename );
public slots:
	virtual void onAction_OpenFile();
	virtual void onAction_SaveFile();
public:
	   virtual void paintEvent(QPaintEvent* e);
	   virtual void execute();
	   virtual void export();
       virtual void setProgress(int min,int max,int val);
	   virtual void updateSolarPanel();
	   void createMonthlyPlot(QCustomPlot* customPlot);
	   void createDailyPlot(QCustomPlot* customPlot);
	   void createFacadeWidget(QWidget* widget);
	   bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
	   virtual void installFrameTimer();
private:
	//Ui::CMainWindow ui;
	const osgEarth::Map*       g_pMap;
	const osgEarth::Terrain*   g_pTerrain;
	bool             g_pMouseDown;
	osgViewer::Viewer* g_pViewer;
	osg::ref_ptr<osg::Group> g_pRoot;
	SolarParam g_mSParam;
	osg::ref_ptr<osg::MatrixTransform> g_pSolarNodes;
	osg::ref_ptr<osg::MatrixTransform> g_pSolarPanelNodes;
	osg::ref_ptr<osgEarth::MapNode> g_pMapNode; 
	osg::ref_ptr<osgEarth::Annotation::PlaceNode> g_pMousePoint;
	osg::ref_ptr<osg::Node> g_pCityNode;
	City* g_pCity;
	osg::Vec3d g_mTranslation;
	DlgBuildingParams* g_pDlgBuildingParams;	
	QCustomPlot* m_gDailyRadPlot;
	QCustomPlot* m_gMonthlyRadPlot;
	QWidget* m_gFacadeWidget;
    QProgressBar* g_ProgressBar;
	QLineEdit* g_pTextBox;
	bool g_mIsMouseDown;
	osg::Vec2 g_mLastMousePos;
	VGEViewerWidget* g_pViewerWidget;
	QTimer	g_mTimer;
	QDockWidget *g_pCatalogDock;
};

#endif // MYMAINWINDOW_H
