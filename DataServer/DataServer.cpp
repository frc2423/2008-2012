
#include "DataAdjuster.h"
#include "VariableProxy.h"


DataAdjuster * DataAdjuster::GetInstance()	
{
	if (m_instance == NULL)
		m_instance = new DataAdjuster();
	return m_instance;
}

void DataServer::DataServerThreadStart(void * param)
{
	((DataServer*)param)->ThreadFn();
}


void DataServer::ThreadFn()
{
    // Initialise server.
    http::server::server s("0.0.0.0", m_port, m_rootDir);
	
	// Run the server until stopped.
    s.run();
}

void DataServer::Enable()
{
	// generate some HTML for this thing, so we're not constantly
	// regenerating data for it
	m_html.clear();
	
	for (i = 0; i < groups; i++)
	{
		// group name/header
		m_html.append("\t<div class=\"group\">\n");
		m_html.append("\t\t<div class=\"grouphr\">\n");
		m_html.append("\t\t\t");
		m_html.append(group_name);
		m_html.append("\n\t\t</div>\n\t\t<div><table>\n\t\t\t");
		
		// for each variable, create a table row for it
		{
			m_html.append("\t\t\t<tr><td>");
			m_html.append(variable_name);
			m_html.append("</td><td><div class=\"selectbar\" id=\"");
					
			/* 
				bar parameters are done using the id tag, with each parameter
				seperated by an underscore, so generate the id for each one:
				
					0 - group
					1 - variable name 
					2 - min
					3 - max
					4 - step
					5 - current value
					
					If the numeric items have a p in them, that is
					actually a decimal point. So 0.1 is now 0p3
			*/
			
			
			m_html.append("\">");
			m_html.append( default_value );
			m_html.append("</div></td></tr>\n");
		}
			
		
		// finish it off
		m_html.append("\t\t</table></div>\n\t</div>");
	}
		
	// then start the task/thread
}


