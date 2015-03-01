#include "detail_window.h"

typedef struct {
	Session session;
} DetailWindowContext;

static TextLayer *text_layer;

static void window_load(Window *window) {
	Layer *layer = window_get_root_layer(window);
	GRect frame = layer_get_bounds(layer);
	text_layer = text_layer_create(frame);

	DetailWindowContext *context = (DetailWindowContext*)window_get_user_data(window);
	text_layer_set_text(text_layer, context->session.name);
	layer_add_child(layer, text_layer_get_layer(text_layer));
}
 
static void window_unload(Window *window) {
	DetailWindowContext *context = (DetailWindowContext*)window_get_user_data(window);
	free(context);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "DETAIL UNLOADED");
	text_layer_destroy(text_layer);
	window_destroy(window);
}

Window* detail_window_create(Session session) {
	DetailWindowContext *context = malloc(sizeof(DetailWindowContext));
	context->session = session;
	
	Window *window = window_create();
	window_set_user_data(window, (void*)context);

	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	
	return window;
}
