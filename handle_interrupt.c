/*
handle_interrupt.c
written by Jinglan Qin and Maria Halvarsson
*/

#include <stdint.h>
#include <pic32mx.h>
#include "headerfile.h"
int timecounter=0;

/* interrupt service routine */
void user_isr (void)
{
  // if timer interrupt
  if (IFS(0) & 1 << 8)
  {
    timecounter++;
    /* display main menu */
    if (game_mode == 0)
    {
      game_init();
      clear_screen();
      display_main_menu();
      update_cursor();
      display_image(0, screen);

      if ((getbtns() & 0x2) && (cursor_point == 1))
        game_mode = 1;
      if ((getbtns() & 0x2) && (cursor_point == 2))
        game_mode = 2;
    }
    /* single-player mode */
    if (game_mode == 1 && winner == 0)
    {
      update_paddle1();
      move_paddle_ai();
      move_ball ();
      clear_screen();
      draw_display ();

      display_image (0, screen);
    }
    /* multiplater mode */
    if (game_mode == 2 && winner == 0)
    {
      update_paddles();
      move_ball ();
      clear_screen();
      draw_display();

      display_image (0, screen);
    }

    if (winner > 0)
    {
      clear_screen();
      display_winner(winner);
      display_image (0, screen);
      if (getbtns() & 0x1)
      {
        winner = 0;
        game_mode = 0;
      }
    }
     IFSCLR(0) = (1 << 8);              // resets flag to 0 for timer 2
  }

  if(PORTE & 0xff)//turn off leds
  {
    if(timecounter %20 == 0)
      PORTE = 0x00;
  }
}

/* interrupt initialization */
void int_init (void)
{
  T2CON = 0x0;                       // reset timer
  T2CONSET = 0x70;                   // prescale to  1:256
  TMR2 = 0;                          // set timer 2 to 0
  PR2 = 10400;                       // set period to (80 000 000 / 256)/10 = 31250
  IECSET(0) = (1 << 8);              // set the bit for the interrupt that should be enabled
  IPCSET(2) = (1 << 2);              // configure priority for timer 2 to priority 4
  enable_interrupt();                // enable interrupt
  T2CONSET = 0x8000;                 // turns on timer 2
  return;
}
