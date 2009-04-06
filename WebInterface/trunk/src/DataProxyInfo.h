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

#ifndef WI_DATAPROXYINFO_H
#define WI_DATAPROXYINFO_H

#include <string>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/thread/mutex.hpp>

#include "VariableProxyFlags.h"
#include "VariableProxy.h"


// base type
struct DataProxyInfo {

	virtual bool SetValue( const std::string & ) = 0;
	virtual std::string GetHtmlDisplay(std::size_t, std::size_t) const = 0;
	virtual ~DataProxyInfo(){}

};




// numeric implementation
template <typename T>
struct NumericProxyInfoImpl : public DataProxyInfo {

	typedef NumericProxyFlagsImpl<T>	Flags;
	typedef VariableProxyImpl<T>	Proxy;

	/// constructor
	NumericProxyInfoImpl(const Flags& flags) :
		m_proxied_value(flags.default_value_),
		m_flags(flags)
	{}

	/// external thing sets value
	bool SetValue( const std::string &value)
	{
		try {
			boost::lock_guard<boost::mutex> lock(m_mutex);
			m_proxied_value = boost::lexical_cast<T>(value);
		} 
		catch (boost::bad_lexical_cast &)
		{
			return false;
		}
		
		return true;
	}
	
	/// returns html
	std::string GetHtmlDisplay(std::size_t gid, std::size_t vid) const
	{
		T current_value;
	
		{
			boost::lock_guard<boost::mutex> lock(m_mutex);
			current_value = m_proxied_value;
		}
	
		std::string html;
		
		html.append("<div class=\"selectbar\" id=\"");
		
		// generate an id based on the flags	
		std::stringstream idstr;
		idstr.precision(m_flags.precision_);
		
		idstr 	<< "g" << gid
				<< "_v" << vid
				<< "_" << current_value
				<< "_" << m_flags.minval_
				<< "_" << m_flags.maxval_
				<< "_" << m_flags.step_;
		
		std::string id = idstr.str();
		
		for (std::size_t i = 0; i < id.size(); i++)
			if (id[i] == '.')
				id[i] = 'p';
		
		html.append(id);
		html.append("\">");
		
		// makes sure the value is formatted correctly
		idstr.str("");
		idstr	<< current_value;
		
		html.append(idstr.str());
		html.append("</div>");
		
		return html;
	}
	
	Proxy GetProxy()
	{
		return Proxy(&m_proxied_value, &m_mutex);
	}
	
private:
	NumericProxyInfoImpl();

	T 						m_proxied_value;
	mutable boost::mutex 	m_mutex;
	const Flags				m_flags;
};

/// implementation for floats
typedef NumericProxyInfoImpl<float> FloatProxyInfo;

/// implementation for doubles
typedef NumericProxyInfoImpl<double> DoubleProxyInfo;

/// implementation for integers
typedef NumericProxyInfoImpl<int> 	IntProxyInfo;



// boolean implementation
struct BoolProxyInfo : public DataProxyInfo {

	typedef VariableProxyImpl<bool>	Proxy;

	/// constructor
	BoolProxyInfo(bool default_value) :
		m_proxied_value(default_value)
	{}

	/// external thing sets value
	bool SetValue( const std::string &value)
	{
		try {
			boost::lock_guard<boost::mutex> lock(m_mutex);
			m_proxied_value = boost::lexical_cast<bool>(value);
		} 
		catch (boost::bad_lexical_cast &)
		{
			return false;
		}
		
		return true;
	}
	
	/// returns html
	std::string GetHtmlDisplay(std::size_t gid, std::size_t vid) const
	{
		bool current_value;
	
		{
			boost::lock_guard<boost::mutex> lock(m_mutex);
			current_value = m_proxied_value;
		}
	
		std::string html;
		
		html = "<input class=\"booleans\" type=\"checkbox\" id=\"g"
				+ boost::lexical_cast<std::string>(gid)
				+ "_v"
				+ boost::lexical_cast<std::string>(vid)
				+ "\"";
				
		if (current_value)
			html.append(" checked");
		
		html.append(" />");
		
		return html;
	}
	
	Proxy GetProxy()
	{
		return Proxy(&m_proxied_value, &m_mutex);
	}
	
private:
	BoolProxyInfo();

	bool 					m_proxied_value;
	mutable boost::mutex 	m_mutex;
};




#endif
