/*
 main
*/
#include <stdint.h>
#include <pic32mx.h>
#include "headerfile.h"

int main(void)
{
    SYSKEY = 0xAA996655;        /* Unlock OSCCON, step 1 */
    SYSKEY = 0x556699AA;        /* Unlock OSCCON, step 2 */
    while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
    OSCCONCLR = 0x180000;       /* clear PBDIV bit <0,1> */
    while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
    SYSKEY = 0x0;               /* Lock OSCCON */

    /* Set up output pins */
    AD1PCFG = 0xFFFF;
    ODCE = 0x0;
    TRISECLR = 0xFF;
    PORTE = 0x0;

    /* Output pins for display signals */
    PORTF = 0xFFFF;
    PORTG = (1 << 9);
    ODCF = 0x0;
    ODCG = 0x0;
    TRISFCLR = 0x70;
    TRISGCLR = 0x200;

    /* Set up input pins */
    TRISFSET = 0x2;         /* BTN1 */
    TRISDSET = 0xe0;        /* BTN2-4 */

    /* Set up SPI2 master mode */
    SPI2CON = 0;            /* resets the SPI2 */
    SPI2BRG = 4;            /* spi baud rate register */
    SPI2STATCLR = 0x40;     /* clear SPI2STAT bit SPIROV = 0 */
    SPI2CONSET = 0x40;      /* clock polarity select bit SPI2CONSET bit CKP = 1 */
    SPI2CONSET = 0x20;      /* enable master mode, SPI2CON bit MSTEN = 1 */
    SPI2CONSET = 0x8000;    /* enable SPI Peripheral, SPI2CON bit ON = 1 */

    display_init ();
    display_image(0, pong_icon);
    quicksleep (20000000);
    game_mode = 0;   // declare game mode 
    int_init ();

    while( 1 )
    {
    }
    return 0;
}
