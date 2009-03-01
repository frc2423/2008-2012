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


unsigned char sequence_end = 0;
unsigned char receive_mode = 0;

// whatever was received
unsigned char received_byte = 0;
unsigned char send_byte = 0;

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
	
	// send a byte
	enable_clk_interrupt();
	asm("cli");
	DDRH = 0x01;
	PTH &= 0xFE;
	
	receive_mode = 0;
	send_byte = 0xff;
	
	// todo: some timeout
	while (!sequence_end);
	sequence_end = 0;
	
	putchar('.');
	
	// re-enable input, see what happens
	receive_mode = 1;
	DDRH = 0x00;

	while (!sequence_end);
	
	receive_mode = 0;

	
	putchar('.');
	
	//printf("Doing something\r\n");
	
	while (1)
	{
		if (error)
		{
			printf("Error: %d %d %d\r\n", idx, tmp, received_byte);
			asm("swi");
		}
	
		if (sequence_end)
		{		
			printf("Got something: %d %d\r\n", received_byte, bytes_received);
			sequence_end = 0;
		}
	}

	asm("swi");
	
}


char odd_parity(unsigned char x)
{
	x ^= x >>  4;
	x &= 0x0F;
	return ((0x6996 >> x) & 1) == 0;
}

#pragma interrupt_handler falling_edge
void falling_edge()
{
	switch (idx)
	{
		// start bit
		case 0:
			// bit should be 0
			if (receive_mode)
			{
				if (PTH & 0x01)
					error = 1;
			}
			else
				PTH &= 0xFE;
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
			if (receive_mode)
				tmp |= ((PTH & 0x01) << (idx-1));
			else
			{	
				if ((send_byte >> (idx-1)) & 0x1)
					PTH |= 0x01;
	
				else
					PTH &= 0xFE;
			}
			break;
		
		// parity
		case 9:
			if (!receive_mode)
			{
				// http://bits.stephan-brumme.com/parity.html
				if (odd_parity(send_byte))
					PTH |= 0x01;
				else
					PTH &= 0xFE;
			}
			break;
		// stop bit
		case 10:
			if (receive_mode)
			{
				if (!(PTH & 0x01))
				{
					error = 1;
					break;
				}
				
				received_byte = tmp;
				bytes_received += 1;
			}
			
			idx = 0-1;
			sequence_end = 1;
			
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