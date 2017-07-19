#pragma once
#include "common.h"
#include "ILayerManager.h"
#include "ObstacleManager.h"
class CMyDataManager
{
public:
	CMyDataManager(osgEarth::MapNode* mapnode,osgViewer::Viewer* viewer,osg::Group* root,QMainWindow* mainWin);
	~CMyDataManager(void);
	void addShape();
private:
	osgEarth::Map* _map;
	osg::Group* _labelGroup;
	osg::Group* _editorGroup;
	osg::Group* _annoGroup;
	osg::Group* _root;
	osgEarth::MapNode* _mapNode;
	osgViewer::Viewer* _viewer;
	CObstacleManager* m_ShapeManager;
	ILayerManager* m_RouteManager;
	QStringList m_ShapeNameList;
};
