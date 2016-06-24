#pragma strong_types,rtt_checks,save_types

struct frame_s {
  int    id;
  int    type;
  string name;
  string prog;
  string obj;
  int    loc;
  int    ticks;
};

struct note_s {
  int    id;
  int    time;
  string user;
  string txt;
};

struct base_issue_s {
  int    id;
  string hashkey;
  string uid;
  int    type;
  int    mtime;
  int    ctime;
  int    atime;
  int    count;
  int    deleted;
  int    resolved;
  int    locked;
  string locked_by;
  int    locked_time;
  string resolver;
  string message;
  string loadname;
};

struct ctissue_s (base_issue_s) {
};

struct userissue_s (ctissue_s) {
  string obj;
  string prog;
  int    loc;
  string titp;
  string tienv;
};

struct rtissue_s (userissue_s) {
  string hbobj;
  int    caught;
  string command;
  string verb;
};

// all possible data including stack trace and notes.
struct fullissue_s (rtissue_s) {
  mixed * notes; //struct note_s *
  mixed * stack; //struct frame_s *
};

