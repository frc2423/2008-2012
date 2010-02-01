
#include <stdio.h>
#include <cmath>


class CoordinateSystem {
public:

	CoordinateSystem( double wheelbase ) :
		m_x(0.0),
		m_y(0.0),
		m_wheelbase(wheelbase)
	{}


	/*
		ComputeNewPosition
		
		Assumes a uniform motion for each wheel over some period
		with no slippage. 
	
		@param l		Left meters
		@param r		Right meters
	
		Left meters/right meters:
			If they are positive, the wheel is moving in the forward
			direction
	*/
	void ComputeNewPosition(double l, double r)
	{
		double dx, dy, dtheta;
	
		dtheta = ( r - l ) / m_wheelbase;
		
		// dy = ((r+l)/(2.0 *dtheta)) * sin(dtheta);
		// dx = - ((r+l)/(2.0 * dtheta)) * (1.0 - cos(dtheta))
		//
		// BUT
		//
		// you need to expand these out using a taylor series approximation
		// to avoid divide by zero errors. So we end up with a more
		// complicated expression that looks like this:
		//
		// dy = ((r+l)/2.0 ) * ( 1 - dtheta^2 / 3! + dtheta^4/5! );
		// dx = - ((r+l)/2.0) * (dtheta/2! - dtheta^3/4! + dtheta^5/6!)
		//
		
		if (fabs(dtheta) > M_PI/36)
		{
			// anything more than five degrees: simple expression
			dy = ((r+l)/(2.0 * dtheta)) * sin(dtheta);
			dx = - ((r+l)/(2.0 * dtheta)) * (1.0 - cos(dtheta));
		}
		else
		{
			// anything less than five degrees: more complex expression
			dy = ((r+l)/2.0 ) * ( 1 - (dtheta*dtheta)/ 6 + (dtheta*dtheta*dtheta*dtheta)/120);
			dx = - ((r+l)/2.0) * (dtheta/2 - (dtheta*dtheta*dtheta)/24 + (dtheta*dtheta*dtheta*dtheta*dtheta)/720);
		}

		TransformCoordinates( dx, dy, dtheta );
	}
	
	/**
		Internal function
		
		Transforms coordinates to the original frame of reference
	*/
	void TransformCoordinates(double dx, double dy, double dtheta)
	{
		m_x = m_x + dx * cos(m_theta) - dy * sin(m_theta);
		m_y = m_y + dx * sin(m_theta) + dy * cos(m_theta);
		
		// calculate this last
		m_theta = m_theta + dtheta;
	}
	
	void Reset()
	{
		m_x = 0.0;
		m_y = 0.0;
		m_theta = 0.0;
	}

	void Print()
	{
		printf("x: %9.6f y: %9.6f theta: %9.6f\n", m_x, m_y, m_theta * (180.0/M_PI) );
	}
	
	
	// these are in meters, relative to the original starting
	// position
	double m_x;
	double m_y;
	
	/**
		Units in radians; traditional math format
		
		0 degrees straight ahead, 90 degrees is directly left
		
		   0
		90  270
		  180
	*/
	double m_theta;
	
	double m_wheelbase;
	
};

#define STEP_CS( l, r) \
	printf("Testing: (%9.6f, %9.6f) -> ", (double)(l), (double)(r) ); \
	cs.ComputeNewPosition( (l), (r) );	\
	cs.Print()	


#define SINGLE_CS( l , r ) \
	STEP_CS( l, r );		\
	cs.Reset()
	

int main()
{
	CoordinateSystem cs(1.0);
	
	SINGLE_CS( 0, 0 );
	
	// obvious cases: straight ahead
	SINGLE_CS( 1, 1 );				// x: 0, y: 1,  theta: 0
	SINGLE_CS( -1, -1 );			// x: 0, y: -1, theta: 0
	
	// turning in place
	SINGLE_CS( -M_PI/4, M_PI/4 ); 	// left: x: 0, y: 0, theta: 90 
	SINGLE_CS( M_PI/4, -M_PI/4 );	// right: x: 0, y: 0, theta: -90
	
	
	SINGLE_CS( 0 , M_PI/2.0 ); 		// turning left
									// x: -.5, y: .5, theta: 90
	SINGLE_CS( M_PI/2.0, 0 );		// turning right
									// x: .5, y: .5, theta: -90
									
	// three degrees left								
	SINGLE_CS( 0, 3 * (M_PI/180.0) );
	
	// three degrees right
	SINGLE_CS( 0, 3 * (M_PI/180.0) );
	
	// lots of circles
	SINGLE_CS( 0, 18 * M_PI );
			
	// lots of circles and a half
	SINGLE_CS( 0, 17 * M_PI );
	
	//
	SINGLE_CS( 7 + M_PI / 2, 7 );
			
	// going forward, turning in place to left, forward, left
	printf("Series:\n");
	STEP_CS( 1, 1 );
	STEP_CS( M_PI/4, -M_PI/4 );
	STEP_CS( 1, 1 );
	STEP_CS( 0 , M_PI/2.0 ); 
									
}

