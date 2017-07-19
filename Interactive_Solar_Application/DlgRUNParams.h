#ifndef DLGRUNTIMEINTEGRATED_H
#define DLGRUNTIMEINTEGRATED_H
#include <QtGui>
#include <QDialog>
class DlgRUNParams : public QDialog
{
	Q_OBJECT

public:
	DlgRUNParams(QWidget *parent = 0);
	~DlgRUNParams();
	private slots:
		void onParamsChanged(int val);
		void onParamsChanged( double val );
		void buttonClicked();
		void onExecute();
private:
	//Ui::DlgRUNTimeIntegrated ui;
	QSpinBox* g_pFirstDay;
	QSpinBox* g_pLastDay;
	QSpinBox* g_pYear;
	QDoubleSpinBox* g_pLinke;
	QDoubleSpinBox* g_pElevation;
	QDoubleSpinBox* g_pBSKY;
	QDoubleSpinBox* g_pDSKY;
	QDoubleSpinBox* g_pTimeStep;
	QCheckBox* g_pInstaneousCheck;
	QSpinBox* g_pDay;
	QSpinBox* g_pHour;
	QSpinBox* g_pMinute;
	QSpinBox* g_pSecond;

};

#endif // DLGRUNTIMEINTEGRATED_H
