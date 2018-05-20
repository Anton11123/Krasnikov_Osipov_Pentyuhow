struct snake_body_part_t *
init_snake(int fieldWidth, int fieldHeight) {
    struct snake_body_part_t *current_part;
    struct snake_body_part_t *previous_part = NULL;
    struct snake_body_part_t *first_part;
    int snake_length = 4;
    assert(snake_length <= fieldWidth);
    for (int i = 0; i < 4; ++i) {
        current_part = malloc(sizeof(*current_part));
        current_part->next = NULL;
        current_part->position.x = (fieldWidth - snake_length) / 2 + i;
        current_part->position.y = fieldHeight / 2;
        if (previous_part != NULL) {
            previous_part->next = current_part;
        } else {
            first_part = current_part;
        }

        previous_part = current_part;
    }

    return first_part;
}

void
move_snake(struct game_state_t *state) {
    // calculate the new position for the head of the snake
    struct position_t new_head_pos;
    new_head_pos = state->snake->position;
    switch (state->direction) {
        case LEFT : new_head_pos.x -= 1; state->blocked_direction = RIGHT; break;
        case UP : new_head_pos.y -= 1; state->blocked_direction = DOWN; break;
        case RIGHT : new_head_pos.x += 1; state->blocked_direction = LEFT; break;
        case DOWN : new_head_pos.y += 1; state->blocked_direction = UP; break;
    }
    if (new_head_pos.x >= state->fieldWidth) new_head_pos.x = 0;
    if (new_head_pos.x < 0) new_head_pos.x = state->fieldWidth - 1;
    if (new_head_pos.y >= state->fieldHeight) new_head_pos.y = 0;
    if (new_head_pos.y < 0) new_head_pos.y = state->fieldHeight - 1;

    // check if the game is finished, if the snake moves
    struct snake_body_part_t *part = state->snake;
    int snake_length = 0;
    while (part) {
        if (positions_equal(&new_head_pos, &(part->position))) {
            state->running = 0;
        }
        part = part->next;
        ++snake_length;
    }

    // decide if THE game was won or lost, if it is finished
    if (!state->running) {
        if (snake_length == state->fieldWidth * state->fieldHeight) {
            state->game_won = 1;
        }

        return;
    }

    // check if the snake consumes the food
    if (positions_equal(&new_head_pos, &(state->food->position))) {
        // move the snakes head to the new (food) position, keep tail 
        struct snake_body_part_t *new_part = malloc(sizeof(*new_part));
        new_part->position = new_head_pos;
        new_part->next = state->snake;
        state->snake = new_part;
        // spawn new food
        spawn_food(state);
    } else {
        
        struct snake_body_part_t *current_part = state->snake;
        struct snake_body_part_t *previous_part = NULL;
        while (current_part->next) {
            previous_part = current_part;
            current_part = current_part->next;
        }
        current_part->position = new_head_pos;
        current_part->next = state->snake;
        if (previous_part) {
            previous_part->next = NULL;
        }
        state->snake = current_part;
    }
}

void
deinit_snake(struct snake_body_part_t *snake) {
    struct snake_body_part_t *current_part = snake;
    struct snake_body_part_t *next_part;
    while (current_part) {
        next_part = current_part->next;
        free(current_part);
        current_part = next_part;
    }
}

struct food_t *
init_food() {
    return malloc(sizeof(struct food_t));
}

void
spawn_food(struct game_state_t *state) {
    // generate a field buffer that indicates if a field is blocked snake body part
    int fields = state->fieldWidth * state->fieldHeight;
    int *field_buffer = calloc(sizeof(*field_buffer), fields);
    struct snake_body_part_t *part = state->snake;
    int snake_length = 0;
    while (part) {
        int field_index = part->position.y * state->fieldWidth + part->position.x;
        field_buffer[field_index] = 1;
        part = part->next;
        ++snake_length;
    }

    // generate a random index for the food position (fields with snake body parts aren't indexed
    int random_food_idx = rand() % (fields - snake_length); // not evenly distributed
    int current_food_idx = 0;
    int searching = 1;
    for (int x = 0; x < state->fieldWidth && searching; ++x) {
        for (int y = 0; y < state->fieldHeight && searching; ++y) {
            int current_field_index = y * state->fieldWidth + x;
            int is_snake_body_part = field_buffer[current_field_index];
            if (!is_snake_body_part) {
                if (current_food_idx == random_food_idx) {
                    searching = 0;
                    state->food->position.x = x;
                    state->food->position.y = y;
                }
                ++current_food_idx;
            }
        }
    }
    free(field_buffer);
}

void
deinit_food(struct food_t *food) {
    free(food);
}

int
positions_equal(struct position_t *p1, struct position_t *p2) {
    return p1->x == p2->x && p1->y == p2->y;
}

double get_time() {
    return clock() * 1.0 / CLOCKS_PER_SEC;
}

int main() {
    // init
    srand(time(0));

    struct game_state_t *state = init_game_state();

    double time_per_update = 1.0 / 8.0;
    double previous_time = get_time();
    double update_lag = 0;
    while (state->running) {
        double current_time = get_time();
        double elapsed_time = current_time - previous_time;
        previous_time = current_time;
        update_lag += elapsed_time;

        read_inputs(state);
        while (update_lag >= time_per_update) {
            update_state(state);
            update_lag -= time_per_update;
        }
        render_state(state);
    }
    // deinit    

    deinit_game_state(state);
    return 0;
}

