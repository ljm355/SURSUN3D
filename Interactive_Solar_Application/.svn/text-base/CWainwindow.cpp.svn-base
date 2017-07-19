#include "CWainwindow.h"
#include "osgEarth\Registry"
#include "FeatureFieldSelector.h"
#include "osgDB\ReadFile"
#include "osgUtil\Optimizer"
#include <set>
osg::Node* extrude(QString filename,std::string heightField, FeatureSource* features)
{	


	
	// assemble a stylesheet and add our styles to it:
	static osg::ref_ptr<StyleSheet> styleSheet;
	// a style for the building data:
	static Style buildingStyle;
	// a style for the rooftop textures:
	static Style roofStyle;
	// a style for the wall textures:
	static Style wallStyle;
	if(!styleSheet || !styleSheet.valid())
	{
		styleSheet = new StyleSheet;



		buildingStyle.setName( "default" );

		osg::ref_ptr<ExtrusionSymbol> extrusion = buildingStyle.getOrCreate<ExtrusionSymbol>();
		//extrusion->heightExpression() = 3.5;//NumericExpression( "3.5 * max( [story_ht_], 1 )" );
		//extrusion->heightExpression() = NumericExpression("[height]");
		extrusion->heightExpression() = NumericExpression("[" + heightField + "]");
		extrusion->flatten() = true;
		extrusion->wallStyleName() = "building-wall";
		extrusion->roofStyleName() = "building-roof";


		wallStyle.setName( "building-wall" );
		osg::ref_ptr<SkinSymbol> wallSkin = wallStyle.getOrCreate<SkinSymbol>();
		wallSkin->libraryName() = "us_resources";
		wallSkin->addTag( "building" );
		wallSkin->randomSeed() = 1;

		osg::ref_ptr<PolygonSymbol> poly = buildingStyle.getOrCreate<PolygonSymbol>();
		poly->fill()->color() = Color::White;

		roofStyle.setName( "building-roof" );
		osg::ref_ptr<SkinSymbol> roofSkin = roofStyle.getOrCreate<SkinSymbol>();
		roofSkin->libraryName() = "us_resources";
		roofSkin->addTag( "rooftop" );
		roofSkin->randomSeed() = 1;
		roofSkin->isTiled() = true;

		styleSheet->addStyle( buildingStyle );
		styleSheet->addStyle( wallStyle );
		styleSheet->addStyle( roofStyle );

		// load a resource library that contains the building textures.
		osg::ref_ptr<ResourceLibrary> reslib = new ResourceLibrary( "us_resources", "./data/resources/textures_us/catalog.xml" );
		styleSheet->addResourceLibrary( reslib.get() );
	}


	
	
	const std::string wtk = features->getFeatureProfile()->getSRS()->getWKT();
	const SpatialReference* spatialRef = features->getFeatureProfile()->getSRS(); 
	GeometryCompiler compiler; 

	//osg::BoundingBoxd extent;
	FeatureList flist;
	int featurecount = features->getFeatureCount();
	osgEarth::MapOptions opt;
	osgEarth::CacheOptions cacheOpt;
	osgEarth::ProfileOptions profileOpt;
	cacheOpt.setDriver("filesystem");
	cacheOpt.getConfig().add("path","./cache");
	opt.cachePolicy() = osgEarth::CachePolicy::USAGE_READ_WRITE;
	opt.cache() = cacheOpt;
	opt.coordSysType() = MapOptions::CSTYPE_PROJECTED;
	//opt.profile() = featureProfile->getProfile()->toProfileOptions();
	profileOpt.bounds() = features->getFeatureProfile()->getExtent().bounds();//osgEarth::Bounds(featureProfile->getExtent().bounds());
	const osgEarth::SpatialReference* srs = features->getFeatureProfile()->getSRS();
	profileOpt.srsString() = features->getFeatureProfile()->getSRS()->getKey().first;
	profileOpt.vsrsString() = features->getFeatureProfile()->getSRS()->getKey().second;
	opt.profile() = profileOpt;
	osg::ref_ptr<Map> tmpMap = new Map(opt);

	FilterContext context(new Session(tmpMap.get(),styleSheet.get()),features->getFeatureProfile(),features->getFeatureProfile()->getExtent());

	FeatureList feaList;
	for (int i=0;i<featurecount;i++)
	{	
		feaList.push_back(features->getFeature(i));
	}
	ExtrudeGeometryFilter2 extrude;
	extrude.useTextureArrays() = false;
	extrude.setStyle( buildingStyle );
	return extrude.push( feaList, context );

}

bool removeDirectory( QDir dir )
{
	bool ok = dir.exists();
	if ( ok )
	{
		QFileInfoList entries = dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::AllDirs );
		foreach ( QFileInfo entryInfo, entries )
		{
			QString path = entryInfo.absoluteFilePath();
			if ( entryInfo.isDir() )
			{
				if ( ! removeDirectory( QDir( path ) ) )
				{
					ok = false;
					break;
				}
			}
			else
			{
				QFile file( path );
				if ( ! file.remove() )
				{
					ok = false;
					break;
				}
			}
		}
	}

	if ( ok && ! dir.rmdir( dir.absolutePath() ) )
		ok = false;

	return ok;
}
osgEarth::MapNode* createMapNode(FeatureSource* feature)
{
	// create the map.
	osgEarth::MapOptions opt;

	Config cacheConf;
	cacheConf.add("path","./cache");
	cacheConf.add("type","filesystem");
	osgEarth::CacheOptions cacheOpt(cacheConf);
	osgEarth::ProfileOptions profileOpt;
	cacheOpt.setDriver("filesystem");
	//cacheOpt.getConfig().add("path","./cache");
	//cacheOpt.getConfig().add("type","filesystem");
	//

	opt.cachePolicy() = osgEarth::CachePolicy::USAGE_READ_WRITE;
	opt.cache() = cacheOpt;
	opt.coordSysType() = MapOptions::CSTYPE_PROJECTED;
	//opt.profile() = featureProfile->getProfile()->toProfileOptions();
	profileOpt.bounds() = feature->getFeatureProfile()->getExtent().bounds();//osgEarth::Bounds(featureProfile->getExtent().bounds());
	const osgEarth::SpatialReference* srs = feature->getFeatureProfile()->getSRS();
	profileOpt.srsString() = srs->getKey().first;
	profileOpt.vsrsString() = srs->getKey().second;
	opt.profile() = profileOpt;
	osg::ref_ptr<Map> map = new Map(opt);

	// add a TMS imager layer:
	//TMSOptions imagery;
	//imagery.url() = "http://readymaps.org/readymap/tiles/1.0.0/7/";
	//map->addImageLayer( new ImageLayer("Imagery", imagery) );

	ArcGISOptions imageryStreet;
	imageryStreet.url() = "http://services.arcgisonline.com/ArcGIS/rest/services/World_Street_Map/MapServer";
	map->addImageLayer( new ImageLayer("World_Street_Map", imageryStreet) );

	ArcGISOptions imagerySatellite;
	imagerySatellite.url() = "http://services.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer";
	map->addImageLayer( new ImageLayer("World_Imagery", imagerySatellite) );


	//// add a TMS elevation layer:
	//TMSOptions elevation;
	//elevation.url() = "http://readymap.org/readymap/tiles/1.0.0/9/";
	//map->addElevationLayer( new ElevationLayer("Elevation", elevation) );
	// make the map scene graph:
	MapNode* node = new MapNode( map.get() );

	return node;
}
class TextureExportVisitor : public osgUtil::Optimizer::TextureAtlasVisitor
{
public:
	TextureExportVisitor():
	  osgUtil::Optimizer::TextureAtlasVisitor()
	  {
	  }
	  void export(QDir dir)
	  {
		  if(!dir.exists())
		  {
			  dir.mkdir(".");
		  }    
		  QDir parentDir(dir.absolutePath() + "/../");
		  Textures::iterator titr;
		  for(titr = _textures.begin();
			  titr != _textures.end();
			  ++titr)
		  {
			  osg::Texture2D* tex = *titr;
			  if(!tex || !tex->getImage())
				  continue;
			  QFileInfo fileInfo(tex->getImage()->getFileName().data());
			  QString outfilename = dir.absolutePath() + "/" + fileInfo.baseName() + ".jpg";

			  QString relativePath = parentDir.relativeFilePath(outfilename);
			  tex->getImage()->setFileName(relativePath.toLocal8Bit().data());
			  osgDB::writeImageFile(*tex->getImage(),outfilename.toLocal8Bit().data());
			 
		  }
		  
	  }
protected:
	 QDir g_mImageDir;

};

void CMainWindow::createMonthlyPlot(QCustomPlot* customPlot)
{

	SolarAnalysisParams* analysisParam = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	int xrang = analysisParam->MonthlySeries.size();
	float miny = FLT_MAX;
	float maxy = -FLT_MAX;
	for(int i=0;i<analysisParam->MonthlySeries.size();i++)
	{
		SolarRadiation& rad = analysisParam->MonthlySeries[i];
		if(miny>rad.global)
			miny = rad.global;
		if(maxy<rad.global)
			maxy=rad.global;
	}
	float yrange = maxy-miny;
	int w = customPlot->width();
	int h = customPlot->height();
	QPainterPath path;

	customPlot->legend->setVisible(true);
	customPlot->legend->setFont(QFont("Arial", 12));
	customPlot->legend->setName("irradiation [kWh/m-2]");
	customPlot->xAxis->setName("irradiation");
	customPlot->xAxis->setLabel("month");	
	customPlot->xAxis->setLabelFont(QFont("Arial", 12));
	customPlot->yAxis->setName("irradiation [kWh/m-2]");
	customPlot->yAxis->setLabel("irradiation [kWh/m-2]");
	customPlot->yAxis->setLabelFont(QFont("Arial", 12));

	QPen pen;
	QStringList lineNames;

	//global radiation
	customPlot->addGraph();
	pen.setColor(QColor(0,0,255));
	customPlot->graph()->setPen(pen);
	customPlot->graph()->setName("Global radiation");
	customPlot->graph()->setLineStyle((QCPGraph::LineStyle)1);
	customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));

	QVector<double> x(analysisParam->MonthlySeries.size()), y(analysisParam->MonthlySeries.size());
	for (int j=0; j<analysisParam->MonthlySeries.size(); ++j)
	{
		SolarRadiation& rad = analysisParam->MonthlySeries[j];
		x[j] = analysisParam->FirstDay+j;
		y[j] = rad.global;
	}


	customPlot->graph()->setData(x, y);
	customPlot->graph()->rescaleAxes(true);



	///direct
	customPlot->addGraph();
	pen.setColor(QColor(0,255,255));
	customPlot->graph()->setPen(pen);
	customPlot->graph()->setName("Direct radiation");
	customPlot->graph()->setLineStyle((QCPGraph::LineStyle)1);
	customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));

	for (int j=0; j<analysisParam->MonthlySeries.size(); ++j)
	{
		SolarRadiation& rad = analysisParam->MonthlySeries[j];
		x[j] = analysisParam->FirstDay+j;
		y[j] = rad.beam;
	}


	customPlot->graph()->setData(x, y);
	customPlot->graph()->rescaleAxes(true);


	//diffuse
	customPlot->addGraph();
	pen.setColor(QColor(255,255,0));
	customPlot->graph()->setPen(pen);
	customPlot->graph()->setName("Diffuse radiation");
	customPlot->graph()->setLineStyle((QCPGraph::LineStyle)1);
	customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 2));
	for (int j=0; j<analysisParam->MonthlySeries.size(); ++j)
	{
		SolarRadiation& rad = analysisParam->MonthlySeries[j];
		x[j] = analysisParam->FirstDay+j;
		y[j] = rad.diffuse;
	}


	customPlot->graph()->setData(x, y);
	customPlot->graph()->rescaleAxes(true);
	QVector<double> tickVector(12);
	for (int i=0;i<12;i++)
	{
		tickVector[i]=i+1;
	}

	// zoom out a bit:
	//customPlot->yAxis->scaleRange(1.1, customPlot->yAxis->range().center());
	//customPlot->xAxis->scaleRange(1.1, customPlot->xAxis->range().center());
	customPlot->xAxis->setTickStep(1);
	customPlot->xAxis->setTicks(true);
	customPlot->xAxis->setTickVector(tickVector);
	customPlot->xAxis->setAutoTicks(false);
	customPlot->yAxis->setTicks(true);
	customPlot->xAxis->setTickLabels(true);
	customPlot->yAxis->setTickLabels(true);
	customPlot->xAxis->setRange(1,12);
	// make top right axes clones of bottom left axes:
	customPlot->axisRect()->setupFullAxesBox();
	customPlot->setWindowTitle("monthly solar irridiation [kWh/m-2]");
	customPlot->setCaption("monthly solar irridiation [kWh/m-2]");
}

void CMainWindow::createDailyPlot(QCustomPlot* customPlot)
{

	SolarAnalysisParams* analysisParam = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	int xrang = analysisParam->DailySeries.size();
	float miny = FLT_MAX;
	float maxy = -FLT_MAX;
	for(int i=0;i<analysisParam->DailySeries.size();i++)
	{
		SolarRadiation& rad = analysisParam->DailySeries[i];
		if(miny>rad.global)
			miny = rad.global;
		if(maxy<rad.global)
			maxy=rad.global;
	}
	float yrange = maxy-miny;
	int w = customPlot->width();
	int h = customPlot->height();
	QPainterPath path;

	customPlot->legend->setVisible(true);
	customPlot->legend->setFont(QFont("Arial", 12));
	customPlot->legend->setName("irradiation [Wh/m-2]");
	customPlot->xAxis->setName("irradiation");
	customPlot->xAxis->setLabel("day");	
	customPlot->xAxis->setLabelFont(QFont("Arial", 12));
	customPlot->yAxis->setName("irradiation [Wh/m-2]");
	customPlot->yAxis->setLabel("irradiation [Wh/m-2]");
	customPlot->yAxis->setLabelFont(QFont("Arial", 12));

	QPen pen;
	QStringList lineNames;

	//global radiation
	customPlot->addGraph();
	pen.setColor(QColor(0,0,255));
	customPlot->graph()->setPen(pen);
	customPlot->graph()->setName("Global radiation");
	customPlot->graph()->setLineStyle((QCPGraph::LineStyle)1);
	customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));

	QVector<double> x(analysisParam->DailySeries.size()), y(analysisParam->DailySeries.size());
	for (int j=0; j<analysisParam->DailySeries.size(); ++j)
	{
	    SolarRadiation& rad = analysisParam->DailySeries[j];
		x[j] = analysisParam->FirstDay+j;
		y[j] = rad.global;
	}


	customPlot->graph()->setData(x, y);
	customPlot->graph()->rescaleAxes(true);



	///direct
	customPlot->addGraph();
	pen.setColor(QColor(0,255,255));
	customPlot->graph()->setPen(pen);
	customPlot->graph()->setName("Direct radiation");
	customPlot->graph()->setLineStyle((QCPGraph::LineStyle)1);
	customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));

	for (int j=0; j<analysisParam->DailySeries.size(); ++j)
	{
		SolarRadiation& rad = analysisParam->DailySeries[j];
		x[j] = analysisParam->FirstDay+j;
		y[j] = rad.beam;
	}


	customPlot->graph()->setData(x, y);
	customPlot->graph()->rescaleAxes(true);


	//diffuse
	customPlot->addGraph();
	pen.setColor(QColor(255,255,0));
	customPlot->graph()->setPen(pen);
	customPlot->graph()->setName("Diffuse radiation");
	customPlot->graph()->setLineStyle((QCPGraph::LineStyle)1);
	customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 2));
	for (int j=0; j<analysisParam->DailySeries.size(); ++j)
	{
		SolarRadiation& rad = analysisParam->DailySeries[j];
		x[j] = analysisParam->FirstDay+j;
		y[j] = rad.diffuse;
	}


	customPlot->graph()->setData(x, y);
	customPlot->graph()->rescaleAxes(true);


	// zoom out a bit:
	//customPlot->yAxis->scaleRange(1.1, customPlot->yAxis->range().center());
	//customPlot->xAxis->scaleRange(1.1, customPlot->xAxis->range().center());
	// set blank axis lines:
	QVector<double> tickVector(analysisParam->DailySeries.size());
	for (int i=0;i<analysisParam->DailySeries.size();i++)
	{
       tickVector[i]=(analysisParam->FirstDay+i);
	}
	 //tickVector[12]=(365);
	customPlot->xAxis->setAutoTicks(true);
	customPlot->xAxis->setRange(1,analysisParam->DailySeries.size());
	//customPlot->xAxis->setTickStep(13);
	customPlot->xAxis->setTickVector(tickVector);
	customPlot->xAxis->setTicks(true);
	customPlot->yAxis->setTicks(true);
	customPlot->xAxis->setTickLabels(true);
	customPlot->yAxis->setTickLabels(true);
	// make top right axes clones of bottom left axes:
	customPlot->axisRect()->setupFullAxesBox();
	customPlot->setWindowTitle("daily solar irridiation [Wh/m-2]");
	customPlot->setCaption("daily solar irridiation [Wh/m-2]");
}

void CMainWindow::createFacadeWidget(QWidget* widget)
{


	//QPixmap pixmap(g_pCity->getFacadeImagePath(0));
	//QPalette   palette;
	//palette.setBrush(widget->backgroundRole(),QBrush(pixmap));
	//widget->setPalette(palette);
	//widget->setMask(pixmap.mask());  //可以将图片中透明部分显示为透明的
	//widget->setAutoFillBackground(true);//width: 100%; height:100%; 

	//widget->setStyleSheet("background-image: url(" + g_pCity->getFacadeImagePath(0) + "); background-repeat: none; border: none");
    widget->setStyleSheet("border-image: url(" + g_pCity->getFacadeImagePath(0) + ") 10 10 10 10 stretch stretch;");

	//:"border-image:url(background.png) 10 10 10 10 stretch stretch;");  ///使用border-image可以实现背景的
	widget->setCaption("Facade irradiation");
}


bool CMainWindow::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{


	if(!g_pMapNode || !g_pMapNode.valid())
		return false;
	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();

	if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH)
	{
		g_mIsMouseDown = true;
		g_mLastMousePos = osg::Vec2(ea.getX(),ea.getY());
	}

	if (ea.getEventType() == osgGA::GUIEventAdapter::KEYUP)
	{
		if(ea.getKey() ==  osgGA::GUIEventAdapter::KEY_F)
		{
			g_pViewerWidget->setFullScreenOn(!g_pViewerWidget->getFullScreenOn());
		}
	}


	if (params->QueryMode != SpatialQueryMode::Disabled 
		&& ea.getEventType() == osgGA::GUIEventAdapter::RELEASE 
		&& ea.getEventType() != osgGA::GUIEventAdapter::DOUBLECLICK
		&& ea.getEventType() != osgGA::GUIEventAdapter::DRAG)
	{
		g_mIsMouseDown = false;
		osg::Vec2 curMousePos(ea.getX(),ea.getY());
		if((curMousePos-g_mLastMousePos).length() > 2)
		{
			return false;
		}

		g_mIsMouseDown = false;
		osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);   

		osg::ref_ptr<osgUtil::LineSegmentIntersector> ray = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::PROJECTION, ea.getXnormalized(), ea.getYnormalized());
		osgUtil::IntersectionVisitor visitor(ray);
		view->getCamera()->accept(visitor);
		if (ray->containsIntersections())
		{
			osgUtil::LineSegmentIntersector::Intersection intersection = ray->getFirstIntersection();
			params->WorldIntersectionPoint = intersection.getWorldIntersectPoint();
			params->IntersectionNormal = intersection.getWorldIntersectNormal();
			params->IntersectionPoint = intersection.getWorldIntersectPoint()-g_mTranslation;//osg::Vec3d(329542.6607715,4690819.062525,0);
			osg::Vec3d geopos;
			g_pMapNode->getMapSRS()->transform( params->WorldIntersectionPoint,osgEarth::Registry::instance()->getGlobalGeodeticProfile()->getSRS(),geopos);
			std::stringstream ss;
			ss << std::fixed << std::setprecision(2) << "lonlat: " << geopos.x() << "," << geopos.y();
			QString info = ss.str().data();
			//QString info =QString("coordinates: ") + QString::number(params->IntersectionPoint.x(), 'g', 2) + "," + QString::number(params->IntersectionPoint.y(), 'g', 2);
			setMessage(info);
			//g_pMainWin->setMessage(QString("coordinates: %1, %2, %3 (meters)").arg(params->IntersectionPoint.x()).arg(params->IntersectionPoint.y()).arg(params->IntersectionPoint.z()));  
			g_pCity->setMousePos(params->IntersectionPoint);/*params->QueryMode == SpatialQueryMode::Point &&*/
			g_pMousePoint->setPosition(osgEarth::GeoPoint(g_pMapNode->getMapSRS(),params->WorldIntersectionPoint));

			if(g_pSolarPanelNodes->getNumChildren() > 0)
			{
				g_pSolarPanelNodes->removeChildren(0,g_pSolarPanelNodes->getNumChildren());
			}
			if(params->QueryMode == SpatialQueryMode::Point)
			{			

				if(params->SolarPanelIncluded)
				{			
					Building* building;	
					osg::ref_ptr<osg::Node> panelNode = g_pCity->getPanelNode(params->IntersectionPoint.x(),params->IntersectionPoint.y(),building);
				    g_pDlgBuildingParams->setBuilding(building);
					//g_pSolarPanelNodes->removeChildren(0,g_pSolarPanelNodes->getNumChildren());
					g_pSolarPanelNodes->addChild(panelNode.get());
				}


			}
			return true;
		}
	}

	return false;
}
CMainWindow::CMainWindow()
	: QMainWindow(NULL)

{
	g_pViewerWidget = new VGEViewerWidget(this);
	g_pRoot = new osg::Switch();
	g_pCity = new City(g_pViewerWidget->getViewer());
	//g_pTerrain = NULL;
	g_pElevationQuery = NULL;
	g_pViewerWidget->getViewer()->setSceneData( g_pRoot.get() );
	g_pViewer = g_pViewerWidget->getViewer();
	setCentralWidget( g_pViewerWidget );

	//SceneEventHandler* handler = new SceneEventHandler;

   	g_pViewer->addEventHandler((osgGA::GUIEventHandler*)this);

	IGlobalInterfaces::setInstance(this);
	this->setWindowTitle(QString::fromLocal8Bit("Interactive urban solar analysis"));

	QDockWidget *buildingParamsDock = new QDockWidget(QWidget::tr("Building Parameters"));
	buildingParamsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

   DlgParams* dlg = new DlgParams;
   buildingParamsDock->setWidget(dlg);
   addDockWidget(Qt::LeftDockWidgetArea, buildingParamsDock);

   QDockWidget *buildPropDock = new QDockWidget(QWidget::tr("Parameters"));
   buildPropDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
   g_pDlgBuildingParams = new DlgBuildingParams;

   buildPropDock->setWidget(g_pDlgBuildingParams);

   addDockWidget(Qt::RightDockWidgetArea, buildPropDock);

   g_pCatalogDock = new QDockWidget(QWidget::tr("MapLayers"));
   g_pCatalogDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);

   addDockWidget(Qt::RightDockWidgetArea, g_pCatalogDock);


   IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->Viewer = g_pViewer;
   IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->MainWin = this;
   IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->ModelTranslation = g_mTranslation;
   IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->Elevation = 0;

   m_gDailyRadPlot = NULL;
   m_gMonthlyRadPlot = NULL;
   m_gFacadeWidget = NULL;


   g_ProgressBar = new QProgressBar();
   g_ProgressBar->setAlignment(Qt::AlignRight);
   g_ProgressBar->setVisible(true);
   //g_ProgressBar->setMaximumSize(512, 19);
   g_pTextBox = new QLineEdit();
   g_pTextBox->setMinimumWidth(128);
   g_pTextBox->setMaximumWidth(128);
   statusBar()->addWidget(g_pTextBox,0);
   statusBar()->addWidget(g_ProgressBar,1);

   QMenu* menuFile	 = this->menuBar()->addMenu(QString::fromLocal8Bit("File"));
  
   //文件
   QAction* openFileAction = new QAction(QString::fromLocal8Bit("Open"),this);
   menuFile->addAction(openFileAction);
   connect(openFileAction,SIGNAL(triggered()),this,SLOT(onAction_OpenFile()));

   QAction* saveFileAction = new QAction(QString::fromLocal8Bit("Save"),this);
   menuFile->addAction(saveFileAction);
   connect(saveFileAction,SIGNAL(triggered()),this,SLOT(onAction_SaveFile()));
   installFrameTimer();
}

CMainWindow::~CMainWindow()
{
	UINT ExitCode = 0 ;
	::TerminateProcess(::GetCurrentProcess(), ExitCode);
}

void CMainWindow::execute()
{
	QDir texDir("./out_texture_dir");
	removeDirectory(texDir);
	if(!texDir.exists())
	{
		texDir.mkdir(".");
	}
	if(getSolarAnalysisParams()->AnalysisMode == Surface_based)
	{
		if(!g_pCity->getVertices())
			return;
		osg::ref_ptr<osg::Node> solarNodes = g_pCity->createSolarNodes();
		//osgDB::writeNodeFile(*solarNodes,"solar.osg");
		g_pSolarNodes->removeChildren(0,g_pSolarNodes->getNumChildren());
		g_pSolarNodes->addChild(solarNodes.get());
		g_pSolarPanelNodes->removeChildren(0,g_pSolarPanelNodes->getNumChildren());
		if(g_pDlgBuildingParams->getBuilding())
			g_pDlgBuildingParams->getBuilding()->Panel.Node = NULL;
	}
	else
	{
	
		if(getSolarAnalysisParams()->IsInstantaneous)
			return;
		getSolarAnalysisParams()->DailySeries = g_pCity->getDailySolarRadiation(getSolarAnalysisParams()->getDays());
		//getSolarAnalysisParams()->MonthlySeries = g_pCity->getMonthlySolarRadiation(getSolarAnalysisParams()->DailySeries,getSolarAnalysisParams()->FirstDay,getSolarAnalysisParams()->LastDay);
		//if(getSolarAnalysisParams()->DailySeries.size() < 10)
		//{
		//	QMessageBox::critical(this,"ALERT","at least 10 days needed!");
		//	return;
		//}
		//TemporalSolarWidget solarWidget;
		//solarWidget.exec();

		if(m_gDailyRadPlot)
		{
			delete m_gDailyRadPlot;
			m_gDailyRadPlot = NULL;
		}
		m_gDailyRadPlot = new QCustomPlot;
		m_gDailyRadPlot->setMinimumSize(512,256);
		m_gDailyRadPlot->setGeometry(m_gDailyRadPlot->x(),30,512,256);
		createDailyPlot(m_gDailyRadPlot);
		m_gDailyRadPlot->showNormal();


		//if(m_gMonthlyRadPlot)
		//{
		//	delete m_gMonthlyRadPlot;
		//	m_gMonthlyRadPlot = NULL;
		//}
		//m_gMonthlyRadPlot = new QCustomPlot;

		//createMonthlyPlot(m_gMonthlyRadPlot);
		//m_gMonthlyRadPlot->showNormal();
	}

}

void CMainWindow::export()
{
	//QString filters("ive files (*.ive);;osg files (*.osg);;osgb files (*.osgb);;obj files (*.obj);;3ds files (*.3ds);;dae files (*.dae);;fbx files (*.fbx);;all files (*.*)");
	//QString filters("ive files (*.ive);;osg files (*.osg);;osgb files (*.osgb);;obj files (*.obj);;3ds files (*.3ds);;dae files (*.dae);;fbx files (*.fbx);;all files (*.*)");

	/*QString filters("osgb files (*.osgb);;ive files (*.ive);;osg files (*.osg);;all files (*.*)");
	QString filename = QFileDialog::getSaveFileName(0,QString::fromLocal8Bit("Export"),"./",filters);
	if(filename == "") 
		return;*/

	//osgDB::Options* opt = osgDB::Registry::instance()->getOptions();
	//osg::ref_ptr<osgDB::Options> opt = new osgDB::Options("WriteImageHint=IncludeData");
	//opt->setPluginStringData("WriteImageHint=IncludeFile");
	//osgDB::writeNodeFile(*g_pSolarNodes,fileName,opt);

	if(g_pSolarNodes->getNumChildren() < 1)
		return;

	QString dir = QFileDialog::getExistingDirectory(this, tr("Output Directory"),
		".",
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	if(dir == "")
		return;
	dir = dir + "/";
	QString filename = dir + g_pCityNode->getName().data() + ".osgb";
	TextureExportVisitor exportVisitor;
	g_pSolarNodes->getChild(0)->accept(exportVisitor);
	QFileInfo fileinfo(filename);
	if(!fileinfo.completeSuffix().contains("osgb"))
	{
	   exportVisitor.export(fileinfo.absoluteDir().absolutePath() + "/images");
	}

	osgDB::writeNodeFile(*g_pSolarNodes,filename.toLocal8Bit().data());

    g_pCity->exportSolarData(dir);

}

void CMainWindow::setProgress( int min,int max,int val )
{
	if(min == 0 && max == 0)
	{
		g_ProgressBar->setEnabled(false);
		g_ProgressBar->setRange(0,0);
	    g_ProgressBar->setValue(0);
		return;
	}
	else if(!g_ProgressBar->isEnabled())
	{
		g_ProgressBar->setEnabled(true);
	}
	g_ProgressBar->setRange(min,max);
	g_ProgressBar->setValue(val);
}

void CMainWindow::updateSolarPanel()
{
	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	if(g_pSolarPanelNodes->getNumChildren() > 0)
	{
		g_pSolarPanelNodes->removeChildren(0,g_pSolarPanelNodes->getNumChildren());
	}
	if(params->QueryMode == SpatialQueryMode::Point)
	{			

		if(params->SolarPanelIncluded)
		{			
			Building* building;	
			osg::ref_ptr<osg::Node> panelNode = g_pCity->getPanelNode(params->IntersectionPoint.x(),params->IntersectionPoint.y(),building);
			g_pDlgBuildingParams->setBuilding(building);
			//g_pSolarPanelNodes->removeChildren(0,g_pSolarPanelNodes->getNumChildren());
			g_pSolarPanelNodes->addChild(panelNode.get());
		}

	}
}

void CMainWindow::installFrameTimer()
{
	connect(&g_mTimer, SIGNAL(timeout()), this, SLOT(update()));
	g_mTimer.start(0);
}

void CMainWindow::paintEvent( QPaintEvent* e )
{
	g_pViewerWidget->frame();
}

void CMainWindow::saveFile( const QString& filename )
{
	QFileInfo fileInfo(filename);

	QString dir = fileInfo.absoluteDir().absolutePath();
	QString customFile =  dir + QString("/") + fileInfo.baseName() + QString(".city");
	QString mapFile =  dir + QString("/") + fileInfo.baseName() + QString(".earth");
	QString nodeFile =  dir + QString("/") + fileInfo.baseName() + QString(".osgb");

	if(!QFileInfo(customFile).exists())
	{
	   g_pCity->save(customFile.toLocal8Bit().data());
	}

	osgDB::writeNodeFile(*g_pMapNode,mapFile.toLocal8Bit().data());//create MapNode using API
	osgDB::writeNodeFile(*g_pCityNode,nodeFile.toLocal8Bit().data());

}

void CMainWindow::onAction_SaveFile()
{
	QString filename = QFileDialog::getSaveFileName(0,QString::fromLocal8Bit("load city"),".", "*.city");
	if(filename == "") return;
	saveFile(filename);

}

void CMainWindow::onAction_OpenFile()
{	
	QString filename = QFileDialog::getOpenFileName(0,QString::fromLocal8Bit("load city"),".", "*.shp *.city");
	if(filename == "") return;
	if(filename.endsWith(".shp"))
	{
	   openShapeFile(filename);
	}
	else
	{
	   openFile(filename,true);
	}

}

void CMainWindow::openShapeFile( const QString& filename )
{

	QFileInfo fileInfo(filename);

	QString dir = fileInfo.absoluteDir().absolutePath();
	QString customFile =  dir + QString("/") + fileInfo.baseName() + QString(".city");
	//if(QFileInfo(customFile).exists())
	//{
	//	return openFile(filename,true);
	//}

	
	//if(g_pCity)
	//{
	//	delete g_pCity;
	//	g_pCity = NULL;
	//}

	OGRFeatureOptions feature_opt;
	feature_opt.name() = "buildings";
	feature_opt.url() = filename.toLocal8Bit().data();
	feature_opt.buildSpatialIndex() = true;
    osg::ref_ptr<FeatureSource>	features = FeatureSourceFactory::create( feature_opt );
	features->initialize();
	features->getFeatureProfile();
	FeatureFieldSelector fieldSel;
	fieldSel.setFeatureSource(features);
	std::string fieldName = "";
	if(fieldSel.exec())
	{
		fieldName = fieldSel.getFieldName();
		if(fieldName == "")
			return;
	}
	else
	{
		return;
	}
	g_pRoot->removeChildren(0,g_pRoot->getNumChildren());
	osg::ref_ptr<osg::Node> extrudeNode = extrude(filename,fieldName,features);

	g_mTranslation = features->getFeatureProfile()->getExtent().bounds().center();
	g_mTranslation.z() = 0;
	g_pCity->loadFromOSGNode(extrudeNode,g_mTranslation);
	g_pCity->save(customFile.toLocal8Bit().data());
	g_pCity->setFileName(customFile.toLocal8Bit().data(),"./out_texture_dir");
	g_pCity->open();
	g_pMapNode = createMapNode(features);//create MapNode using API
	g_pCityNode = g_pCity->toOSG(features->getFeatureProfile()->getExtent().bounds());
	g_pCityNode->setName(fileInfo.baseName().toLocal8Bit().data());
	
	saveFile(customFile);



	g_mTranslation = g_pMapNode->getMap()->getProfile()->getExtent().bounds().center();
	g_mTranslation.z() = 0;

	openFile(customFile);
}

void CMainWindow::openFile( const QString& filename,bool read)
{

	QFileInfo fileInfo(filename);
	QString dir = fileInfo.absoluteDir().absolutePath();
	QString mapFile =   dir + QString("/") + fileInfo.baseName() + QString(".earth");
	QString nodeFile =  dir + QString("/") + fileInfo.baseName() + QString(".osgb");


	if(read)
	{
	    g_pRoot->removeChildren(0,g_pRoot->getNumChildren());
		osg::ref_ptr<osg::Node> earthNode = osgDB::readNodeFile(mapFile.toLocal8Bit().data());
		if(!earthNode)
			return;
		g_pMapNode = osgEarth::MapNode::findMapNode(earthNode.get());
		if(!g_pMapNode || !g_pMapNode.valid())
			return;

		g_pCityNode = osgDB::readNodeFile(nodeFile.toLocal8Bit().data());// g_pCity->toOSG(g_pMap->getProfile()->getExtent().bounds());
		if(!g_pCityNode || !g_pCityNode.valid())
			return;
		g_pCityNode->setName(QFileInfo(nodeFile).baseName().toLocal8Bit().data());
		g_mTranslation = g_pMapNode->getMap()->getProfile()->getExtent().bounds().center();
		g_mTranslation.z() = 0;

		g_pCity->setFileName(filename.toLocal8Bit().data(),"./out_texture_dir");
		g_pCity->open();

	}

	g_pCity->setCityNode((osg::Geode*)g_pCityNode.get());
	osg::ref_ptr<osgEarth::QtGui::DataManager> dataManager = new osgEarth::QtGui::DataManager(g_pMapNode);
	osgEarth::QtGui::MapCatalogWidget* layerCatalog = new osgEarth::QtGui::MapCatalogWidget(dataManager.get(), osgEarth::QtGui::MapCatalogWidget::ALL_LAYERS);
	//layerCatalog->setActiveViews(views);
	layerCatalog->setHideEmptyGroups(true);
	g_pCatalogDock->setWidget(layerCatalog);


	osg::ref_ptr<osg::MatrixTransform> matNode = new osg::MatrixTransform;
	matNode->setMatrix(osg::Matrix::translate(g_mTranslation));

	g_pViewerWidget->resetView();
	g_pRoot->addChild( g_pMapNode.get() );
	g_pMap = g_pMapNode->getMap(); 

	//g_pTerrain  = g_pMapNode->getTerrain();


	osg::ref_ptr<osg::MatrixTransform> boston = new osg::MatrixTransform;
	boston->setMatrix(osg::Matrix::translate(g_mTranslation));

	boston->addChild(g_pCityNode.get());

	g_pSolarNodes = new osg::MatrixTransform;
	g_pSolarNodes->setMatrix(osg::Matrix::translate(g_mTranslation));

	g_pSolarPanelNodes = new osg::MatrixTransform;
	g_pSolarPanelNodes->setMatrix(osg::Matrix::translate(g_mTranslation));

	g_pRoot->addChild(boston.get());
	g_pRoot->addChild(g_pSolarNodes.get());
	g_pRoot->addChild(g_pSolarPanelNodes.get());
	//g_pRoot->addChild(createNorthArrow(g_pViewer,"northarrow.png"));
	//g_pElevationQuery = new osgEarth::ElevationQuery(g_pMapNode->getMap());
    IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->ModelTranslation = g_mTranslation;



	osgEarth::Symbology::Style style("PlaceNode");

	osg::ref_ptr<osgEarth::Symbology::TextSymbol> textSymbol = new osgEarth::Symbology::TextSymbol;
	textSymbol->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_BASE_LINE;
	textSymbol->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	textSymbol->font() =  std::string("fonts/simfang.ttf");
	//textSymbol->fill()->color() = osgEarth::Symbology::Color:;
	textSymbol->size() = 15;
	//textSymbol->halo()->width() = priority;
	style.addSymbol(textSymbol.get());

	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("placemark32.png");

	g_pMousePoint = new osgEarth::Annotation::PlaceNode(g_pMapNode,osgEarth::GeoPoint(g_pMapNode->getMapSRS(),g_mTranslation),image.get(),"",style);
	g_pMousePoint->setName("PlaceNode");
	g_pRoot->addChild(g_pMousePoint.get());

	//osgEarth::Util::EarthManipulator* manip = (osgEarth::Util::EarthManipulator*)g_pViewer->getCameraManipulator();
	//osg::Vec3d center = g_pMap->getProfile()->getExtent().bounds().center();
	 osgEarth::Bounds extent = g_pMap->getProfile()->getExtent().bounds();
	//double range = extent.width() > extent.height() ? extent.width() : extent.height();
	//range = range * 2;
	//osg::Vec3d eye = center;
	//osgEarth::Viewpoint vp(center, 0, -90, range);
	//eye.z() += range;
	////manip->setHomePosition(eye,center,osg::Vec3d(0,1,0));
	//manip->setViewpoint(vp); 
	osg::Vec3d geopos;
	g_pMapNode->getMapSRS()->transform(extent.center(),osgEarth::Registry::instance()->getGlobalGeodeticProfile()->getSRS(),geopos);
	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	params->Lon = geopos.x();
	params->Lat = geopos.y();

	QDir texDir("./out_texture_dir");
	removeDirectory(texDir);
	if(!texDir.exists())
	{
		texDir.mkdir(".");
	}
}


