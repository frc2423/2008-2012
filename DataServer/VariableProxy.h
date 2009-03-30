
#ifndef DATAADJUSTER_DATAPROXY_H
#define DATAADJUSTER_DATAPROXY_H


/**
	\class VariableProxy
	\brief A generic proxy for a type
	
*/
template < typename T >
class VariableProxy {
public:

	VariableProxy() :
		m_parent(NULL)
	{}

	
	~VariableProxy()
	{
		Synchronized sync(m_mutex);
		
		if (m_parent)
			m_parent->Unregister(this);
	}
	
	operator T() 
	{
		Synchronized sync(m_mutex);
		return m_value; 
	}

private:
	
	// no copying, no assignment
	VariableProxy();
	VariableProxy( const VariableProxy<T> &);
	void operator=(const VariableProxy<T> &);
	
	T 					m_value;
	DataAdjuster * 		m_parent;

	
};


#endif
