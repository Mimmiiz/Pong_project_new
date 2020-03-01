/*
game.c
Written by Maria Halvarsson & Jinglan Qin
*/

#include <stdint.h>
#include <pic32mx.h>
#include "headerfile.h"

#define SCREEN_WIDTH (128)    // width of the display
#define SCREEN_HEIGHT (32)    // height of the display

#define BALL_WIDTH (3)        // height of ball
#define BALL_HEIGHT (3)      // width of ball

#define PADDLE_WIDTH (3)      // height of paddle
#define PADDLE_HEIGHT (8)     // width of paddle

#define MAX_SCORE (11)       // max score

int game_mode; // game mode 1 = single-player, game mode 2 = multiplayer
int winner;
int cursor_point = 1;
int ai_lose = 0;

Paddle p1, p2;
Ball ball;

void game_init (void)
{
    p1.x = 3;
    p1.y = 12;
    p1.score = 0;

    p2.x = 123;
    p2.y = 12;
    p2.score = 0;

    ball.x = 64;
    ball.y = 16;
    ball.dx = 1;
    ball.dy = 1;

    winner = 0;
}

void move_ball(void)
{
    ball.x = (ball.x + ball.dx);
    ball.y = (ball.y + ball.dy);

    // detect horizontal collision
    if (ball.x <= 3)
    {
        if (ball.y < p1.y || ball.y > p1.y + PADDLE_HEIGHT - 1)
        {
            PORTE = (0xff); //Leds turns on
            p2.score += 1;
            if (p2.score == MAX_SCORE) /* player 2 wins */
            {
              winner = 2;
            }
            else
            {
                ball.dx = -1;
                ball_reset();
            }
        }
        else /* if ball hits paddle */
        {
            ball.x=3;
            ball.dx *= -1;
            return;
        }
    }
    if (ball.x >= 123-2) //The ball is 3*3pixels
    {
        if(ball.y < p2.y || ball.y > p2.y + PADDLE_HEIGHT - 1)
        {
            PORTE = (0xff); //Leds turns on
            p1.score+=1;
            if (p1.score == MAX_SCORE)  /* player1 wins */
            {
              winner = 1;
            }
            else
            {
                ball.dx = 1;
                ball_reset();
            }
        }
        else  /* if ball hits paddle */
        {
            ball.x=123-2;//The ball is 3*3 pixels
            ball.dx *= -1;
            return;
        }

    }
    //detect verical collision
    if (ball.y <= 0)
    {
        ball.y=0;
        ball.dy *= -1;
    }
    if ( ball.y >= SCREEN_HEIGHT)
    {
        ball.y = SCREEN_HEIGHT-1;
        ball.dy *= -1;
    }
}

void move_paddle_ai ()
{
    if (ball.x > 100 && ball.dy < 0 && ball.dx > 0) // om bollen går uppåt
    {
        if (ball.y < (p2.y + PADDLE_HEIGHT/2))
            move_paddle2_up();
        if (ball.y > (p2.y + PADDLE_HEIGHT/2))
            move_paddle2_down();
    }
    if (ball.x > 100 && ball.dy > 0 && ball.dx > 0) // om bollen går nedåt
    {
        if (ball.y < (p2.y + PADDLE_HEIGHT/2))
            move_paddle2_up();
        if (ball.y > (p2.y + PADDLE_HEIGHT/2))
            move_paddle2_down();
    }
    else if (ball.dx < 0) // om ballen flyger åt vänste sidan, reset paddle2
    {
        if(p2.y>12)
        {
            move_paddle2_up();

        }
        if(p2.y<12)
        {
            move_paddle2_down();
        }
    }
}

void move_paddle1_up() /* om BTN4 är nedtryckt*/
{
    if (p1.y <= 0) //om paddel ligger på skärmens toppen
    {
        p1.y = 0;  //ingenting händer
    }
    else
    {
        p1.y -= 1; // annars, paddle flyttar på sig 1 pixel uppåt
    }
}
void move_paddle2_up() /* om BTN2 är nedtryckt */
{
    if (p2.y <= 0) //om paddel ligger på skärmens toppen
    {
        p2.y = 0; //ingenting händer
    }
    else
    {
        p2.y -= 1; // annars, paddle flyttar på sig 1 pixel uppåt.
    }
}

void move_paddle1_down() /* if BTN3 is pushed */
{
    if (p1.y >= SCREEN_HEIGHT - PADDLE_HEIGHT)
    {
        p1.y = SCREEN_HEIGHT - PADDLE_HEIGHT; /* nothing happens */
    }
    else
    {
        p1.y += 1;                            /* move 1 pixel down */
    }
}

void move_paddle2_down() /* if BTN1 is pushed */
{
    if (p2.y >= SCREEN_HEIGHT - PADDLE_HEIGHT)
    {
        p2.y = SCREEN_HEIGHT - PADDLE_HEIGHT; /* nothing happens */
    }
    else
    {
        p2.y += 1;                            /* move 1 pixel down */
    }
}

void ball_reset (void)
{
    ball.x = 64;
    ball.y = 16;
    ball.dy = 1;
}

/* updates the paddles */
void update_paddles ()
{
    if(getbtns() & 0x1)//if BTN1 is pushed,move paddle 2 down. Count from the leftside
    {
     move_paddle2_down();

    }
    if(getbtns() & 0x2) //if BTN2 is pushed, move paddle 2 up
    {
     move_paddle2_up();

    }
    if(getbtns() & 0x4)//if BTN3 is pushed, move paddle 1 down
    {
     move_paddle1_down();
    }
    if(getbtns() & 0x8) //if BTN4 is pushed, move paddle 1 up
    {
      move_paddle1_up();
    }
}

void update_paddle1 ()
{
  if(getbtns() & 0x4)//if BTN3 is pushed, move paddle 1 down
  {
   move_paddle1_down();
  }
  if(getbtns() & 0x8) //if BTN4 is pushed, move paddle 1 up
  {
    move_paddle1_up();
  }

}

void draw_display ()
{
    draw_paddle(p1);
    draw_paddle(p2);
    draw_ball(ball);
    draw_scoreboard(p1, p2);
}

void update_cursor()
{
  if (cursor_point == 1)
  {
    draw_cursor (cursor_point);
    if (getbtns() & 0x4)
    {
      cursor_point = 2;
      draw_cursor (cursor_point);
    }
  }

  if (cursor_point == 2)
  {
    draw_cursor (cursor_point);
    if (getbtns() & 0x8)
    {
      cursor_point = 1;
      draw_cursor (cursor_point);
    }
  }
}

int getbtns (void)
{
  int i = 0x0;
  int j = 0x0;
  i = PORTD & 0xe0;
  j = PORTF & 0x2;
  return (i >> 4 | j >> 1);
}
