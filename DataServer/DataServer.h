
#ifndef DATA_ADJUSTER_H
#define DATA_ADJUSTER_H

#include <vector>
#include <string>

#include "AllocateFlags.h"

#include "server/server.hpp"

/**
	\class DataAdjuster
	\brief Singleton class used to allow users to remotely modify variables
	via a webserver launched in a seperate task.
	
	@todo Make values persistent, by writing to file?
*/
class DataAdjuster {
public:

	/// you should call this after initializing all variables, otherwise the
	/// data proxies will always return the default values
	static void Enable();


	/** 
		Use this to initialize an integer proxy at its construction time
		
		@param groupName	group this variable belongs to
		@param name			name of this variable (should be unique)
		@param flags		Use this to set min/max/default values and etc
	*/
	static IntProxy ConstructIntProxy( const char * groupName, const char * name, const IntDataFlags &flags);
	
	/** 
		Use this to initialize an integer proxy after construction. You
		should avoid initializing a proxy twice, that will cause either
		an assert error or undefined results.
		
		@param groupName	group this variable belongs to
		@param name			name of this variable (should be unique)
		@param flags		Use this to set min/max/default values and etc
	*/
	static void InitializeIntProxy( IntProxy &proxy, const char * groupName, const char * name, const IntDataFlags &flags);
	

	/** 
		Use this to initialize a floating point proxy at its construction time
		
		@param groupName	group this variable belongs to
		@param name			name of this variable (should be unique)
		@param flags		Use this to set min/max/default values and etc
	*/
	static FloatProxy ConstructFloatProxy( const char * groupName, const char * name, const FloatDataFlags &flags);
	
	/** 
		Use this to initialize a floating point proxy after construction. You
		should avoid initializing a proxy twice, that will cause either
		an assert error or undefined results.
		
		@param proxy		The proxy to initialize (passed by ref)
		@param groupName	group this variable belongs to
		@param name			name of this variable (should be unique)
		@param flags		Use this to set min/max/default values and etc
	*/
	static void InitializeFloatProxy( FloatProxy &proxy, const char * groupName, const char * name, const FloatDataFlags &flags);
	

	/// @todo other types of variables: enums, bools, etc

	/// Set the port that the HTTP server should listen on
	void SetPort(std::string port);
	
	/// Set the root directory for web files
	void SetRootDir(std::string dir);
	
private:

	static DataAdjuster * m_instance;
	DataAdjuster();
	
	static DataAdjuster * GetInstance();

	void InitIntProxy( IntProxy &proxy, const char * groupName, const char * name, const IntDataFlags &flags);
	void InitFloatProxy( FloatProxy &proxy, const char * groupName, const char * name, const FloatDataFlags &flags);


	

	

	static void DataServerThreadStart(void * param);
	void ThreadFn();
	
	void ParsePacket(unsigned char * pkt);
	
	
	std::map< string , shared_ptr< std::map< string , IntDataFlags > > > m_ints;
	std::map< string , shared_ptr< std::map< string , FloatDataFlags > > > m_floats;
	
	SEM_ID					m_mutex;
	
	// server data
	std::string				m_port;
	std::string				m_rootDir;
	
	// generated HTML for the server
	std::string				m_html;
};

#endif
