#ifndef SESSIONINFO_H
#define SESSIONINFO_H

#define SESSION_INFO_NAME_MAX_LENGTH 20

typedef struct {
  char name[SESSION_INFO_NAME_MAX_LENGTH];
  int id;
} Session;

typedef struct {
  Session *sessions;
  int length;
} SessionInfo;

SessionInfo* new_session_info(int session_size);

void free_session_info(SessionInfo* session_info);

#endif /* SESSIONINFO_H */
