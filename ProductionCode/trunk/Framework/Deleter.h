
#ifndef KWARQS_DELETER_H
#define KWARQS_DELETER_H

/// this class is used to delete arbitrary pointers on destruction
template <typename T>
struct Deleter {
	
	Deleter(T ** ptr) : 
		m_ptr(ptr) 
	{}
	
	~Deleter() {
		if (*m_ptr)
			delete *m_ptr;
	}
	
	T ** m_ptr;
};

#endif
