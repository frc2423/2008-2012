#!/bin/bash

CC=/cygdrive/c/Mcc18/bin/mcc18.exe
CFLAGS='-p 18f8520 -I c:\Mcc18\h -I c:\projects\FIRST\robot_controller\firmware\FrcCode_2007_8722 -D_FRC_BOARD -D_LARGE_CODE -D_DONT_USE_TMR0 -mL -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-'

$CC $CFLAGS user_routines.c 
$CC $CFLAGS ifi_startup.c  
$CC $CFLAGS ifi_utilities.c  
$CC $CFLAGS main.c 
$CC $CFLAGS user_SerialDrv.c  
$CC $CFLAGS user_routines_fast.c
/cygdrive/c/Mcc18/bin/mplink mylkr.lkr user_routines.o ifi_startup.o ifi_utilities.o main.o user_SerialDrv.o user_routines_fast.o /l "c:\Mcc18\lib" /m myapp.map /w /o"myapp.cof"