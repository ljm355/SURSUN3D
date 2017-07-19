#ifndef FEATUREFIELDSELECTOR_H
#define FEATUREFIELDSELECTOR_H
#include <osgEarthFeatures/Feature>
#include "osgEarthFeatures/FeatureSource"
#include <QDialog>
#include "ui_FeatureFieldSelector.h"
#include "qdialog.h"
using namespace osgEarth::Features;

class FeatureFieldSelector : public QDialog
{
	Q_OBJECT

public:
	FeatureFieldSelector(QWidget *parent = 0);
	~FeatureFieldSelector();
	void setFeatureSource(osgEarth::Features::FeatureSource* featureSource);
	void setFeatureFile(QString filename);
	std::string getFieldName();
private:	
	Ui::FeatureFieldSelector ui;
private:
	QPushButton g_mButtonOK;
	std::vector<std::string> g_mFields;
};
#endif // FEATUREFIELDSELECTOR_H
