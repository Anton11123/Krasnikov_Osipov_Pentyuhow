#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

struct position_t { //
    int x;
    int y;
};

struct snake_body_part_t {
    struct position_t position;
    struct snake_body_part_t *next;
};

enum direction_t {
    LEFT,
    UP,
    RIGHT,
    DOWN
};

struct food_t {
    struct position_t position;
    int value;
};

struct game_state_t {
    int fieldWidth;
    int fieldHeight;
    struct snake_body_part_t *snake;
    struct food_t *food;
    enum direction_t direction;
    enum direction_t blocked_direction;
    int running;
    int game_won;
    WINDOW *win;
};


