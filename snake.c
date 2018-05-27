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
void
render_state(struct game_state_t *state) {
    // clear window content
    werase(state->win);

    // render box around field
    box(state->win, 0, 0);

    if (state->running) {
        // snake
        struct snake_body_part_t *current = state->snake;
        mvwaddch(state->win, current->position.y + 1, current->position.x + 1, '0' | COLOR_PAIR(1));
        current = current->next;
        while (current) {
            mvwaddch(state->win, current->position.y + 1, current->position.x + 1, 'O' | COLOR_PAIR(1));
            current = current->next;
        }

        // food
        mvwaddch(state->win, state->food->position.y + 1, state->food->position.x + 1, 'X' | COLOR_PAIR(2));
    } else {
        int rows, cols;
        getmaxyx(state->win, rows, cols);
        if (state->game_won) {
            char msg[] = "You won.";
            mvwprintw(state->win, rows / 2, (cols - strlen(msg)) / 2, msg);
        } else {
            char msg[] = "You lost.";
            mvwprintw(state->win, rows / 2, (cols - strlen(msg)) / 2, msg);
        }
    }
    // show window content
    wrefresh(state->win);

    // wait for enter after the game was finished
    if (!state->running) {
        nodelay(state->win, 0);
        nocbreak();
        wgetch(state->win);
    }
}

struct game_state_t *
init_game_state() {
    // initialise ncurses
    initscr();
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    int fieldHeight = 16;
    int fieldWidth = 32;
    int winHeight = fieldHeight + 2;
    int winWidth = fieldWidth + 2;

    struct game_state_t *state = malloc(sizeof(*state));
    state->fieldWidth = fieldWidth;
    state->fieldHeight = fieldHeight;
    state->snake = init_snake(fieldWidth, fieldHeight);
    state->food = init_food();
    state->direction = LEFT;
    state->blocked_direction = RIGHT;
    state->running = 1;
    state->game_won = 0;
    state->win = newwin(winHeight, winWidth, (rows - winHeight) / 2, (cols - winWidth) / 2);
    spawn_food(state);

    // set ncurses options for interactive usage
    cbreak();
    noecho();
    nodelay(state->win, 1);
    keypad(state->win, 1);
    intrflush(state->win, 1);

    nonl();

    if (has_colors()) {
        start_color();
    }
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // snake
    init_pair(2, COLOR_RED, COLOR_BLACK); // food

    curs_set(0);


    return state;
}
