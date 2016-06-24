// MorgenGrauen MUDlib
//
// telnetneg-structs.c -- Structs fuer die Telnet Option handler
//
#pragma strict_types,save_types
#pragma range_check
#pragma no_clone
#pragma no_shadow
#pragma pedantic

struct to_state_s {
  int localside;      // wish for the local side (MUD)
  int remoteside;     // wish for the remote side (CLIENT)
  int *sbdata;        // last SB data sent/received by us
};

struct telopt_s {
  int option;
   // Receivehandler, wird gerufen, wenn wir vom Client irgendwas bzgl. dieser
   // Telnet Option empfangen. Wenn gesetzt, darf der Client die Option
   // einschalten.
  closure remotehandler;
  // Wird gerufen, wenn die Option auf unserer Seite eingeschaltet wurde.
  // Wenn gesetzt, soll versucht werden, die Option auf Mudseite
  // einzuschalten
  closure localhandler;
  // Die Wuensche _waehrend_ einer Verhandlung (bzw. gesendete (lo_wishes) und
  // empfangene (re_wishes) SB-Daten auch ausserhalb von Verhandlungen).
  struct to_state_s lo_wishes;    // our wishes (sent by us)
  struct to_state_s re_wishes;    // remote wishes (received by us)
  // currently effective/active state
  struct to_state_s state;
  // data used by the handlers - NOT USED BY this program!
  mixed data;
};
/* explanations:
   telopt_s->lo_wishes->localside: the state we want to be in  (WILL/WONT)
   telopt_s->lo_wishes->remoteside: the state we want the other side to
                                    be in  (DO/DONT)
   telopt_s->re_wishes->localside: the state the other side wants US to be in
                                   (DO/DONT)
   telopt_s->re_wishes->remoteside: the state the other side wants to be in
                                    (WILL/WONT)
   telopt_s->state:   the currently effective state of the option on the two
                      sides.
   */

