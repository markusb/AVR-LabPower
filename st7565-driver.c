/*
*
* Graphic display driver for LCD with ST7565 controller
*
* Originally by Sebastian, see http://www.mikrocontroller.net/topic/75589
*
* Reorganized and modified by Markus Baertschi
*/

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "st7565-driver.h"

// RAM data (top to bottom; left to right)
// Pixel(x,y):          index = y/8 + x*8
//                            = (y >> 3) + (x << 3)
//                      bit   = y%8
//                            = (y & 0x07)
// Set pixel (x,y):     disp_ram[(y >> 3) + (x << 3)] |=  (1 << (y & 0x07));
// Clear pixel (x,y):   disp_ram[(y >> 3) + (x << 3)] &= ~(1 << (y & 0x07));
uint8_t disp_ram[DISP_RAM_SIZE];



// (Pre-declare functions)
void disp_clear();



// hardware functions (only used by "disp_..."-functions)

void dogm_send(unsigned char spi_data, unsigned char a0)
{
//	printf("dogm_send: %d:%02x\n",a0,spi_data);
	//a0 = 0: Command
	//a0 = 1: Display data

	//set / clear A0-Bit
	if (a0) {
//		A0_PORT |= (1<<A0_BIT);
		lcd_senddata(spi_data);
	} else {
//		A0_PORT &= ~(1<<A0_BIT);
		lcd_sendcmd(spi_data);
	}
	//Byte senden
//	spi_master_send_char(spi_data);
}
#define dogm_send_command(command)      dogm_send(command, 0)
#define dogm_send_display_data(data)    dogm_send(data, 1)


void dogm_reset()
{
	//Reset display (50ms)
	RES_PORT &= ~(1<<RES_BIT); // RES\ = LOW (Reset)
	_delay_ms(50);
	RES_PORT |= (1<<RES_BIT); // RES\ = HIGH (kein Reset)
}


// basic implementations (used by graphics functions)

void disp_init()
{
	//uart_comment("[DOGM128] Initializing...");

	//SET DATA DIRECTION REGISTER
	//Outputs: (DDR=1)
	A0_DDR |= (1<<A0_BIT); //Output: A0
	RES_DDR |= (1<<RES_BIT); //Output: RES
	//Inputs: (DDR=0)
	//(none)


	//RESET
	dogm_reset(); //(100ms)


	//Display start line
	dogm_send_command(0x40); // Display start line 0

	//Bottom view
	dogm_send_command(0xA1); // ADC reverse
	dogm_send_command(0xC0); // Normal COM0~COM63

	//Normal / Inverted
	dogm_send_command(0xA6); // Display normal

	//Hardware options
	dogm_send_command(0xA2); // Set bias 1/9 (Duty 1/65)
	dogm_send_command(0x2F); // Booster, Regulator and Follower on
	dogm_send_command(0xF8); // Set internal Booster to 4x
	dogm_send_command(0x00);

	//Contrast options
	dogm_send_command(0x27); // Contrast set
	dogm_send_command(0x81);
	dogm_send_command(0x16);

	//Indicator options
	dogm_send_command(0xAC); // No indicator
	dogm_send_command(0x00);

	//(Init done)
	dogm_send_command(0xAF); // Display on

	//uart_comment("[DOGM128] Ready.");


	disp_clear();
}

void disp_send_frame()
{
	uint8_t page;
	for (page = 0; page < 9; page++)
	{
		dogm_send_command(0xB0 + page); //Set page address to <page>
		dogm_send_command(0x10 + 0); //Set column address to 0 (4 MSBs)
		dogm_send_command(0x00 + 4); //Set column address to 0 (4 LSBs)

		uint8_t column;
		for (column = 0; column < 128; column++)
			dogm_send_display_data(disp_ram[page + (column << 3)]);
	}
}

void disp_clear()
{
	//clears the local RAM and send this cleared frame
	uint16_t d;
	for (d = 0; d < 1024; d++)
		disp_ram[d] = 0x00;
	disp_send_frame();
}

void disp_clear_dont_refresh()
{
	//clears the local RAM but don't send it
	uint16_t d;
	for (d = 0; d < 1024; d++)
		disp_ram[d] = 0x00;
}


/*
void disp_frame_begin()
{
	//not relevant for this display
}
*/
//ignore "disp_frame_begin()" in source code
#define disp_frame_begin()


void disp_frame_end()
{
	//send the display data
	disp_send_frame();
}

void disp_set_pixel(uint8_t x, uint8_t y, uint8_t pixel_status)
{
	if (x < DISP_WIDTH && y < DISP_HEIGHT) {
		if (pixel_status != 0)
			disp_ram[(y >> 3) + (x << 3)] |=  (1 << (y & 0x07));
		else
			disp_ram[(y >> 3) + (x << 3)] &= ~(1 << (y & 0x07));
        }
}
