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

#define MAX_DELAYTIME       90   /* max. delay ist 2*MAX_DELAYTIME */
#define DEFAULT_WALK_DELAY 180   /* ist der billigste Wert :) */
#define MAX_JOB_COST    200000   /* Wieviel Zeit darf ein NPC max. nutzen */

// Funktionen zum vereinfachten Zugriff auf die Komprimierten Daten
// im Walkerarray
#define TIME(t)        (t & 0x00ff)                   /* 8 Bit = 256 */
#define RANDOM(r)     ((r & 0xff00) >> 8)             /* 8 Bit = 256 */
#define WERT(t, r)    ((t & 0x00ff)+((r << 8) & 0xff00))   /* 16 Bit */

// Indizes fuer walker
#define WALK_DELAY   0
#define WALK_CLOSURE 1

static int counter;    // zur Orientierung im walker-array
static int num_walker; // anzahl der walker im array
//static mixed *walker;  // ({ ..., ({ ..., ({ wert, closure }), ...}), ...})
nosave < < <int|closure>* >* >* walker;
// Mapping mit allen registrierten MNPC (als Objekte) als Key und deren
// Zeitdaten (1. wert) und deren walk_closure (2. Wert).
nosave mapping clients = m_allocate(0,2);

int Registration();

protected void create()
{
  walker=map(allocate(MAX_DELAYTIME+1), #'allocate);
  enable_commands(); // ohne das, kein heart_beat()
}

#define ERROR(x) raise_error(sprintf(x, previous_object()));

// Man muss selbst darauf aufpassen, das sich ein NPC immer nur einmal
// anmeldet, da sonst auch mehrere Paralelle Walk-Ketten laufen!!!
// Am besten nie direkt sondern nur ueber einen Standardnpc benutzen.
varargs void RegisterWalker(int time, int rand, closure walk_closure)
{
  int wert, next;
  closure func;
  
  // pruefen ob die Paramter zulaessig sind...
  if (time<0) ERROR("negative time to RegisterWalker() from %O.\n");
  if (rand<0) ERROR("negative random to RegisterWalker() from %O.\n");
  if ((time+rand) > (2*MAX_DELAYTIME)) 
    ERROR("Too long delaytime from %s to RegisterWalker().\n");

  if (Registration())
    raise_error(sprintf("Mehrfachanmeldung nicht erlaubt. Objekt: %O\n",
        previous_object()));

  wert=WERT(time, rand);
  if (!wert && !rand) wert=DEFAULT_WALK_DELAY;
  if (walk_closure)
    func=walk_closure;
  else {
    func=symbol_function("Walk", previous_object());
    if (!func)
      raise_error("RegisterWalker() call from Object without Walk() function.\n");
  }
  if (!num_walker) {
    set_heart_beat(1);
  }
  next=counter;
  next+=(time+random(rand))/2;
  if (next>MAX_DELAYTIME) next-=MAX_DELAYTIME;
  walker[next]+=({ ({ wert, func }) });
  clients += ([ get_type_info(func, 2): wert; func ]);
  num_walker++;
}

int dummy_walk()  // liefert immer 0 fuer abbrechen...
{   return 0;  }

// Aufruf nach Moeglichkeit bitte vermeiden, da recht aufwendig. Meist ist
// es leicht im NPC "sauber Buch zu fuehren" und dann ggf. aus Walk() 
// 0 zu returnen.
void RemoveWalker()
{
  if (!member(clients, previous_object()))
    return;

  for (int i=MAX_DELAYTIME; i>=0; i--) {
    for (int j=sizeof(walker[i])-1; j>=0; j--)
    {
      if (get_type_info(walker[i][j][WALK_CLOSURE], 2)==previous_object())
      {
        if (i==counter) // koennte gerade im heart_beat stecken...
          walker[i][j]=({ 0, #'dummy_walk });
        else
          walker[i][j]=0;
        num_walker--;
      }
    }
    if (i!=counter) // koennte gerade im heart_beat stecken...
      walker[i]-=({ 0 });
  }
  m_delete(clients, previous_object());
}

int Registration()
{
  return member(clients, previous_object());
}

void heart_beat()
{
   int i;
   if (num_walker && i=sizeof(walker[counter])) {
     int tmp;
     num_walker-=i;
     for (i--;i>=0;i--) {
       if (get_eval_cost() < MAX_JOB_COST) {
         // nicht abgefertigte NPCs im naechsten heart_beat ausfuehren
         walker[counter]=walker[counter][0..i];
         num_walker+=i+1;
         return;
       }
       else {
         if (walker[counter][i][1] &&
             !catch(tmp=(int)funcall(walker[counter][i][WALK_CLOSURE]))
             && tmp)
         {
           tmp=counter+(TIME(walker[counter][i][WALK_DELAY])
              +random(RANDOM(walker[counter][i][WALK_DELAY])))/2;
           if (tmp>MAX_DELAYTIME) tmp-=MAX_DELAYTIME;
           walker[tmp]+=({ walker[counter][i] });
           num_walker++;
         }
       }
     }
     walker[counter]=({}); // komplett leeren
   }
   if (counter == MAX_DELAYTIME)
     counter=0;
   else counter++;
   if (!num_walker) {
     set_heart_beat(0);
   }
}

void reset()
// kostet maximal einen unnoetigen heart_beat() pro reset -> vertretbar
// dient zu einem wieder anwerfen im Falle eines Fehlers im heart_beat()
{
  if (set_heart_beat(0)<=0) {
    int i;
    num_walker=0; // neu berechnen...
    if (!sizeof(walker)) return;
    for (i=MAX_DELAYTIME; i>=0; i--)
      num_walker+=sizeof(walker[i]);
    if (num_walker>0) {
       write_file(object_name()+".err", sprintf(
         "%s: Fehler im heart_beat(). %d aktive Prozesse.\n",
          dtime(time()), num_walker));
       enable_commands();
       set_heart_beat(1);
    }
  }
  else set_heart_beat(1);
}

mixed *WalkerList() // nur fuer Debugzwecke
{  return ({ num_walker, walker, counter });  }
