/*
type_structures.h

written by Maria Halvarsson
*/

/* ball */
typedef struct Ball
{
    int x, y;                  // ball x and y value
    int w, h;                  // width and height of ball
    int dx, dy;                // ball movement vector
} Ball;

/* paddle */
typedef struct Paddle
{
    int x, y;                  // paddle x, y value
    int dy;                    // paddle vertical movement
    int score;                 // player score
} Paddle;
