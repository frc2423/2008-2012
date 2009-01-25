#include "math.h"

#define ACCEL_HIST_LEN 100 /* 10s at .1s rate */
#define FILT_COEFF .05
#define MOTION_THRESH .4

/* global variables */

static float yBuff[ACCEL_HIST_LEN];
static float xBuff[ACCEL_HIST_LEN];
static int buffIndex = 0;

static float xUpdate = 0;
static float yUpdate = 0;

static float xVelocity = 0;
static float yVelocity = 0;
static float xPos = 0;
static float yPos = 0;

static float xBias = 0;
static float yBias = 0;


void GetPosition( float *x, float *y)
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

void GetAcceleration( float *x, float *y)
{
  *x = xUpdate;
  *y = yUpdate;
    
}


void GetVelocity( float *x, float *y)
{
  *x = xVelocity;
  *y = yVelocity;
}


void AccelerationUpdate( float x, float y, float deltaTime)
{
  float alpha = FILT_COEFF;
  float mag;
  
  //mexPrintf("X: %f Y: %f\n",x,y);
  
  /* store the acclerations for estimating the bias */
  yBuff[buffIndex] = y;
  xBuff[buffIndex] = x;
  buffIndex++;
  if( buffIndex > ACCEL_HIST_LEN ) 
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

void AccelerationEstimateBias( float *x, float *y )
{

  int ind;
  float xSum = 0, ySum = 0;
  
  for( ind = 0; ind < ACCEL_HIST_LEN; ind++ )
  {
    xSum += xBuff[ind];
    ySum += yBuff[ind];
      
  }
  *x = xBias = xSum/ACCEL_HIST_LEN;
  *y = yBias = ySum/ACCEL_HIST_LEN;
      
    
}

void AccelerationSetBias( float x, float y)
{
    xBias = x;
    yBias = y;
}



