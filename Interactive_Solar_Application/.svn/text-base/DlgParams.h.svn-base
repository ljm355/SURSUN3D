/***************************************************************************
                         QgsMapTilerParamsDialog.h  -  description
                             -------------------
    begin                : 2010-03-19
    copyright            : (C) 2010 by Gary Sherman
    email                : gsherman@mrcc.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef DlgPathSetting_H
#define DlgPathSetting_H
#include <QtGui>

#include "DlgRUNParams.h"
#include "qradiobutton.h"
#include "osg\Node"
class DlgParams: public QDialog
{
    Q_OBJECT

public:
    DlgParams(QWidget *parent = 0);
    ~DlgParams();	
signals:
	
private slots:
	void onParamsChanged(int val);
	void onParamsChanged( double val );
	void onSelectionModeChanged(bool checked);
	void buttonClicked();
	void onSetRSUNParams();
	void onExecute();
	void onExport();
private:
    //QGroupBox* createGroupParameters();
	QGroupBox* createGroupAnalysisMode();
	QGroupBox* createGroupBuildingParts();
	QGroupBox* createGroupQueryMode();
	QGroupBox* createGroupControl();
	QSpinBox* g_pRadius;
	QRadioButton* g_pCircleSelMode;
	QRadioButton* g_pPointSelMode;
	QRadioButton* g_pOffSelMode;

	QRadioButton* g_pSurfaceAnalysisMode;
	QRadioButton* g_pPointAnalysisMode;

	QCheckBox* g_pRooftopCheck;
	QCheckBox* g_pFacadeCheck;
	QCheckBox* g_pSolarPanelCheck;
	//QSpinBox* g_pFirstDay;
	//QSpinBox* g_pLastDay;
	//QSpinBox* g_pYear;
	//QDoubleSpinBox* g_pLinke;
	//QDoubleSpinBox* g_pElevation;
	//QDoubleSpinBox* g_pBSKY;
 //   QDoubleSpinBox* g_pDSKY;
	//QDoubleSpinBox* g_pTimeStep;
	//QCheckBox* g_pInstaneousCheck;
	//QSpinBox* g_pDay;
	//QSpinBox* g_pHour;
	//QSpinBox* g_pMinute;
	//QSpinBox* g_pSecond;
	DlgRUNParams g_mDlgRUN;
};

#endif // QGSNEWVECTORLAYERDIALOG_H
