#ifndef SESSIONINFO_H
#define SESSIONINFO_H

typedef struct {
  char name[20];
  int id;
} Session;

typedef struct {
  Session *sessions;
  int length;
} SessionInfo;

SessionInfo* new_session_info(int session_size);

void free_session_info(SessionInfo* session_info);

#endif /* SESSIONINFO_H */
