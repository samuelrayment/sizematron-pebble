#include "sizes_layer.h"

static const int SIZING_LETTER_SIZE = 30;
static const int SIZING_LETTER_HORIZONTAL_PADDING = 12;
static const int SIZING_LETTER_VERTICAL_PADDING = 10;

#define FIB_SIZES_COUNT 8
static const char* fib_sizes[FIB_SIZES_COUNT] = { "0", "1", "2", "3", "5", "8", "13", "21" };

typedef struct {
	int selected_index;
} SizesLayerData;

static void sizes_layer_update_proc(struct Layer *layer, GContext *ctx);

SizesLayer *sizes_layer_create(GRect frame) {
	Layer *layer = layer_create_with_data(frame, sizeof(SizesLayerData));
	layer_set_update_proc(layer, &sizes_layer_update_proc);
	SizesLayer *sizes_layer = malloc(sizeof(SizesLayer));
	sizes_layer->layer = layer;
	SizesLayerData *data = (SizesLayerData*)layer_get_data(layer);
	data->selected_index = 0;

	return sizes_layer;
}

void sizes_layer_destroy(SizesLayer *sizes_layer) {
	layer_destroy(sizes_layer->layer);
	free(sizes_layer);
}

Layer* sizes_layer_get_layer(SizesLayer *sizes_layer) {
	return sizes_layer->layer;
}

static void sizes_layer_update_proc(struct Layer *layer, GContext *ctx) {
	SizesLayerData *data = (SizesLayerData*)layer_get_data(layer);

	GRect frame = layer_get_frame(layer);
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, frame, 0, GCornerNone);
	for (int i=0; i<FIB_SIZES_COUNT; i++) {
		int column = i % 3;
		int row = i / 3;
		GRect letter_frame = GRect(
				frame.origin.x + SIZING_LETTER_SIZE * column + SIZING_LETTER_HORIZONTAL_PADDING * (column + 1), 
				frame.origin.y + SIZING_LETTER_SIZE * row + SIZING_LETTER_VERTICAL_PADDING * row, 
				SIZING_LETTER_SIZE, SIZING_LETTER_SIZE
		);
		if (data->selected_index == i) {
			graphics_context_set_text_color(ctx, GColorWhite);
			graphics_context_set_fill_color(ctx, GColorBlack);
			graphics_fill_rect(ctx, letter_frame, 0, GCornerNone);
		} else {
			graphics_context_set_text_color(ctx, GColorBlack);
		}
		graphics_draw_text(ctx, fib_sizes[i], 
			fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), letter_frame,
			GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter,
			NULL
		);
	}
}

void sizes_layer_increase_size(SizesLayer *sizes_layer) {
	Layer *layer = sizes_layer->layer;
	SizesLayerData *data = (SizesLayerData*)layer_get_data(layer);
	if (data->selected_index < FIB_SIZES_COUNT - 1) {
		data->selected_index++;
		layer_mark_dirty(layer);
	}
}

void sizes_layer_decrease_size(SizesLayer *sizes_layer) {
	Layer *layer = sizes_layer->layer;
	SizesLayerData *data = (SizesLayerData*)layer_get_data(layer);
	if (data->selected_index > 0) {
		data->selected_index--;
		layer_mark_dirty(layer);
	}
}
