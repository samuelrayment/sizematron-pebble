#ifndef SIZESLAYER_H
#define SIZESLAYER_H

#include <pebble.h>

typedef struct {
  Layer *layer;
} SizesLayer;

SizesLayer *sizes_layer_create(GRect frame);

void sizes_layer_destroy(SizesLayer *sizes_layer);

Layer* sizes_layer_get_layer(SizesLayer *sizes_layer);

void sizes_layer_increase_size(SizesLayer *sizes_layer);

void sizes_layer_decrease_size(SizesLayer *sizes_layer);

#endif /* SIZESLAYER_H */
