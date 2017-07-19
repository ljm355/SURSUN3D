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

#include "DlgParams.h"
#include "InteractiveImageHandler.h"
#include <osgQt/QGraphicsViewAdapter>
#include <osgQt/QWidgetImage>
#include <osg/MatrixTransform>
#include <osg/ref_ptr>
#include "IGlobalInterfaces.h"

DlgParams::DlgParams(QWidget *parent)
    : QDialog( parent)
{

	
	QVBoxLayout *mainLayout = new QVBoxLayout;
	//! [2] //! [3]
	//mainLayout->setMenuBar(menuBar);
	//! [3] //! [4]
	//mainLayout->addWidget(createGroupParameters());

	QPushButton* btnRSUN = new QPushButton;
    btnRSUN->setText("r.sun parameters");

	btnRSUN->setGeometry(0,0,128,64);
	btnRSUN->setMinimumSize(128,64);btnRSUN->setMaximumSize(128,64);;	
	btnRSUN->setGeometry(0,0,128,64);


	connect(btnRSUN, SIGNAL(released()), this, SLOT(onSetRSUNParams()));
	mainLayout->addWidget(btnRSUN);
	mainLayout->addWidget(createGroupAnalysisMode());
	mainLayout->addWidget(createGroupBuildingParts());
	mainLayout->addWidget(createGroupQueryMode());
	mainLayout->addWidget(createGroupControl());
	//! [4] //! [5]
	setLayout(mainLayout);

	setWindowTitle(QString::fromLocal8Bit("Parameters for r.sun"));

    int val = 0;
	double dval = 0;
	onParamsChanged(val);
	onParamsChanged(dval);
	onSelectionModeChanged(true);
	buttonClicked();

}


DlgParams::~DlgParams()
{
  
}

//
//QGroupBox* DlgParams::createGroupParameters()
//{
//
//	//QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
//
//
//	QGroupBox* formGroupBox = new QGroupBox(tr("GRASS GIS r.sun"));
//	QFormLayout *layout = new QFormLayout;
//	//formGroupBox->setStyleSheet("QLabel { background-color : rgba(255, 0, 0, 0); color : white; }");
//
//
//	QDoubleSpinBox* qspinLinkie = new QDoubleSpinBox();
//	qspinLinkie->setRange(1,10);
//	qspinLinkie->setValue(3);
//	qspinLinkie->setSingleStep(0.1);
//	layout->addRow(new QLabel(QString::fromLocal8Bit("linkein")), qspinLinkie);
//	g_pLinke = qspinLinkie;
//
//	g_pElevation = new QDoubleSpinBox();
//	g_pElevation->setRange(-1000,10000);
//	g_pElevation->setValue(0);
//	g_pElevation->setSingleStep(10);
//	layout->addRow(new QLabel(QString::fromLocal8Bit("elevation")), g_pElevation);
//
//	QDoubleSpinBox* qspinCoefbh = new QDoubleSpinBox();
//	qspinCoefbh->setRange(0,2);
//	qspinCoefbh->setValue(1);
//	qspinCoefbh->setSingleStep(0.1);
//	layout->addRow(new QLabel(QString::fromLocal8Bit("coefbh")), qspinCoefbh);
//	g_pBSKY = qspinCoefbh;
//
//	QDoubleSpinBox* qspinCoefdh = new QDoubleSpinBox();
//	qspinCoefdh->setRange(0,2);
//	qspinCoefdh->setValue(1);
//	qspinCoefdh->setSingleStep(0.1);
//	layout->addRow(new QLabel(QString::fromLocal8Bit("coefdh")), qspinCoefdh);
//	g_pDSKY = qspinCoefdh;
//
//
//
//	QSpinBox* qspinFirstDay = new QSpinBox();
//	qspinFirstDay->setRange(1,365);
//	qspinFirstDay->setValue(1);
//	layout->addRow(new QLabel(QString::fromLocal8Bit("start day")), qspinFirstDay);
//
//	QSpinBox* qspinLastDay = new QSpinBox();
//	qspinLastDay->setRange(1,365);
//	qspinLastDay->setValue(1);
//	layout->addRow(new QLabel(QString::fromLocal8Bit("end day")), qspinLastDay);
//
//
//	QDoubleSpinBox* qspinStep= new QDoubleSpinBox();
//	qspinStep->setRange(0,2);
//	qspinStep->setValue(1);
//	qspinStep->setSingleStep(0.1);
//	layout->addRow(new QLabel(QString::fromLocal8Bit("step")), qspinStep);
//
//
//	g_pInstaneousCheck = new QCheckBox();
//	g_pInstaneousCheck->setChecked(false);
//	layout->addRow(new QLabel(QString::fromLocal8Bit("instaneous mode")), g_pInstaneousCheck);
//
//
//	g_pHour = new QSpinBox();
//	g_pHour->setRange(0,23);
//	g_pHour->setValue(12);
//	g_pHour->setSingleStep(1);
//	layout->addRow(new QLabel(QString::fromLocal8Bit("hour")), g_pHour);
//
//	g_pMinute = new QSpinBox();
//	g_pMinute->setRange(0,59);
//	g_pMinute->setValue(1);
//	g_pMinute->setSingleStep(0);
//	layout->addRow(new QLabel(QString::fromLocal8Bit("minute")), g_pMinute);
//
//	g_pSecond= new QSpinBox();
//	g_pSecond->setRange(0,59);
//	g_pSecond->setValue(1);
//	g_pSecond->setSingleStep(0);
//	layout->addRow(new QLabel(QString::fromLocal8Bit("minute")), g_pSecond);
//	//QSpinBox* qspinYear = new QSpinBox();
//	//qspinYear->setRange(1990,1990);
//	//qspinYear->setValue(1990);
//	//layout->addRow(new QLabel(QString::fromLocal8Bit("year")), qspinYear);
//
//	//g_pDay = qspinDay;
//	g_pTimeStep=qspinStep;
//	g_pFirstDay = qspinFirstDay;
//	g_pLastDay =  qspinLastDay;
//
//	connect(qspinLinkie, SIGNAL(valueChanged(double)), this, SLOT(onParamsChanged(double)));
//	connect(qspinCoefbh, SIGNAL(valueChanged(double)), this, SLOT(onParamsChanged(double)));
//	connect(g_pElevation, SIGNAL(valueChanged(double)), this, SLOT(onParamsChanged(double)));
//
//	connect(qspinCoefdh, SIGNAL(valueChanged(double)), this, SLOT(onParamsChanged(double)));
//	connect(qspinStep, SIGNAL(valueChanged(double)), this, SLOT(onParamsChanged(double)));
//	//connect(qspinDay, SIGNAL(valueChanged(int)), this, SLOT(updateParams(int)));
//	//connect(qspinYear, SIGNAL(valueChanged(int)), this, SLOT(updateParams(int)));
//	//connect(qcheckUseDataBase, SIGNAL(clicked()), this, SLOT(buttonClicked()));	
//	connect(qspinFirstDay, SIGNAL(valueChanged(int)), this, SLOT(onParamsChanged(int)));
//	connect(qspinLastDay, SIGNAL(valueChanged(int)), this, SLOT(onParamsChanged(int)));
//
//
//	connect(g_pHour, SIGNAL(valueChanged(int)), this, SLOT(onParamsChanged(int)));
//	connect(g_pMinute, SIGNAL(valueChanged(int)), this, SLOT(onParamsChanged(int)));
//	connect(g_pSecond, SIGNAL(valueChanged(int)), this, SLOT(onParamsChanged(int)));
//	connect(g_pInstaneousCheck, SIGNAL(clicked()), this, SLOT(buttonClicked()));
//
//	formGroupBox->setLayout(layout);
//
//
//	return formGroupBox;
//}


QGroupBox* DlgParams::createGroupAnalysisMode()
{


	QGroupBox* formGroupBox = new QGroupBox(tr("Analysis mode"));
	QFormLayout *layout = new QFormLayout;
	

	QRadioButton* qradioPoint = new QRadioButton();
	qradioPoint->setChecked(false);
	layout->addRow(new QLabel(QString::fromLocal8Bit("Point-based")), qradioPoint);

	QRadioButton* qradioSuface = new QRadioButton();
	qradioSuface->setChecked(true);
	layout->addRow(new QLabel(QString::fromLocal8Bit("Surface-based")), qradioSuface);

	g_pSurfaceAnalysisMode = qradioSuface;
	g_pPointAnalysisMode = qradioPoint;

	formGroupBox->setLayout(layout);
	return formGroupBox;
}

QGroupBox* DlgParams::createGroupBuildingParts()
{


	QGroupBox* formGroupBox = new QGroupBox(tr("Building parts"));
	QFormLayout *layout = new QFormLayout;
	
	QCheckBox* qcheckRooftop= new QCheckBox();
	qcheckRooftop->setChecked(true);
	layout->addRow(new QLabel(QString::fromLocal8Bit("Rooftop")), qcheckRooftop);

	QCheckBox* qcheckFacade = new QCheckBox();
	qcheckFacade->setChecked(true);
	layout->addRow(new QLabel(QString::fromLocal8Bit("Racade")), qcheckFacade);

	QCheckBox* qcheckPanel = new QCheckBox();
	qcheckPanel->setChecked(false);
	layout->addRow(new QLabel(QString::fromLocal8Bit("Solar panel")), qcheckPanel);


	formGroupBox->setLayout(layout);


	connect(qcheckRooftop, SIGNAL(clicked()), this, SLOT(buttonClicked()));
	connect(qcheckFacade, SIGNAL(clicked()), this, SLOT(buttonClicked()));
	connect(qcheckPanel, SIGNAL(clicked()), this, SLOT(buttonClicked()));


	g_pRooftopCheck = qcheckRooftop;
	g_pFacadeCheck = qcheckFacade;
	g_pSolarPanelCheck = qcheckPanel;

	return formGroupBox;
}

QGroupBox* DlgParams::createGroupQueryMode()
{



	QGroupBox* formGroupBox = new QGroupBox(tr("Selection mode"));
	QFormLayout *layout = new QFormLayout;
	QRadioButton* qradioPoint= new QRadioButton();
	qradioPoint->setChecked(true);
	layout->addRow(new QLabel(QString::fromLocal8Bit("Point")), qradioPoint);

	QRadioButton* qradioCircle = new QRadioButton();
	qradioCircle->setChecked(false);
	layout->addRow(new QLabel(QString::fromLocal8Bit("Circle")), qradioCircle);

	QRadioButton* qradioSelectioDisabled = new QRadioButton();
	qradioSelectioDisabled->setChecked(false);
	layout->addRow(new QLabel(QString::fromLocal8Bit("Disabled")), qradioSelectioDisabled);
	formGroupBox->setLayout(layout);

	QSpinBox* qspinRadius = new QSpinBox();
	qspinRadius->setRange(0,5000);
	qspinRadius->setValue(100);
	layout->addRow(new QLabel(QString::fromLocal8Bit("Radius (m)")), qspinRadius);

	g_pRadius = qspinRadius;
	connect(qspinRadius, SIGNAL(valueChanged(int)), this, SLOT(onParamsChanged(int)));
	formGroupBox->setLayout(layout);


	connect(qradioPoint, SIGNAL(clicked(bool)), this, SLOT(onSelectionModeChanged(bool)));
	connect(qradioCircle, SIGNAL(clicked(bool)), this, SLOT(onSelectionModeChanged(bool)));
	connect(qradioSelectioDisabled, SIGNAL(clicked(bool)), this, SLOT(onSelectionModeChanged(bool)));

	g_pOffSelMode=qradioSelectioDisabled;
	g_pPointSelMode=qradioPoint;
	g_pCircleSelMode=qradioCircle;
	g_pCircleSelMode->setChecked(true);
	return formGroupBox;
}

QGroupBox* DlgParams::createGroupControl()
{

	QGroupBox* formGroupBox = new QGroupBox(tr("Control"));
	QFormLayout *layout = new QFormLayout;
	// Create the button, make "this" the parent
	QPushButton* btnExecute = new QPushButton("Execute", formGroupBox);
	btnExecute->setGeometry(0,0,128,64);
	connect(btnExecute, SIGNAL(released()), this, SLOT(onExecute()));
	btnExecute->setMinimumSize(128,64);	btnExecute->setMaximumSize(128,64);;	
	btnExecute->setGeometry(0,0,128,64);

	layout->addWidget(btnExecute);


	QPushButton* btnExport = new QPushButton("Export", formGroupBox);
	btnExport->setMinimumSize(128,64);	btnExport->setMaximumSize(128,64);;	
	btnExport->setGeometry(0,0,128,64);

	connect(btnExport, SIGNAL(released()), this, SLOT(onExport()));

	layout->addWidget(btnExport);
	formGroupBox->setLayout(layout);

	return formGroupBox;
}


void DlgParams::onParamsChanged( double val )
{  
	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	params->Radius=g_pRadius->value();
	//params->Linke = g_pLinke->value();
	//params->Step = g_pTimeStep->value();
	//params->FirstDay = g_pFirstDay->value();
	//params->LastDay = g_pLastDay->value();
	//params->Coefbh = g_pBSKY->value();
	//params->Coefdh = g_pDSKY->value();
	//params->Elevation = g_pElevation->value();
	//params->IsInstantaneous = g_pInstaneousCheck->isChecked();
	//params->Time = SolarTime(g_pHour->value(),g_pMinute->value(),g_pSecond->value());
}
void DlgParams::onParamsChanged( int val )
{
    
    SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	params->Radius=g_pRadius->value();
	/*params->Linke = g_pLinke->value();
	params->Elevation = g_pElevation->value();
    params->Step = g_pTimeStep->value();
	params->FirstDay = g_pFirstDay->value();
	params->LastDay = g_pLastDay->value();
	params->Coefbh = g_pBSKY->value();
	params->Coefdh = g_pDSKY->value();
	params->Time = SolarTime(g_pHour->value(),g_pMinute->value(),g_pSecond->value());*/

}

void DlgParams::onSelectionModeChanged( bool checked )
{

	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();

   if(g_pPointSelMode->isChecked())
   {
	   params->QueryMode=SpatialQueryMode::Point;
	   return;
   }
   else if(g_pCircleSelMode->isChecked())
   {
	   params->QueryMode=SpatialQueryMode::Circle;
	   return;
   }
   else if(g_pOffSelMode->isChecked())
   {
	   params->QueryMode=SpatialQueryMode::Disabled;
	   return;
   }

}

void DlgParams::buttonClicked()
{
	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	params->RooftopIncluded=g_pRooftopCheck->isChecked();
	params->FacadeIncluded=g_pFacadeCheck->isChecked();
	params->SolarPanelIncluded=g_pSolarPanelCheck->isChecked();	  
	//params->IsInstantaneous = g_pInstaneousCheck->isChecked();
	if(params->SolarPanelIncluded)
	{
		IGlobalInterfaces::getInstance()->updateSolarPanel();
	}
	//params->UseDataBase = g_pUseDataBase->isChecked();
	
}

void DlgParams::onExecute()
{

	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	if(g_pSurfaceAnalysisMode->isChecked())
	{
		params->AnalysisMode = Surface_based;
	}
	else
	{
		params->AnalysisMode = Point_based;
	}
	IGlobalInterfaces::getInstance()->execute();
}

void DlgParams::onExport()
{
	IGlobalInterfaces::getInstance()->export();
}

void DlgParams::onSetRSUNParams()
{
	if(g_mDlgRUN.isVisible())
		return;
   g_mDlgRUN.show();
}






