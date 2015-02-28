#include "main_menu.h"

static const uint16_t MENU_ITEM_PADDING = 5;

static uint16_t main_menu_get_num_sections(struct MenuLayer *main_menu, void *callback_context) { 
	// we only have 1 section as we don't have section headers.
	return 1;
}

static uint16_t main_menu_get_num_rows_in_section(struct MenuLayer *main_menu, uint16_t section_index, void *callback_context) { 
	return 10;
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
}

static void main_menu_draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) { 
	graphics_context_set_text_color(ctx, GColorBlack);
	char item_label[15];
	snprintf(item_label, 15, "Item %d", cell_index->row);
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

MenuLayer* create_main_menu(GRect frame) {
	MenuLayer *main_menu = menu_layer_create(GRect(0, 0, frame.size.w, frame.size.h));
	
	MenuLayerCallbacks menu_callbacks = main_menu_callbacks();
	menu_layer_set_callbacks(main_menu, NULL, menu_callbacks);
	return main_menu;
}
