#ifndef DETAILWINDOW_H
#define DETAILWINDOW_H

#include <pebble.h>
#include "session_info.h"

Window* detail_window_create(Session session);

void detail_window_ticket_chosen(bool ticket_chosen);

#endif /* DETAILWINDOW_H */
