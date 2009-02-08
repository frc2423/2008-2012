CC = icc12w
LIB = ilibw
CFLAGS =  -e -D__ICC_VERSION="7.05" -DHC9S12DP256  -l 
ASFLAGS = $(CFLAGS) 
LFLAGS =  -btext:0x1000 -dinit_sp:0x4000 -fmots19
FILES = main.o 

PS2:	$(FILES)
	$(CC) -o PS2 $(LFLAGS) @PS2.lk  
	C:\iccv712\bin\SRecCVT -lp -m c0000 FFFFF 32 -o PS2cvt.s19 PS2.s19 
main.o: C:\iccv712\include\mc9s12dp256.h .\dbug12.h C:\iccv712\include\stdio.h C:\iccv712\include\stdarg.h C:\iccv712\include\_const.h
main.o:	main.c
	$(CC) -c $(CFLAGS) main.c
