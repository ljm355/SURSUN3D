#include "CWainwindow.h"
#include "qfiledialog.h"
#include "osgDB\ReadFile"

CMainWindow::CMainWindow(osgViewer::Viewer* viewer,osg::Group* root)
	: QMainWindow(NULL),
	g_pViewer(viewer),
	g_pRoot(root)

{
	QMenu* fileMenu = this->menuBar()->addMenu(("File"));
	QAction* openAction = new QAction("Open",this);
	fileMenu->addAction(openAction);
	connect(openAction,SIGNAL(triggered()),this,SLOT(on_OpenFile()));
}


CMainWindow::~CMainWindow()
{

}

void CMainWindow::on_OpenFile()
{
	//QString filters("ive files (*.ive);;osg files (*.osg);;osgb files (*.osgb);;obj files (*.obj);;3ds files (*.3ds);;dae files (*.dae);;fbx files (*.fbx);;all files (*.*)");
	QString filters("osgb files (*.osgb);;ive files (*.ive);;osg files (*.osg);;all files (*.*)");
	QString filename = QFileDialog::getOpenFileName(0,QString::fromLocal8Bit("Export"),"./",filters);
	if(filename == "") return;
	g_pRoot->removeChild(0,g_pRoot->getNumChildren());
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(filename.toLocal8Bit().data());
	g_pRoot->addChild(node.get());
}
