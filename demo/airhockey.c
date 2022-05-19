#include "body.h"
#include "collision.h"
#include "forces.h"
#include "sdl_wrapper.h"
#include "time.h"
#include "vector.h"
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const double X_SIZE = 1200.0;
const double Y_SIZE = 800.0;
const double X_TABLE = 1000.0;
const double Y_TABLE = 500.0;
// const double X_SCOREBOARD = 
const double Y_SCOREBOARD = 200.0;
const double PADDING = 100.0;
const double X_ORIGIN = 0;
const double Y_ORIGIN = 0;
const double WALL_THICKNESS = 25.0;

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
const vector_t LEFT_VEL = {-200, 0};
const vector_t RIGHT_VEL = {200, 0};
const vector_t UP_VEL = {0, 200};
const vector_t DOWN_VEL = {0, -200};
const double PUCK_MASS = 1;
const int PUCK_RADIUS = 25;
const int CIRCLE_SIDES = 40;

const char PLAYER_1_INFO_C = '1';
const char PLAYER_2_INFO_C = '2';
const char PUCK_INFO_C = 'p';
const char WALL_INFO_C = 'w';  

char *PLAYER_1_INFO = "1";
char *PLAYER_2_INFO = "2";
char *PUCK_INFO = "p";
char *WALL_INFO = "w";  

int PPG = 1; 
int PPG_POWERUP = 2; 

typedef struct state {
  scene_t *scene;
  body_t *last_touched;
  body_t *other_player;
  int ppg; // points per goal
} state_t;

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

void check_player_1_boundary(state_t *state) {
  body_t *player = list_get(get_bodies_by_type(state->scene, PLAYER_1_INFO), 0);
  vector_t curr_centroid = body_get_centroid(player);
  if ((X_TABLE/2 + PADDING) - body_get_centroid(player).x < PADDLE_RADIUS) {
    body_set_centroid(player,
                      (vector_t){(X_TABLE/2 + PADDING) - PADDLE_RADIUS, curr_centroid.y});
    body_set_velocity(player, VEC_ZERO);
  // } else if (body_get_centroid(player).x < PADDLE_RADIUS + PADDING) {
  //   body_set_centroid(player, (vector_t){PADDLE_RADIUS + PADDING, curr_centroid.y});
  //   body_set_velocity(player, VEC_ZERO);
  // }
  // else if (Y_TABLE + PADDING - body_get_centroid(player).y < PADDLE_RADIUS) {
  //   body_set_centroid(player, (vector_t){curr_centroid.x, Y_TABLE + PADDING - PADDLE_RADIUS});
  //   body_set_velocity(player, VEC_ZERO);
  // }
  // else if (body_get_centroid(player).y < PADDLE_RADIUS + PADDING) {
  //   body_set_centroid(player, (vector_t){curr_centroid.x, PADDLE_RADIUS + PADDING});
  //   body_set_velocity(player, VEC_ZERO);
  // }
  }
}

void check_player_2_boundary(state_t *state) {
  body_t *player = list_get(get_bodies_by_type(state->scene, PLAYER_2_INFO), 0);
  vector_t curr_centroid = body_get_centroid(player);
  if (body_get_centroid(player).x < (X_TABLE/2 + PADDLE_RADIUS + PADDING)) {
    body_set_centroid(player,
                      (vector_t){(X_TABLE/2 + PADDLE_RADIUS + PADDING), curr_centroid.y});
    body_set_velocity(player, VEC_ZERO);
  // } else if (X_TABLE + PADDING - body_get_centroid(player).x < PADDLE_RADIUS) {
  //   body_set_centroid(player, (vector_t){X_TABLE + PADDING - PADDLE_RADIUS, curr_centroid.y});
  //   body_set_velocity(player, VEC_ZERO);
  // }
  // else if (Y_TABLE + PADDING - body_get_centroid(player).y < PADDLE_RADIUS) {
  //   body_set_centroid(player, (vector_t){curr_centroid.x, Y_TABLE + PADDING - PADDLE_RADIUS});
  //   body_set_velocity(player, VEC_ZERO);
  // }
  // else if (body_get_centroid(player).y < PADDLE_RADIUS + PADDING) {
  //   body_set_centroid(player, (vector_t){curr_centroid.x, PADDLE_RADIUS + PADDING});
  //   body_set_velocity(player, VEC_ZERO);
  // }
  }
}

body_t *make_vertical_wall(double mass, vector_t center, char *info) {
  list_t *shape = list_init(4, free);

  add_vec_ptr(shape, -WALL_THICKNESS / 2, Y_TABLE / 2);
  add_vec_ptr(shape, WALL_THICKNESS / 2, Y_TABLE / 2);
  add_vec_ptr(shape, -WALL_THICKNESS / 2, -Y_TABLE / 2);
  add_vec_ptr(shape, WALL_THICKNESS / 2, -Y_TABLE / 2);

  body_t *rec_body =
      body_init_with_info(shape, mass, RGB_BLACK, (void *)info, free);
  body_set_centroid(rec_body, center);
  return rec_body;
}

body_t *make_horizontal_wall(double mass, vector_t center, char *info) {
  list_t *shape = list_init(4, free);

  add_vec_ptr(shape, -X_TABLE / 2, WALL_THICKNESS / 2);
  add_vec_ptr(shape, X_TABLE / 2, WALL_THICKNESS / 2);
  add_vec_ptr(shape, -X_TABLE / 2, -WALL_THICKNESS / 2);
  add_vec_ptr(shape, X_TABLE / 2, -WALL_THICKNESS / 2);

  body_t *rec_body =
      body_init_with_info(shape, mass, RGB_BLACK, (void *)info, free);
  body_set_centroid(rec_body, center);
  return rec_body;
}

void make_walls(state_t *state) {
  scene_add_body(
      state->scene,
      make_horizontal_wall(INFINITY,
                           (vector_t){(X_SIZE / 2) + PADDING, Y_TABLE + PADDING - (WALL_THICKNESS / 2)},
                           WALL_INFO));
  scene_add_body(
      state->scene,
      make_horizontal_wall(INFINITY,
                           (vector_t){(X_SIZE / 2) + PADDING, PADDING + (WALL_THICKNESS / 2)},
                           WALL_INFO));
  scene_add_body(state->scene,
                 make_vertical_wall(INFINITY,
                                    (vector_t){PADDING + (WALL_THICKNESS / 2), PADDING + (Y_TABLE / 2)},
                                    WALL_INFO));
  scene_add_body(state->scene,
                 make_vertical_wall(
                     INFINITY, (vector_t){X_SIZE + PADDING - (WALL_THICKNESS / 2), PADDING + (Y_TABLE / 2)},
                     WALL_INFO));
}

void initialize_players(state_t *state) {
  body_t *player_1 = make_circle(PADDLE_MASS, GREEN_1, (vector_t){(X_SIZE / 2) + PADDING - 2 * WALL_THICKNESS, (Y_TABLE / 2) + PADDING}, PADDLE_RADIUS, PLAYER_1_INFO);
  body_t *player_2 = make_circle(PADDLE_MASS, GREEN_1, (vector_t){(X_SIZE / 2) + PADDING + 2 * WALL_THICKNESS, (Y_TABLE / 2) + PADDING}, PADDLE_RADIUS, PLAYER_2_INFO);

  scene_add_body(state->scene, player_1);
  scene_add_body(state->scene, player_2);

  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), WALL_INFO) == 0) {
      create_physics_collision(state->scene, 1, player_1, body);
      create_physics_collision(state->scene, 1, player_2, body);
    }
  }
}

void initialize_puck(state_t *state) {
  body_t *puck = make_circle(PUCK_MASS, RED, (vector_t){(X_SIZE / 2) + PADDING, (Y_TABLE / 2) + PADDING}, PUCK_RADIUS, PUCK_INFO);
  scene_add_body(state->scene, puck);
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), PUCK_INFO) != 0) {
      create_physics_collision(state->scene, 1, puck, body);
    }
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
  //sdl_on_key((key_handler_t)key_handler_func);
  return state;

}

void emscripten_main(state_t *state) {
  sdl_clear();
  double dt = time_since_last_tick();
  check_player_1_boundary(state);
  check_player_2_boundary(state);
  scene_tick(state->scene, dt);
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}