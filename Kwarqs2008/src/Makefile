#
# Makefile
#

OUTPUT_NAME=kwarqs

PROC_TYPE=18f8520

TOOLS=../tools/bin
CC=$(TOOLS)/mcc18.exe

CFLAGS=-p $(PROC_TYPE) -I "../tools/h"  
CFLAGS+=-I "../src" 
CFLAGS+=-D_FRC_BOARD -D_LARGE_CODE -D_DONT_USE_TMR0 -mL -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

OBJECTS		= 	user_routines.o \
			ifi_startup.o	\
			ifi_utilities.o	\
			main.o	\
			user_SerialDrv.o\
			user_routines_fast.o \
			kwarqs.o	

LNKFILE=$(OUTPUT_NAME).lkr
LDOPTS		= /l "..\tools\lib" /m $(OUTPUT_NAME).map /w /o $(OUTPUT_NAME).cof
LD=$(TOOLS)/mplink.exe



$(OUTPUT_NAME): $(OBJECTS) $(LNKFILE)
	$(LD) $(LNKFILE) $(OBJECTS) $(LDOPTS) 
	
clean:
	rm -f *.o
	rm -f *.hex
	rm -f *.cof
	rm -f *.map
	rm -f *.out
	
#rule to make object file from source	
%.o : %.c
	$(CC) $(CFLAGS) $< 