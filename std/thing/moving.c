// MorgenGrauen MUDlib
//
// thing/moving.c -- object moving
//
// $Id: moving.c 8892 2014-08-04 19:48:28Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone

#include <defines.h>
#include <moving.h>
#include <properties.h>
#define NEED_PROTOTYPES
#include <thing/properties.h>

// Das Objekt bewegen.
// Rueckgabe 1 ist Erfolg, <=0 ist Fehler

// a) P_NODROP/P_NOGET-Behandlung.
// b) zum Ueberschreiben
protected int PreventMove(object dest, object oldenv, int method) {
  int tmp;

  // M_NOCHECK? -> Bewegung eh erlaubt (und Rueckgabewert wuerde ignoriert)
  if ((method&M_NOCHECK)) {
    // Bei M_NOCHECK zwar Prevent* aufrufen, aber das Resultat ignorieren
    if (oldenv) ({int})oldenv->PreventLeave(this_object(),dest);
    ({int})dest->PreventInsert(this_object());

    return 0; // das wars, rest ist egal.
  }

  // P_NODROP verhindert weggeben
  if ((method & (M_PUT|M_GIVE)) && QueryProp(P_NODROP))
      return ME_CANT_BE_DROPPED;

  // P_NOGET verhindert nehmen
  if ((method & (M_GET|M_GIVE)) && QueryProp(P_NOGET))
      return ME_CANT_BE_TAKEN;

  // Gewicht ermitteln
  if ( !(tmp = ({int})QueryProp(P_TOTAL_WEIGHT)) )
      tmp = ({int})QueryProp(P_WEIGHT);

  // Ist das Objekt nicht zu schwer?
  if ( (tmp = ({int})dest->MayAddWeight(tmp)) < 0) {
      if ( tmp == -2 ) return ME_TOO_HEAVY_FOR_ENV;
      return ME_TOO_HEAVY;
  }

  // Ist das Zielobjekt schon voll?
  if ( !({int})dest->MayAddObject(this_object()) )
      return TOO_MANY_OBJECTS;

  // Darf hinausbewegt werden?
  if ( oldenv && ({int})oldenv->PreventLeave(this_object(), dest) )
      return ME_CANT_LEAVE_ENV;

  // Darf hineinbewegt werden?
  if ( ({int})dest->PreventInsert(this_object()) )
      return ME_CANT_BE_INSERTED;

  return(0);
}

// zum Ueberschreiben...
protected void NotifyMove(object dest, object oldenv, int method) {
}

protected object move_norm_dest(object|string dest)
{
  if (objectp(dest))
    return dest;

  int parawelt;
  // Wenn dieses Objekt gerade in einem Lebewesen ist, dann soll das Ziel der
  // Bewegung implizit in dieselbe Para-Welt erfolgen wie das Living.
  // parawelt ist die Ziel-Parallelweltnummer
  if (!environment() || !living(environment()) ||
      !intp(parawelt =({int})environment()->Query(P_PARA)))
    parawelt=0;

  // Wenn das Objekt von einem in der Parawelt befindlichen Spieler
  // oder NPC bewegt wird, sollte es auch wieder in der Parawelt landen.
  // Um Rechenzeit zu sparen, wird angenommen, dass bei Bewegungen in
  // das Inv oder Env des Spielers 'dest' als Objekt uebergeben wird,
  // wohingegen bei Bewegungen in Nachbarraeume (die eigentlich nur
  // interessant sind) 'dest' als Filename angegeben wird.
  if (parawelt && !environment(dest))
  {
      // Falls der Zielraum nicht schon explizit in der Parallelwelt ist,
      // neuen Zielraum suchen. Aber nur, wenn das Ziel kein Clone ist. Sonst
      // buggt, wenn man versucht, nach raum#42^para zu bewegen.
      if (!IS_PARA(dest) && strrstr(dest,"#")==-1)
      {
        string fn=dest+"^"+parawelt;
        string vc;
        int valid;
        // Der Parawelt-Raum wird nur zum Ziel, wenn er a) existiert
        // und b) auch von Spielern betreten werden darf. Letzteres
        // Kriterium kann nur mit im Objekt gesetzter Property
        // P_TESTPLAYER umgangen werden.
          if ( (find_object(fn) || ((file_size(fn+".c")>0||
                  (file_size(vc=implode(explode(fn,"/")[0..<2],"/")+
                  "/virtual_compiler.c")>0 &&
                  !catch(valid=({int})vc->QueryValidObject(fn);publish)
                  && valid>0)) &&
                  !catch(load_object(fn);publish))) &&
              (!({int})fn->QueryProp(P_NO_PLAYERS) || ({int|string})QueryProp(P_TESTPLAYER)) )
           dest = fn;
      }
  }
  // dest auf Objekt normieren.
  if (stringp(dest))
      dest = load_object(dest);
  return dest;
}

public varargs int move( object|string dest, int method )
{
  // Jetzige Umgebung merken
  object oldenv = environment();

  dest = move_norm_dest(dest);

  // testen, ob das Objekt bewegt werden will
  int valid = PreventMove(dest, oldenv, method);
  if (valid)
  {
      // auf gueltigen Fehler pruefen, wer weiss, was Magier da evtl.
      // versehentliche zurueckgeben.
      if (VALID_MOVE_ERROR(valid))
        return(valid);
      else
        return ME_DONT_WANT_TO_BE_MOVED;
  }

  // Sensitive Objekte muessen entfernt werden
  mixed *sens = QueryProp(P_SENSITIVE);
  if (sens && environment())
  {
    ({void})environment()->RemoveSensitiveObject( this_object() );
    if (!objectp(ME))
      return ME_WAS_DESTRUCTED;
  }
  // Bewegen
  move_object(ME, dest);

  //falls (sich) das objekt im init() zerstoert (wurde). (Die u. stehenden
  //Funktionsaufrufe werden dann vom Driver eh groesstenteils ignoriert.)
  if (!objectp(this_object())) return(ME_WAS_DESTRUCTED);

  // Objekt informieren. ;-)
  NotifyMove(environment(), oldenv, method);

  // Alte Umgebung informieren
  if (oldenv) ({void})oldenv->NotifyLeave(this_object(), dest);

  // Wenn das Objekt eine Umgebung hat, selbige informieren
  if (environment()) {
    if (sens)
    {
      ({void})environment()->InsertSensitiveObject(this_object(),sens);
      if (!objectp(ME)) return ME_WAS_DESTRUCTED;
    }
    ({void})environment()->NotifyInsert(this_object(), oldenv);
  }
  //wurde das Objekt vielleicht noch zerstoert?
  if (!objectp(ME)) return(ME_WAS_DESTRUCTED);

  //scheint wohl alles ok zu sein.
  return MOVE_OK;
}

// Das Objekt zerstoeren
public varargs int remove(int silent)
{
    if (environment() ) {
        if(QueryProp(P_SENSITIVE))
                ({void})environment()->RemoveSensitiveObject(this_object());
        ({void})environment()->NotifyRemove(this_object());
    }
    if (objectp(this_object()))
        destruct(this_object());
    return 1;
}

public string NotifyDestruct(object caller) {
  // Lichtsystem mit der aenderung versorgen. :-/
  foreach(object env : all_environment() || ({})) {
      // Ja. Man ruft die _set_xxx()-Funktionen eigentlich nicht direkt auf.
      // Aber das Lichtsystem ist schon *so* rechenintensiv und gerade der
      // P_LAST_CONTENT_CHANGE-Cache wird *so* oft benoetigt, dass es mir
      // da um jedes bisschen Rechenzeit geht.
      // Der Zweck heiligt ja bekanntlich die Mittel. ;-)
      //
      // Tiamak
      ({int})env->_set_last_content_change();
  }
  return 0;
}

