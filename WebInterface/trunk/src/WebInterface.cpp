/*
    WebInterface
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


#include "WebInterface.h"
#include "VariableProxy.h"

#include "server/server.hpp"

#include <ctime>
#include <iostream>

#include <boost/tokenizer.hpp>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "StaticDeleter.h"


#if defined(__VXWORKS__)
#	include <WPILib/WPILib.h>
#endif



static
WebInterface * m_instance = NULL;

StaticDeleter<WebInterface> m_deleter(&m_instance);


WebInterface * WebInterface::GetInstance()	
{
	if (m_instance == NULL)
		m_instance = new WebInterface();
	return m_instance;
}

IntProxy WebInterface::CreateIntProxy( 
	const char * groupName, 
	const char * name, 
	const IntProxyFlags &flags)
{
	IntProxyInfo * proxy = new IntProxyInfo(flags);
	GetInstance()->InitProxy( proxy, groupName, name );
	return proxy->GetProxy();
}
	
FloatProxy WebInterface::CreateFloatProxy( 
	const char * groupName, 
	const char * name, 
	const FloatProxyFlags &flags)
{
	FloatProxyInfo * proxy = new FloatProxyInfo(flags);
	GetInstance()->InitProxy( proxy, groupName, name );
	return proxy->GetProxy();
}

DoubleProxy WebInterface::CreateDoubleProxy( 
	const char * groupName, 
	const char * name, 
	const DoubleProxyFlags &flags)
{
	DoubleProxyInfo * proxy = new DoubleProxyInfo(flags);
	GetInstance()->InitProxy( proxy, groupName, name );
	return proxy->GetProxy();
}


BoolProxy WebInterface::CreateBoolProxy( 
	const char * groupName, 
	const char * name, 
	bool default_value)
{
	BoolProxyInfo * proxy = new BoolProxyInfo(default_value);
	GetInstance()->InitProxy( proxy, groupName, name );
	return proxy->GetProxy();
}

WebInterface::WebInterface() :
	m_port("8080"),
	m_rootDir("www"),
	m_html_valid(false),
	m_creation_time(time(NULL))
{}




/**
	@note This function will assume ownership of the proxy object
*/	
void WebInterface::InitProxy(
	DataProxyInfo * proxy, 
	const std::string &groupName, 
	const std::string &name)
{
	lock_guard lock(m_mutex);

	DataProxyGroupPtr ptr;
	
	// small data sets, linear search is ok
	foreach ( DataProxyGroupPtr &group, m_groups)
		if (group->name == groupName)
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
		if (variable->name == name)
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

	m_html_valid = false;
}


void WebInterface::WebInterfaceThreadStart(void * param)
{
	((WebInterface*)param)->ThreadFn();
}


void WebInterface::ThreadFn()
{
	std::string port, rootdir;
	
	{
		lock_guard lock(m_mutex);
		port = m_port;
		rootdir = m_rootDir;
	}

	try
	{
		// Initialise server.
		http::server::server s("0.0.0.0", port, rootdir);
	
		// Run the server until stopped.
		s.run();
	}
	catch (std::exception &e)
	{
		std::cerr << "WebInterface Server exception: " << e.what() << std::endl;
	}
}

void WebInterface::Enable(const std::string &port, const std::string &rootdir)
{
	GetInstance()->EnableInternal(port, rootdir);
}

void WebInterface::EnableInternal(const std::string &port, const std::string &rootdir)
{
	lock_guard lock(m_mutex);
		
	if (!port.empty())
		m_port = port;
		
	if (!rootdir.empty())
		m_rootDir = rootdir;
		
	// then start the task/thread
#if defined(__VXWORKS__)
	Task * task = new Task("WebInterface", (FUNCPTR)WebInterface::WebInterfaceThreadStart, Task::kDefaultPriority, 64000);
	task->Start((INT32)this);
#else
	if (m_thread.get() == NULL)
		m_thread.reset( new boost::thread(boost::bind(&WebInterface::ThreadFn, this)) );
#endif
}

std::string WebInterface::get_html()
{
	// lock globally
	lock_guard lock(m_mutex);

	if (m_html_valid)
		return m_html;

	// generate some HTML for this thing and cache it so we're not constantly
	// regenerating data for it
	m_html.clear();

	size_t g = 0;
	
	foreach( const DataProxyGroupPtr &gptr  , m_groups )
	{
		// group name/header
		m_html.append("\n\t<div class=\"group\">\n");
		m_html.append("\t\t<div class=\"grouphdr\">\n");
		m_html.append("\t\t\t");
		m_html.append(gptr->name);
		m_html.append("\n\t\t</div>\n\t\t<div><table>\n");
		
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
		g += 1;
	}

	// and add a javascript portion that allows us
	// to store the 'current value'
	m_html.append("\n\n\t<script type=\"text/javascript\"><!--\n\tvar current_instance = ");
	m_html.append(boost::lexical_cast<std::string>(m_creation_time));
	m_html.append(";\n\t//--></script>");

	m_html_valid = true;
	return m_html;
}

std::string WebInterface::ProcessRequest(const std::string &post_data)
{
	size_t group = 0, variable = 0;
	int found_flags = 0;
	std::string value;

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

	// parse the POST request into name/value pairs
	boost::char_separator<char> sep("&");
	tokenizer tokens(post_data, sep);

	foreach (const std::string &token, tokens)
	{
		size_t pos = token.find('=');
		if (pos == std::string::npos || pos > token.size()-2)
			return "INVALID";

		// we have a key/value pair now.. 
		std::string key = token.substr(0, pos); 
		std::string val = token.substr(pos+1);

		try
		{
			if (key == "group")
			{
				found_flags |= 0x01;
				group = boost::lexical_cast<size_t>(val);
			}
			else if (key == "var")
			{
				found_flags |= 0x02;
				variable = boost::lexical_cast<size_t>(val);
			}
			else if (key == "value")
			{
				found_flags |= 0x04;
				value = val;
			}
			else if (key == "instance")
			{
				found_flags |= 0x08;
				time_t inst = boost::lexical_cast<time_t>(val);

				if (inst != GetInstance()->m_creation_time)
					return "RELOAD";
			}
		}
		catch (boost::bad_lexical_cast &)
		{
			return "INVALID";
		}
	}

	if (found_flags != 0x0F)
		return "INVALID";
	
	// ok, if everything is good then modify the proxy
	
	if (GetInstance()->ModifyProxy(group, variable, value))
		return "OK";

	return "FAIL";
}

bool WebInterface::ModifyProxy(size_t group, size_t variable, const std::string &value)
{
	lock_guard lock(m_mutex);
	
	if (m_groups.size() < group)
		return false;
		
	if (m_groups[group]->variables.size() < variable)
		return false;
	
	m_html_valid = false;
	return m_groups[group]->variables[variable]->info->SetValue(value);
}



