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
#ifndef DlgBuildingParams_H
#define DlgBuildingParams_H
#include <QtGui>
#include "City.h"

class DlgBuildingParams: public QDialog
{
    Q_OBJECT

public:
    DlgBuildingParams(QWidget *parent = 0);
    ~DlgBuildingParams();	
	void setBuilding(Building* building);
	Building* getBuilding(){return g_pBuilding;}
protected:
    QGroupBox* createFormGroupBox1();
public Q_SLOTS:
	void updateParams(int val);
	QGroupBox* createSolarPanelParams();
	QGroupBox* createPointAnalysis();
	QGroupBox* createSufaceAnalysis();
	QGroupBox* createBuildingProperties();
private:	
	Building* g_pBuilding;
	QSpinBox* g_pOrientation;
	QSpinBox* g_pTilt;
	QSpinBox* g_pOffsetX;
	QSpinBox* g_pOffsetY;
	QSpinBox* g_pWidth;
	QSpinBox* g_pHeight;
	QLineEdit* g_pPosition;
	QLineEdit* g_pRoofArea;
	QLineEdit* g_pFacadeArea;
	QLineEdit* g_pTotalArea;
	QLineEdit* g_pBuildingHeight;
	QLineEdit* g_pID;

	QLineEdit* g_pRoofPotentialSuface;
	QLineEdit* g_pFacadePotentialSuface;
	QLineEdit* g_pPanelPotentialSuface;

	QLineEdit* g_pSolarPotentialPoint;



};

#endif // QGSNEWVECTORLAYERDIALOG_H
