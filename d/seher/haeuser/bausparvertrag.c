/*
 *  bausparvertrag.c -- Wir geben unsern Sehern ein Zuhause...
 *
 *  Grundversion von Jof, weiterbearbeitet von Wargon.
 *
 * $Date: 1994/12/17 15:56:11 $
 * $Revision: 1.4 $
 * $Log: bausparvertrag.c,v $
 * Revision 1.4  1994/12/17  15:56:11  Wargon
 * Etwas mehr Sicherheit gegen Zugriffe von aussen.
 *
 * Revision 1.3  1994/10/21  09:52:03  Wargon
 * zerreissen eingebaut.
 *
 * Revision 1.2  1994/10/09  21:00:59  Wargon
 * Laufzeit etc. ausgebaut, da das ueber den Block abgewickelt
 * wird.
 * Funktion Einzahlung() eingebaut (wird von der Bank aufgerufen).
 *
 * Revision 1.1  1994/10/09  20:49:28  Wargon
 * Initial revision
 *
 */
#pragma strong_types,rtt_checks

#include "haus.h"
#include <properties.h>
#include <moving.h>

inherit "/std/thing";

protected void create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT,"Ein Bausparvertrag");
  SetProp(P_NAME,"Bausparvertrag");
  Set(P_AUTOLOADOBJ, ({0, V_EP}) );
  Set(P_AUTOLOADOBJ, SECURED, F_MODE); // Sicher ist sicher...
  SetProp(P_NODROP,1);
  SetProp(P_NEVERDROP,1);
  SetProp(P_ARTICLE,1);
  SetProp(P_WEIGHT, 0);
  SetProp(P_GENDER,1);
  
  AddId( ({"bausparvertrag", "vertrag", "sehe\rvertrag"}) );

  AddCmd("lies","lesen");
  AddCmd("unterschreibe|unterschreib&\nimp", 0,
      "Hier kannst Du den Vertrag nicht unterschreiben!\n");
  AddCmd("zerreiss|zerreisse&@ID", function int (string str) {
      object blk = present_clone(HAEUSERPFAD+"block", this_player());
      write("Du zerreisst Deinen Vertrag.\n");
      write_file(HAEUSERPFAD+"log/BANK.LOG", sprintf(
          "%s - Vertrag: %s zerriss ihn.\n", 
          dtime(time()), getuid(this_player())));
      if (objectp(blk)) 
        blk->remove(1);
      remove(1);
      return 1;
    }, "Was willst Du zerreissen?");
}

string _query_long()
{
  int req_points = (Query(P_AUTOLOADOBJ)[1] & V_FAST) ? SUMME_S : SUMME_L;
  
  if (Query(P_AUTOLOADOBJ)[1] & V_RAUM)
    req_points = req_points * 4/10;

  return break_string(sprintf(
    "Dies ist ein Bausparvertrag. Du hast bisher %d (von %d) Punkten "
    "eingezahlt.\n",
      QueryProp(P_AUTOLOADOBJ)[0], req_points));
}

#if 0
int query_prevent_shadow(object ob)
{
  HLOG("SHADOW", sprintf("%s, von %O im Vertrag.\n", dtime(time())[5..], ob));
  return 1;
}
#endif

int lesen(string str)
{
  if (id(str))
  {
    if (Query(P_AUTOLOADOBJ)[1] & V_RAUM)
      this_player()->More(PATH+"txt/vertrag_raum.txt",1);
    else
      this_player()->More(PATH+"txt/vertrag.txt",1);
    return 1;
  }
  return 0;
}

int Einzahlung()
{
  int *al;
  object env, po;

  if (old_explode(object_name(po=previous_object()),"#")[0] != PATH+"sb_einzahlung") {
    env = environment();
    HLOG( (env ? getuid(env) : "BANK.LOG"), sprintf("Vertrag: Einzahlung von aussen! TI = %O, PO = %O\n", this_interactive(), previous_object()));
  }
  al = Query(P_AUTOLOADOBJ);
  return Set(P_AUTOLOADOBJ, ({ al[0]+RATENHOEHE, al[1] }) )[0];
}

void Sign(int flag)
{
  Set(P_AUTOLOADOBJ, ({ Query(P_AUTOLOADOBJ)[0], flag }) );
}

varargs public int move(object|string dest, int meth)
{
  object penv;
  int ret;

  penv = environment();
  ret = ::move(dest, meth);

  if (ret == 1 && (penv || (object_name(previous_object()) != PATH+"sb_antrag" &&
			    object_name(previous_object()) != (stringp(dest) ? dest : object_name(dest)))))
    HLOG("MOVING", sprintf("Vertrag: von %O -> %O mit %O (%O)\n", environment(), dest, previous_object(), getuid(previous_object())));
  return ret;
}

mixed _set_autoloadobj(mixed al)
{
  // Loggen, wenn nicht vom Spielerobjekt selber via den
  // Autoloader-Mechanismus gesetzt.
  if (previous_object(1)
      && strstr(load_name(previous_object(1)),"/std/shells") != 0)
    HLOG("SET_AL",sprintf("Vertrag: TP = %O, PO = %O, ENV = %O\n",
          this_player(), previous_object(), environment()));

  /* Kompatibilitaet zu alten Vertraegen */
  if (pointerp(al) && !(al[1] & V_EP)) {
    al[0] *= 4;
    al[1] |= V_EP;
    call_out("notifyChange", 0, this_player());
  }
  return Set(P_AUTOLOADOBJ, al);
}

void notifyChange(object wem)
{
  tell_object(wem,
       "\nNACHRICHT VON DER SEHERBANK:\n"
      +"Die Vertragsmodalitaeten haben sich geaendert! Bitte lesen Sie den\n"
      +"Vertrag noch einmal genau durch (insbes. Art. 4)!\n"
      +"Statt Geld haben Sie jetzt Erfahrungspunkte zu zahlen!\n\n" );
}

