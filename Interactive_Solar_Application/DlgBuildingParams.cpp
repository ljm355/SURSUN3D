/***************************************************************************
                         QgsMapTilerParamsDialog.cpp
        Creates a new Spatialite layer. This dialog borrows heavily from
        qgsnewvectorlayerdialog.cpp
                             -------------------
    begin                : 2010-03-18
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

//#include "stdafx.h"

#include "DlgBuildingParams.h"
#include <osgQt/QGraphicsViewAdapter>
#include <osgQt/QWidgetImage>
#include <osg/MatrixTransform>
#include <osg/ref_ptr>

DlgBuildingParams::DlgBuildingParams(QWidget *parent)
    : QDialog( parent)
{

	
	QVBoxLayout *mainLayout = new QVBoxLayout;
	//! [2] //! [3]
	//mainLayout->setMenuBar(menuBar);
	//! [3] //! [4]
	mainLayout->addWidget(createFormGroupBox1());
	//mainLayout->addWidget(createFormGroupBox2());
	//mainLayout->addWidget(createFormGroupBox3());
	//mainLayout->addWidget(createFormGroupBox4());
	//mainLayout->addWidget(createFormGroupBox5());
	//! [4] //! [5]
	setLayout(mainLayout);

	setWindowTitle(QString::fromLocal8Bit("Build properties"));

	g_pBuilding = NULL;
	updateParams(0);

}

DlgBuildingParams::~DlgBuildingParams()
{
  
}

QGroupBox* DlgBuildingParams::createBuildingProperties()
{

	QGroupBox* formGroupBox = new QGroupBox(tr("Building properties"));
	QFormLayout *layout = new QFormLayout;
	//formGroupBox->setStyleSheet("QLabel { background-color : rgba(255, 0, 0, 0); color : white; }");

	QLineEdit* buildingID = new QLineEdit("0");
	layout->addRow(new QLabel(QString::fromLocal8Bit("id")), buildingID);

	QLineEdit* pos = new QLineEdit("0");
	layout->addRow(new QLabel(QString::fromLocal8Bit("location")), pos);

	QLineEdit* roofArea = new QLineEdit("0");
	layout->addRow(new QLabel(QString::fromLocal8Bit("roof area")), roofArea);

	QLineEdit* facadeArea = new QLineEdit("0");
	layout->addRow(new QLabel(QString::fromLocal8Bit("facade area")), facadeArea);

	QLineEdit* totalArea = new QLineEdit("0");
	layout->addRow(new QLabel(QString::fromLocal8Bit("total area")), totalArea);

	QLineEdit* height = new QLineEdit("0");
	layout->addRow(new QLabel(QString::fromLocal8Bit("height")), height);
	formGroupBox->setLayout(layout);
    g_pID = buildingID;
    g_pPosition = pos;
	g_pRoofArea = roofArea;
	g_pFacadeArea = facadeArea;
	g_pTotalArea = totalArea;
	g_pBuildingHeight = height;
	return formGroupBox;
}

QGroupBox* DlgBuildingParams::createSufaceAnalysis()
{

	QGroupBox* formGroupBox = new QGroupBox(tr("Surface-based analysis"));
	QFormLayout *layout = new QFormLayout;

	QLineEdit* roofPotential = new QLineEdit("0");
	layout->addRow(new QLabel(QString::fromLocal8Bit("roof potential")), roofPotential);

	QLineEdit* facadePotential = new QLineEdit("0");
	layout->addRow(new QLabel(QString::fromLocal8Bit("facade potential")), facadePotential);

	QLineEdit* panelPotential = new QLineEdit("0");
	layout->addRow(new QLabel(QString::fromLocal8Bit("solar panel potential")), panelPotential);

	formGroupBox->setLayout(layout);
	g_pRoofPotentialSuface = roofPotential;
	g_pFacadePotentialSuface=facadePotential;
	g_pPanelPotentialSuface=panelPotential;
	return formGroupBox;
}



QGroupBox* DlgBuildingParams::createPointAnalysis()
{

	QGroupBox* formGroupBox = new QGroupBox(tr("Point-based analysis"));
	QFormLayout *layout = new QFormLayout;

	QLineEdit* potential = new QLineEdit("0");
	layout->addRow(new QLabel(QString::fromLocal8Bit("potential")), potential);

	//QLineEdit* facadePotential = new QLineEdit("0");
	//layout->addRow(new QLabel(QString::fromLocal8Bit("facade potential")), facadePotential);

	//QLineEdit* panelPotential = new QLineEdit("0");
	//layout->addRow(new QLabel(QString::fromLocal8Bit("solar panel potential")), panelPotential);

	g_pSolarPotentialPoint=potential;
	formGroupBox->setLayout(layout);
	return formGroupBox;

}



QGroupBox* DlgBuildingParams::createSolarPanelParams()
{
	//QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));


	QGroupBox* formGroupBox = new QGroupBox(tr("Solar panel properties"));
	QFormLayout *layout = new QFormLayout;

	QSpinBox* orient = new QSpinBox();
	orient->setRange(-360,360);
	orient->setValue(270);
	orient->setSingleStep(1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("solar panel orientation")), orient);

	QSpinBox* tilt = new QSpinBox();
	tilt->setRange(-90,90);
	tilt->setValue(45);
	tilt->setSingleStep(1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("solar panel tilt")), tilt);


	QSpinBox* posX = new QSpinBox();
	posX->setRange(-200,200);
	posX->setRange(-200,200);
	posX->setSingleStep(1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("solar panel offset X")), posX);


	QSpinBox* posY = new QSpinBox();
	posY->setRange(-200,200);
	posY->setRange(-200,200);
	posY->setSingleStep(1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("solar panel offset Y")), posY);

	QSpinBox* width = new QSpinBox();
	width->setRange(0,200);
	width->setRange(0,200);
	width->setSingleStep(1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("solar panel width")), width);

	QSpinBox* height = new QSpinBox();
	height->setRange(0,200);
	height->setRange(0,200);
	height->setSingleStep(1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("solar panel height")), height);



	formGroupBox->setLayout(layout);
	g_pOrientation=orient;
	g_pTilt=tilt;
	g_pHeight=height;
	g_pWidth=width;
	g_pOffsetX=posX;
	g_pOffsetY=posY;

	connect(tilt, SIGNAL(valueChanged(int)), this, SLOT(updateParams(int)));
	connect(orient, SIGNAL(valueChanged(int)), this, SLOT(updateParams(int)));
	connect(width, SIGNAL(valueChanged(int)), this, SLOT(updateParams(int)));
	connect(height, SIGNAL(valueChanged(int)), this, SLOT(updateParams(int)));
	connect(posX, SIGNAL(valueChanged(int)), this, SLOT(updateParams(int)));
	connect(posY, SIGNAL(valueChanged(int)), this, SLOT(updateParams(int)));
	return formGroupBox;
}



QGroupBox* DlgBuildingParams::createFormGroupBox1()
{

	//QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));


	QGroupBox* formGroupBox = new QGroupBox(tr("Building"));
	QFormLayout *layout = new QFormLayout;
	layout->add(createBuildingProperties());
	layout->add(createSolarPanelParams());
	//layout->add(createSufaceAnalysis());
	//layout->add(createPointAnalysis());

	formGroupBox->setLayout(layout);

	return formGroupBox;
}


void DlgBuildingParams::updateParams(int val)
{
	if(!g_pBuilding)
		return;
     SolarPanel& panel = g_pBuilding->Panel;
    
	panel.Tilt=g_pTilt->value();
	panel.Orientation=g_pOrientation->value();
	panel.OffsetX=g_pOffsetX->value();
	panel.OffsetY=g_pOffsetY->value();
	panel.Width=g_pWidth->value();
	panel.Height=g_pHeight->value();
	panel.updateMatrixTransform();
}

void DlgBuildingParams::setBuilding( Building* building )
{
   g_pBuilding = NULL;
   SolarPanel& panel = building->Panel;
   g_pTilt->setValue(panel.Tilt);
   g_pOrientation->setValue(panel.Orientation);
   g_pOffsetX->setValue(panel.OffsetX);
   g_pOffsetY->setValue(panel.OffsetY);
   g_pWidth->setValue(panel.Width);
   g_pHeight->setValue(panel.Height);

   g_pID->setText( QString("%1").arg(building->ID));
   g_pRoofArea->setText( QString("%1").arg(building->RoofArea));
   g_pFacadeArea->setText( QString("%1").arg(building->FacadeArea));
   g_pTotalArea->setText(QString::number(building->RoofArea+building->FacadeArea,'g',2));
   g_pBuildingHeight->setText( QString("%1").arg(building->Bound.zMax()));
   QString str = QString("%1, %2, %3").arg((int)building->Bound.center().x()).arg((int)building->Bound.center().y()).arg(0); 
   g_pPosition->setText(str);
   g_pBuilding = building;
 
}
