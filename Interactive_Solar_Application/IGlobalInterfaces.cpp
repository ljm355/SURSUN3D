#include "IGlobalInterfaces.h"
IGlobalInterfaces* g_pInstance;
IGlobalInterfaces::IGlobalInterfaces(void)
{
}

IGlobalInterfaces::~IGlobalInterfaces(void)
{
}

IGlobalInterfaces* IGlobalInterfaces::getInstance()
{
	return g_pInstance;
}
void IGlobalInterfaces::setInstance(IGlobalInterfaces* pInstance)
{
	g_pInstance = pInstance;
}

void IGlobalInterfaces::setMessage( QString message )
{

}

osgEarth::ElevationQuery* IGlobalInterfaces::getElevationQuery()
{
   return g_pElevationQuery;
}

SolarAnalysisParams* IGlobalInterfaces::getSolarAnalysisParams()
{
   return &m_pAnalysisParams;
}

void IGlobalInterfaces::execute()
{

}

void IGlobalInterfaces::export()
{

}

void IGlobalInterfaces::setProgress( int min,int max,int val )
{

}
