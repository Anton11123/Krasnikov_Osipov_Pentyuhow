void
render_state(struct game_state_t *state) {
    // очищение содержимого в окне
    werase(state->win);

    // вокруг поля
    box(state->win, 0, 0);

    if (state->running) {
        // змея
        struct snake_body_part_t *current = state->snake;
        mvwaddch(state->win, current->position.y + 1, current->position.x + 1, '0' | COLOR_PAIR(1));
        current = current->next;
        while (current) {
            mvwaddch(state->win, current->position.y + 1, current->position.x + 1, 'O' | COLOR_PAIR(1));
            current = current->next;
        }

        // то, что она кушает (еда)
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
    // показ содержимого в окне
    wrefresh(state->win);

    // нажми enter, чтобы выйти из игры
    if (!state->running) {
        nodelay(state->win, 0);
        nocbreak();
        wgetch(state->win);
    }
}

struct game_state_t *
init_game_state() {
    // инициализация библиотеки ncurses
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

    // настройка параметров библиотеки для интерактивного использования
    cbreak();
    noecho();
    nodelay(state->win, 1);
    keypad(state->win, 1);
    intrflush(state->win, 1);

    nonl();

    if (has_colors()) {
        start_color();
    }
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // цвет змеи
    init_pair(2, COLOR_RED, COLOR_BLACK); // цвет еды

    curs_set(0);


    return state;
}


