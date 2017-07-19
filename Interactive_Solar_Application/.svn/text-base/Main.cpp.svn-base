

#include "CWainwindow.h"
#include "osgUtil\Optimizer"
/** Helper base class for implementing Optimizer techniques.*/
class InstanceGeometryVisitor : public osg::NodeVisitor
{
public:

	InstanceGeometryVisitor():
	  osg::NodeVisitor(osg::NodeVisitor::NODE_VISITOR,osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	  {
		  setNodeMaskOverride(0xffffffff);
	  }
	  void apply( osg::Geode& node )
	  {
		  node.getOrCreateStateSet()->setDataVariance(osg::Object::DYNAMIC);
		  for (int i=0;i<node.getNumDrawables();i++)
		  {
			  node.getDrawable(i)->getOrCreateStateSet()->setDataVariance(osg::Object::STATIC);
		  }
	  }

};



int
main(int argc, char** argv)
{

	/*osg::Node* atlasNode = osgDB::readNodeFile("atlas.osg");
	InstanceGeometryVisitor geoVisitor;
	atlasNode->accept(geoVisitor);
	osgUtil::Optimizer::TextureAtlasVisitor atlasVisitor;
	atlasNode->accept(atlasVisitor);
	atlasVisitor.optimize();
	osgDB::writeNodeFile(*atlasNode,"atlas2.osgb");
	return 0;*/
    QApplication app(argc, argv);

    CMainWindow win;
	
    //win.setGeometry(100, 100, 1024, 800);
    win.statusBar()->showMessage(QString::fromLocal8Bit("Interactive Application for Urban Solar Radiation"));
	app.setWindowIcon(QIcon("./images/logo.png"));
 //   win.showFullScreen();
	win.showMaximized();
    app.exec();
}
