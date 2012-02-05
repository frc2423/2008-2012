#ifndef _POSITIONTRACKING_H_
#define _POSITIONTRACKING_H_

void GetPosition( float *x, float *y);
void GetHeading( void );
void ResetHeading( void );
void GetAcceleration( float *x, float *y);
void GetVelocity( float *x, float *y);
void AccelerationUpdate( float x, float y, float deltaTime);
void AccelerationReset( void );
void AccelerationEstimateBias( float *x, float *y );
void AccelerationSetBias( float x, float y);

#endif
