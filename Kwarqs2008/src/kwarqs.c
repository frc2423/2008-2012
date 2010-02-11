#include <stdio.h>

#include "ifi_aliases.h"
#include "ifi_default.h"
#include "ifi_utilities.h"
#include "kwarqs.h"

#define MOTOR_IDLE   127

/* define the hybrid state */
#define IDLE        0
#define FORWARD     1
#define SPIN_LEFT   2
#define SPIN_RIGHT  3
#define WAIT        4

/* define the times */
#define FORWARD_TIME 10 // ~ 1 sec
#define SPIN_TIME 2
#define WAIT_TIME 1

int filterParam = 3;

/* local prototypes */
void RunTele(void);
void RunHybrid( int in0, int in1, int in2 );
void HybridStop(void);
void HybridMove(int valLeft, int valRight);
int FilterInput( int input, int history, int filterParam);
void OneJoyStickControl(void);
int ScaleInput( int input );
void StepperControl( void );

/*
 * function: KwarqsStart
*/
int PrevRightTrigState = 0;
int PrevLeftTrigState = 0;
int AmoryMode=0;
int RobotMode = 0;


void KwarqsStart(void)
{
  int i;

  /* print out the values of the operator controls */
#if 0
   {
    printf("L/R=%03d/%03d,  trig: %d->%d: %d,  buttons: %d %d %d %d\n\r", 
		   leftJoystickMotorCtl_p1_y, rightJoystickMotorCtl_p2_y,
		   PrevRightTrigState, RightTrigState, RobotMode, irButton0, irButton1,
		   irButton2,irButton3
		   );
  }
#endif 
  
  /* determine the mode based on the thumb trigger input */
  if (PrevRightTrigState == 0 && RightTrigState == 1)
  {
	    RobotMode = 1 - RobotMode;
  }
  
  if (PrevLeftTrigState == 0 && LeftTrigState == 1)
  {
	    AmoryMode = 1 - AmoryMode;
  }
  
//#if 0
  /* choose which function to run bases on the mode */
  if( AmoryMode == 1)
  {
    //printf("Hello Amory!!! Do you want to play a game ? ...\r\n");
	/* Amory you can place your own fucntion here */
	
	/* HEY!  I've got a great idea ... why not try your 1 joystick function here */`
	OneJoyStickControl();
	//OneJoyStickControl();
	//HybridStop();
	
  }
  else if( RobotMode == 1)
    RunTele();
   else
    RunTele();
   //RunHybrid(irButton0, irButton1, irButton2);
 //#endif
 
 //StepperControl();
 
 

  PrevRightTrigState = RightTrigState;
}


/*
 * function: RunTele
*/


void RunTele(void)
{
  int leftJoystickDampened;
  int rightJoystickDampened;

  static int leftJoystickPrev = 127;
  static int rightJoystickPrev = 127;
  static int count = 0;

  /*
  if( count++ > 100 )
  {
    HybridStop();
	return;
  }
  */
  	
  /* smooth the jostick inputs */	
  leftJoystickPrev = FilterInput( leftJoystickMotorCtl_p1_y, leftJoystickPrev, filterParam);
  rightJoystickPrev = FilterInput( rightJoystickMotorCtl_p2_y, rightJoystickPrev, filterParam);
	
  // scale the input by to slow the output down a bit
  //REMEMBER: the scaling factor may have to change based on the wieght
  leftJoystickDampened  =  ScaleInput(leftJoystickPrev) ;
  rightJoystickDampened =  ScaleInput(rightJoystickPrev);
  
  if (count == 10)
  {  
	count = 0;
    printf("Left In: %d  Left Out:  %d  Right In:  %d  Right Out: %d \n\r", leftJoystickMotorCtl_p1_y, leftJoystickDampened,rightJoystickMotorCtl_p2_y, rightJoystickDampened );
  }
  else 
    count++;
  leftMotorOut1_pwm03 = 255 - leftJoystickDampened;
  leftMotorOut2_pwm04 = 255 - leftJoystickDampened;
  
  rightMotorOut1_pwm01 = rightJoystickDampened;
  rightMotorOut2_pwm02 = rightJoystickDampened;
}



void OneJoyStickControl(void)
{
  int p1_xOP =  255 - p1_x;
  int rightMotors, leftMotors;
  static int count2 = 0;
  int add = 0;
  //printf("function is being called \n\r");
  int p1_x_local = p1_x;
  int p1_y_local = p1_y;  
  
  leftMotors = 381 - (p1_y_local + p1_xOP);
  rightMotors = -127 + (p1_y_local + p1_x_local);
  
  if (leftMotors < 0)
  {  
	leftMotors = 0;
  } 
  else if (leftMotors > 255)
  {
    leftMotors = 255;
  }
  else if ((leftMotors > 117) && (leftMotors < 137))
  {
    leftMotors = 127;
  }	
  leftMotors = ScaleInput(leftMotors);
  leftMotorOut1_pwm03 = leftMotors;
  leftMotorOut2_pwm04 = leftMotors;

  
  if (rightMotors < 0)
  {
	rightMotors = 0;

  }
  else if (rightMotors > 255)
  {
    rightMotors = 255;

  }
  else if ((rightMotors > 117) && (rightMotors < 137))
  {
	rightMotors = 127;

  }
  rightMotors = ScaleInput(rightMotors);  
  rightMotorOut1_pwm01 = rightMotors;
  rightMotorOut2_pwm02 = rightMotors;  
  

  if (count2 == 200)
  {    
	count2 = 0;
	printf("leftMotors = 381 - (p1_y + p1_xOP)\r\n");
	printf("p1_y = %i\r\n", p1_y_local);
	printf("p1_xOP = %i\r\n", p1_xOP);
	printf("%d + %d = %i\r\n", p1_y_local, p1_xOP, p1_y_local + p1_xOP);
	printf("381 - %d = %i\r\n", p1_y_local + p1_xOP, 381 - (p1_y_local + p1_xOP));
	printf("\r\n");
	printf("rightMotors = -127 + (p1_y_local + p1_x_local)\r\n");
	printf("p1_y = %d\r\n", p1_y_local);
	printf("p1_x = %d\r\n", p1_x_local);
	printf("%d + %d = %d\r\n", p1_y_local, p1_x_local, p1_y_local + p1_x_local);
	printf("-127 + %d = %d\r\n", p1_y_local + p1_x_local, -127 + (p1_y_local + p1_x_local));
    while ( add <20) 
	{
   	  add++;   
	  printf("\r\n");
	}
  }
  else 
    count2++;     

   /*  if ((p1_y+p1_xOP) <128)										//Sets right motors
  {
    rightMotorOut1_pwm01 = 255;
    rightMotorOut2_pwm02 = 255;

  }
  else if ((p1_y+p1_xOP) > 381)
  {
    rightMotorOut1_pwm01 = 0;
    rightMotorOut2_pwm02 = 0;
  }
  else if (((p1_y+p1_xOP) > 127) && ((p1_y+p1_xOP) <382))	
  {
	rightMotorOut1_pwm01 = 382 - (p1_y+p1_xOP);
	rightMotorOut2_pwm02 = 382 - (p1_y+p1_xOP);
  }

  if ((p1_y+p1_x) <128)											//Sets left motors
  {
	leftMotorOut1_pwm03 = 0;
	leftMotorOut2_pwm04 = 0;
  }
  else if ((p1_y+p1_x) > 381)
  {
    leftMotorOut1_pwm03 = 255;
	leftMotorOut2_pwm04 = 255;
  }
  else if (((p1_y+p1_x) > 127) && ((p1_y+p1_x) <382))	
  {
	leftMotorOut1_pwm03 = -127 + (p1_y + p1_x);
	leftMotorOut2_pwm04 = -127 + (p1_y + p1_x);
  }
  
  if (rightMotorOut1_pwm01 < 0)									//Sets right motors if they are greater than max values
  {  
	rightMotorOut1_pwm01 = 0;
	rightMotorOut2_pwm02 = 0;
  }
  if (rightMotorOut1_pwm01 > 255)
  { 
	rightMotorOut1_pwm01 = 255;
	rightMotorOut2_pwm02 = 255;
  }
  if (leftMotorOut1_pwm03 < 0)									//Sets left motors if they are greater than max values
  {  
	leftMotorOut1_pwm03 = 0;
	leftMotorOut2_pwm04 = 0;
  }
  if(leftMotorOut1_pwm03 > 255)
  {  
	leftMotorOut1_pwm03 = 255;
	leftMotorOut2_pwm04 = 255;
  }
  */
/*  if (count2 == 50)
  {  
	count2 = 0;
    printf("Y input value is %d, X input value is %d, XOP value is %d \r\n", p1_y, p1_x, p1_xOP);
    printf("Left motors are set to %d, right motors are set to %d\r\n\n", leftMotorOut1_pwm03, rightMotorOut1_pwm01);
    printf("Other left motor: %d, other right motor: %d\r\n", leftMotorOut2_pwm04, rightMotorOut2_pwm02);
  }
  else 
    count2++; 
*/	
  //printf("Y input value is %d, X input value is %d, XOP value is %d, Left motors are set to %d, right motors are set to %d.\r\n", p1_y, p1_x, p1_xOP,rightMotorOut1_pwm01, leftMotorOut1_pwm03);
  //printf("cheese");
	
/*		Motor speeds:  0-127 is counterclockwise and the closer it is to 0 the faster the motor is spinning counterclockwise.
				   127-255 is clockwise and the closer it is to 255 the faster the motor is spinning clockwise.
				   
		The joystick: The joystick has two axis. One is the y_axis which is bottom to top ranging from 0 to 255. Bottom most value is 0 and top most value is 255.
				 The other is the x_axis which is left to right ranging from 0 to 255. Left most value is 0 and right most value is 255.
				 
		Mapping the joystick to the motor speeds: You can get any motor speed by combining the current value on the x_axis on the joystick and the y_value currently
				on the joystick and plugging them into the different cases above (each if statement). I will now explain why you would combine the values on the
				x_axis and y_axis.
		
		Why combine the x_axis and y_axis: This is the simplest way I can put it... when I was making this one joystick system I put in the motor speeds that would 
				move the robot in the directions I thought were apropriate for the positions on the joystick (i.e. move the joystick top left (x_axis = 0, y_axis = 255)
				the robot should move forward left). I came out with the values you might have seen on the board. I also thought that the further the joystick was away
				from the center the faster it should go (just like in tank). I discovered a pattern in the values I put along the edges... the value directly diagnal to that value
				had a value that was equal to it. So if I had a value of 0,255 the value diagnal to it would have a value of 0,0. You can see the first values are the same.
				On the other oposite I saw a value of 255,255. You can see the second values are the same. This means every value running on the diagnals were exactly the same.
				I use the equations above to get the correct values according to what I thought would be correct according to the placement of the joystick.
*/				


}
/*
 * function: RunHybrid
*/

void RunHybrid( int button0, int button1, int button2 )
{
  // IR button 1

  static int hybridState = IDLE;
  static int timer=0;
 
  switch ( hybridState )
  {
    case IDLE:
	  timer = 0;
	  if( button0 == 1)
	    hybridState = FORWARD;	
      else if( button1 == 1)
        hybridState = SPIN_LEFT;
	  else if( button2 == 1)
	    hybridState = SPIN_RIGHT;
	break;
	
	case FORWARD:
	  //if( timer++ < FORWARD_TIME )
	  if( button0 )
	    HybridMove(255-150,150);
	  else
	  {
 		hybridState = WAIT;
		timer = 0;
	  }
	break;

	case SPIN_LEFT:
	  if( timer++ < SPIN_TIME )
	    HybridMove(127,150);
	  else
	  {
		hybridState = WAIT;
		timer =0;
	  }
	break;
	
	case SPIN_RIGHT:
	  if( timer++ < SPIN_TIME )
	    HybridMove(255-150,127);
	  else
	  {
		hybridState = WAIT;
		timer = 0;
	  }
	break;
	
	
	case WAIT:
	  if( timer++ > WAIT_TIME )
	  {
	    HybridStop();
		hybridState = IDLE;
		timer = 0;
	  }
	  
	
	
	
    default:   
	break;
  }
}

//HybridMove
void HybridMove(int valLeft, int valRight)
{
  /* map the joystick controls to the motor controls */
  leftMotorOut1_pwm03 = valLeft;
  leftMotorOut2_pwm04 = valLeft;
  
  rightMotorOut1_pwm01 = valRight;
  rightMotorOut2_pwm02 = valRight;
}

void HybridStop(void)
{
  int leftVal = MOTOR_IDLE;
  int rightVal = MOTOR_IDLE;  
	
  /* map the joystick controls to the motor controls */
  leftMotorOut1_pwm03 = leftVal;
  leftMotorOut2_pwm04 = leftVal;
  
  rightMotorOut1_pwm01 = rightVal;
  rightMotorOut2_pwm02 = rightVal;
}

//FilterInput
int FilterInput( int input, int previous, int filterParam)
{
  int output=0;

  if (input >= previous)
	output = previous + (( input - previous) >> filterParam);
  else
	output = previous - (( previous - input) >> filterParam);
	
  //printf("Inputs: %d Prev %d Output %d \r\n", input, previous, output);	
  
  return( output );
}


//ScaleInput
int ScaleInput( int input )
{

  int temp;
  int sign;
  int val;
  
  //subract 127, the joystick vaule
  //REMEMBER: 127 is the neutral value for the joystick
  temp = input - 127;
  
  // poor man's abssolute value
  if( temp < 0)
  {
    val = -1*temp;
  }
  else
    val = temp;
  
  //scale the tresult by 80 %
  //Integer math is used  
  val = (val*4)>>4;
  
  if( temp < 0 )
    val *=-1; 
  
  
  //add back the joystcik
  val += 127;
  
  return( val); 
  

}

//StepperControl
void StepperControl( void )
{

  int control0 = 0;
  int control1 = 0;
  int control2 = 0;


  if( leftJoystickMotorCtl_p1_y > 193 )
  {
    control0 = 1;
  }    
  else if( leftJoystickMotorCtl_p1_x > 193 )
  {
    control1 = 1;
  }    
  else if( leftJoystickMotorCtl_p1_x < 64 )
  {
    control2 = 1;
  }    
 
  //printf("x val %d\r\n",leftJoystickMotorCtl_p1_x);
 
  RunHybrid(control0,control1,control2);


}


