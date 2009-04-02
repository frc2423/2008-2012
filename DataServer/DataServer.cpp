/*
    NotSureWhatThisProjectNameIsYet
    Copyright (C) 2009 Dustin Spicuzza <dustin@virtualroadside.com>
	
	$Id$

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License v3 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "DataServer.h"
#include "VariableProxy.h"

#include <boost/foreach.hpp>
#define BOOST_FOREACH foreach


DataServer * DataServer::GetInstance()	
{
	if (m_instance == NULL)
		m_instance = new DataServer();
	return m_instance;
}

IntProxy DataServer::CreateIntProxy( 
	const char * groupName, 
	const char * name, 
	const IntDataFlags &flags)
{
	IntProxyInfo * proxy = new IntProxyInfo(flags);
	GetInstance()->InitProxy( proxy, groupName, name);
	return proxy->GetProxy();
}
	
FloatProxy DataServer::CreateFloatProxy( 
	const char * groupName, 
	const char * name, 
	const FloatDataFlags &flags)
{
	FloatProxyInfo * proxy = new FloatProxyInfo(flags);
	GetInstance()->InitProxy( proxy, groupName, name);
	return proxy->GetProxy();
}

/**
	@note This function will assume ownership of the proxy object
*/	
void DataServer::InitProxy(
	DataProxyInfo * proxy, 
	const std::string &groupName, 
	const std::string &name)
{
	boost::lock_guard(m_mutex);

	DataProxyGroupPtr ptr;
	
	// small data sets, linear search is ok
	foreach ( DataProxyGroupPtr &group, m_groups)
		if (group.name == groupName)
		{
			ptr = group;
			break;
		}
		
	if (ptr.get() == NULL)
	{
		ptr.reset( new DataProxyGroup() );
		ptr->name = groupName;
		m_groups.push_back(ptr);
	}
	
	DataProxyVariablePtr dptr;
	
	foreach( DataProxyVariablePtr &variable, ptr->variables)
		if (variable.name == name)
		{
			dptr = variable;
			break;
		}
	
	if (dptr.get() == NULL)
	{
		dptr.reset( new DataProxyVariable());
		dptr->name = name;
		dptr->info.reset( proxy );
		ptr->variables.push_back( dptr );
	}
	else
	{
		assert(0 && "key/value pair already registered!");
	}
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
	m_enabled = true;
		
	// then start the task/thread
}

void DataServer::regen_html()
{
	// lock globally
	boost::lock_guard(m_mutex);

	// generate some HTML for this thing, so we're not constantly
	// regenerating data for it
	m_html.clear();

	size_t g = 0;
	
	foreach( const DataProxyGroupPtr &gptr  , m_groups )
	{
		// group name/header
		m_html.append("\t<div class=\"group\">\n");
		m_html.append("\t\t<div class=\"grouphr\">\n");
		m_html.append("\t\t\t");
		m_html.append(gptr->name);
		m_html.append("\n\t\t</div>\n\t\t<div><table>\n\t\t\t");
		
		size_t v = 0;
		
		// for each variable, create a table row for it
		foreach( const DataProxyVariablePtr &dptr, gptr->variables )
		{
			m_html.append("\t\t\t<tr><td>");
			m_html.append(dptr->name);
			m_html.append("</td><td>");
			m_html.append(dptr->info->GetHtmlDisplay(g, v));
			m_html.append("</td></tr>\n");
			
			v += 1;
		}
			
		// finish it off
		m_html.append("\t\t</table></div>\n\t</div>");
		
		// and add a javascript portion that allows us
		// to store the 'current value'
		m_html.append("<script type=\"text/javascript\"><!--\nvar current_value=");
		m_html.append(m_current_value);
		m_html.append(";\n//--></script>");
		
		g += 1;
	}
}

bool DataServer::ProcessData(const std::string &post_data)
{
	// parse the POST request
	
	// ok, if everything is good then modify the proxy
	
	return ModifyProxy(group, variable, value);
}

bool DataServer::ModifyProxy(size_t group, size_t variable, const std::string &value)
{
	boost::lock_guard(m_mutex);
	
	if (m_groups.size() < group)
		return false;
		
	if (m_groups[group]->variables.size() < variable)
		return false;
		
	return m_groups[group]->variables[variable]->info->SetValue(value);
}



