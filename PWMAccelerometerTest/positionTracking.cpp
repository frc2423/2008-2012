#include "math.h"

#define ACCEL_HIST_LEN 50 /* 10s at .1s rate */
#define FILT_COEFF .1
#define MOTION_THRESH .4

/* global variables */

static float yBuff[ACCEL_HIST_LEN];
static float xBuff[ACCEL_HIST_LEN];
static int buffIndex = 0;

static double xUpdate = 0;
static double yUpdate = 0;

static double xVelocity = 0;
static double yVelocity = 0;
static double xPos = 0;
static double yPos = 0;

static double xBias = 0;
static double yBias = 0;


void GetPosition( double *x, double *y)
{
  *x = xPos;
  *y = yPos;
    
}

void GetHeading()
{
    
}


void ResetHeading()
{
    
    
}

void GetAcceleration( double *x, double *y)
{
  *x = xUpdate;
  *y = yUpdate;
    
}


void GetVelocity( double *x, double *y)
{
  *x = xVelocity;
  *y = yVelocity;
}


void AccelerationUpdate( double x, double y, double deltaTime)
{
  double alpha = FILT_COEFF;
  double mag;
  
  //mexPrintf("X: %f Y: %f\n",x,y);
  
  /* store the acclerations for estimating the bias */
  yBuff[buffIndex] = y;
  xBuff[buffIndex] = x;
  buffIndex++;
  if( buffIndex >= ACCEL_HIST_LEN ) 
      buffIndex = 0;

  /* filter the accelerations and subtract the means */
  xUpdate = alpha*( x - xBias ) + (1-alpha)*xUpdate;
  yUpdate = alpha*( y - yBias ) + (1-alpha)*yUpdate;
    
  /* compute the magnitude of the acceleration to check if we are moving */
  mag = sqrt(xUpdate*xUpdate + yUpdate*yUpdate);
  
  if( mag > MOTION_THRESH ) 
  {
      
    /* update velocity and position */  
    xVelocity += xUpdate*deltaTime;
    xPos += xVelocity*deltaTime;
    
    yVelocity += yUpdate*deltaTime;
    yPos += yVelocity*deltaTime;
  }
  else
  {    
	  xVelocity = 0;
	  yVelocity = 0;
	  
  }
      
  
  
}


void AccelerationReset( void )
{
    
  xUpdate = 0;
  xVelocity = 0;
  xPos = 0;
  
  yUpdate = 0;
  yVelocity = 0;
  yPos = 0;
 
  buffIndex = 0;
  
    
    
}

void AccelerationEstimateBias( double *x, double *y )
{

  int ind;
  double xSum = 0, ySum = 0;
  
  for( ind = 0; ind < ACCEL_HIST_LEN; ind++ )
  {
    xSum += xBuff[ind];
    ySum += yBuff[ind];
      
  }
  *x = xBias = xSum/ACCEL_HIST_LEN;
  *y = yBias = ySum/ACCEL_HIST_LEN;
      
    
}

void AccelerationSetBias( double x, double y)
{
    xBias = x;
    yBias = y;
}



