#include "TemporalSolarWidget.h"
#include <qpainter>
#include "IGlobalInterfaces.h"
TemporalSolarWidget::TemporalSolarWidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setBackgroundRole(QPalette::Base);
	setAutoFillBackground(true);
}

TemporalSolarWidget::~TemporalSolarWidget()
{

}

void TemporalSolarWidget::paintEvent( QPaintEvent *event )
{
    QPainter painter(this);
	painter.setPen(QPen(QColor(255,0,0), 2, Qt::SolidLine, Qt::RoundCap,
		Qt::RoundJoin));	
	SolarAnalysisParams* analysisParam = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	int xrang = analysisParam->DailySeries.size();
	float miny = FLT_MAX;
	float maxy = -FLT_MAX;
	for(int i=0;i<analysisParam->DailySeries.size();i++)
	{
		SolarRadiation& rad = analysisParam->DailySeries[i];
		if(miny>rad.global)
			miny = rad.global;
		if(maxy<rad.global)
			maxy=rad.global;
	}
    float yrange = maxy-miny;
	int w = this->width();
	int h = this->height();
	QPainterPath path;



	int spacing = (int)((float)w / (float)xrang);
    int offset = 0;
	for(int i=0;i<analysisParam->DailySeries.size();i++)
	{	
		
		SolarRadiation& rad = analysisParam->DailySeries[i];
		if(i==0)
		{
	        path.moveTo(offset, h-(rad.global-miny)/yrange*h);
		}
        else
		{
		    path.lineTo(offset, h-(rad.global-miny)/yrange*h);
		}
		offset+=spacing;

	}
	QPen pen(QColor(255,0,0),2);
	painter.setPen(pen);
	painter.drawPath(path);
}
