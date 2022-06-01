#include "body.h"
#include "collision.h"
#include "forces.h"
#include "sdl_wrapper.h"
#include "vector.h"
#include <string.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const double X_SIZE = 1200.0;
const double Y_SIZE = 800.0;
const double X_TABLE = 1000.0;
const double Y_TABLE = 500.0;
const double Y_SCOREBOARD = 200.0;
const double PADDING = 100.0;
const double X_ORIGIN = 0;
const double Y_ORIGIN = 0;
const double WALL_THICKNESS = 20.0;
const double GOAL_WIDTH = 200.0;

const double REC_HEIGHT = 25.0;
const double REC_WIDTH = 100.0;
const rgb_color_t RGB_GRAY = {0.5, 0.5, 0.5};
const rgb_color_t RGB_BLACK = {0.0, 0.0, 0.0};
const rgb_color_t RGB_WHITE = {1.0, 1.0, 1.0};
const rgb_color_t RED = {1.0, 0.0, 0.0};
const rgb_color_t ORANGE = {1.0, 0.5, 0.0};
const rgb_color_t YELLOW = {1.0, 1.0, 0.0};
const rgb_color_t GREEN_1 = {0.5, 1.0, 0.0};
const rgb_color_t GREEN_2 = {0.0, 1.0, 0.0};
const rgb_color_t TEAL = {0.0, 1.0, 1.0};
const rgb_color_t BLUE = {0.0, 0.5, 1.0};
const rgb_color_t INDIGO = {0.0, 0.0, 1.0};
const rgb_color_t VIOLET = {1.0, 0.0, 1.0};
const rgb_color_t PINK = {1.0, 0.0, 0.5};

const double PADDLE_MASS = 5.0;
const double PADDLE_RADIUS = 40;
const vector_t UP_ACCEL = {0, 100};
const vector_t DOWN_ACCEL = {0, -100};
const vector_t LEFT_ACCEL = {-100, 0};
const vector_t RIGHT_ACCEL = {100, 0};
const vector_t UP_VEL = {0, 250};
const vector_t DOWN_VEL = {0, -250};
const vector_t LEFT_VEL = {-250, 0};
const vector_t RIGHT_VEL = {250, 0};
const double MIN_VEL = 150.0;
const double MAX_VEL = 400.0;
const double PUCK_MASS = 1;
const int PUCK_RADIUS = 25;
const int POWERUP_RADIUS = 20; 
const int POWERUP_MASS = 10;
const int CIRCLE_SIDES = 40;
const int WIN_THRESHOLD = 7;
const int MAX_NUM_POWERUPS = 1;
const double POWERUP_TIME = 500.0;

const char PLAYER_1_INFO_C = '1';
const char PLAYER_2_INFO_C = '2';
const char PUCK_INFO_C = 'p';
const char WALL_INFO_C = 'w';  

const char X2_PUCK_VEL_INFO_C = 'v';
const char X2_NEXT_GOAL_INFO_C = 'g';
const char X2_PLAYER_ACC_INFO_C = 'a';
const char HALF_ENEMY_ACC_INFO_C = 'e';
const char FREEZE_ENEMY_INFO_C = 'f';

char *PLAYER_1_INFO = "1";
char *PLAYER_2_INFO = "2";
char *PUCK_INFO = "p";
char *WALL_INFO = "w";  

char *X2_PUCK_VEL_INFO = "v";
char *X2_NEXT_GOAL_INFO = "g";
char *X2_PLAYER_ACC_INFO = "a";
char *HALF_ENEMY_ACC_INFO = "e";
char *FREEZE_ENEMY_INFO = "f";

char *INDICATOR_1_INFO = "i";
char *INDICATOR_2_INFO = "j";

int PPG = 1; 
int PPG_POWERUP = 2; 

TTF_Font *PACIFICO;
Mix_Music *BACKGROUND_MUSIC;
Mix_Chunk *BOUNCE_SOUND;
Mix_Chunk *GOAL_SOUND;
Mix_Chunk *POWERUP_SOUND; 
SDL_Surface *PUCK_IMG; 
SDL_Surface *BLUE_PADDLE;
SDL_Surface *RED_PADDLE; 
SDL_Surface *SCORE0;
SDL_Surface *SCORE1;
SDL_Surface *SCORE2;
SDL_Surface *SCORE3;
SDL_Surface *SCORE4;
SDL_Surface *SCORE5;
SDL_Surface *SCORE6;
SDL_Surface *SCORE7;
SDL_Surface *DOUBLEACC_P;
SDL_Surface *DOUBLEGOAL_P;
SDL_Surface *DOUBLEVEL_P;
SDL_Surface *FREEZE_P;
SDL_Surface *HALFACC_P; 
SDL_Surface *FIELD;

typedef void (*powerup_func)(state_t *state); 

typedef struct state {
  scene_t *scene;
  body_t *last_touched;
  body_t *other_player;
  char *powerup_active;
  double powerup_time; 
  body_t *powerup_affects;
  char *powerup_available; 
  int ppg; // points per goal
  int player_1_score;
  int player_2_score;
  double time_passed;
  powerup_func powerup; 
  bool paused;
} state_t;

int rand_between(int lower, int upper) {
  return (rand() % (upper + 1 - lower)) + lower; 
}

void add_vec_ptr(list_t *shape, double x, double y) {
  vector_t v = vec_init(x, y);
  vector_t *v_p = malloc(sizeof(v));
  *v_p = v;
  list_add(shape, v_p);
}

body_t *make_circle(double mass, rgb_color_t color, vector_t center, double size, void *info) {
  // makes a ball at given using the given attributes
  // initialize and return a body constituting that ball
  list_t *shape = list_init(CIRCLE_SIDES, free);
  for (int i = 0; i < CIRCLE_SIDES; i++) {
    vector_t vec = vec_init(size, 0);
    double angle = i * 2 * M_PI / CIRCLE_SIDES;
    vec = vec_rotate(vec, angle);
    vector_t *vec_s = malloc(sizeof(vec));
    *vec_s = vec;
    list_add(shape, vec_s);
  }
  body_t *circle =
      body_init_with_info(shape, mass, color, (void *)info, free);
  body_set_centroid(circle, center);
  return circle;
}

list_t *get_bodies_by_type(scene_t *scene, char *type) {
  int n = scene_bodies(scene);
  list_t *body_list = list_init(1, (free_func_t)body_free);
  for (int i = 0; i < n; i++) {
    body_t *body = scene_get_body(scene, i);
    if ((char *)(body_get_info(body)) == type) {
      list_add(body_list, body);
      // there's only one player of a particular type
      // and one puck , so this will break out of the loop to save
      // time
      if ((char *)(body_get_info(body)) == PUCK_INFO) {
        break;
      }
      else if ((char *)(body_get_info(body)) == PLAYER_1_INFO) {
        break;
      }
      else if ((char *)(body_get_info(body)) == PLAYER_2_INFO) {
        break;
      }
    }
  }
  return body_list;
}

void key_handler_func_helper(double dt, body_t *body, vector_t acceleration) {
  if (sqrt(vec_dot(body_get_velocity(body), body_get_velocity(body))) == 0) {
    body_set_velocity(body, vec_multiply(MIN_VEL, unit_vector(acceleration)));
    return;
  }
  vector_t new_velocity = vec_add(body_get_velocity(body), vec_multiply(dt, acceleration));
  if (sqrt(vec_dot(new_velocity, new_velocity)) > MAX_VEL) {
    new_velocity = vec_multiply(MAX_VEL, unit_vector(new_velocity));
  } 
  body_set_velocity(body, new_velocity);  
}

void key_handler_func(state_t *state, char key_pressed, key_event_type_t event_type, double dt) {
  body_t *player_1 = list_get(get_bodies_by_type(state->scene, PLAYER_1_INFO), 0);
  body_t *player_2 = list_get(get_bodies_by_type(state->scene, PLAYER_2_INFO), 0);
  Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);
  vector_t new_vel_1 = {0, 0};
  vector_t new_vel_2 = {0, 0};
  if (keyboard_states[SDL_SCANCODE_W]) {
    new_vel_1 = vec_add(new_vel_1, UP_VEL);
  }
  if (keyboard_states[SDL_SCANCODE_A]) {
    new_vel_1 = vec_add(new_vel_1, LEFT_VEL);
  }
  if (keyboard_states[SDL_SCANCODE_S]) {
    new_vel_1 = vec_add(new_vel_1, DOWN_VEL);
  }
  if (keyboard_states[SDL_SCANCODE_D]) {
    new_vel_1 = vec_add(new_vel_1, RIGHT_VEL);
  }
  if (keyboard_states[SDL_SCANCODE_UP]) {
    new_vel_2 = vec_add(new_vel_2, UP_VEL);
  }
  if (keyboard_states[SDL_SCANCODE_LEFT]) {
    new_vel_2 = vec_add(new_vel_2, LEFT_VEL);
  }
  if (keyboard_states[SDL_SCANCODE_DOWN]) {
    new_vel_2 = vec_add(new_vel_2, DOWN_VEL);
  }
  if (keyboard_states[SDL_SCANCODE_RIGHT]) {
    new_vel_2 = vec_add(new_vel_2, RIGHT_VEL);
  }
  if (!(keyboard_states[SDL_SCANCODE_W] || keyboard_states[SDL_SCANCODE_A] || keyboard_states[SDL_SCANCODE_S] || keyboard_states[SDL_SCANCODE_D])) {
    body_set_velocity(player_1, VEC_ZERO);
  }
  if (!(keyboard_states[SDL_SCANCODE_UP] || keyboard_states[SDL_SCANCODE_LEFT] || keyboard_states[SDL_SCANCODE_DOWN] || keyboard_states[SDL_SCANCODE_RIGHT])) {
    body_set_velocity(player_2, VEC_ZERO);
  }
  if (keyboard_states[SDL_SCANCODE_P] && event_type == KEY_PRESSED) {
    state->paused = !(state->paused);
  }
  body_set_velocity(player_1, new_vel_1);
  body_set_velocity(player_2, new_vel_2);
}

void accel_key_handler_func(state_t *state, char key_pressed, key_event_type_t event_type, double dt) {
  body_t *player_1 = list_get(get_bodies_by_type(state->scene, PLAYER_1_INFO), 0);
  body_t *player_2 = list_get(get_bodies_by_type(state->scene, PLAYER_2_INFO), 0);
  Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);
  if (keyboard_states[SDL_SCANCODE_W]) {
    key_handler_func_helper(dt, player_1, UP_ACCEL);
  }
  if (keyboard_states[SDL_SCANCODE_A]) {
    key_handler_func_helper(dt, player_1, LEFT_ACCEL);
  }
  if (keyboard_states[SDL_SCANCODE_S]) {
    key_handler_func_helper(dt, player_1, DOWN_ACCEL);
  }
  if (keyboard_states[SDL_SCANCODE_D]) {
    key_handler_func_helper(dt, player_1, RIGHT_ACCEL);
  }
  if (keyboard_states[SDL_SCANCODE_UP]) {
    key_handler_func_helper(dt, player_2, UP_ACCEL);
  }
  if (keyboard_states[SDL_SCANCODE_LEFT]) {
    key_handler_func_helper(dt, player_2, LEFT_ACCEL);
  }
  if (keyboard_states[SDL_SCANCODE_DOWN]) {
    key_handler_func_helper(dt, player_2, DOWN_ACCEL);
  }
  if (keyboard_states[SDL_SCANCODE_RIGHT]) {
    key_handler_func_helper(dt, player_2, RIGHT_ACCEL);
  }
  if (!(keyboard_states[SDL_SCANCODE_W] || keyboard_states[SDL_SCANCODE_A] || keyboard_states[SDL_SCANCODE_S] || keyboard_states[SDL_SCANCODE_D])) {
    body_set_velocity(player_1, VEC_ZERO);
  }
  if (!(keyboard_states[SDL_SCANCODE_UP] || keyboard_states[SDL_SCANCODE_LEFT] || keyboard_states[SDL_SCANCODE_DOWN] || keyboard_states[SDL_SCANCODE_RIGHT])) {
    body_set_velocity(player_2, VEC_ZERO);
  }
}

void pause_key_handler_func(state_t *state, char key_pressed, key_event_type_t event_type, double dt) {
  Uint8 *keyboard_states = SDL_GetKeyboardState(NULL);
  if (keyboard_states[SDL_SCANCODE_P] && event_type == KEY_PRESSED) {
    state->paused = !(state->paused);
  }
}

void check_player_1_boundary(state_t *state) {
  body_t *player = list_get(get_bodies_by_type(state->scene, PLAYER_1_INFO), 0);
  vector_t curr_centroid = body_get_centroid(player);
  if ((X_TABLE/2 + PADDING) - body_get_centroid(player).x < PADDLE_RADIUS) {
    body_set_centroid(player,
                      (vector_t){(X_TABLE/2 + PADDING) - PADDLE_RADIUS, curr_centroid.y});
    body_set_velocity(player, VEC_ZERO);
  } else if (body_get_centroid(player).x < PADDLE_RADIUS + PADDING + WALL_THICKNESS) {
    body_set_centroid(player, (vector_t){PADDLE_RADIUS + PADDING + WALL_THICKNESS, curr_centroid.y});
    body_set_velocity(player, VEC_ZERO);
  }
  else if (Y_TABLE + PADDING - WALL_THICKNESS - body_get_centroid(player).y < PADDLE_RADIUS) {
    body_set_centroid(player, (vector_t){curr_centroid.x, Y_TABLE + PADDING - WALL_THICKNESS - PADDLE_RADIUS});
    body_set_velocity(player, VEC_ZERO);
  }
  else if (body_get_centroid(player).y < PADDLE_RADIUS + PADDING + WALL_THICKNESS) {
    body_set_centroid(player, (vector_t){curr_centroid.x, PADDLE_RADIUS + PADDING + WALL_THICKNESS});
    body_set_velocity(player, VEC_ZERO);
  }
}

void check_player_2_boundary(state_t *state) {
  body_t *player = list_get(get_bodies_by_type(state->scene, PLAYER_2_INFO), 0);
  vector_t curr_centroid = body_get_centroid(player);
  if (body_get_centroid(player).x < (X_TABLE/2 + PADDLE_RADIUS + PADDING)) {
    body_set_centroid(player,
                      (vector_t){(X_TABLE/2 + PADDLE_RADIUS + PADDING), curr_centroid.y});
    body_set_velocity(player, VEC_ZERO);
  } else if (X_TABLE + PADDING - WALL_THICKNESS - body_get_centroid(player).x < PADDLE_RADIUS) {
    body_set_centroid(player, (vector_t){X_TABLE + PADDING - PADDLE_RADIUS - WALL_THICKNESS, curr_centroid.y});
    body_set_velocity(player, VEC_ZERO);
  }
  else if (Y_TABLE + PADDING - WALL_THICKNESS - body_get_centroid(player).y < PADDLE_RADIUS) {
    body_set_centroid(player, (vector_t){curr_centroid.x, Y_TABLE + PADDING - WALL_THICKNESS - PADDLE_RADIUS});
    body_set_velocity(player, VEC_ZERO);
  }
  else if (body_get_centroid(player).y < PADDLE_RADIUS + PADDING + WALL_THICKNESS) {
    body_set_centroid(player, (vector_t){curr_centroid.x, PADDLE_RADIUS + PADDING + WALL_THICKNESS});
    body_set_velocity(player, VEC_ZERO);
  }
}

body_t *make_vertical_wall(double mass, vector_t center, char *info) {
  list_t *shape = list_init(4, free);
  
  add_vec_ptr(shape, WALL_THICKNESS / 2, -(Y_TABLE - GOAL_WIDTH)/4);
  add_vec_ptr(shape, WALL_THICKNESS / 2, (Y_TABLE - GOAL_WIDTH)/4);
  add_vec_ptr(shape, -WALL_THICKNESS / 2, (Y_TABLE - GOAL_WIDTH)/4);
  add_vec_ptr(shape, -WALL_THICKNESS / 2, -(Y_TABLE - GOAL_WIDTH)/4);

  body_t *rec_body =
      body_init_with_info(shape, mass, RGB_BLACK, (void *)info, free);
  body_set_centroid(rec_body, center);
  return rec_body;
}

body_t *make_horizontal_wall(double mass, vector_t center, char *info) {
  list_t *shape = list_init(4, free);

  add_vec_ptr(shape, X_TABLE / 2, -WALL_THICKNESS / 2);
  add_vec_ptr(shape, X_TABLE / 2, WALL_THICKNESS / 2);
  add_vec_ptr(shape, -X_TABLE / 2, WALL_THICKNESS / 2);
  add_vec_ptr(shape, -X_TABLE / 2, -WALL_THICKNESS / 2);
  
  body_t *rec_body =
      body_init_with_info(shape, mass, RGB_BLACK, (void *)info, free);
  body_set_centroid(rec_body, center);
  return rec_body;
}

void make_walls(state_t *state) {
  scene_add_body(
      state->scene,
      make_horizontal_wall(INFINITY,
                           (vector_t){(X_SIZE / 2), Y_TABLE + PADDING - (WALL_THICKNESS / 2)},
                           WALL_INFO));
  scene_add_body(
      state->scene,
      make_horizontal_wall(INFINITY,
                           (vector_t){(X_SIZE / 2), PADDING + (WALL_THICKNESS / 2)},
                           WALL_INFO));
  scene_add_body(state->scene,
                 make_vertical_wall(INFINITY,
                                    (vector_t){PADDING + (WALL_THICKNESS / 2), PADDING + (Y_TABLE - GOAL_WIDTH)/4},
                                    WALL_INFO));
  scene_add_body(state->scene,
                 make_vertical_wall(INFINITY,
                                    (vector_t){PADDING + (WALL_THICKNESS / 2), PADDING + Y_TABLE - (Y_TABLE - GOAL_WIDTH)/4},
                                    WALL_INFO));
  scene_add_body(state->scene,
                 make_vertical_wall(
                     INFINITY, (vector_t){X_SIZE - PADDING - (WALL_THICKNESS / 2), PADDING + (Y_TABLE - GOAL_WIDTH)/4},
                     WALL_INFO));
  scene_add_body(state->scene,
                 make_vertical_wall(
                     INFINITY, (vector_t){X_SIZE - PADDING - (WALL_THICKNESS / 2), PADDING + Y_TABLE - (Y_TABLE - GOAL_WIDTH)/4},
                     WALL_INFO));
}

void initialize_players(state_t *state) {
  body_t *player_1 = make_circle(PADDLE_MASS, RGB_WHITE, (vector_t){PADDING + WALL_THICKNESS + (X_TABLE / 4) , (Y_TABLE / 2) + PADDING}, PADDLE_RADIUS, PLAYER_1_INFO);
  body_t *player_2 = make_circle(PADDLE_MASS, RGB_WHITE, (vector_t){X_SIZE - PADDING - WALL_THICKNESS - (X_TABLE / 4), (Y_TABLE / 2) + PADDING}, PADDLE_RADIUS, PLAYER_2_INFO);

  scene_add_body(state->scene, player_1);
  scene_add_body(state->scene, player_2);

  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    switch (*((char *)body_get_info(body))) {
      case WALL_INFO_C:
      create_physics_collision(state->scene, 0, player_1, body);
      create_physics_collision(state->scene, 0, player_2, body);
      break;
    }
  }
}

void initialize_puck(state_t *state) {
  body_t *puck = make_circle(PUCK_MASS, RGB_WHITE, (vector_t){(X_SIZE / 2), (Y_TABLE / 2) + PADDING}, PUCK_RADIUS, PUCK_INFO);
  scene_add_body(state->scene, puck);
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    switch (*((char *)body_get_info(body))) {
      case WALL_INFO_C:
        create_physics_collision(state->scene, 1, puck, body);
        break;
      case PLAYER_1_INFO_C:
        create_physics_collision(state->scene, 1, puck, body);
        break;
      case PLAYER_2_INFO_C:
        create_physics_collision(state->scene, 1, puck, body);
        break;
    }
  }
}
/*
void initialize_indicators(state_t *state) {
  body_t *p1_indicator = make_circle(10, YELLOW, (vector_t) {900.0, 500.0}, POWERUP_RADIUS, INDICATOR_1_INFO);
  body_t *p2_indicator = make_circle(10, YELLOW, (vector_t) {1100.0, 500.0}, POWERUP_RADIUS, INDICATOR_2_INFO); 
  scene_add_body(state->scene, p1_indicator);
  scene_add_body(state->scene, p2_indicator); 
}
*/
char* rand_powerup() {
  int num = rand_between(0, 5); 
  switch(num) {
    case 0:
      return X2_PUCK_VEL_INFO;
    case 1:
      return X2_NEXT_GOAL_INFO;
    case 2: 
      return X2_PLAYER_ACC_INFO;
    case 3:
      return HALF_ENEMY_ACC_INFO;
    case 4: 
      return FREEZE_ENEMY_INFO;
    default:
      return FREEZE_ENEMY_INFO; 
  }
}

void check_pause(state_t *state) {
  sdl_render_text("Game paused", PACIFICO, RGB_BLACK, (vector_t){500.0, 100.0}); 
  sdl_render_text("Press 'P' to resume", PACIFICO, RGB_BLACK, (vector_t){500.0, 500.0});
}

void check_win(state_t *state) {
  if (state->player_1_score >= WIN_THRESHOLD) {
    sdl_render_text("Player 1 wins!", PACIFICO, RGB_BLACK, (vector_t){500.0, 300.0}); 
    SDL_Delay(1500); 
    exit(0);
  } else if (state->player_2_score >= WIN_THRESHOLD) {
    sdl_render_text("Player 2 wins!", PACIFICO, RGB_BLACK, (vector_t){500.0, 300.0}); 
    SDL_Delay(1500); 
    exit(0);
  }
}

void reset_positions(state_t *state) {
  body_t *player1 = list_get(get_bodies_by_type(state->scene, PLAYER_1_INFO), 0);
  body_t *player2 = list_get(get_bodies_by_type(state->scene, PLAYER_2_INFO), 0);
  body_set_centroid(player1, (vector_t) {PADDING + WALL_THICKNESS + (X_TABLE / 4), (Y_TABLE / 2) + PADDING});
  body_set_centroid(player2, (vector_t) {X_SIZE - PADDING - WALL_THICKNESS - (X_TABLE / 4), (Y_TABLE / 2) + PADDING}); 
  body_set_velocity(player1, VEC_ZERO);
  body_set_velocity(player2, VEC_ZERO);
  state->ppg = PPG;
  state->powerup_active = NULL;
  state->powerup_time = 0.0;
  state->powerup_affects = NULL; 
  state->powerup = NULL; 
}

void check_goal(state_t *state) {
  body_t *puck = list_get(get_bodies_by_type(state->scene, PUCK_INFO), 0);
  if (body_get_centroid(puck).x > X_SIZE - PADDING - (WALL_THICKNESS / 2)) {
    state->player_1_score = state->player_1_score + state->ppg;
    body_set_centroid(puck, (vector_t){(X_SIZE / 2), (Y_TABLE / 2) + PADDING});
    body_set_velocity(puck, VEC_ZERO);
    Mix_PlayChannel(-1, GOAL_SOUND, 0); 
    reset_positions(state); 
    printf("%i || %i \n", state->player_1_score, state->player_2_score); 
  } else if (body_get_centroid(puck).x < PADDING + (WALL_THICKNESS / 2)) {
    state->player_2_score = state->player_2_score + state->ppg;
    body_set_centroid(puck, (vector_t){(X_SIZE / 2), (Y_TABLE / 2) + PADDING});
    body_set_velocity(puck, VEC_ZERO);
    Mix_PlayChannel(-1, GOAL_SOUND, 0); 
    reset_positions(state); 
    printf("%i || %i \n", state->player_1_score, state->player_2_score); 
  }
}

void double_velocity(state_t *state) {
  body_t *puck = list_get(get_bodies_by_type(state->scene, PUCK_INFO), 0);
  state->powerup_affects = puck; 
  vector_t curr_velocity = body_get_velocity(puck); 
  body_set_velocity(puck, vec_multiply(2.0, curr_velocity));
  
}

void double_accel(state_t *state) {
  if(state->powerup_affects == NULL) {
    state->powerup_affects = state->last_touched; 
  }
  body_set_velocity(state->powerup_affects, vec_multiply(2.0, body_get_velocity(state->powerup_affects)));   
  
}

void half_accel(state_t *state) {
  if(state->powerup_affects == NULL) {
    state->powerup_affects = state->other_player; 
  }
  body_set_velocity(state->powerup_affects, vec_multiply(0.5, body_get_velocity(state->powerup_affects)));   
}

void freeze_enemy(state_t *state) {
  if(state->powerup_affects == NULL) {
    state->powerup_affects = state->other_player; 
  }
  body_set_velocity(state->powerup_affects, (vector_t) {0, 0});
}

void double_goal(state_t *state) {
  state->ppg = PPG_POWERUP; 
}

list_t *get_all_powerups(state_t *state) {
  list_t *x2pucks = get_bodies_by_type(state->scene, X2_PUCK_VEL_INFO);
  list_t *x2goals = get_bodies_by_type(state->scene, X2_NEXT_GOAL_INFO);
  list_t *x2acc = get_bodies_by_type(state->scene, X2_PLAYER_ACC_INFO);
  list_t *h2acc = get_bodies_by_type(state->scene, HALF_ENEMY_ACC_INFO);
  list_t *freeze = get_bodies_by_type(state->scene, FREEZE_ENEMY_INFO);
  list_t *powerups = list_init(1, NULL);
  for (int i = 0; i < list_size(x2pucks); i++) {
    list_add(powerups, list_get(x2pucks, i));
  }
  for (int i = 0; i < list_size(x2goals); i++) {
    list_add(powerups, list_get(x2goals, i));
  }
  for (int i = 0; i < list_size(x2acc); i++) {
    list_add(powerups, list_get(x2acc, i));
  }
  for (int i = 0; i < list_size(h2acc); i++) {
    list_add(powerups, list_get(h2acc, i));
  }
  for (int i = 0; i < list_size(freeze); i++) {
    list_add(powerups, list_get(freeze, i));
  }
  return powerups; 
}

void add_powerup(state_t *state, char* powerup) {
  double max_x = X_TABLE + PADDING - WALL_THICKNESS - POWERUP_RADIUS; 
  double min_x = POWERUP_RADIUS + PADDING + WALL_THICKNESS; 
  double rand_x = rand_between(min_x, max_x);
  double max_y = Y_TABLE + PADDING - WALL_THICKNESS - POWERUP_RADIUS; 
  double min_y =  POWERUP_RADIUS + PADDING + WALL_THICKNESS; 
  double rand_y = rand_between(min_y, max_y); 
  vector_t rand_center = (vector_t) {rand_x, rand_y};
  body_t *powerup_body = make_circle(POWERUP_MASS, RGB_WHITE, rand_center, POWERUP_RADIUS, powerup); 
  scene_add_body(state->scene, powerup_body);   
  state->powerup_available = powerup; 
}

powerup_func get_correct_powerup(char *info, state_t *state) {
  switch(*info) {
    case X2_PUCK_VEL_INFO_C:
      return double_velocity;
    case X2_PLAYER_ACC_INFO_C:
      return double_accel;
    case X2_NEXT_GOAL_INFO_C:
      return double_goal;
    case HALF_ENEMY_ACC_INFO_C:
      return half_accel;
    case FREEZE_ENEMY_INFO_C:
      return freeze_enemy;
    default:
      return freeze_enemy;
  }
  return freeze_enemy;
}

void powerup_collide(state_t *state) { 
  list_t *powerups = get_all_powerups(state);
  body_t *puck = list_get(get_bodies_by_type(state->scene, PUCK_INFO), 0); 
  for(int i = 0; i < list_size(powerups); i++) {
    body_t *powerup_body = list_get(powerups, i); 
    if((fabs(body_get_centroid(powerup_body).x - body_get_centroid(puck).x) < PUCK_RADIUS) && (fabs(body_get_centroid(powerup_body).y - body_get_centroid(puck).y) < PUCK_RADIUS)) {
      Mix_PlayChannel(-1, POWERUP_SOUND, 0); 
      state->powerup = get_correct_powerup(body_get_info(powerup_body), state);
      state->powerup_active = body_get_info(powerup_body); 
      state->powerup_available = NULL; 
      body_remove(powerup_body); 
    }
  }
}

void wall_sounds(state_t *state) {
  list_t *walls = get_bodies_by_type(state->scene, WALL_INFO);
  body_t *puck = list_get(get_bodies_by_type(state->scene, PUCK_INFO), 0); 
  for(int i = 0; i < list_size(walls); i++) {
    body_t *this_wall = list_get(walls, i); 
    collision_info_t collided = find_collision(body_get_shape(puck), body_get_shape(this_wall));
    if(collided.collided) {
      Mix_PlayChannel(-1, BOUNCE_SOUND, 0); 
    }
  }
}

void change_player_designations(state_t *state) {
  body_t *player1 = list_get(get_bodies_by_type(state->scene, PLAYER_1_INFO), 0); 
  body_t *player2 = list_get(get_bodies_by_type(state->scene, PLAYER_2_INFO), 0); 
  body_t *puck = list_get(get_bodies_by_type(state->scene, PUCK_INFO), 0); 
  collision_info_t collided1 = find_collision(body_get_shape(player1), body_get_shape(puck));
  collision_info_t collided2 = find_collision(body_get_shape(player2), body_get_shape(puck)); 
  if(collided1.collided) {
    state->last_touched = player1;
    state->other_player = player2; 
    Mix_PlayChannel(-1, BOUNCE_SOUND, 0); 
  }
  else if(collided2.collided) {
    state->last_touched = player2;
    state->other_player = player1; 
    Mix_PlayChannel(-1, BOUNCE_SOUND, 0); 
  }
}

void speed_limit(state_t *state) {
  if(state->powerup_active == NULL) {
    return; 
  }
  body_t *affected = state->powerup_affects; 
  if(strcmp(state->powerup_active, X2_PLAYER_ACC_INFO) == 0 || strcmp(state->powerup_active, X2_PUCK_VEL_INFO) == 0) {
    double checkx = body_get_velocity(affected).x;
    if(body_get_velocity(affected).x > 600.0) {
      checkx = 600.0; 
    }
    double checky = body_get_velocity(affected).y;
    if(body_get_velocity(affected).y > 600.0) {
      checky = 600.0;
    }
    body_set_velocity(affected, (vector_t) {checkx, checky}); 
  }
  else if(strcmp(state->powerup_active, HALF_ENEMY_ACC_INFO) == 0) {
    double checkx = body_get_velocity(affected).x;
    if(body_get_velocity(affected).x > 200.0) {
      checkx = 200.0; 
    }
    double checky = body_get_velocity(affected).y;
    if(body_get_velocity(affected).y > 200.0) {
      checky = 200.0;
    }
    body_set_velocity(affected, (vector_t) {checkx, checky}); 
  }
}

void render_circle_sprites(state_t *state) {
  body_t *puck = list_get(get_bodies_by_type(state->scene, PUCK_INFO), 0);
  body_t *player1 = list_get(get_bodies_by_type(state->scene, PLAYER_1_INFO), 0);
  body_t *player2 = list_get(get_bodies_by_type(state->scene, PLAYER_2_INFO), 0);
  sdl_make_sprite(PUCK_IMG, puck, PUCK_RADIUS);
  sdl_make_sprite(BLUE_PADDLE, player1, PADDLE_RADIUS);
  sdl_make_sprite(RED_PADDLE, player2, PADDLE_RADIUS); 
}

SDL_Surface *surface_from_score(int score) {
  switch(score) {
    case 0:
      return SCORE0;
    case 1:
      return SCORE1;
    case 2:
      return SCORE2;
    case 3:
      return SCORE3;
    case 4: 
      return SCORE4;
    case 5: 
      return SCORE5;
    case 6:
      return SCORE6;
    case 7:
      return SCORE7;
    default:
      return SCORE0;
  }
  return SCORE0;
}

void draw_scoreboard(state_t *state) {
  SDL_Surface *player1score = surface_from_score(state->player_1_score);
  SDL_Surface *player2score = surface_from_score(state->player_2_score);
  render_scoreboard(player1score, player2score); 
}

void render_powerup_sprite(state_t *state) {
  SDL_Surface *powerup; 
  switch(*(state->powerup_available)) {
    case X2_PUCK_VEL_INFO_C:
      powerup = DOUBLEVEL_P;
      break;
    case X2_PLAYER_ACC_INFO_C:
      powerup = DOUBLEACC_P;
      break;
    case X2_NEXT_GOAL_INFO_C:
      powerup = DOUBLEGOAL_P;
      break;
    case FREEZE_ENEMY_INFO_C:
      powerup = FREEZE_P;
      break;
    case HALF_ENEMY_ACC_INFO_C:
      powerup = HALFACC_P;
      break; 
    default:
      powerup = NULL; 
  }
  if(powerup != NULL) {
    body_t *powerup_body = list_get(get_bodies_by_type(state->scene, state->powerup_available), 0); 
    sdl_make_sprite(powerup, powerup_body, POWERUP_RADIUS); 
  }
}
/*
void render_indicator_sprite(state_t *state) {
  body_t *puck = list_get(get_bodies_by_type(state->scene, PUCK_INFO), 0);
  body_t *player1 = list_get(get_bodies_by_type(state->scene, PLAYER_1_INFO), 0);
  body_t *player2 = list_get(get_bodies_by_type(state->scene, PLAYER_2_INFO), 0);
  SDL_Surface *powerup; 
  switch(*(state->powerup_active)) {
    case X2_PUCK_VEL_INFO_C:
      powerup = DOUBLEVEL_P;
      break;
    case X2_PLAYER_ACC_INFO_C:
      powerup = DOUBLEACC_P;
      break;
    case X2_NEXT_GOAL_INFO_C:
      powerup = DOUBLEGOAL_P;
      break;
    case FREEZE_ENEMY_INFO_C:
      powerup = FREEZE_P;
      break;
    case HALF_ENEMY_ACC_INFO_C:
      powerup = HALFACC_P;
      break; 
    default:
      powerup = NULL; 
  }
  body_t *indicator1 = list_get(get_bodies_by_type(state->scene, INDICATOR_1_INFO), 0);
  body_t *indicator2 = list_get(get_bodies_by_type(state->scene, INDICATOR_2_INFO), 0);
  if(state->powerup_affects == puck) {
    sdl_make_sprite(powerup, indicator1, POWERUP_RADIUS);
    sdl_make_sprite(powerup, indicator2, POWERUP_RADIUS);
  }
  else if(state->powerup_affects == player1) {
    sdl_make_sprite(powerup, indicator1, POWERUP_RADIUS);
  }
  else if(state->powerup_affects == player2) {
    sdl_make_sprite(powerup, indicator2, POWERUP_RADIUS);
  }
}
*/

void render_powerup_message(state_t *state) {
  if(state->powerup_active == NULL) {
    return; 
  }
  if(strcmp(state->powerup_active, X2_NEXT_GOAL_INFO) == 0) {
    sdl_render_text("Next goal worth double!", PACIFICO, RGB_BLACK, (vector_t) {500, 800});
    return;
  }
  char *message;
  body_t *puck = list_get(get_bodies_by_type(state->scene, PUCK_INFO), 0);
  body_t *player1 = list_get(get_bodies_by_type(state->scene, PLAYER_1_INFO), 0);
  body_t *player2 = list_get(get_bodies_by_type(state->scene, PLAYER_2_INFO), 0);
  if(state->powerup_affects == puck) {
    message = "Puck is ";
  }
  else if(state->powerup_affects == player1) {
    message = "Player 1 is ";
  }
  else if(state->powerup_affects == player2) {
    message = "Player 2 is ";
  }
  switch(*(state->powerup_active)) {
    case X2_PUCK_VEL_INFO_C:
      strcat(message, "extra fast!");
      break;
    case X2_PLAYER_ACC_INFO_C:
      strcat(message, "zooming!");
      break;
    case FREEZE_ENEMY_INFO_C:
      strcat(message, "real cold!");
      break;
    case HALF_ENEMY_ACC_INFO_C:
      strcat(message, "real slow!");
      break; 
    default:
      return; 
  }
  if(message != NULL) {
    sdl_render_text(message, PACIFICO, RGB_BLACK, (vector_t) {500, 800});
  }  
}

state_t *emscripten_init() {
  srand(time(NULL));
  sdl_init(VEC_ZERO, (vector_t){X_SIZE, Y_SIZE});
  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();
  make_walls(state);
  initialize_players(state);
  initialize_puck(state);
  state->last_touched = list_get(get_bodies_by_type(state->scene, PLAYER_1_INFO), 0);
  state->other_player = list_get(get_bodies_by_type(state->scene, PLAYER_2_INFO), 0);
  state->ppg = PPG;
  state->powerup_active = NULL;
  state->powerup_time = 0.0;
  state->powerup_affects = NULL; 
  state->powerup = NULL; 
  state->player_1_score = 0;
  state->player_2_score = 0;
  state->time_passed = 0.0; 
  state->powerup_available = NULL; 
  state->paused = false;
  PACIFICO = TTF_OpenFont("assets/Pacifico.ttf", 65); 
  BOUNCE_SOUND = Mix_LoadWAV("assets/bounce.wav");
  GOAL_SOUND = Mix_LoadWAV("assets/goal.wav");
  POWERUP_SOUND = Mix_LoadWAV("assets/powerup.wav"); 
  PUCK_IMG = IMG_Load("assets/puck.png");
  BLUE_PADDLE = IMG_Load("assets/bpaddle.png");
  RED_PADDLE = IMG_Load("assets/rpaddle.png");
  SCORE0 = IMG_Load("assets/score0.png");
  SCORE1 = IMG_Load("assets/score1.png");
  SCORE2 = IMG_Load("assets/score2.png");
  SCORE3 = IMG_Load("assets/score3.png");
  SCORE4 = IMG_Load("assets/score4.png");
  SCORE5 = IMG_Load("assets/score5.png");
  SCORE6 = IMG_Load("assets/score6.png");
  SCORE7 = IMG_Load("assets/score7.png");
  DOUBLEACC_P = IMG_Load("assets/doubleacc.png");
  DOUBLEGOAL_P = IMG_Load("assets/doublegoal.png");
  DOUBLEVEL_P = IMG_Load("assets/doublevel.png");
  FREEZE_P = IMG_Load("assets/freeze.png");
  HALFACC_P = IMG_Load("assets/halfaccel.png");
  FIELD = IMG_Load("assets/grid.png");
  sdl_on_key((key_handler_t)key_handler_func);
  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  if (dt > 0) {
    state->time_passed += 1;
    if (state->time_passed >= POWERUP_TIME && list_size(get_all_powerups(state)) < MAX_NUM_POWERUPS && state->powerup_active == NULL) { //adjust this for powerup cap
      add_powerup(state, rand_powerup()); 
      state->time_passed = 0.0;
    }
  }
  if (state->powerup_active) {
    state->powerup_time += 1;
    if(state->powerup_time <= POWERUP_TIME) {
      (*state).powerup(state); 
    }
    else {
      if(state->powerup_active == X2_NEXT_GOAL_INFO) {
        state->ppg = PPG; 
      }
      state->powerup_active = NULL;
      state->powerup_affects = NULL;
      state->powerup_time = 0.0; 
      printf("Powerup deactivated! \n");
    }
  }
  if (!(state->paused)) {
    speed_limit(state);
    powerup_collide(state);
    wall_sounds(state);
    check_player_1_boundary(state);
    check_player_2_boundary(state);
    change_player_designations(state);
    scene_tick(state->scene, dt);
    speed_limit(state);
    check_goal(state);
    check_win(state);
    sdl_render_scene(state->scene);
    sdl_make_table(FIELD, (vector_t) {X_SIZE / 4 + WALL_THICKNESS/2, Y_SIZE / 4 + WALL_THICKNESS/2}, X_TABLE - WALL_THICKNESS, Y_TABLE - WALL_THICKNESS);
    render_circle_sprites(state);
    draw_scoreboard(state);
    sdl_render_text("Canada, eh?!", PACIFICO, RGB_BLACK, (vector_t) {500, 0}); 
    if(state->powerup_available != NULL) {
      render_powerup_sprite(state); 
    }
    if(state->powerup_active != NULL) {
      //render_indicator_sprite(state);
      render_powerup_message(state); 
    }
  }
  else {
    check_pause(state);
  }
  sdl_clear();
}

void emscripten_free(state_t *state) {
  SDL_FreeSurface(PUCK_IMG);
  SDL_FreeSurface(BLUE_PADDLE);
  SDL_FreeSurface(RED_PADDLE);
  SDL_FreeSurface(SCORE0);
  SDL_FreeSurface(SCORE1);
  SDL_FreeSurface(SCORE2);
  SDL_FreeSurface(SCORE3);
  SDL_FreeSurface(SCORE4);
  SDL_FreeSurface(SCORE5);
  SDL_FreeSurface(SCORE6);
  SDL_FreeSurface(SCORE7);
  SDL_FreeSurface(DOUBLEACC_P);
  SDL_FreeSurface(DOUBLEVEL_P);
  SDL_FreeSurface(DOUBLEGOAL_P);
  SDL_FreeSurface(FREEZE_P);
  SDL_FreeSurface(HALFACC_P);
  Mix_CloseAudio();
  TTF_CloseFont(PACIFICO);
  scene_free(state->scene);
  free(state);
}