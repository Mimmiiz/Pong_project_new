/*
display.c
Written by Maria Halvarsson & Jinglan Qin
Note: Parts of the program are written by F Lundevall and Axel Isaksson
 */

#include <stdint.h>
#include <pic32mx.h>
#include "headerfile.h"

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int );

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)


#define SCREEN_LENGTH (128)
#define PADDLE_HEIGHT (8)
#define PADDLE_WIDTH (2)

#define BALL_HEIGHT (2)
#define BALL_WIDTH (2)

uint8_t screen[128 * 4] = {0};
uint8_t screen_2[128 * 36] = {0};

/* function that creates a small delay */
void quicksleep(int cyc)
{
    int i;
    for(i = cyc; i > 0; i--);
}

/* written by F Lundevall & Axel Isaksson */
void display_debug( volatile int * const addr )
{
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}

/* written by F Lundevall & Axel Isaksson */
uint8_t spi_send_recv(uint8_t data)
{
	while(!(SPI2STAT & 0x08));   // if SPIxTXB (transmit buffer) is not empty, SPI2BUF = data
	SPI2BUF = data;              // send data to slave
	while(!(SPI2STAT & 1));      // if SPIxRXB (receive buffer) is not full, return SPI2BUF.
	return SPI2BUF;              // return received data.  *When first bit(SPIRBF) in SPI2STAT is 0, 
	                             // receive buffer is not full, return data in recieved buffer.
}

/* written by F Lundevall & Axel Isaksson */
void display_init(void)
{
    
	DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
  	quicksleep(1000000);

	spi_send_recv(0xAE); 	// display off 
  	DISPLAY_ACTIVATE_RESET;	//bring reset low
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;	//bring reset high
	quicksleep(10);
         
	//Send the Set Charge Pump and Set Pre-Charge Period commands
	spi_send_recv(0x8D); 	// charge pump setting
	spi_send_recv(0x14);	// enable charge pump

	spi_send_recv(0xD9); 	// set pre charge period
	spi_send_recv(0xF1);

	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
        /* Send the commands to invert the display. This puts the display origin ** in the upper left corner.*/
	spi_send_recv(0xA1); 	// set segment re-map
	spi_send_recv(0xC8); 	// set COM output scan direction
        
	/* Send the commands to select sequential COM configuration. This makes the ** display memory non-interleaved.*/
	spi_send_recv(0xDA); 	// set COM pins hardware configuration
	spi_send_recv(0x20); 	// set memory adressing mode

	spi_send_recv(0xAF); 	// display on
}

/* this function draws one pixel on the screen, 
this function was based on Simone Stefani and Marcel Eschmann function from chipkit-pong project on github */
void draw_pixel (int x, int y)
{
    int page = 0;
    if (y > 0)
	page = y / 8;
    screen[page * SCREEN_LENGTH + x] |= 1 << (y - page * 8);
}

/* this function draws the ball on the screen 
Written by Maria Halvarsson */
void draw_ball (Ball b)
{ int i, j;
	for ( i = 0; i < BALL_WIDTH; i++)
	{
	    for ( j = 0; j < BALL_HEIGHT; j++)
	    {
		draw_pixel(b.x + i, b.y + j);
	    }
	}
}

/* written by Jinglan Qin */
void draw_paddle(Paddle pad)
 {
     int i,j;
     for (i = 0; i < PADDLE_HEIGHT; i++)
     {
 	    for(j=0;j < PADDLE_WIDTH ;j++)
            	draw_pixel(pad.x+j, pad.y + i);
     }
 }

/* written by Maria Halvarsson */
void draw_scoreboard (Paddle pad1, Paddle pad2)
{
    int x = 58;
    int k;
    char c = last_digit(pad1.score) + '0';
    /* draws score for pad1 */
    for (k = 0; k < 8; k++)
      screen[x + k] = font[c * 8 + k];

    if (pad1.score > 9)
    {
      x = 51;
      c = first_digit(pad1.score) + '0';

      for (k = 0; k < 8; k++)
        screen[x + k] = font[c * 8 + k];
    }

    x = 64;
    c = '|';
    for (k = 0; k < 8; k++)
      screen[x + k] = font[c * 8 + k];

    /* draws score for pad2 */
    if (pad2.score < 10)
    {
      x = 70;
      c = last_digit (pad2.score) + '0';

      for (k = 0; k < 8; k++)
        screen[x + k] = font [c * 8 + k];
    }
    else
    {
      x = 77;
      c = last_digit (pad2.score) + '0';

      for (k = 0; k < 8; k++)
        screen[x + k] = font [c * 8 + k];

      x = 70;
      c = first_digit(pad2.score) + '0';

      for (k = 0; k < 8; k++)
        screen[x + k] = font[c * 8 + k];
    }
}

/* written by Maria Halvarsson */ 
void draw_string (int x)
{
  int i, j, k;
  char c;

  for(i = 0; i < 4; i++)
  {
    for (j = 0; j < 16; j++)
    {
      c = textbuffer[i][j];
      if (c & 0x80)
        continue;

      for (k = 0; k < 8; k++)
      {
        screen[x + k] = font[c * 8 + k];
      }
      x += 8;
    }
  }
}

/* written by Maria Halvarsson */ 
void draw_cursor(int n)
{
  int x = 10;
  int i;

  for (i = 0; i < 8; i++)
    screen[x +  n * 128 + i] = cursor[i];
}

/* written by F Lundevall & Axel Isaksson */
void display_string(int line, char *s)
{
    int i;
    if(line < 0 || line >= 4)
        return;
    if(!s)
        return;

    for(i = 0; i < 16; i++)
        if(*s) {
            textbuffer[line][i] = *s;
            s++;
        } else
            textbuffer[line][i] = ' ';
}

/* written by Maria Halvarsson */
void display_main_menu ()
{
  clear_string();
  display_string (1, "SINGLE-PLAYER");
  display_string (2, "MULTIPLAYER");
  draw_string (20);
}

/* written by Maria Halvarsson */
void display_winner (int n)
{
  clear_string();
  display_string (0, "WINNER:");
  display_string (2, "Press BTN1");
  display_string (3, "to continue");

  if (n == 1)
    display_string (1, "PLAYER 1!");

  if (n == 2)
    display_string (1, "PLAYER 2!");

  draw_string (20);
}

/* written by F Lundevall & Axel Isaksson, partly modified */
void display_image(int x, const uint8_t *data)
{
	int i, j;

	for(i = 0; i < 4; i++)
   {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 128; j++)
			spi_send_recv(~data[i*128 + j]);
	}
}

/* written by F Lundevall & Axel Isaksson*/
void display_update(void)
{
    int i, j, k;
    int c;
    for(i = 0; i < 4; i++)
    {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        spi_send_recv(0x22);// set page adress
        spi_send_recv(i);

        spi_send_recv(0x0);// set lower column start address for page addressing mode
        spi_send_recv(0x10); // set higher column start adress for page addressing mode

        DISPLAY_CHANGE_TO_DATA_MODE;

        for(j = 0; j < 16; j++)
        {
            c = textbuffer[i][j];
            if(c & 0x80)
                continue;

            for(k = 0; k < 8; k++)
                spi_send_recv(font[c* 8 + k]);
        }
    }
}

/* function clears the screen, written by Maria Halvarsson */
void clear_screen ()
{
    int i;
    for (i = 0; i < 512; i++)
    {
        screen[i] = 0;
    }
}

/* clears textbuffer, written by Maria Halvarsson */
void clear_string ()
{
  int i, j;
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 24; j++)
      textbuffer[i][j] = '\0';
  }
}

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n )
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}

/* returns first number of an integer (local to this file) */
int first_digit (int n)
{
  while (n > 9)
    n /= 10;
  return n;
}

/* returns last number of an integer (local to this file) */
int last_digit (int n)
{
  return (n % 10);
}
