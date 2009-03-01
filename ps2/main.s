	.module main.c
	.area memory(abs)
	.org 0x3e4c
_pth_handler_addr::
	.word 0
	.area data
_receive_mode::
	.blkb 1
	.area idata
	.byte 0
	.area data
_send_complete::
	.blkb 1
	.area idata
	.byte 0
	.area data
_byte_to_send::
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
_buf_pos::
	.blkb 2
	.area idata
	.word 0
	.area data
_buf_cnt::
	.blkb 2
	.area idata
	.word 0
	.area data
	.area text
;        buf_idx -> 2,SP
;           byte -> 4,SP
_get_byte::
	pshd
	leas -4,S
; /*
; 	Dustin Spicuzza
; 
; 	PS/2 interface test
; 	
; 	Assumes PS/2 clk at PTH:1, and PS/2 data at PTH:0
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
; void delay100us(void);
; 
; 
; unsigned char receive_mode = 0;
; 
; unsigned char send_complete = 0;
; unsigned char byte_to_send = 0;
; 
; // interrupt service vars
; unsigned char tmp = 0;
; unsigned char idx = 0;
; unsigned char error = 0;
; 
; int bytes_received = 0;
; 
; #define enable_clk_interrupt() PIEH = 0x02; PIFH = 0x00
; #define disable_clk_interrupt() PIEH = 0x00; PIFH = 0x00
; 
; // circular buffer
; #define BUFFER_SZ 16
; char received[BUFFER_SZ];
; int buf_pos = 0;
; int buf_cnt = 0;
; 
; // returns false if no byte received
; int get_byte(char * byte)
; {
; 	int buf_idx = buf_pos - buf_cnt;
	ldd _buf_pos
	subd _buf_cnt
	tfr D,Y
	std 2,S
; 
; 	if (buf_cnt == 0)
	ldy _buf_cnt
	cpy #0
	bne L3
; 		return 0;
	ldd #0
	bra L2
L3:
; 		
; 	if (buf_idx < 0)
	ldy 2,S
	cpy #0
	bge L5
; 		buf_idx += BUFFER_SZ;
	ldd 2,S
	addd #16
	tfr D,Y
	std 2,S
L5:
; 	
; 	*byte = received[buf_idx];
	ldd 2,S
	ldy #_received
	sty 0,S
	addd 0,S
	tfr D,Y
	ldab 0,Y
	tfr B,Y
	ldx 4,S
	tfr Y,B
	stab 0,X
; 	buf_cnt -= 1;
	ldy _buf_cnt
	dey
	sty _buf_cnt
; 	return 1;
	ldd #1
L2:
	.dbline 0 ; func end
	leas 6,S
	rts
;           byte -> 3,SP
_put_byte::
	pshd
	leas -2,S
; }
; 
; void put_byte(char byte)
; {
; 	received[buf_pos] = byte;
	ldd _buf_pos
	ldy #_received
	sty 0,S
	addd 0,S
	tfr D,Y
	ldab 3,S
	tfr B,B
	stab 0,Y
; 	buf_pos += 1;
	ldy _buf_pos
	iny
	sty _buf_pos
; 	if (buf_cnt < 16)
	ldy _buf_cnt
	cpy #16
	bge L8
; 		buf_cnt += 1;
	ldy _buf_cnt
	iny
	sty _buf_cnt
L8:
; 	if (buf_pos == BUFFER_SZ)
	ldy _buf_pos
	cpy #16
	bne L10
; 		buf_pos = 0;
	movw #0,_buf_pos
L10:
; 		
; 	bytes_received += 1;
	ldy _bytes_received
	iny
	sty _bytes_received
L7:
	.dbline 0 ; func end
	leas 4,S
	rts
;           byte -> 1,SP
_send_byte::
	pshd
; }
; 
; 
; void send_byte(unsigned char byte)
; {
; 	send_complete = 0;
	clr _send_complete
; 	
; 	// bring clock low for at least 100us
; 	DDRH = 0x02;
	movb #2,0x262
; 	PTH &= 0xFD;
	bclr 0x260,#2
; 	delay100us();
	jsr _delay100us
; 	
; 	byte_to_send = byte;
	movb 1,S,_byte_to_send
; 	receive_mode = 0;
	clr _receive_mode
; 	
; 	// send a byte
; 	DDRH = 0x01;
	movb #1,0x262
; 	PTH &= 0xFE;		// pull it low
	bclr 0x260,#1
L13:
; 	
; 	// todo: some timeout
; 	while (!send_complete);
L14:
	ldab _send_complete
	cmpb #0
	beq L13
; 	send_complete = 0;
	clr _send_complete
L12:
	.dbline 0 ; func end
	leas 2,S
	rts
;        retries -> 12,SP
;        success -> 13,SP
;           recv -> 14,SP
_main::
	leas -15,S
; }
; 
; 
; void main()
; {
; 	// data: 	PTH:0
; 	// clock: 	PTH:1
; 
; 	char recv = 0, retries = 0, success = 0;
	clr 14,S
	clr 12,S
	clr 13,S
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
; 	PPSH = 0x00;	// falling edge, pull down for input
	clr 0x265
; 	PERH = 0x01;	// enable pulling for this device
	movb #1,0x264
; 	PIFH &= 0xFD;
	bclr 0x267,#2
; 	
; 	// ready to go
; 	enable_clk_interrupt();
	movb #2,0x266
	clr 0x267
; 	asm("cli");
	cli
	bra L18
L17:
; 
; 	
; 	while (retries++ < 3)
; 	{
; 		/*
; 		send_byte(0xff);
; 
; 		while (!get_byte(&recv));
; 		
; 		if (recv != 0xFA)
; 			continue;
; 		*/
; 			
; 		// send another byte
; 		send_byte(0xf4);
	ldd #244
	jsr _send_byte
L20:
; 		
; 		while (!get_byte(&recv));
L21:
	leay 14,S
	tfr Y,D
	jsr _get_byte
	tfr D,Y
	cpy #0
	beq L20
; 		
; 		if (recv != 0xFA)
	ldab 14,S
	cmpb #250
	beq L23
; 			continue;
	bra L18
L23:
; 		
; 		success = 1;
	movb #1,13,S
; 		break;
	bra L19
L18:
	ldab 12,S
	clra
	tfr D,Y
	std 10,S
	tfr D,Y
	iny
	tfr Y,B
	stab 12,S
	ldy 10,S
	cpy #3
	blt L17
L19:
; 	}
; 	
; 	putchar('x');
	ldd #120
	jsr _putchar$device_specific$
; 	
; 	if (!success)
	ldab 13,S
	cmpb #0
	bne L25
; 	{
; 		printf("Failed: %d %d\n", recv, bytes_received);
	ldy _bytes_received
	sty 4,S
	ldab 14,S
	clra
	tfr D,Y
	std 2,S
	ldy #L27
	sty 0,S
	jsr _printf
; 		asm("swi");
	swi
; 	}
L25:
; 	
; 	printf("Ready\n");
	ldy #L28
	sty 0,S
	jsr _printf
	bra L30
L29:
; 	
; 	while (1)
; 	{
; 		if (error)
	ldab _error
	cmpb #0
	beq L32
; 		{
; 			printf("Error: %d %d %d %d\r\n", idx, tmp, buf_pos, buf_cnt);
	ldy _buf_cnt
	sty 8,S
	ldy _buf_pos
	sty 6,S
	ldab _tmp
	clra
	tfr D,Y
	sty 4,S
	ldab _idx
	clra
	tfr D,Y
	std 2,S
	ldy #L34
	sty 0,S
	jsr _printf
; 			asm("swi");
	swi
; 		}
L32:
; 	
; 		if (get_byte(&recv))
	leay 14,S
	tfr Y,D
	jsr _get_byte
	tfr D,Y
	cpy #0
	beq L35
; 		{	
; 			printf("Got something: %d\r\n", recv);
	ldab 14,S
	clra
	tfr D,Y
	std 2,S
	ldy #L37
	sty 0,S
	jsr _printf
; 		}
L35:
; 	}
L30:
	bra L29
X0:
L16:
	.dbline 0 ; func end
	leas 15,S
	rts
;              x -> 5,SP
_odd_parity:
	pshd
	leas -4,S
; 
; 	asm("swi");
; 	
; }
; 
; static
; char odd_parity(unsigned char x)
; {
; 	x ^= x >>  4;
	ldab 5,S
	clra
	asra
	rorb
	asra
	rorb
	asra
	rorb
	asra
	rorb
	tfr D,Y
	ldab 5,S
	clra
	sty 0,S
	eora 0,S
	eorb 1,S
	stab 5,S
; 	x &= 0x0F;
	bclr 5,S,#240
; 	return ((0x6996 >> x) & 1) == 0;
	ldab 5,S
	clra
	tfr D,Y
	ldd #27030
	tfr Y,X
	jsr asr16
	anda #0
	andb #1
	tfr D,Y
	cpd #0
	bne L40
	movw #1,2,S
	bra L41
L40:
	movw #0,2,S
L41:
	ldd 2,S
	clra
L38:
	.dbline 0 ; func end
	leas 6,S
	rts
_falling_edge::
	leas -2,S
; }
; 
; #pragma interrupt_handler falling_edge
; void falling_edge()
; {
; 	if (idx == 0)
	ldab _idx
	cmpb #0
	bne L43
; 	{
; 		// bit should be 0
; 		if (receive_mode)
	ldab _receive_mode
	cmpb #0
	beq L45
; 		{
; 			if (PTH & 0x01)
	brclr 0x260,#1,L47
; 				error = 1;
	movb #1,_error
L47:
; 			DDRH |= 0x04;
	bset 0x262,#4
; 		}
	lbra L44
L45:
; 		else
; 			PTH &= 0xFE;
	bclr 0x260,#1
; 	}
	lbra L44
L43:
; 	else if (idx <= 8)
	ldab _idx
	cmpb #8
	lbhi L49
; 	{
; 		if (receive_mode)
	ldab _receive_mode
	cmpb #0
	beq L51
; 		{
; 			if (PTH & 0x01)
	brclr 0x260,#1,L53
; 			{
; 				tmp |= (1 << (idx-1));
	ldab _idx
	clra
	tfr D,Y
	ldx #1
	dey
	tfr X,D
	tfr Y,X
	jsr lsl16
	tfr D,Y
	ldab _tmp
	clra
	sty 0,S
	ora 0,S
	orb 1,S
	stab _tmp
; 				PTH |= 0x04;
	bset 0x260,#4
; 			}
	lbra L50
L53:
; 			else
; 			{
; 				tmp &= (~(1 << (idx-1)));
	ldab _idx
	clra
	tfr D,Y
	ldx #1
	dey
	tfr X,D
	tfr Y,X
	jsr lsl16
	coma
	comb
	tfr D,Y
	ldab _tmp
	clra
	sty 0,S
	anda 0,S
	andb 1,S
	stab _tmp
; 				PTH &= 0xFB;
	bclr 0x260,#4
; 			}
; 		}
	lbra L50
L51:
; 		else
; 		{	
; 			if ((byte_to_send >> (idx-1)) & 0x1)
	ldab _idx
	clra
	tfr D,Y
	dey
	tfr Y,X
	ldab _byte_to_send
	clra
	jsr asr16
	anda #0
	andb #1
	tfr D,Y
	cpd #0
	beq L55
; 				PTH |= 0x01;
	bset 0x260,#1
	lbra L50
L55:
; 			else
; 				PTH &= 0xFE;
	bclr 0x260,#1
; 		}
; 	}
	lbra L50
L49:
; 	else if (idx == 9)
	ldab _idx
	cmpb #9
	bne L57
; 	{
; 		if (receive_mode)
	ldab _receive_mode
	cmpb #0
	lbeq L58
; 		{
; 			// http://bits.stephan-brumme.com/parity.html
; 			if (odd_parity(byte_to_send))
	ldab _byte_to_send
	clra
	jsr _odd_parity
	clra
	cmpb #0
	beq L61
; 				PTH |= 0x01;
	bset 0x260,#1
	bra L58
L61:
; 			else
; 				PTH &= 0xFE;
	bclr 0x260,#1
; 		}
; 	}
	bra L58
L57:
; 	else if (idx == 10)
	ldab _idx
	cmpb #10
	bne L63
; 	{
; 		if (receive_mode)
	ldab _receive_mode
	cmpb #0
	beq L65
; 		{
; 			if (!(PTH & 0x01))
	ldab 0x260
	bitb #1
	bne L67
; 				error = 1;
	movb #1,_error
	bra L68
L67:
; 			else
; 				put_byte(tmp);
	ldab _tmp
	clra
	jsr _put_byte
L68:
; 				
; 			idx = 0-1;
	ldd #65535
	stab _idx
; 		}
	bra L64
L65:
; 		else
; 		{
; 			// always re-enable input, for the final bit
; 			DDRH = 0x00;
	clr 0x262
; 		}
; 	}
	bra L64
L63:
; 	else if (idx == 11)
	ldab _idx
	cmpb #11
	bne L69
; 	{
; 		receive_mode = 1;
	movb #1,_receive_mode
; 		send_complete = 1;
	movb #1,_send_complete
; 		idx = 0-1;
	ldd #65535
	stab _idx
; 	}
	bra L70
L69:
; 	else
; 		error = 1;
	movb #1,_error
L70:
L64:
L58:
L50:
L44:
; 	
; 
; 	idx += 1;
	inc _idx
; 	
; 	// clear the associated interrupt
; 	PIFH |= 0x02;
	bset 0x267,#2
L42:
	.dbline 0 ; func end
	leas 2,S
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
L71:
	.dbline 0 ; func end
	leas 2,S
	rts
_delay100us::
; }
; 
; /*
; 	delay100us
; 	
; 	Delays a program by approximately 100us
; 	
; */
; void delay100us()
; {
; asm("	ldx #$25		");		// 2 cycles (Y variable)
		ldx #$25		
; asm("l_out:				");	
	l_out:				
; asm("	ldy #$15		");		// 2 cycles (X variable)
		ldy #$15		
; asm("l_in:				");
	l_in:				
; asm("	dbne y, l_in	");		// 3 (not taken) /1 (taken) cycles
		dbne y, l_in	
; asm("	dbne x, l_out	");		// 3 (not taken) /1 (taken) cycles
		dbne x, l_out	
L72:
	.dbline 0 ; func end
	rts
	.area bss
_received::
	.blkb 16
	.area text
L37:
	.byte 'G,'o,'t,32,'s,'o,'m,'e,'t,'h,'i,'n,'g,58,32,37
	.byte 'd,13,10,0
L34:
	.byte 'E,'r,'r,'o,'r,58,32,37,'d,32,37,'d,32,37,'d,32
	.byte 37,'d,13,10,0
L28:
	.byte 'R,'e,'a,'d,'y,10,0
L27:
	.byte 'F,'a,'i,'l,'e,'d,58,32,37,'d,32,37,'d,10,0
