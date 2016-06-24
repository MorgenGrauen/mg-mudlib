
#ifndef _DEATH_ROOM_H_
#define _DEATH_ROOM_H_


// x muss ein Spielerobjekt sein
#define IS_IN_DEATH_ROOM(x) (environment(x)&&object_name(environment(x))==("/room/death/virtual/death_room_"+getuid(x)))

#endif
