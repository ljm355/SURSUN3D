#include "VGEViewerWidget.h"
#include "osgViewer\ViewerEventHandlers"
#include "osgGA\StateSetManipulator"

VGEViewerWidget::VGEViewerWidget(QWidget* parent)
	:QWidget(parent)

{
	g_mParentHandle = this->winId();
	g_pWin = NULL;
	g_pViewer = NULL;
	g_mIsFullScreen = false;
	m_Initilized =false;
	initViewer();
}	
VGEViewerWidget::~VGEViewerWidget()
{

}

void VGEViewerWidget::resetView()
{
	g_pEarthManipulator = new osgEarth::Util::EarthManipulator();
	g_pViewer->setCameraManipulator(g_pEarthManipulator.get());
}
void VGEViewerWidget::initViewer()
{
	if(m_Initilized)
		return;
	if(g_mParentHandle <= 0)
		return;
	g_pViewer = new osgViewer::Viewer;
	g_pViewer->setThreadingModel(osgViewer::Viewer::DrawThreadPerContext);
	g_pEarthManipulator = new osgEarth::Util::EarthManipulator();
	g_pViewer->setCameraManipulator(g_pEarthManipulator.get());
	g_pViewer->addEventHandler(new osgViewer::StatsHandler);
	g_pViewer->addEventHandler(new osgGA::StateSetManipulator());
	g_pViewer->addEventHandler(new osgViewer::ThreadingHandler());

	g_pViewer->setKeyEventSetsDone(0);
	g_pViewer->setQuitEventSetsDone(false);

	g_pViewer->setUpViewAcrossAllScreens();
	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
	osg::DisplaySettings* displaySettings = osg::DisplaySettings::instance().get();

	osg::GraphicsContext::ScreenIdentifier si;
	si.readDISPLAY();

	if(si.displayNum<0) si.displayNum = 0;

	unsigned int wid, hei;
	wsi->getScreenResolution(si,wid,hei);
	g_mWidth = wid;
	g_mHeight = hei;
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(displaySettings);
	traits->hostName = si.hostName;
	traits->displayNum = si.displayNum;
	traits->screenNum = si.screenNum;
	traits->x = 0;
	traits->y = 0;
	traits->width = wid;
	traits->height = hei;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->quadBufferStereo = true;  
	traits->setInheritedWindowPixelFormat = true;
	HWND hwnd = (HWND)g_mParentHandle;
	//osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(hwnd);
	// traits->inheritedWindowData = windata;
	osg::Camera* camera = g_pViewer->getCamera();	
	//osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	//GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	camera->setDrawBuffer(GL_BACK);
	camera->setReadBuffer(GL_BACK);

	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	camera->setGraphicsContext(gc.get());

	camera->setViewport(new osg::Viewport(0, 0,wid, hei));

	double fovy, aspectRatio, zNear, zFar;
	camera->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
	double newAspectRatio = double(traits->width) / double(traits->height);
	double aspectRatioChange = newAspectRatio / aspectRatio;
	camera->getProjectionMatrix() *= osg::Matrix::scale(1.0/aspectRatioChange,1.0,1.0);


	camera->setNearFarRatio(0.00002);
	camera->setSmallFeatureCullingPixelSize(-1.0f);
	g_pViewer->setCamera(camera);
	g_pViewer->realize();

	osgViewer::ViewerBase::Windows _windows;
	g_pViewer->getWindows(_windows);
	g_pWin = dynamic_cast<osgViewer::GraphicsWindowWin32*>(_windows[0]);
	g_pWin->setWindowDecoration(false);

	::SetParent(g_pWin->getHWND(),hwnd);
	osg::DisplaySettings::instance()->setMinimumNumStencilBits(8);
	displaySettings->setStereo(false);

	m_Initilized = true;


}
void VGEViewerWidget::frame()
{
	if(!g_pViewer)
		return;
	if ((g_pViewer->getRunFrameScheme() == osgViewer::ViewerBase::CONTINUOUS || 
		g_pViewer->checkNeedToDoFrame()))
	{	
		setSize(0,0,this->width(),this->height());
		g_pViewer->frame();
	}
}
void VGEViewerWidget::setSize(int x,int y,int width,int height)
{

	if(g_mIsFullScreen)
	{
		width = g_mWidth;
		height = g_mHeight;
	}
	if(!g_pWin)
		return;
	int oldx,oldy,oldH,oldW;
	g_pWin->getWindowRectangle(oldx,oldy,oldW,oldH);
	if( oldW != width || oldH != height)
	{
		g_pWin->setWindowRectangle(0,0,width,height);

	}
}

void VGEViewerWidget::setFullScreenOn( const bool& val )
{

	if(val)
	{
		g_mIsFullScreen = true;
		setSize(0,0,g_mWidth,g_mHeight);
		::SetParent(g_pWin->getHWND(),NULL);
	}
	else
	{	
		g_mIsFullScreen = false;
		//setSize(0,0,g_mWidth,g_mHeight);
		::SetParent(g_pWin->getHWND(),(HWND)g_mParentHandle);
	}

}

osgViewer::Viewer* VGEViewerWidget::getViewer()
{
	return g_pViewer;
}
