#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QMainWindow>
#include "ui_mymainwindow.h"
#include "osgViewer\Viewer"

class CMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	CMainWindow(osgViewer::Viewer* viewer,osg::Group* root);
	~CMainWindow();	
private:
	osgViewer::Viewer* g_pViewer;
	osg::Group* g_pRoot;
	private slots:
		void on_OpenFile();
	
};

#endif // MYMAINWINDOW_H
