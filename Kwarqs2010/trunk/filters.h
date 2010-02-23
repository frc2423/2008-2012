
#ifndef KWARQSLIB_FILTERS_H
#define KWARQSLIB_FILTERS_H

#include <vector>
#include <cassert>

/**
	\class LowPassFilter
	\brief A first order low-pass filter of the form y = a*y-1 + (1-a)*Xk
*/
class LowPassFilter {
public:

	/**
		Initializes a filter where alpha is the filter constant
	*/
	LowPassFilter(double alpha = 0.7) :
		y(0), a(alpha)
	{
		assert(alpha > 0 && alpha < 1);
	}
	
	/// Calculates a filtered value and returns it, based on the previously 
	/// passed in values
	double Calculate(double x)
	{
		y = a * y + (1-a)*x;
		return y;
	}

private:
	double y, a;
};

/**
	\class AverageWindowFilter
	\brief Implements a simple average window filter
*/
template <typename Type, size_t num_points>
class AverageWindowFilter 
{
	public:
	
		/// boring constructor
		AverageWindowFilter() :
			m_idx(0)
		{}

		/**
			\brief Adds a new point to the filter input, automatically
			cycles the point buffer if needed
		*/
		void AddPoint(Type point)
		{
			if (m_points.size() != num_points)
				m_points.push_back(point);
			else
				m_points[m_idx] = point;
				
			if (++m_idx == num_points)
				m_idx = 0;
		}
			
		/// returns the average of the points in the current window
		Type GetAverage()
		{
			Type result = 0;
			const typename std::vector<Type>::iterator end = m_points.end();
			for (typename std::vector<Type>::iterator i = m_points.begin(); i != end; i++)
				result += *i;
			
			return result / m_points.size();
		}
		
	private:
	
		std::vector<Type> m_points;
		size_t m_idx;
};


#endif
