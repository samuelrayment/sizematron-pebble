#include <pebble.h>
#include "session_info.h"
#include "detail_window.h"

#define MSG_TYPE 0
#define SESSION_COUNT 1
#define SESSION_NAMES 2

static Window *window;
static Window *detail_window = NULL;
// Sizing session menu for selecting a session
static MenuLayer *main_menu;

////////////
// MAIN MENU
////////////

static const uint16_t MENU_ITEM_PADDING = 5;

// Struct for holding menu context data
typedef struct {
	SessionInfo *session_info;
} MenuContext;

static MenuContext *menu_context;

static uint16_t main_menu_get_num_sections(struct MenuLayer *main_menu, void *callback_context) { 
	// we only have 1 section as we don't have section headers.
	return 1;
}

static uint16_t main_menu_get_num_rows_in_section(struct MenuLayer *main_menu, uint16_t section_index, void *callback_context) { 
	MenuContext *menu_context = (MenuContext*) callback_context;
	return menu_context->session_info->length;
}

static int16_t main_menu_get_cell_height(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) { 
	return 50;
}

static int16_t main_menu_get_header_height(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) { 
	return 0;
}

static int16_t main_menu_get_separator_height(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
	return 0;
}

static void main_menu_select_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) { 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Selected item: %d", cell_index->row);
	MenuContext *menu_context = (MenuContext*) callback_context;

	Session session = menu_context->session_info->sessions[cell_index->row];
	detail_window = detail_window_create(session);
  window_stack_push(detail_window, true);
}

static void main_menu_draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) { 
	MenuContext *menu_context = (MenuContext*) callback_context;

	graphics_context_set_text_color(ctx, GColorBlack);
	char *item_label = menu_context->session_info->sessions[cell_index->row].name;
	graphics_draw_text(ctx, item_label, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), 
			GRect(MENU_ITEM_PADDING, MENU_ITEM_PADDING, 
					  layer_get_frame(cell_layer).size.w - 2 * MENU_ITEM_PADDING, 
						layer_get_frame(cell_layer).size.h - 2 * MENU_ITEM_PADDING),
			GTextOverflowModeTrailingEllipsis,
			GTextAlignmentLeft, NULL);
}

static void main_menu_draw_header(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context) { 
}

static void main_menu_draw_separator(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
}

static MenuLayerCallbacks main_menu_callbacks() {
	return ((MenuLayerCallbacks) {
		.get_num_sections = &main_menu_get_num_sections,
		.get_num_rows = &main_menu_get_num_rows_in_section,
		.get_cell_height = &main_menu_get_cell_height,
		.get_header_height = &main_menu_get_header_height, 
		.get_separator_height = &main_menu_get_separator_height,
		.select_click = &main_menu_select_click,
		.draw_row = &main_menu_draw_row,
		.draw_header = &main_menu_draw_header,
		.draw_separator = &main_menu_draw_separator,
	});
}

// Create and return a main menu the size of frame
MenuLayer* create_main_menu(GRect frame) {
	MenuLayer *main_menu = menu_layer_create(GRect(0, 0, frame.size.w, frame.size.h));
	
	menu_context = malloc(sizeof(MenuContext));
	menu_context->session_info = new_session_info(0);
	MenuLayerCallbacks menu_callbacks = main_menu_callbacks();
	menu_layer_set_callbacks(main_menu, (void*)menu_context, menu_callbacks);
	return main_menu;
}

// free the main menu
void free_main_menu() {
	free_session_info(menu_context->session_info);
	free(menu_context);
}

// Update the main menu with the new session info, the main menu
// now owns the session info, so it should not be freed by the 
// caller.
void main_menu_update_session_info(SessionInfo *session_info) {
	// we always have a session info object
	free(menu_context->session_info);
	menu_context->session_info = session_info;
}
static void window_load(Window *window) {
	GRect frame;

	frame = layer_get_bounds(window_get_root_layer(window));
	main_menu = create_main_menu(frame);
	menu_layer_set_click_config_onto_window(main_menu, window);
	layer_add_child(window_get_root_layer(window), menu_layer_get_layer(main_menu));
}

static void window_unload(Window *window) {
 	APP_LOG(APP_LOG_LEVEL_ERROR, "Unload");
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
 	APP_LOG(APP_LOG_LEVEL_ERROR, "Destroy");
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
