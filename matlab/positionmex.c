#include "mex.h"
#include "math.h"
#include "positionTracking.h"

#define FRAME_SIZE 1000000
float x[FRAME_SIZE];
float y[FRAME_SIZE];

#define MAX_COEFFS 40
short nCoeffs[MAX_COEFFS];
short nHist[MAX_COEFFS];

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    
    unsigned int ulNumSamples;
	unsigned int ulIndex = 0;
	unsigned int i,j;
    
    float x_accel, y_accel, x_pos, y_pos, x_bias, y_bias;

	double *pdXData, *pdYData;
    double *pdOut0, *pdOut1, *pdOut2, *pdOut3;
 	
	/* check for proper number of arguments */
	if( nrhs != 2)
		mexErrMsgTxt("2 Inputs Required");
	
   
    /* assign pointers to the input arguments */
    pdXData  = mxGetPr(prhs[0]);
    pdYData  = mxGetPr(prhs[1]);
    
    ulNumSamples = (unsigned int)mxGetM(prhs[1]);
    printf("Num samples: %d\n", ulNumSamples);
    


	/* create a matrix for the return argument and assign a pointer to it */
   plhs[0] = mxCreateDoubleMatrix((int)ulNumSamples, 1, mxREAL);
   pdOut0 = mxGetPr(plhs[0]);
   #if 1
   plhs[1] = mxCreateDoubleMatrix((int)ulNumSamples, 1, mxREAL);
   pdOut1 = mxGetPr(plhs[1]);
   plhs[2] = mxCreateDoubleMatrix((int)ulNumSamples, 1, mxREAL);
   pdOut2 = mxGetPr(plhs[2]);
   plhs[3] = mxCreateDoubleMatrix((int)ulNumSamples, 1, mxREAL);
   pdOut3 = mxGetPr(plhs[3]);
   #endif

    
    AccelerationReset();
    AccelerationSetBias( .9511, .6285);
    
    for( i=0; i<ulNumSamples; i++)
    {
       x[i] = (float)*pdXData++;
       y[i] = (float)*pdYData++;
  

        AccelerationUpdate( x[i], y[i], (float).1);
        GetAcceleration( &x_accel, &y_accel);
        GetPosition( &x_pos, &y_pos);
        
        pdOut0[i] = (double)x_accel;
        pdOut1[i] = (double)y_accel;
        
        pdOut2[i] = (double)x_pos;
        pdOut3[i] = (double)y_pos;
  
        
 
        
    }
    
    AccelerationEstimateBias( &x_bias, &y_bias);
    printf("Bias x: %f y: %f \n", x_bias, y_bias);

    
    
}

