#ifndef VGEViewerWidget_H
#define VGEViewerWidget_H
#include <osgViewer/Viewer>
#include <osgViewer/api/Win32/GraphicsWindowWin32>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/EarthManipulator>
#include "osgEarthUtil/Ocean"
#include "qwidget.h"
#include "osgViewer/Renderer"
#include "osgEarthUtil/EarthManipulator"
class  VGEViewerWidget : public QWidget
{
	Q_OBJECT
public:
	VGEViewerWidget(QWidget* parent);
	~VGEViewerWidget();
	void resetView();
	virtual void frame();
	virtual void setSize(int x,int y,int width,int height);
	virtual osgViewer::Viewer* getViewer();
	void setFullScreenOn( const bool& val );
	bool getFullScreenOn(){return g_mIsFullScreen;}
protected:
	osgViewer::Viewer* g_pViewer;
	osgViewer::GraphicsWindowWin32* g_pWin;
	int g_mWidth, g_mHeight;
	bool g_mIsFullScreen;
	WId g_mParentHandle;
	bool m_Initilized;
    osg::ref_ptr<osgEarth::Util::EarthManipulator> g_pEarthManipulator;
protected:
	void initViewer();
};

#endif