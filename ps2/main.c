/*
	Dustin Spicuzza

	PS/2 interface test
*/

#include <mc9s12dp256.h>
#include "dbug12.h"
#include <stdio.h>

#pragma abs_address: 0x3e4c
	void (*pth_handler_addr)(void) = 0;
#pragma end_abs_address


void falling_edge(void);
void delay100ms(void);


int got_something = 0;

// whatever was received
unsigned char received_byte = 0;

unsigned char tmp = 0;
unsigned char idx = 0;
unsigned char error = 0;

int bytes_received = 0;

#define enable_clk_interrupt() PIEH = 0x02; PIFH = 0x00
#define disable_clk_interrupt() PIEH = 0x00; PIFH = 0x00


void main()
{

	// data: 	PTH:0
	// clock: 	PTH:1

	disable_clk_interrupt();
	
	pth_handler_addr = &falling_edge;

	DDRH = 0x00;	// everything is an input
	PPSH = 0x02;	// rising edge, pull down for input
	PERH = 0x01;	// enable pulling for this device
	PIFH &= 0xFD;
	
	// bring clock low for at least 100us
	DDRH = 0x02;
	PTH &= 0xFD;
	
	delay100ms();
	
	enable_clk_interrupt();

	
	// re-enable input, see what happens
	DDRH = 0x00;

	
	while (1)
	{
		if (error)
		{
			printf("Error: %d %d %d\r\n", idx, tmp, received_byte);
			asm("swi");
		}
	
		if (got_something)
		{		
			printf("Got something: %d %d\r\n", received_byte, bytes_received);
			got_something = 0;
		}
	}

	asm("swi");
	
}

#pragma interrupt_handler falling_edge
void falling_edge()
{
	switch (idx)
	{
		// start bit
		case 0:
			// bit should be 0
			if (PTH & 0x01)
				error = 1;
			break;
			
		// data bits
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			tmp |= ((PTH & 0x01) << (idx-1));
			break;
		
		// parity
		case 10:
			break;
		// stop bit
		case 11:
			if (!(PTH & 0x01))
			{
				error = 1;
				break;
			}
			
			received_byte = tmp;
			idx = 0;
			bytes_received += 1;
			got_something += 1;
			
			break;
			
		default:
			error = 1;
	}
	
	if (!error)
	{
		idx += 1;
	
		// clear the associated interrupt
		PIFH |= 0x02;
	}
}

// needed for printf
int putchar(char c)
{
	return MyUserFNP->DB12putchar(c);
}

/*
	delay100ms
	
	Delays a program by approximately 100ms

	cycles = 100ms/125ns = 800000
	3xy + 3y = cycles
	3xy + 3y --> x=964, y = 276
	
*/
void delay100ms()
{
asm("	ldx #$114		");		// 2 cycles (Y variable)
asm("l_out:				");	
asm("	ldy #$3c4		");		// 2 cycles (X variable)
asm("l_in:				");
asm("	dbne y, l_in	");		// 3 (not taken) /1 (taken) cycles
asm("	dbne x, l_out	");		// 3 (not taken) /1 (taken) cycles
}