
#include "DataAdjuster.h"
#include "VariableProxy.h"


DataAdjuster * DataAdjuster::GetInstance()	
{
	if (m_instance == NULL)
		m_instance = new DataAdjuster();
	return m_instance;
}



