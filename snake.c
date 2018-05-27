#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

struct position_t {
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
void
read_inputs(struct game_state_t *state);

void
update_state(struct game_state_t *state);

void
render_state(struct game_state_t *state);

struct game_state_t *
init_game_state();

void
deinit_game_state(struct game_state_t *state);

struct snake_body_part_t *
init_snake(int fieldWidth, int fieldHeight);

void
move_snake(struct game_state_t *state);

void
deinit_snake(struct snake_body_part_t *snake);

struct food_t *
init_food();

void
spawn_food(struct game_state_t *state);

void
deinit_food(struct food_t *food);

int
positions_equal(struct position_t *p1, struct position_t *p2);

void
read_inputs(struct game_state_t *state) {
    int key = wgetch(state->win);
    switch (key)  {
        case KEY_DOWN : if (state->blocked_direction != DOWN) state->direction = DOWN; break;
        case KEY_UP : if (state->blocked_direction != UP) state->direction = UP; break;
        case KEY_LEFT : if (state->blocked_direction != LEFT) state->direction = LEFT; break;
        case KEY_RIGHT : if (state->blocked_direction != RIGHT) state->direction = RIGHT; break;
    }
}
