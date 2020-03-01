/*
 header file
 written by Maria Halvarsson
*/

#include "type_structures.h"

/* display related functions */
void display_init (void);
void display_string (int line, char *s);
void display_image (int x, const uint8_t *data);
void display_update (void);
void display_winner (int n);
void display_main_menu (void);
void clear_screen (void);
void clear_string (void);
uint8_t spi_send_recv (uint8_t data);
uint8_t screen[128 * 4];
void draw_ball (Ball ball);
void draw_paddle (Paddle pad);
void draw_scoreboard (Paddle pad1, Paddle pad2);
void draw_string (int x);
void draw_cursor (int n);

/* delay function */
void quicksleep (int cyc);

/* game related functions */
void game_init (void);
void move_ball (void);
void move_paddle_ai (void);
void move_paddle1_up (void);
void move_paddle2_up (void);
void move_paddle1_down (void);
void move_paddle2_down (void);
void update_paddles (void);
void update_paddle1 (void);
void ball_reset (void);
void draw_display (void);

/* function checks if buttons are pushed */
int getbtns (void);

/* function that updates the cursor */
void update_cursor(void);


/* declare bitmap array containing font */
extern const uint8_t const font[];

/* declare text buffer for display output */
extern char textbuffer [4][16];

/* declare bitmap array containing pong icon */
extern const uint8_t const pong_icon[4*128];

/* declare bitmap array cursor */
extern const uint8_t const cursor[8];

extern int game_mode;
extern int winner;
extern int cursor_point;

/* innitialize interrupts */
void int_init (void);
