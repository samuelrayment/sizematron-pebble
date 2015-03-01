#include <pebble.h>
#include "session_info.h"

#ifndef MAINMENU_H
#define MAINMENU_H

MenuLayer* create_main_menu(GRect frame);

void free_main_menu();

void main_menu_update_session_info(SessionInfo *session_info);
 
#endif /* MAINMENU_H */
