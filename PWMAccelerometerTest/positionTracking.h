#ifndef _POSITIONTRACKING_H_
#define _POSITIONTRACKING_H_

void GetPosition( double *x, double *y);
void GetHeading( void );
void ResetHeading( void );
void GetAcceleration( double *x, double *y);
void GetVelocity( double *x, double *y);
void AccelerationUpdate( double x, double y, double deltaTime);
void AccelerationReset( void );
void AccelerationEstimateBias( double *x, double *y );
void AccelerationSetBias( double x, double y);

#endif
