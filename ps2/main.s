	.module main.c
	.area memory(abs)
	.org 0x3e4c
_pth_handler_addr::
	.word 0
	.area data
_got_something::
	.blkb 2
	.area idata
	.word 0
	.area data
_received_byte::
	.blkb 1
	.area idata
	.byte 0
	.area data
_tmp::
	.blkb 1
	.area idata
	.byte 0
	.area data
_idx::
	.blkb 1
	.area idata
	.byte 0
	.area data
_error::
	.blkb 1
	.area idata
	.byte 0
	.area data
_bytes_received::
	.blkb 2
	.area idata
	.word 0
	.area data
	.area text
_main::
	leas -8,S
; /*
; 	Dustin Spicuzza
; 
; 	PS/2 interface test
; */
; 
; #include <mc9s12dp256.h>
; #include "dbug12.h"
; #include <stdio.h>
; 
; #pragma abs_address: 0x3e4c
; 	void (*pth_handler_addr)(void) = 0;
; #pragma end_abs_address
; 
; 
; void falling_edge(void);
; void delay100ms(void);
; 
; 
; int got_something = 0;
; 
; // whatever was received
; unsigned char received_byte = 0;
; 
; unsigned char tmp = 0;
; unsigned char idx = 0;
; unsigned char error = 0;
; 
; int bytes_received = 0;
; 
; #define enable_clk_interrupt() PIEH = 0x02; PIFH = 0x00
; #define disable_clk_interrupt() PIEH = 0x00; PIFH = 0x00
; 
; 
; void main()
; {
; 
; 	// data: 	PTH:0
; 	// clock: 	PTH:1
; 
; 	disable_clk_interrupt();
	clr 0x266
	clr 0x267
; 	
; 	pth_handler_addr = &falling_edge;
	ldy #_falling_edge
	sty _pth_handler_addr
; 
; 	DDRH = 0x00;	// everything is an input
	clr 0x262
; 	PPSH = 0x02;	// rising edge, pull down for input
	movb #2,0x265
; 	PERH = 0x01;	// enable pulling for this device
	movb #1,0x264
; 	PIFH &= 0xFD;
	bclr 0x267,#2
; 	
; 	// bring clock low for at least 100us
; 	DDRH = 0x02;
	movb #2,0x262
; 	PTH &= 0xFD;
	bclr 0x260,#2
; 	
; 	delay100ms();
	jsr _delay100ms
; 	
; 	enable_clk_interrupt();
	movb #2,0x266
	clr 0x267
; 
; 	
; 	// re-enable input, see what happens
; 	DDRH = 0x00;
	clr 0x262
	bra L4
L3:
; 
; 	
; 	while (1)
; 	{
; 		if (error)
	ldab _error
	cmpb #0
	beq L6
; 		{
; 			printf("Error: %d %d %d\r\n", idx, tmp, received_byte);
	ldab _received_byte
	clra
	tfr D,Y
	sty 6,S
	ldab _tmp
	clra
	tfr D,Y
	sty 4,S
	ldab _idx
	clra
	tfr D,Y
	std 2,S
	ldy #L8
	sty 0,S
	jsr _printf
; 			asm("swi");
	swi
; 		}
L6:
; 	
; 		if (got_something)
	ldy _got_something
	cpy #0
	beq L9
; 		{		
; 			printf("Got something: %d %d\r\n", received_byte, bytes_received);
	ldy _bytes_received
	sty 4,S
	ldab _received_byte
	clra
	tfr D,Y
	std 2,S
	ldy #L11
	sty 0,S
	jsr _printf
; 			got_something = 0;
	movw #0,_got_something
; 		}
L9:
; 	}
L4:
	bra L3
X0:
L2:
	.dbline 0 ; func end
	leas 8,S
	rts
_falling_edge::
	leas -4,S
; 
; 	asm("swi");
; 	
; }
; 
; #pragma interrupt_handler falling_edge
; void falling_edge()
; {
; 	switch (idx)
	ldab _idx
	clra
	tfr D,Y
	std 2,S
	cpd #0
	tfr D,Y
	beq L16
	ldy 2,S
	cpy #1
	beq L19
	ldy 2,S
	cpy #2
	beq L19
	ldy 2,S
	cpy #3
	beq L19
	ldy 2,S
	cpy #4
	beq L19
	ldy 2,S
	cpy #5
	beq L19
	ldy 2,S
	cpy #6
	beq L19
	ldy 2,S
	cpy #7
	beq L19
	ldy 2,S
	cpy #8
	beq L19
	ldy 2,S
	cpy #10
	lbeq L14
	ldy 2,S
	cpy #11
	beq L21
	bra L13
L16:
; 	{
; 		// start bit
; 		case 0:
; 			// bit should be 0
; 			if (PTH & 0x01)
	brclr 0x260,#1,L14
; 				error = 1;
	movb #1,_error
; 			break;
	bra L14
L19:
; 			
; 		// data bits
; 		case 1:
; 		case 2:
; 		case 3:
; 		case 4:
; 		case 5:
; 		case 6:
; 		case 7:
; 		case 8:
; 			tmp |= ((PTH & 0x01) << (idx-1));
	ldab _idx
	clra
	tfr D,Y
	dey
	tfr Y,X
	ldab 0x260
	clra
	anda #0
	andb #1
	jsr lsl16
	tfr D,Y
	ldab _tmp
	clra
	sty 0,S
	ora 0,S
	orb 1,S
	stab _tmp
; 			break;
	bra L14
L21:
; 		
; 		// parity
; 		case 10:
; 			break;
; 		// stop bit
; 		case 11:
; 			if (!(PTH & 0x01))
	ldab 0x260
	bitb #1
	bne L22
; 			{
; 				error = 1;
	movb #1,_error
; 				break;
	bra L14
L22:
; 			}
; 			
; 			received_byte = tmp;
	movb _tmp,_received_byte
; 			idx = 0;
	clr _idx
; 			bytes_received += 1;
	ldy _bytes_received
	iny
	sty _bytes_received
; 			got_something += 1;
	ldy _got_something
	iny
	sty _got_something
; 			
; 			break;
	bra L14
L13:
; 			
; 		default:
; 			error = 1;
	movb #1,_error
; 	}
L14:
; 	
; 	if (!error)
	ldab _error
	cmpb #0
	bne L24
; 	{
; 		idx += 1;
	inc _idx
; 	
; 		// clear the associated interrupt
; 		PIFH |= 0x02;
	bset 0x267,#2
; 	}
L24:
L12:
	.dbline 0 ; func end
	leas 4,S
	rti
;              c -> 1,SP
_putchar$device_specific$::
	pshd
; }
; 
; // needed for printf
; int putchar(char c)
; {
; 	return MyUserFNP->DB12putchar(c);
	ldab 1,S
	clra
	ldy 0xee86
	jsr 0,Y
L26:
	.dbline 0 ; func end
	leas 2,S
	rts
_delay100ms::
; }
; 
; /*
; 	delay100ms
; 	
; 	Delays a program by approximately 100ms
; 
; 	cycles = 100ms/125ns = 800000
; 	3xy + 3y = cycles
; 	3xy + 3y --> x=964, y = 276
; 	
; */
; void delay100ms()
; {
; asm("	ldx #$114		");		// 2 cycles (Y variable)
		ldx #$114		
; asm("l_out:				");	
	l_out:				
; asm("	ldy #$3c4		");		// 2 cycles (X variable)
		ldy #$3c4		
; asm("l_in:				");
	l_in:				
; asm("	dbne y, l_in	");		// 3 (not taken) /1 (taken) cycles
		dbne y, l_in	
; asm("	dbne x, l_out	");		// 3 (not taken) /1 (taken) cycles
		dbne x, l_out	
L27:
	.dbline 0 ; func end
	rts
L11:
	.byte 'G,'o,'t,32,'s,'o,'m,'e,'t,'h,'i,'n,'g,58,32,37
	.byte 'd,32,37,'d,13,10,0
L8:
	.byte 'E,'r,'r,'o,'r,58,32,37,'d,32,37,'d,32,37,'d,13
	.byte 10,0
