// (c) by Padreic (Padreic@mg.mud.de)

/* 12 Juli 1998, Padreic
 *
 * Dieser Master dient zum einsparen von call_out Ketten bei Laufnpcs.
 * Anmelden bei diesem Master geschieht ueber die Funktion RegisterWalker().
 * varargs void RegisterWalker(int time, int random, closure walk_closure)
 *   time - in welchen Abstaenden soll das Objekt bewegt werden
 *   rand - dieser Wert wird als random immer beim bewegen zu time addiert
 *   walk_closure - die Closure die immer aufgerufen werden muss, wenn dieser
 *                  Parameter weggelassen wird, wird statdessen die Funktion
 *                  Walk() im NPC aufgerufen.
 *
 * Abgemeldet wird der NPC sobald die Closure bzw. die Walk-Funktion 0
 * returned. Bei allen Werten !=0 bleibt der NPC aktiv.
 *
 * Hinweis: Der NPC muss sich mind. alle 180 sek. bewegen (time+random),
 *          ansonsten kann dieser Master nicht verwendet werden.
 */

#pragma strong_types,rtt_checks
#pragma no_clone,no_inherit

#include "/p/service/padreic/mnpc/mnpc.h"

#define MAX_DELAY_HBS (MAX_MASTER_TIME/__HEART_BEAT_INTERVAL__)
#define DEFAULT_WALK_DELAY MAX_MASTER_TIME   /* ist der billigste Wert :) */

/* Ab welcher Rest-Tickmenge wird die Verarbeitung von Walkers unterbrochen */
#define MAX_JOB_COST    200000

// Funktionen zum vereinfachten Zugriff auf die Komprimierten Daten
// im Walkerarray
#define TIME(t)        (t & 0x00ff)                   /* 8 Bit = 256 */
#define RANDOM(r)     ((r & 0xff00) >> 8)             /* 8 Bit = 256 */
#define WERT(t, r)    ((t & 0x00ff)+((r << 8) & 0xff00))   /* 16 Bit */

// Indizes fuer clients
#define WALK_CLOSURE  0
#define WALK_DELAY    1
#define WALK_TIMESLOT 2

nosave int counter;    // Markiert aktuellen Zeitslot im Array walker
nosave < < closure >* >* walker;
// Mapping mit allen registrierten MNPC (als Objekte) als Key und deren
// walk_closure (1. Wert), Zeitdaten (2. wert) und dem aktuellen
// Zeitslot in <walker> (3. Wert).
nosave mapping clients = m_allocate(0,3);

public int Registration();

protected void create()
{
  walker=map(allocate(MAX_DELAY_HBS+1), #'allocate);
}

#define ERROR(x) raise_error(sprintf(x, previous_object()));

// Am besten nicht direkt sondern nur ueber einen Standardnpc benutzen.
public varargs void RegisterWalker(int time, int rand, closure walk_closure)
{
  // pruefen ob die Paramter zulaessig sind...
  if (time<0) ERROR("negative time to RegisterWalker() from %O.\n");
  if (rand<0) ERROR("negative random to RegisterWalker() from %O.\n");
  // das max. Delay darf max. MAX_MASTER_TIME-1 sein, sonst landet der MNPC im
  // HB evtl. wieder in dem Slot, der gerade bearbeitet wird und der direkt
  // danach komplett genullt wird...
  if ((time+rand) >= (MAX_MASTER_TIME))
    ERROR("Too long delaytime from %s to RegisterWalker().\n");

  if (Registration())
    raise_error(sprintf("Mehrfachanmeldung nicht erlaubt. Objekt: %O\n",
        previous_object()));

  int wert=WERT(time, rand);
  if (!wert && !rand) wert=DEFAULT_WALK_DELAY;

  closure func = walk_closure;
  if (!closurep(func))
  {
    func=symbol_function("CheckWalk", previous_object());
    if (!func)
      raise_error("RegisterWalker() call from Object without Walk() function.\n");
  }
  else
  {
    // Closures auf fremde lfuns fuehren zu Inkonsistenzen/Fehlers und man
    // kann sie auch nicht wieder abmelden. Daher abfangen.
    if (get_type_info(func, 2) != previous_object())
      raise_error(sprintf("Anmeldung von Closures auf fremde lfuns ist nicht "
                  "erlaubt. Closure: %O\n",func));
  }
  // Erster Client? -> HB einschalten.
  if (!sizeof(clients)) {
    set_heart_beat(1);
  }
  int next=counter;
  //min. 1 Heartbeat delay erzwingen, ab jetzt in Heartbeats
  next += max(1, (time+random(rand))/__HEART_BEAT_INTERVAL__);
  if (next>MAX_DELAY_HBS) next-=MAX_DELAY_HBS;
  walker[next]+=({ func });
  clients += ([ get_type_info(func, 2): func; wert; next ]);
}

// Aufruf nach Moeglichkeit bitte vermeiden, da recht aufwendig. Meist ist
// es leicht im NPC "sauber Buch zu fuehren" und dann ggf. aus Walk() 
// 0 zu returnen.
public void RemoveWalker()
{
  if (!member(clients, previous_object()))
    return;
  // Naechster Zeitslot und index in dem Slotarrays ermitteln
  int next = clients[previous_object(), WALK_TIMESLOT];
  closure func = clients[previous_object(), WALK_CLOSURE];
  int idx = member(walker[next], func);
  // Durch 0 ersetzen. Aber wir koennten gerade im heart_beat stecken... In
  // dem Fall den Eintrag ersetzen durch was, was beim Abarbeiten ausgetragen
  // wird.
  if (next==counter)
    walker[next][idx]=function () {return 0;};
  else
    walker[next][idx]=0;

  // 0-Eintraege entfernen, aber nur, wenn wir gerade nicht evtl. in dem HB
  // stecken und den Slot abarbeiten.
  if (next!=counter)
    walker[next]-=({ 0 });
  // und noch die Stammdaten entfernen
  m_delete(clients, previous_object());
}

public int Registration()
{
  return member(clients, previous_object());
}

void heart_beat()
{
   int i = sizeof(walker[counter]);
   if (i)
   {
     for (i--;i>=0;i--)
     {
       if (get_eval_cost() < MAX_JOB_COST)
       {
         // nicht abgefertigte NPCs im naechsten heart_beat ausfuehren
         walker[counter]=walker[counter][0..i];
         return;
       }
       else
       {
         closure func = walker[counter][i];
         if (func)
         {
           object mnpc = get_type_info(func, 2);
           mixed res;
           if (!catch(res=funcall(func);publish)
               && intp(res) && res)
           {
             // Es gab keinen Fehler und das Objekt will weiterlaufen.
             // Naechsten Zeitslot bestimmen und dort die closure eintragen.
             int delay = clients[mnpc, WALK_DELAY];
             // das delay muss min. 1 sein, sonst tragen wir den MNPC in
             // diesen aktuellen zeitslot wieder ein (ja, Magier tun solche
             // Dinge), der ja nach Abarbeiten des Slots genullt wird.
             int next = counter
                        + max(1, (TIME(delay) + random(RANDOM(delay)))
                                 /__HEART_BEAT_INTERVAL__);
             if (next > MAX_DELAY_HBS)
               next -= MAX_DELAY_HBS;
             walker[next] += ({ func });
             clients[mnpc, WALK_TIMESLOT] = next;
           }
           else // Fehler oder Objekt will abschalten
             m_delete(clients, mnpc);
         }
         // else: Falls die closure nicht mehr existiert, existiert das Objekt
         // nicht mehr, dann ist es ohnehin auch schon aus clients raus und es
         // muss nix gemacht werden.
       }
     }
     walker[counter]=({}); // fertiger Zeitslot, komplett leeren
   }
   // Wrap-around am Ende des Arrays.
   if (counter == MAX_DELAY_HBS)
     counter=0;
   else
     counter++;

   // wenn keine Clients mehr uebrig, kann pausiert werden. Es kann sein, dass
   // noch Dummy-Eintraege in walker enthalten sind, die stoeren nicht, bis
   // wir das naechste Mal drueber laufen.
   if (!sizeof(clients)) {
     set_heart_beat(0);
   }
}

// im reset zur Sicherheit mal den heart_beat ggf. einschalten.
// dient zu einem wieder anwerfen im Falle eines Fehlers im heart_beat()
// wenn doch nix gemacht werden musss, schaltet sich der HB eh wieder aus.
void reset()
{
  // set_heart_beat() wird als query_heart_beat() 'missbraucht' und daher muss
  // im else der HB in jedem Fall auch wieder eingeschaltet werden.
  if (set_heart_beat(0)<=0)
  {
    if (sizeof(clients) > 0)
    {
       write_file(object_name()+".err", sprintf(
         "%s: Fehler im heart_beat(). %d aktive Prozesse.\n",
          dtime(time()), sizeof(clients)));
      set_heart_beat(1);
    }
  }
  else
    set_heart_beat(1);
}

// Bemerkung: damit kann jeder die Closures ermitteln und dann selber rufen.
mixed *WalkerList() // nur fuer Debugzwecke
{  return ({ clients, walker, counter });  }
