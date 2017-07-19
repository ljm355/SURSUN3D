#include "DlgRUNParams.h"
#include "IGlobalInterfaces.h"

DlgRUNParams::DlgRUNParams(QWidget *parent)
	: QDialog(parent)
{
	//ui.setupUi(this);
	//QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));


	setWindowTitle(tr("GRASS GIS r.sun"));
	QFormLayout *layout = new QFormLayout;
	//formGroupBox->setStyleSheet("QLabel { background-color : rgba(255, 0, 0, 0); color : white; }");


	QDoubleSpinBox* qspinLinkie = new QDoubleSpinBox();
	qspinLinkie->setRange(1,10);
	qspinLinkie->setValue(3);
	qspinLinkie->setSingleStep(0.1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("linkein")), qspinLinkie);
	g_pLinke = qspinLinkie;

	g_pElevation = new QDoubleSpinBox();
	g_pElevation->setRange(-1000,10000);
	g_pElevation->setValue(0);
	g_pElevation->setSingleStep(10);
	layout->addRow(new QLabel(QString::fromLocal8Bit("elevation")), g_pElevation);

	QDoubleSpinBox* qspinCoefbh = new QDoubleSpinBox();
	qspinCoefbh->setRange(0,2);
	qspinCoefbh->setValue(1);
	qspinCoefbh->setSingleStep(0.1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("coefbh")), qspinCoefbh);
	g_pBSKY = qspinCoefbh;

	QDoubleSpinBox* qspinCoefdh = new QDoubleSpinBox();
	qspinCoefdh->setRange(0,2);
	qspinCoefdh->setValue(1);
	qspinCoefdh->setSingleStep(0.1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("coefdh")), qspinCoefdh);
	g_pDSKY = qspinCoefdh;



	QSpinBox* qspinFirstDay = new QSpinBox();
	qspinFirstDay->setRange(1,365);
	qspinFirstDay->setValue(1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("start day")), qspinFirstDay);

	QSpinBox* qspinLastDay = new QSpinBox();
	qspinLastDay->setRange(1,365);
	qspinLastDay->setValue(1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("end day")), qspinLastDay);


	QDoubleSpinBox* qspinStep= new QDoubleSpinBox();
	qspinStep->setRange(0,2);
	qspinStep->setValue(1);
	qspinStep->setSingleStep(0.1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("step")), qspinStep);

	g_pInstaneousCheck = new QCheckBox();
	g_pInstaneousCheck->setChecked(false);
	layout->addRow(new QLabel(QString::fromLocal8Bit("instaneous mode")), g_pInstaneousCheck);

	g_pDay = new QSpinBox();
	g_pDay->setRange(1,365);
	g_pDay->setValue(1);
	g_pDay->setSingleStep(1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("day")), g_pDay);

	g_pHour = new QSpinBox();
	g_pHour->setRange(0,23);
	g_pHour->setValue(12);
	g_pHour->setSingleStep(1);
	layout->addRow(new QLabel(QString::fromLocal8Bit("hour")), g_pHour);

	g_pMinute = new QSpinBox();
	g_pMinute->setRange(0,59);
	g_pMinute->setValue(0);
	g_pMinute->setSingleStep(0);
	layout->addRow(new QLabel(QString::fromLocal8Bit("minute")), g_pMinute);

	g_pSecond= new QSpinBox();
	g_pSecond->setRange(0,59);
	g_pSecond->setValue(0);
	g_pSecond->setSingleStep(0);
	layout->addRow(new QLabel(QString::fromLocal8Bit("second")), g_pSecond);
	//QSpinBox* qspinYear = new QSpinBox();
	//qspinYear->setRange(1990,1990);
	//qspinYear->setValue(1990);
	//layout->addRow(new QLabel(QString::fromLocal8Bit("year")), qspinYear);

	//g_pDay = qspinDay;
	g_pTimeStep=qspinStep;
	g_pFirstDay = qspinFirstDay;
	g_pLastDay =  qspinLastDay;

	connect(qspinLinkie, SIGNAL(valueChanged(double)), this, SLOT(onParamsChanged(double)));
	connect(qspinCoefbh, SIGNAL(valueChanged(double)), this, SLOT(onParamsChanged(double)));
	connect(g_pElevation, SIGNAL(valueChanged(double)), this, SLOT(onParamsChanged(double)));

	connect(qspinCoefdh, SIGNAL(valueChanged(double)), this, SLOT(onParamsChanged(double)));
	connect(qspinStep, SIGNAL(valueChanged(double)), this, SLOT(onParamsChanged(double)));
	//connect(qspinDay, SIGNAL(valueChanged(int)), this, SLOT(updateParams(int)));
	//connect(qspinYear, SIGNAL(valueChanged(int)), this, SLOT(updateParams(int)));
	//connect(qcheckUseDataBase, SIGNAL(clicked()), this, SLOT(buttonClicked()));	
	connect(qspinFirstDay, SIGNAL(valueChanged(int)), this, SLOT(onParamsChanged(int)));
	connect(qspinLastDay, SIGNAL(valueChanged(int)), this, SLOT(onParamsChanged(int)));

	connect(g_pInstaneousCheck, SIGNAL(clicked()), this, SLOT(buttonClicked()));

	connect(g_pHour, SIGNAL(valueChanged(int)), this, SLOT(onParamsChanged(int)));
	connect(g_pMinute, SIGNAL(valueChanged(int)), this, SLOT(onParamsChanged(int)));
	connect(g_pSecond, SIGNAL(valueChanged(int)), this, SLOT(onParamsChanged(int)));
	connect(g_pDay, SIGNAL(valueChanged(int)), this, SLOT(onParamsChanged(int)));

	setLayout(layout);

	int val = 0;
	double dval = 0;
	onParamsChanged(val);
	onParamsChanged(dval);
	buttonClicked();

	g_pHour->setEnabled(false);
	g_pMinute->setEnabled(false);
	g_pSecond->setEnabled(false);
	g_pDay->setEnabled(false);


	// Create the button, make "this" the parent
	QPushButton* btnExecute = new QPushButton("Execute", this);
	layout->addWidget(btnExecute);
	//btnExecute->setGeometry(0,0,128,64);
	connect(btnExecute, SIGNAL(released()), this, SLOT(onExecute()));
	//btnExecute->setMinimumSize(128,64);	btnExecute->setMaximumSize(128,64);;	
	//btnExecute->setGeometry(0,0,128,64);

}

DlgRUNParams::~DlgRUNParams()
{

}


void DlgRUNParams::onExecute()
{
	IGlobalInterfaces::getInstance()->execute();
}

void DlgRUNParams::onParamsChanged( double val )
{  
	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	params->Linke = g_pLinke->value();
	params->Step = g_pTimeStep->value();
	params->FirstDay = g_pFirstDay->value();
	params->LastDay = g_pLastDay->value();
	params->Coefbh = g_pBSKY->value();
	params->Coefdh = g_pDSKY->value();
	params->Elevation = g_pElevation->value();
	params->IsInstantaneous = g_pInstaneousCheck->isChecked();
	params->Time = SolarTime(g_pHour->value(),g_pMinute->value(),g_pSecond->value());
	params->Day = g_pDay->value();

}
void DlgRUNParams::onParamsChanged( int val )
{

	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	params->Linke = g_pLinke->value();
	params->Elevation = g_pElevation->value();
	params->Step = g_pTimeStep->value();
	params->FirstDay = g_pFirstDay->value();
	params->LastDay = g_pLastDay->value();
	params->Coefbh = g_pBSKY->value();
	params->Coefdh = g_pDSKY->value();
	params->Time = SolarTime(g_pHour->value(),g_pMinute->value(),g_pSecond->value());
	params->Day = g_pDay->value();

}

void DlgRUNParams::buttonClicked()
{
	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams(); 
	params->IsInstantaneous = g_pInstaneousCheck->isChecked();

	if(params->IsInstantaneous)
	{
		g_pFirstDay->setEnabled(false);
		g_pLastDay->setEnabled(false);
		g_pTimeStep->setEnabled(false);

		g_pHour->setEnabled(true);
		g_pMinute->setEnabled(true);
		g_pSecond->setEnabled(true);
		g_pDay->setEnabled(true);
	}
	else
	{
		g_pFirstDay->setEnabled(true);
		g_pLastDay->setEnabled(true);
		g_pTimeStep->setEnabled(true);

		g_pHour->setEnabled(false);
		g_pMinute->setEnabled(false);
		g_pSecond->setEnabled(false);
		g_pDay->setEnabled(false);
	}
}