#include <pebble.h>
#include "main_menu.h"

static Window *window;
// Sizing session menu for selecting a session
static MenuLayer *main_menu;


static void window_load(Window *window) {
	GRect frame;

	frame = layer_get_bounds(window_get_root_layer(window));
	main_menu = create_main_menu(frame);
	menu_layer_set_click_config_onto_window(main_menu, window);
	layer_add_child(window_get_root_layer(window), menu_layer_get_layer(main_menu));
}

static void window_unload(Window *window) {
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
}

static void deinit(void) {
	menu_layer_destroy(main_menu);
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
