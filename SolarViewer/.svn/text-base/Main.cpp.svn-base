

#include "ViewerWidget2.h"
#include "osg\Node"
#include "osgDB\ReadFile"
#include "osgViewer\ViewerEventHandlers"
#include "osgGA\StateSetManipulator"
#include "ViewerWidget2.h"
#include "qapplication.h"
#include "CWainwindow.h"
#include "osgGA\TrackballManipulator"
#include "osgWidget\ScriptEngine"
#include "osgWidget\Widget"
#include "osgWidget\Box"
#include "osgWidget\WindowManager"
#include "osgWidget\ViewerEventHandlers"
#include "qstring.h"
#include "qfiledialog.h"
using namespace osg;

osg::Group* g_pRoot;
osgViewer::Viewer* g_pViewer;
//////////////////////////////////////////////////////////////////////////
//界面按钮基类
//////////////////////////////////////////////////////////////////////////
struct PanoWidget: public osgWidget::Widget {
	PanoWidget(double w,double h):
	osgWidget::Widget("", w,h) {
		setTexCoord(0.0, 0.0, osgWidget::Widget::LOWER_LEFT);
		setTexCoord(1.0, 0.0, osgWidget::Widget::LOWER_RIGHT);
		setTexCoord(1.0, 1.0, osgWidget::Widget::UPPER_RIGHT);
		setTexCoord(0.0, 1.0, osgWidget::Widget::UPPER_LEFT);
		setColor(1.0, 1.0, 1.0, 0.7);
		setPadRight(10.0);
		setEventMask(osgWidget::EVENT_ALL);

	}
	bool mouseRelease(double, double, const osgWidget::WindowManager*) {
		//QString filters("ive files (*.ive);;osg files (*.osg);;osgb files (*.osgb);;obj files (*.obj);;3ds files (*.3ds);;dae files (*.dae);;fbx files (*.fbx);;all files (*.*)");
		QString filters("osgb files (*.osgb);;ive files (*.ive);;osg files (*.osg);;all files (*.*)");
		QString filename = QFileDialog::getOpenFileName(0,QString::fromLocal8Bit("Export"),"./",filters);
		if(filename == "") 
			return true;
		if(g_pRoot->getNumChildren()>1)
		{
			g_pRoot->removeChild(1,1);
		}
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(filename.toLocal8Bit().data());
		g_pRoot->addChild(node.get());
		g_pViewer->home();
		return true;
	}
	bool mouseEnter(double, double, const osgWidget::WindowManager*) {
		addColor(-osgWidget::Color(0.8f,0.5f, 0.0f,  0.0f));
		return true;
	}

	bool mouseLeave(double, double, const osgWidget::WindowManager*) {
		addColor(osgWidget::Color(0.8f,0.5f,  0.0f, 0.0f));
		return true;
	}
};

//#pragma endregion
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    g_pViewer = new osgViewer::Viewer;
	//createMenuBox(&viewer);
	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
	if(!wsi)
	{
		return NULL;
	}
	unsigned int width, height;
	wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);
	g_pViewer->setCameraManipulator( new osgGA::TrackballManipulator );
	////viewer.getCamera()->setNearFarRatio(0.00002);
 //   viewer.getDatabasePager()->setDoPreCompile( true );
	g_pViewer->addEventHandler(new osgViewer::StatsHandler());
	g_pViewer->addEventHandler(new osgViewer::WindowSizeHandler());
	g_pViewer->addEventHandler(new osgViewer::ThreadingHandler());
	g_pViewer->addEventHandler(new osgViewer::LODScaleHandler());
	g_pViewer->addEventHandler(new osgGA::StateSetManipulator(g_pViewer->getCamera()->getOrCreateStateSet()));
	//g_Viewer->realize();
	g_pViewer->setUpViewInWindow(10,30,width-20,height-60);
	g_pRoot = new osg::Group();

	g_pViewer->setSceneData( g_pRoot );
	g_pViewer->realize();
	
	osg::ref_ptr<osg::Group> group=new osg::Group;
	const unsigned int mask = 0xF0000000;
	osg::ref_ptr<osgWidget::WindowManager> wm = new osgWidget::WindowManager(
		g_pViewer,
		width,
		height,
		mask,
		osgWidget::WindowManager::WM_USE_RENDERBINS
		);

	wm->setPointerFocusMode(osgWidget::WindowManager::PFM_SLOPPY);
	osg::ref_ptr<osgWidget::Box> box=new osgWidget::Box("menu");
	float w = 128;
	float h = 128;
	osg::ref_ptr<osgWidget::Widget> widget =  new PanoWidget(w,h);
	box->getBackground()->setColor(1.0, 1.0, 1.0, 0.0);
	box->setOrigin(width * 0.5 - w * 0.5,5);
	widget->setImage(("openButton.png"));
	widget->setName("Open");
	box->addWidget(widget.get());
	wm->addChild(box.get());
	//设置Viewer
	g_pViewer->addEventHandler(new osgWidget::MouseHandler(wm));
	g_pViewer->addEventHandler(new osgWidget::KeyboardHandler(wm));
	osg::Camera* camera=wm->createParentOrthoCamera();
	g_pRoot->addChild(camera);

	return g_pViewer->run();
  //  app.exec();
}
