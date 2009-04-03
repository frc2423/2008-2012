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

#ifndef DATASERVER_H
#define DATASERVER_H

#include <string>
#include <vector>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "DataProxyInfo.h"

namespace http {
	namespace server {
		class request_handler;
	}
}

/**
	\class DataServer
	\brief Singleton class used to allow users to remotely modify variables
	via a webserver launched in a seperate task.
	
	@todo Make values persistent, by writing to file?
*/
class DataServer {

	// various type definitions
	typedef boost::lock_guard<boost::mutex>			lock_guard;
	
	struct DataProxyVariable {
		std::string 								name;
		boost::shared_ptr<DataProxyInfo> 			info;
	};
	
	typedef boost::shared_ptr< DataProxyVariable > 	DataProxyVariablePtr;
	
	struct DataProxyGroup {
		std::string 								name;
		std::vector< DataProxyVariablePtr > 		variables;
	};
	
	typedef boost::shared_ptr< DataProxyGroup >		DataProxyGroupPtr;
	typedef std::vector< DataProxyGroupPtr >		DataProxyGroups;

	friend class http::server::request_handler;

public:

	/// you should call this after initializing all variables, otherwise the
	/// data proxies will always return the default values
	static void Enable();


	/** 
		Use this to initialize an integer proxy
		
		@param groupName	group this variable belongs to
		@param name			name of this variable (should be unique)
		@param flags		Use this to set min/max/default values and etc. For example,
							IntProxyFlags().default_value(x).minval(x).maxval(x).step(x)
	*/
	static IntProxy CreateIntProxy( const char * groupName, const char * name, const IntProxyFlags &flags);
	

	/** 
		Use this to initialize a floating point proxy
		
		@param groupName	group this variable belongs to
		@param name			name of this variable (should be unique)
		@param flags		Use this to set min/max/default values and etc. For example,
							FloatProxyFlags().default_value(x).minval(x).maxval(x).step(x)
	*/
	static FloatProxy CreateFloatProxy( const char * groupName, const char * name, const FloatProxyFlags &flags);
	
	
	/**
		Use this to initialize a boolean proxy
		
		@param groupName		group this variable belongs to
		@param name				name of this variable (should be unique)
		@param default_value 	the starting value of this proxy
	*/
	static BoolProxy CreateBoolProxy( const char * groupName, const char * name, bool default_value);
	

	/// @todo other types of variables: enums, bools, etc
	

	/// Set the port that the HTTP server should listen on
	void SetPort(unsigned int port);
	
	/// Set the root directory for web files
	void SetRootDir(const std::string &dir);
	
private:

	static DataServer * m_instance;
	DataServer();
	
	static DataServer * GetInstance();


	void InitProxy(
		DataProxyInfo * proxy, 
		const std::string &groupName, 
		const std::string &name);


	void EnableInternal();

	static void DataServerThreadStart(void * param);
	void ThreadFn();
	
	static std::string ProcessRequest(const std::string &post_data);
	std::string get_html();

	bool ModifyProxy(size_t group, size_t variable, const std::string &value);
	
	
	
	// server data
	std::string				m_port;
	std::string				m_rootDir;
	
	// generated HTML for the server
	bool					m_html_valid;
	std::string				m_html;
	
	/// each ajax request is compared against this, and if it doesn't
	/// match then it means the page must reload. This is updated each time
	/// a new proxy is added or deleted
	time_t 					m_creation_time;
	
	/// global access lock
	boost::mutex			m_mutex;
	
	/// thread
	boost::shared_ptr< boost::thread > m_thread;
	
	/// storage of the proxied data
	DataProxyGroups 		m_groups;
	
};

#endif
