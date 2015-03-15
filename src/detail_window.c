#include "detail_window.h"
#include "sizes_layer.h"

typedef struct {
	Session session;
	bool ticket_chosen;
} DetailWindowContext;

static DetailWindowContext *context;
static TextLayer *s_title_layer;
static TextLayer *s_loading_layer;
static TextLayer *s_loaded_layer;
static SizesLayer *s_sizes_layer;
static const int title_height = 18;

void detail_window_ticket_chosen(bool ticket_chosen) {
	context->ticket_chosen = ticket_chosen;
	layer_set_hidden(text_layer_get_layer(s_loading_layer), true);
	layer_set_hidden(text_layer_get_layer(s_loaded_layer), false);
	layer_set_hidden(sizes_layer_get_layer(s_sizes_layer), false);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Ticket Chosen");
}

static TextLayer* create_title_layer(GRect frame) {
	TextLayer *title_layer = text_layer_create(
			GRect(frame.origin.x, frame.origin.y,
					  frame.size.w, title_height)					
	);
	text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
	text_layer_set_background_color(title_layer, GColorBlack);
	text_layer_set_text_color(title_layer, GColorWhite);
	return title_layer;
}

static TextLayer* create_loading_layer(GRect frame) {
	TextLayer *text_layer = text_layer_create(
			GRect(frame.origin.x, frame.origin.y + title_height,
					  frame.size.w, frame.size.h - title_height)					
	);
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	return text_layer;
}

static TextLayer* create_loaded_layer(GRect frame) {
	TextLayer *text_layer = text_layer_create(
			GRect(frame.origin.x, frame.origin.y + title_height,
					  frame.size.w, frame.size.h - title_height)					
	);
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	return text_layer;
}

static SizesLayer* create_sizes_layer(GRect frame) {
	SizesLayer *sizes_layer = sizes_layer_create(
			GRect(frame.origin.x, frame.origin.y + title_height,
					  frame.size.w, frame.size.h - title_height)					
	);
	return sizes_layer;
}

static void detail_window_up_click_handler(ClickRecognizerRef recognizer, void *context) {
	sizes_layer_decrease_size(s_sizes_layer);
}

static void detail_window_down_click_handler(ClickRecognizerRef recognizer, void *context) {
	sizes_layer_increase_size(s_sizes_layer);
}

static void detail_window_click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_UP, detail_window_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, detail_window_down_click_handler);
}

static void window_load(Window *window) {
	Layer *layer = window_get_root_layer(window);
	GRect frame = layer_get_bounds(layer);
	s_title_layer = create_title_layer(frame);
	s_loading_layer = create_loading_layer(frame);
	s_loaded_layer = create_loaded_layer(frame);
	s_sizes_layer = create_sizes_layer(frame);

	layer_set_hidden(text_layer_get_layer(s_loaded_layer), true);
	layer_set_hidden(sizes_layer_get_layer(s_sizes_layer), true);

	DetailWindowContext *context = (DetailWindowContext*)window_get_user_data(window);
	text_layer_set_text(s_title_layer, context->session.name);
	text_layer_set_text(s_loading_layer, "Waiting For Ticket");
	text_layer_set_text(s_loaded_layer, "Ticket Chosen");
	layer_add_child(layer, text_layer_get_layer(s_title_layer));
	layer_add_child(layer, text_layer_get_layer(s_loading_layer));
	layer_add_child(layer, text_layer_get_layer(s_loaded_layer));
	layer_add_child(layer, sizes_layer_get_layer(s_sizes_layer));

	window_set_click_config_provider(window, detail_window_click_config_provider);
}
 
static void window_unload(Window *window) {
	DetailWindowContext *context = (DetailWindowContext*)window_get_user_data(window);
	free(context);
	text_layer_destroy(s_title_layer);
	text_layer_destroy(s_loading_layer);
	text_layer_destroy(s_loaded_layer);
	sizes_layer_destroy(s_sizes_layer);
	window_destroy(window);
}

Window* detail_window_create(Session session) {
	context = malloc(sizeof(DetailWindowContext));
	if (context == NULL) {
		APP_LOG(APP_LOG_LEVEL_ERROR, "Cannot create detail window context, not enough memory.");
	}
	context->session = session;
	context->ticket_chosen = false;
	
	Window *window = window_create();
	window_set_user_data(window, (void*)context);

	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	
	return window;
}
