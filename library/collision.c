#include "collision.h"
#include "list.h"
#include "vector.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

list_t *get_axes(list_t *vertices) {
  list_t *axes = list_init(list_size(vertices), free);
  for (int i = 0; i < list_size(vertices); i++) {
    vector_t vec_1 = ((vector_t *)list_get(vertices, i))[0];
    vector_t vec_2 =
        ((vector_t *)list_get(vertices, (i + 1) % list_size(vertices)))[0];
    vector_t edge = vec_subtract(vec_1, vec_2);
    vector_t *normal = malloc(sizeof(vector_t));
    normal->x = edge.y;
    normal->y = -(edge.x);
    list_add(axes, normal);
  }
  return axes;
}

double *find_projection(list_t *shape, vector_t axis) {
  double *projection = malloc(sizeof(double) * 2);
  double min = vec_dot(vec_norm(axis), ((vector_t *)list_get(shape, 0))[0]);
  double max = min;
  for (int i = 1; i < list_size(shape); i++) {
    double proj = vec_dot(vec_norm(axis), ((vector_t *)list_get(shape, i))[0]);
    if (proj < min) {
      min = proj;
    } else if (proj > max) {
      max = proj;
    }
  }
  projection[0] = min;
  projection[1] = max;
  return projection;
}

collision_info_t find_collision(list_t *shape1, list_t *shape2) {
  collision_info_t collision = {true}; // work on this.
  list_t *axes_shape_1 = get_axes(shape1);
  list_t *axes_shape_2 = get_axes(shape2);
  // maximum separation of bodies, this is a vlaue that will quickly be replaced
  double min_overlap = INFINITY;
  for (int i = 0; i < list_size(axes_shape_1); i++) {
    vector_t axis_to_check = *((vector_t *)list_get(axes_shape_1, i));
    double *proj_1 = find_projection(shape1, axis_to_check);
    double *proj_2 = find_projection(shape2, axis_to_check);
    double overlap_1_on_2 = proj_1[1] - proj_2[0];
    double overlap_2_on_1 = proj_2[1] - proj_1[0];
    // check that objects are not collided
    if (overlap_1_on_2 < 0 || overlap_2_on_1 < 0) {
      collision.collided = false;
    }
    // check for impulse axis
    else if (overlap_1_on_2 > 0 && overlap_1_on_2 < min_overlap) {
      min_overlap = overlap_1_on_2;
      collision.axis = axis_to_check;
    } else if (overlap_2_on_1 > 0 && overlap_2_on_1 < min_overlap) {
      min_overlap = overlap_2_on_1;
      collision.axis = axis_to_check;
    }
    free(proj_1);
    free(proj_2);
  }
  for (int i = 0; i < list_size(axes_shape_2); i++) {
    vector_t axis_to_check = *((vector_t *)list_get(axes_shape_2, i));
    double *proj_1 = find_projection(shape1, axis_to_check);
    double *proj_2 = find_projection(shape2, axis_to_check);
    double overlap_1_on_2 = proj_1[1] - proj_2[0];
    double overlap_2_on_1 = proj_2[1] - proj_1[0];
    // check that objects are not collided
    if (overlap_1_on_2 < 0 || overlap_2_on_1 < 0) {
      collision.collided = false;
    }
    // check for impulse axis
    else if (overlap_1_on_2 > 0 && overlap_1_on_2 < min_overlap) {
      min_overlap = overlap_1_on_2;
      collision.axis = axis_to_check;
    } else if (overlap_2_on_1 > 0 && overlap_2_on_1 < min_overlap) {
      min_overlap = overlap_2_on_1;
      collision.axis = axis_to_check;
    }
    free(proj_1);
    free(proj_2);
  }
  free(axes_shape_1);
  free(axes_shape_2);
  return collision;
}