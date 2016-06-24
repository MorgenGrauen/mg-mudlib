// MorgenGrauen MUDlib
//
// living/comm.c -- communiction module for livings
//
// $Id$

#pragma strong_types,save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

#include <defines.h>
#include <living/comm.h>

void create_super()
{
  set_next_reset(-1);
}

protected string comm_guess_action() {
  string cmd;
  string action = query_verb();
  // Die Aktionen sind intern in der Regel nach den haeufigsten Kommandoverben
  // dieser Aktion benannt. Bei einigen Aktionen sind mehrere Kommandoverben
  // ueblich, die sollen hier noch abgehandelt werden.
  switch(action) {
    case "nehme":
      // MA_TAKE == nimm
      action = MA_TAKE;
      break;

    case "norden":
    case "nordosten":
    case "osten":
    case "suedosten":
    case "sueden":
    case "suedwesten":
    case "westen":
    case "nordwesten":
    case "oben":
    case "unten":
    case "betrete":
    case "verlasse":
    case "teleport":
    case "teleportiere":
      action = MA_MOVE;
      break;

    case "unt":
      action = MA_LOOK;
      break;

    case "wirf":
      if (strstr(query_command(), " weg") > -1)
        action = MA_PUT;
      break;

    case "stecke":
      cmd = query_command();
      if (strstr(cmd, " weg") > -1)
        action = MA_UNWIELD;
      else if (strstr(cmd," in ") > -1)
        action = MA_PUT;
      break;

    case "ziehe":
      cmd = query_command();
      if (strstr(cmd, " an") > -1)
        action = MA_WEAR;
      else if (strstr(cmd, " aus") > -1)
        action = MA_UNWEAR;
      break;

    case "esse":
    case "friss":
      action = MA_EAT;
      break;

    case "saufe":
      action = MA_DRINK;
      break;

    case "hoere":
      //MA_LISTEN == lausche
      action = MA_LISTEN;
      break;
    case "lese":
      action = MA_READ;
      break;

    case ":":
    case ";":
      action = MA_EMOTE;
      break;

    case "zerbreche":
    case "zerstoere":
    case "verbrenne":
    case "entsorge":
      action = MA_REMOVE;
      break;
  }
  return action;
}

protected int comm_guess_message_type(string action, mixed origin) {
  // everything not mentioned in the switch becomes MT_LOOK.
  switch(action) {
    case MA_FIGHT:
      // Kampf kann man meisten sowohl sehen als auch hoeren.
      return MT_LOOK | MT_LISTEN;
    case MA_LISTEN:
    case MA_SAY:
      return MT_LISTEN;
    case MA_FEEL:
      return MT_FEEL;
    case MA_SMELL:
      return MT_SMELL;
    case MA_CHANNEL:
      return MT_COMM | MT_FAR;
    case MA_EMOTE:
      if (objectp(origin)
          && environment(origin) == environment())
        return MT_COMM;
      else
        return MT_COMM | MT_FAR;
    case MA_SHOUT:
      return MT_LISTEN | MT_FAR;
  }
  // die meisten Aktionen sind zumindest sichtbar...
  return MT_LOOK;
}

