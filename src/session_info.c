#include "session_info.h"
#include <stdlib.h>

SessionInfo* new_session_info(int session_size) {
	SessionInfo *session_info = malloc(sizeof(SessionInfo));
	session_info->length = session_size;
	session_info->sessions = calloc(session_size, sizeof(Session));

	return session_info;
}

void free_session_info(SessionInfo *session_info) {
	free(session_info->sessions);
	free(session_info);
}
