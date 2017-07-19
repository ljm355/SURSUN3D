#include "CMyDataManager.h"
#include <osgEarthUtil/FeatureManipTool>
static FeatureManipTool* s_manipTool;

static VBox* s_state_normal;
static VBox* s_state_active;

// Callback to toggle the visibility of the save/cancel buttons based on tool state
struct ToggleUIStateCallback : public FeatureQueryTool::Callback
{
	// called when a valid feature is found under the mouse coords
	virtual void onHit( FeatureSourceIndexNode* index, FeatureID fid, const EventArgs& args )
	{
		s_state_active->setVisible( true );
	}

	// called when no feature is found under the mouse coords
	virtual void onMiss( const EventArgs& args )
	{
		s_state_active->setVisible( false );
	}
};


// Cancels the manipulation when user clicks "cancel"
struct OnCancel : public ControlEventHandler
{
	void onClick( Control* control )
	{
		s_manipTool->cancel();
		s_state_active->setVisible( false );
	}
};


// Commits the manipulation when user clicks "save"
struct OnSave : public ControlEventHandler
{
	void onClick( Control* saveButton )
	{
		s_manipTool->commit();
		s_state_active->setVisible( false );
	}
};


// creaes a simple user interface for the manip demo
Control*
createUI()
{
	HBox* vbox = new HBox();
	vbox->addControl( new LabelControl("Feature Manipulator Demo", Color::Yellow) );

	s_state_normal = vbox->addControl(new VBox());
	s_state_normal->addControl( new LabelControl("Shift-click on a feature to enter edit mode.") );

	s_state_active = vbox->addControl(new VBox());
	s_state_active->setVisible( false );
	s_state_active->addControl( new LabelControl("Drag the handles to position or rotation the feature.") );

	HBox* buttons = s_state_active->addControl(new HBox());

	LabelControl* cancel = buttons->addControl(new LabelControl("cancel"));
	cancel->setBackColor(Color(Color::White,0.5));
	cancel->setActiveColor(Color::Blue);
	cancel->addEventHandler(new OnCancel());
	cancel->setPadding( 5.0f );
	cancel->setVertFill( true );

	LabelControl* save = buttons->addControl(new LabelControl("save"));
	save->setBackColor(Color(Color::White,0.5));
	save->setActiveColor(Color::Blue);
	save->addEventHandler(new OnSave());
	save->setPadding( 5.0f );
	save->setMargin(Control::SIDE_LEFT, 20.0f);
	save->setVertFill( true );

	vbox->setMargin( Control::SIDE_RIGHT, 150.0f );
	return vbox;
} 

CMyDataManager::CMyDataManager(osgEarth::MapNode* mapnode,osgViewer::Viewer* viewer,osg::Group* root,QMainWindow* mainWin)
{


	osg::DisplaySettings::instance()->setMinimumNumStencilBits( 8 );
	_viewer = viewer;
	_map = mapnode->getMap();
	_mapNode = mapnode;
	_root = root;


	//s_manipTool = new FeatureManipTool( _mapNode );
	//_viewer->addEventHandler( s_manipTool );

	//s_manipTool->addCallback( new ToggleUIStateCallback() );
	//// create a surface to house the controls
	//ControlCanvas* cs = ControlCanvas::get( _viewer );
	//_root->addChild( cs );
	////cs->addControl(createUI());
	////
	////return;
	//_annoGroup = new osg::Group();
	//_root->addChild( _annoGroup );

	////A group for all the editors
	//_editorGroup = new osg::Group;
	//_root->addChild( _editorGroup );
	//_editorGroup->setNodeMask( 0 );


	//VBox* vbox = new VBox();
	//vbox->addControl( new LabelControl("Feature Manipulator Demo", Color::Yellow) );

	//s_state_normal = vbox->addControl(new VBox());
	//s_state_normal->addControl( new LabelControl("Shift-click on a feature to enter edit mode.") );

	//s_state_active = vbox->addControl(new VBox());
	//s_state_active->setVisible( false );
	//s_state_active->addControl( new LabelControl("Drag the handles to position or rotation the feature.") );

	//HBox* buttons = s_state_active->addControl(new HBox());

	//LabelControl* cancel = buttons->addControl(new LabelControl("cancel"));
	//cancel->setBackColor(Color(Color::White,0.5));
	//cancel->setActiveColor(Color::Blue);
	//cancel->addEventHandler(new OnCancel());
	//cancel->setPadding( 5.0f );
	//cancel->setVertFill( true );

	//LabelControl* save = buttons->addControl(new LabelControl("save"));
	//save->setBackColor(Color(Color::White,0.5));
	//save->setActiveColor(Color::Blue);
	//save->addEventHandler(new OnSave());
	//save->setPadding( 5.0f );
	//save->setMargin(Control::SIDE_LEFT, 20.0f);
	//save->setVertFill( true );

	//vbox->setMargin( Control::SIDE_RIGHT, 150.0f );


	///*HBox* box = new HBox();    
	//box->setChildSpacing( 5 );*/
	////Add a toggle button to toggle editing
	//CheckBoxControl* editCheckbox = new CheckBoxControl( false );    
	//editCheckbox->addEventHandler( new ToggleNodeHandler( _editorGroup ) );
	//vbox->addControl( editCheckbox );
	//LabelControl* labelControl = new LabelControl( "Edit Annotations" );
	//labelControl->setFontSize( 24.0f );
	//vbox->addControl( labelControl  );
	//vbox->setMargin( Control::SIDE_LEFT, 15.0f );
	//cs->addControl( vbox );


	m_ShapeManager = new CObstacleManager(_mapNode,_viewer);
	//osg::Group* testGroup = new osg::Group();
	//testGroup->setName("liangjm");
    //m_ShapeManager->addItem(testGroup);

	m_RouteManager = new ILayerManager(_mapNode,_viewer);
	//osg::Group* testGroup2 = new osg::Group();
	//testGroup2->setName("liangjm");
	//m_RouteManager->addItem(testGroup2);

	_root->addChild(m_ShapeManager->getLayerNode());
	_root->addChild(m_RouteManager->getLayerNode());
	_root->addChild(m_ShapeManager->getPlaceMarkNode());
	_root->addChild(m_RouteManager->getPlaceMarkNode());


	//m_ShapeManager->setModel(new QStringListModel(m_ShapeNameList));
	//m_RouteManager = new QListView;*/
	QDockWidget *annoDock = new QDockWidget;
	annoDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	annoDock->setWidget((QWidget*)m_ShapeManager);
	mainWin->addDockWidget(Qt::LeftDockWidgetArea, annoDock);

	QDockWidget *annoDock2 = new QDockWidget;
	annoDock2->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	annoDock2->setWidget((QWidget*)m_RouteManager);
	mainWin->addDockWidget(Qt::LeftDockWidgetArea, annoDock2);


//return;
//	// Make a group for 2D items, and activate the decluttering engine. Decluttering
//	// will migitate overlap between elements that occupy the same screen real estate.
//	_labelGroup = new osg::Group();
//	Decluttering::setEnabled( _labelGroup->getOrCreateStateSet(), true );
//	_annoGroup->addChild( _labelGroup );
//
//
//	// set up a style to use for labels:
//	Style placeStyle;
//	placeStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
//
//	Style labelStyle;
//	labelStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_CENTER;
//	labelStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::Yellow;
//	labelStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
//
//
//
//	//CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","NO");
//	placeStyle.getOrCreate<TextSymbol>()->encoding() = TextSymbol::ENCODING_UTF8;
//	placeStyle.getOrCreate<TextSymbol>()->font() = std::string("fonts/SimHei.ttf");
//
//	labelStyle.getOrCreate<TextSymbol>()->encoding() = TextSymbol::ENCODING_UTF8;
//	labelStyle.getOrCreate<TextSymbol>()->font() = std::string("fonts/SimHei.ttf");
//
//	// A lat/long SRS for specifying points.
//	const SpatialReference* geoSRS = _mapNode->getMapSRS()->getGeographicSRS();
//
//	//--------------------------------------------------------------------
//	// A series of place nodes (an icon with a text label)
//	{	
//		osg::Image* pin = osgDB::readImageFile( "../data/placemark32.png" );
//
//		//std::vector<
//		//_labelGroup->addChild( new PlaceNode(_mapNode, GeoPoint(geoSRS, -74.00, 40.71), pin, s, placeStyle) );
//
//		_labelGroup->addChild( new PlaceNode(_mapNode, GeoPoint(geoSRS, -77.04, 38.85), pin, convertUTF16toUTF8(L"Å¦Ô¼"), placeStyle) );
//		_labelGroup->addChild( new PlaceNode(_mapNode, GeoPoint(geoSRS, -87.65, 41.90), pin, convertUTF16toUTF8(L"Ö¥¼Ó¸ç"),placeStyle) );
//		_labelGroup->addChild( new PlaceNode(_mapNode, GeoPoint(geoSRS,-118.40, 33.93), pin, convertUTF16toUTF8(L"ÂåÉ¼í¶"),placeStyle) );
//		_labelGroup->addChild( new PlaceNode(_mapNode, GeoPoint(geoSRS, -71.03, 42.37), pin, convertUTF16toUTF8(L"²¨Ê¿¶Ù"),placeStyle) );
//		_labelGroup->addChild( new PlaceNode(_mapNode, GeoPoint(geoSRS,-157.93, 21.35), pin, convertUTF16toUTF8(L"»ðÅ«Â³Â³"),placeStyle) );
//		_labelGroup->addChild( new PlaceNode(_mapNode, GeoPoint(geoSRS, 139.75, 35.68), pin, convertUTF16toUTF8(L"¶«¾©"),placeStyle) );
//		_labelGroup->addChild( new PlaceNode(_mapNode, GeoPoint(geoSRS, -90.25, 29.98), pin, convertUTF16toUTF8(L"ÐÂ°Â¶ûÁ¼"),placeStyle) );
//		_labelGroup->addChild( new PlaceNode(_mapNode, GeoPoint(geoSRS, -80.28, 25.82), pin, convertUTF16toUTF8(L"Âõ°¢ÃÜ"),placeStyle) );
//		_labelGroup->addChild( new PlaceNode(_mapNode, GeoPoint(geoSRS,-117.17, 32.72), pin, convertUTF16toUTF8(L"Ê¥µØÑÇ¸ç"),placeStyle) );
//	}
//
//	//--------------------------------------------------------------------
//
//	// a box that follows lines of latitude (rhumb line interpolation, the default)
//	{
//		Geometry* geom = new osgEarth::Symbology::Polygon();
//		geom->push_back( osg::Vec3d(0,   40, 0) );
//		geom->push_back( osg::Vec3d(-60, 40, 0) );
//		geom->push_back( osg::Vec3d(-60, 60, 0) );
//		geom->push_back( osg::Vec3d(0,   60, 0) );
//		Style geomStyle;
//		geomStyle.getOrCreate<LineSymbol>()->stroke()->color() = Color::Cyan;
//		geomStyle.getOrCreate<LineSymbol>()->stroke()->width() = 5.0f;
//		geomStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
//		FeatureNode* gnode = new FeatureNode(_mapNode, new Feature(geom, geoSRS, geomStyle));
//		_annoGroup->addChild( gnode );
//
//		_labelGroup->addChild( new LabelNode(_mapNode, GeoPoint(geoSRS,-30, 50), "Rhumb line polygon", labelStyle) );
//	}
//
//	//--------------------------------------------------------------------
//
//	// another rhumb box that crosses the antimeridian
//	{
//		Geometry* geom = new osgEarth::Symbology::Polygon();
//		geom->push_back( -160., -30. );
//		geom->push_back(  150., -20. );
//		geom->push_back(  160., -45. );
//		geom->push_back( -150., -40. );
//		Style geomStyle;
//		geomStyle.getOrCreate<LineSymbol>()->stroke()->color() = Color::Lime;
//		geomStyle.getOrCreate<LineSymbol>()->stroke()->width() = 3.0f;
//		geomStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
//		FeatureNode* gnode = new FeatureNode(_mapNode, new Feature(geom, geoSRS, geomStyle));
//		_annoGroup->addChild( gnode );
//
//		_labelGroup->addChild( new LabelNode(_mapNode, GeoPoint(geoSRS, -175, -35), "Antimeridian polygon", labelStyle) );
//	}
//
//	//--------------------------------------------------------------------
//
//	// A path using great-circle interpolation.
//	{
//		Geometry* path = new LineString();
//		path->push_back( osg::Vec3d(-74, 40.714, 0) );   // New York
//		path->push_back( osg::Vec3d(139.75, 35.68, 0) ); // Tokyo
//
//		Style pathStyle;
//		pathStyle.getOrCreate<LineSymbol>()->stroke()->color() = Color::Red;
//		pathStyle.getOrCreate<LineSymbol>()->stroke()->width() = 3.0f;
//		pathStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
//
//		Feature* pathFeature = new Feature(path, geoSRS, pathStyle);
//		pathFeature->geoInterp() = GEOINTERP_GREAT_CIRCLE;
//
//		//OE_INFO << "Path extent = " << pathFeature->getExtent().toString() << std::endl;
//
//		FeatureNode* pathNode = new FeatureNode(_mapNode, pathFeature);
//		_annoGroup->addChild( pathNode );
//
//		_labelGroup->addChild( new LabelNode(_mapNode, GeoPoint(geoSRS,-170, 61.2), "Great circle path", labelStyle) );
//	}
//
//	//--------------------------------------------------------------------
//
//	// A circle around New Orleans.
//	{
//		Style circleStyle;
//		circleStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Cyan, 0.8);
//
//		ExtrusionSymbol* extrusion = circleStyle.getOrCreate<ExtrusionSymbol>();
//		extrusion->heightExpression() = NumericExpression(350000);
//		extrusion->flatten() = true;
//
//		CircleNode* circle = new CircleNode(
//			_mapNode, 
//			GeoPoint(geoSRS, -90.25, 29.98, 1000., ALTMODE_RELATIVE),
//			Linear(300, Units::KILOMETERS ),
//			circleStyle,
//			false );
//		_annoGroup->addChild( circle );        
//
//		_editorGroup->addChild( new CircleNodeEditor( circle ) );
//	}
//
//	//--------------------------------------------------------------------
//
//	// An draped ellipse around Miami.
//	{
//		Style ellipseStyle;
//		ellipseStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Orange, 0.75);
//		EllipseNode* ellipse = new EllipseNode(
//			_mapNode, 
//			GeoPoint(geoSRS, -80.28, 25.82, 0.0, ALTMODE_RELATIVE),
//			Linear(500, Units::MILES),
//			Linear(100, Units::MILES),
//			Angular(0, Units::DEGREES),
//			ellipseStyle,
//			true );
//		_annoGroup->addChild( ellipse );
//		_editorGroup->addChild( new EllipseNodeEditor( ellipse ) );
//	}
//
//	{
//		// A rectangle around San Diego
//		Style rectStyle;
//		rectStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Green, 0.5);
//		RectangleNode* rect = new RectangleNode(
//			_mapNode, 
//			GeoPoint(geoSRS, -117.172, 32.721),
//			Linear(300, Units::KILOMETERS ),
//			Linear(600, Units::KILOMETERS ),
//			rectStyle,
//			true );
//		_annoGroup->addChild( rect );
//
//		_editorGroup->addChild( new RectangleNodeEditor( rect ) );
//	}
//
//
//
//	//--------------------------------------------------------------------
//
//	// An extruded polygon roughly the shape of Utah. Here we demonstrate the
//	// FeatureNode, where you create a geographic geometry and use it as an
//	// annotation.
//	{
//		Geometry* utah = new osgEarth::Symbology::Polygon();
//		utah->push_back( -114.052, 37.0   );
//		utah->push_back( -109.054, 37.0   );
//		utah->push_back( -109.054, 41.0   );
//		utah->push_back( -111.040, 41.0   );
//		utah->push_back( -111.080, 42.059 );
//		utah->push_back( -114.080, 42.024 );
//
//		Style utahStyle;
//		utahStyle.getOrCreate<ExtrusionSymbol>()->height() = 250000.0; // meters MSL
//		utahStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::White, 0.8);
//
//		Feature*     utahFeature = new Feature(utah, geoSRS, utahStyle);
//		FeatureNode* featureNode = new FeatureNode(_mapNode, utahFeature);
//		_annoGroup->addChild( featureNode );
//	}
//
//	//--------------------------------------------------------------------
//
//	// an image overlay
//	{
//		ImageOverlay* imageOverlay = 0L;
//		osg::Image* image = osgDB::readImageFile( "../data/USFLAG.TGA" );
//		if ( image )
//		{
//			imageOverlay = new ImageOverlay(_mapNode, image);
//			imageOverlay->setBounds( Bounds( -100.0, 35.0, -90.0, 40.0) );
//			_annoGroup->addChild( imageOverlay );
//
//			_editorGroup->addChild( new ImageOverlayEditor( imageOverlay ) );
//		}
//	}
//
//	//--------------------------------------------------------------------
//
//	// install decoration. These change the appearance of an Annotation
//	// based on some user action.
//
//	// highlight annotation upon hover by default:
//
//	DecorationInstaller highlightInstaller("hover", new HighlightDecoration());
//	_annoGroup->accept( highlightInstaller );
//
//	// scale labels when hovering:
//	DecorationInstaller scaleInstaller("hover", new ScaleDecoration(1.1f));
//	_labelGroup->accept( scaleInstaller );
//
//	// install an event handler for picking and hovering.
//	AnnotationEventCallback* cb = new AnnotationEventCallback();
//	cb->addHandler( new MyAnnoEventHandler() );
//
//	_annoGroup->addEventCallback( cb );
//
//	// testing:
//	Config annoConfig = AnnotationRegistry::instance()->getConfig( _annoGroup );
//	_mapNode->externalConfig().add(annoConfig);




}


CMyDataManager::~CMyDataManager(void)
{



}

void CMyDataManager::addShape()
{


}