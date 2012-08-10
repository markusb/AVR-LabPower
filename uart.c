/*
 * Lab Power
 *
 * usart
 *
*/

#include <stdio.h>
#include <avr/io.h> 
#include "uart.h"

FILE mystdout = FDEV_SETUP_STREAM (uart_putc, NULL, _FDEV_SETUP_WRITE);

int uart_putc (char c, FILE *stream) {
//    if (c == '\n')
//        uart_putchar('\r', stream);
 
    // Wait for the transmit buffer to be empty
    while ( !( USARTE0.STATUS & USART_DREIF_bm) );
 
    // Put our character into the transmit buffer
    USARTE0.DATA = c; 
 
    return 0;
}
 
 
// Init USART.  Transmit only (we're not receiving anything)
// We use USARTD0, transmit pin on PC3.
// Want 9600 baud. Have a 2 MHz clock. BSCALE = 0
// BSEL = ( 2000000 / (2^0 * 16*9600)) -1 = 12
// Fbaud = 2000000 / (2^0 * 16 * (12+1))  = 9615 bits/sec
// BSEL = ( 32000000 / (2^0 * 16*9600)) -1 = 207
void uart_init (void) {
    // Set the TxD pin high - set PORTC DIR register bit 3 to 1
    PORTE.OUTSET = PIN3_bm;
 
    // Set the TxD pin as an output - set PORTC OUT register bit 3 to 1
    PORTE.DIRSET = PIN3_bm;
 
    // Set baud rate & frame format
    USARTE0.BAUDCTRLB = 0;			// BSCALE = 0 as well
    USARTE0.BAUDCTRLA = UART_BSEL;
 
    // Set mode of operation
    USARTE0.CTRLA = 0;				// no interrupts please
    USARTE0.CTRLC = 0x03;			// async, no parity, 8 bit data, 1 stop bit
 
    // Enable transmitter only
    USARTE0.CTRLB = USART_TXEN_bm;
}
 
