#include <pebble.h>
#include "main_menu.h"
#include "session_info.h"

#define MSG_TYPE 0
#define SESSION_COUNT 1
#define SESSION_NAMES 2

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
	free_main_menu();
}

static SessionInfo* parseSessionInfo(DictionaryIterator *iter) {
	SessionInfo *session_info;

	Tuple *tuple = dict_read_next(iter);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Session Count: %lu", tuple->value->uint32);
	int info_size = tuple->value->uint32;
	session_info = new_session_info(info_size);
	tuple = dict_read_next(iter);
	while (tuple) {
		// account for the type and count keys
		int key = tuple->key - 2; 
		int index = key / 2;
		if (index > info_size) {
			free(session_info);
			return NULL;
		}
		if (key % 2 == 0) {
			session_info->sessions[index].id = tuple->value->uint32;
		} else {
			strncpy(session_info->sessions[index].name, tuple->value->cstring, 20);
		}
		tuple = dict_read_next(iter);
	}
	return session_info;
}

static void app_message_inbox_received(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "INBOX RECEIVED");
	Tuple *tuple = dict_read_first(iter);
 	APP_LOG(APP_LOG_LEVEL_DEBUG, "Message Type: %s", tuple->value->cstring);
	SessionInfo *session_info = parseSessionInfo(iter);

	main_menu_update_session_info(session_info);
	menu_layer_reload_data(main_menu);
}

static void app_message_inbox_dropped(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "INBOX DROPPED");
}

static void app_message_outbox_sent(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "OUTBOX SENT");
}

static char* app_message_result_string(AppMessageResult reason) {
	switch (reason) {
		case APP_MSG_OK: return "APP_MSG_OK";
		case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
		case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
		case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
		case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
		case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
		case APP_MSG_BUSY: return "APP_MSG_BUSY";
		case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
		case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
		case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
		case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
		case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
		case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
		case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
	}
	return "";
}

static void app_message_outbox_failed(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "OUTBOX FAILED: %s", app_message_result_string(reason));
}

static void init_app_message() {
	app_message_register_inbox_received(&app_message_inbox_received);
	app_message_register_inbox_dropped(&app_message_inbox_dropped);
	app_message_register_outbox_sent(&app_message_outbox_sent);
	app_message_register_outbox_failed(&app_message_outbox_failed);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
	DictionaryIterator *iter;
	AppMessageResult result = app_message_outbox_begin(&iter);
	if (result != APP_MSG_OK) {
  	APP_LOG(APP_LOG_LEVEL_ERROR, "Cannot open outbox to send message");
	} else {
		dict_write_cstring(iter, MSG_TYPE, "ready");
		app_message_outbox_send();
	}
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
	init_app_message();
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
