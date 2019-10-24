// Daemon zum automatisierten Erstellen von Karten.
// Soll spaeter mal wichtige Wege berechnen koennen fuer Anfaenger.
//
//
// Wer mir ohne triftigen Grund daran herumpfuscht, der wird standesrechtlich
// mit faulen Eiern beworfen. ]:->
//
// Tiamak

#pragma strong_types,save_types,rtt_checks
#pragma no_clone,no_inherit,no_shadow
#pragma pedantic,range_check,warn_deprecated
#pragma warn_empty_casts,warn_missing_return,warn_function_inconsistent

#include <strings.h>
#include <wizlevels.h>
#include <player.h>
#include <properties.h>
#include <rooms.h>
#include <moving.h>

#define LOGNAME "PATHD"
#define SAVEFILE "/p/daemon/save/pathd"
#define DUMPFILE "/p/daemon/save/pathd-dump"
//#define DEBUG

// 10 verschiedene Leute werden gebraucht, um eine Verbindung zu verifizieren
#define NEEDED 10
#define TIMEOUT_NEW 1209600  // 14 Tage
#define TIMEOUT_OLD 7776000 // 90 Tage

// Art des Ausgangs
#define NORMAL  0
#define SPECIAL 1
#define COMMAND 2

// Kosten fuer unterschiedliche Wege
#define COST_EXITS ({ 1, 10, 100 })
#define COST_MANY   3
#define COST_FEW    5
#define COST_ONE   10
// Kosten fuer Weltenwechsel (Para -> Normal)
#define COST_WORLDCHANGE 100

#ifdef DEBUG
#define DBG(x) if ( find_player("zesstra") ) \
                   tell_object( find_player("zesstra"), (x)+"\n" );
#define TELL_USER(x) if ( this_player() ) \
                   tell_object( this_player(), (x)+"\n" );
#else
#define DBG(x)
#define TELL_USER(x)
#endif

#define LOG(x) log_file( "PATHD", (x), 100000 );

// Variablen
nosave int time_to_clean_up; // Zeit fuer naechstes Cleanup der Daten

mapping pathmap;
/* Prefix : ([ <submap> ]),
   <submap> ist ein Mapping von Raeumen mit einem Array von Verbindungen:
   ([<room>: ({ conn1, conn2, conn3, ...}), ...])
   Jede Verbindung ist ein Array mit folgenden Indizes:
   */
#define CONN_DEST     0   // Zielraum
#define CONN_CMD      1   // Kommando
#define CONN_USERS    2   // Benutzer: ({spieler, seher})
#define CONN_EXITTYPE 3   // Ausgangsart (s.o.)
#define CONN_TIMES    4   // Zeit letzter Benutzung: ({spieler,seher})
#define CONN_PARA     5   // Parawelt
#define CONN_FLAGS    6   // Flags fuer die Verbindung (z.B. permanent)
// Indizes in CONN_USERS. Beide Eintraege sind entweder gehashte UIDs ODER -1,
// wenn die Verbindung von mehr als NEEDED unterschiedlichen gegangen wurde.
#define CONN_U_PLAYERS 0  // Spieler-UIDs
#define CONN_U_SEERS   1  // Seher-UIDS
// Indizes von CONN_TIMES. Zeitstempel der letzten Benutzung...
#define CONN_T_PLAYERS CONN_U_PLAYERS  // ... durch Spieler
#define CONN_T_SEERS   CONN_U_SEERS  // ... durch Seher

// Konstanten fuer CONN_FLAGS
#define CFLAG_PERMANENT   0x1  // Verbindung nicht expiren
#define CFLAG_DONTUSE     0x2  // Verbindung nicht im Routing nutzen
#define CFLAG_DUPLICATE   0x4  // Kommando hat mehrere Verbindungen

// laufende Pfadsuchen
nosave mapping searchlist = ([]);
/*
   ([ <uuid> : SL_VISITED; SL_DESTINATION;... ... ])
 */
// Indizes in <data>:
#define SL_START       0
#define SL_DESTINATION 1  // Zielraum
#define SL_VISITED     2  // abgelaufene/gepruefte Raeume
#define SL_CANDIDATES  3  // moegliche Ziele
#define SL_WIZLEVEL    4  // Wizlevel
#define SL_PARA        5  // Parawelt
#define SL_TODO        6  // noch zu pruefende Raume (SL_CANDIDATES-SL_VISITED)
#define SL_CALLBACK    7  // Closure fuer Callback nach Routenberechnung
// SL_CANDIDATES: ([<raum>: ({verbindungsdaten}), ...])
// Indizes im Array <verbindungsdaten>:
#define SLT_COSTS     0   // Kosten fuer Verbindung bis hier
#define SLT_WAY       1   // bisheriger Weg von Start bis hier
#define SLT_COMMANDS  2   // Kommandos fuer Weg von Start bis hier

// cache fuer berechnete Wege
nosave mapping pathcache = ([]);
// Aufbau: ([ "startraum": <dests>, ...])
// <dests>: (["zielraum": <wege>, ...])
// <wege>: ([parawelt: (<struct path_s>) ])

struct path_s {
    int costs;      // Kosten des kompletten Pfades
    string *rooms;  // Liste von durchlaufenen Raeumen
    string *cmds;   // Liste von Kommandos, um Raeume zu durchlaufen
    int ttl;        // time-to-live, Ablaufzeit des Pfads im Cache
    int para;       // Parawelt
    int flags;      // Flags zu dem Pfad
};
// Flags fuer Pfade im Pfadcache
#define PFLAG_PERMANENT CFLAG_PERMANENT
#define PFLAG_DONTUSE CFLAG_DONTUSE


// Prototypes
public void create();
public void add_paths( mixed connections );
public varargs void show_pathmap( string path );
public varargs int show_statistic( int verbose, int silent );
public int change_pathmap( string pfad, mixed *verbindungen );
public mapping get_pathmap();
public void reset();
public varargs int remove( int silent );
public varargs int query_prevent_shadow( object ob );
static int insert_paths( mixed *path );
static mixed *format_paths( mixed buf );
static void _search( string fname, string start );
private string _get_prefix( string path );
protected void cleanup_data( string *names, int idx );
protected void expire_path_cache(string *startrooms);


public void create()
{
    // wir muessen das Savefile schreiben duerfen
    seteuid(getuid());

    if ( !restore_object(SAVEFILE) ) {
        pathmap = ([]);
    }
}


// Wird von Spielern aufgerufen mit den Wegen, die sie gelaufen sind
public void add_paths( mixed connections )
{
    mixed paths;

    // keinen Aufruf per Hand bitte
    if ( !previous_object() || !this_player()
         || this_interactive() && getuid(this_interactive()) != "zesstra"
         || file_size( "/std/shells" +
                       explode( object_name(previous_object()), ":" )[0]
                       + ".c" ) < 1 )
        return;

#if __BOOT_TIME__ < 1357042092
    // alte Spielerobjekte liefern noch ein Array von Strings statt eines
    // Arrays von Arrays.
    connections = map(connections, function mixed (string path)
        {
          // Alle Daten kommen als ein String mit '#' als Trenner an.
          // Muster: Startraum#Zielraum#Verb#Methode der Bewegung#Parawelt
          mixed buf = explode( path, "#" );
          // Falls im Verb auch # vorkam (unwahrscheinlich, aber moeglich):
          buf[2..<3] = ({ implode( buf[2..<3], "#" ) });
          return buf;
        } 
        );
#endif

    // Daten aufbereiten
    paths = map( connections, #'format_paths/*'*/ ) - ({0});
    // Neue Raeume eintragen
    filter( paths, #'insert_paths/*'*/ );
}

public mixed get_path_from_cache(string from, string to, int para) {
    // wenn im Cache, aus dem Cache liefern.
    mapping targets = pathcache[from];
    if (targets) {
        mapping paths = targets[to];
        if (paths) {
            if (member(paths, para)) {
                struct path_s p = paths[para];
                if (p->ttl > time())
                    return ({ from, to, para, p->costs,
                             copy(p->rooms), copy(p->cmds) });
                else {
                    m_delete(paths, para);
                }
            }
        }
    }
    return 0;
}

// Suchfunktion, um die kuerzeste Verbindung zwischen zwei Raeumen zu finden
public int SearchPath( string from, string to, int para,
                       closure callback)
{
    // Pro UID darf es nur einen Suchauftrag geben.
    string uid=getuid(previous_object());
    if ( member(searchlist,uid) )
        return -1;
    // und mehr als 5 gleichzeitig erstmal auch nicht.
    if (sizeof(searchlist)>4)
        return -2;

    // Anfrage loggen
    LOG(sprintf("%s: Suche %s -> %s (%d) von %s (%s, %s, %s)\n",
          strftime("%y%m%d-%H%M%S"), from, to, para, uid,
          object_name(previous_object()),
          object_name(this_player()) || "NO-PL",
          object_name(this_interactive()) || "NO-TI") );

    // wenn Start- oder Zielraum nicht bekannt sind, kann es auch keine Route
    // geben.
    if (!member(pathmap[_get_prefix(to)],to)
        || !member(pathmap[_get_prefix(from)],from) )
        return -3;

    mixed path = get_path_from_cache(from, to, para);
    if (path) {
        if (callback)
            apply(callback,path); 
        return 2;
    }

    // Datenstrukturerklaerung: s.o.
    mapping targets = m_allocate(1200,3);
    m_add(targets,from,0,({}),({}) );
    searchlist+= ([ uid: from; to; ({});
                     targets;
                     (query_wiz_level(this_interactive())?1:0);
                     para; ({}); callback]);

    // Die eigentliche Suche starten
    _search( uid, from );

    return 1;
}

// Gespeicherte Informationen zu einem Raum oder einem kompletten Gebiet
// abfragen. Gebiete lassen sich aber nur als "Organisationseinheiten" abfragen.
// Dabei werden Gebiete unterhalb von /d/* als /d/region/magiername
// abgespeichert und der Rest unter den ersten beiden Teilpfaden.
public varargs void show_pathmap( string path )
{
    string pre;

    if ( path )
    {
        pre = _get_prefix(path);

        if ( pre == path )
            // Ganzen Teilbaum ausgeben
            printf( "Pathmap[%O]: %O\n", path, pathmap[path] );
        else
            // Nur Infos ueber einen Raum ausgeben
            printf( "Pathmap[%O]: %O\n", path,
                    pathmap[pre] ? pathmap[pre][path] : 0 );
    }
    else
        // Ohne Argument wird die gesamte Map ausgegeben.
        // Klappt aber eher nicht (mehr) wegen Buffer overflow. ;^)
        printf( "Pathmap: %O\n", pathmap );
}


// Statistic zu den gespeicherten Daten anzeigen.
// Mit 'verbose' wird jede Organisationseinheit einzeln aufgelistet, ohne
// wird grob nach Regionen zusammengefasst.
public varargs int show_statistic( int verbose, int silent )
{
    int i, ges;
    string *ind, name;
    mapping m;

    if ( verbose ){ // Stur jeden Eintrag auflisten
        ind = sort_array( m_indices(pathmap), #'</*'*/ );
        for ( i = sizeof(ind); i--; ){
            if ( !silent )
                printf( "%-30s: %4d\n", ind[i], sizeof(pathmap[ind[i]]) );
            ges += sizeof(pathmap[ind[i]]);
        }
    }
    else {          // Regionen zusammenfassen
        ind = m_indices(pathmap);
        m = ([]);

        for ( i = sizeof(ind); i--; ){
            if (strstr(ind[i],"/d/") == 0)
                // /d... jeweils nach zwei Teilpfaden
                name = implode( explode( ind[i], "/" )[0..2], "/" );
            else if ( strstr(ind[i],"/players") == 0)
                // Alle Playerverzeichnisse zusammenfassen ...
                name = "/players";
            else
                // ... den Rest nach einem Teilpfad
                name = implode( explode( ind[i], "/" )[0..1], "/" );

            if ( !m[name] )
                m[name] = sizeof( pathmap[ind[i]] );
            else
                m[name] += sizeof( pathmap[ind[i]] );
        }

        ind = sort_array( m_indices(m), #'</*'*/ );
        for ( i = sizeof(ind); i--; ){
            if ( !silent )
                printf( "%-30s: %4d\n", ind[i], m[ind[i]] );
            ges += m[ind[i]];
        }
    }

    if ( !silent )
        printf( "\nGesamt: %d Raeume.\n", ges );

    return ges;
}


// Manipulieren der internen Pathmap. Nur zum Debuggen und somit
// nur fuer Zesstra erlaubt. Sonst verhunzt mir noch wer meine Daten. ;^)
public int change_pathmap( string pfad, mixed *verbindungen )
{
    if ( !this_interactive() || getuid(this_interactive()) != "zesstra"
         || !previous_object() || getuid(previous_object()) != "zesstra" )
        return -1;

    if ( mappingp(verbindungen) ){
        if ( !pathmap[_get_prefix(pfad)] )
            return 0;

        pathmap[_get_prefix(pfad)] = verbindungen;
    }
    else {
        if ( !pathmap[_get_prefix(pfad)][pfad] )
            return 0;

        pathmap[_get_prefix(pfad)][pfad] = verbindungen;
    }

    return 1;
}

// Die Funktion gibt alle dem pathd bekannten Raeume als Mapping von Arrays
// zurueck. Schlüssel des Mappings sind dabei Gebiete.
public mapping get_rooms()
{
  string *submaps;
  mapping roommap;
  int i;

  roommap=([]);

  submaps=m_indices(pathmap);

  i=sizeof(submaps);

  while (i--)
    if (sizeof(m_indices(pathmap[submaps[i]])))
        roommap[submaps[i]]=m_indices(pathmap[submaps[i]]);

  return roommap;
}

// Daten zu einer Verbindung sammeln und aufbereiten
static mixed *format_paths( mixed buf )
{
    string cmd, uid;
    object ob;
    mapping exits;
    int art;

    // Magier und Testspieler koennen auch in nicht angeschlossene Gebiete
    // und werden deshalb nicht beachtet.
    if ( IS_LEARNER(previous_object(1)) ||
         IS_LEARNER(lower_case( previous_object(1)->Query(P_TESTPLAYER)||"" )) )
        return 0;

    cmd = trim(buf[2],TRIM_BOTH);
#if __BOOT_TIME__ < 1357042092
    // " 0" am Ende des Befehls abschneiden. *grummel*
    if (cmd[<2..]==" 0")
      cmd = cmd[..<3];
#endif

    // Wenn der Zielraum als String angegeben wurde, kann der fuehrende
    // Slash fehlen!
    if ( buf[1][0] != '/' )
        buf[1] = "/" + buf[1];

    // Zum Abfragen der zusaetzlichen Daten brauchen wir das Objekt selber
    catch(ob = load_object(buf[0]);publish);
    if (!objectp(ob))
        return 0;

    // Kleiner Hack - bei Transportern etc. ist 'ob' die nicht initialisierte
    // Blueprint. Jede Abfrage von P_EXITS wuerde nett auf -Debug scrollen.
    // Da P_IDS im create() auf jeden Fall auf ein Array gesetzt wird,
    // missbrauche ich das hier als "Ist initialisiert"-Flag.
    if ( !ob->QueryProp(P_IDS) )
        return 0;

    // Art des Ausgangs feststellen. 
    if ( mappingp(exits = ob->QueryProp(P_EXITS)) && exits[cmd] )
        art = NORMAL;   // "normaler" Ausgang
    else if ( mappingp(exits = ob->QueryProp(P_SPECIAL_EXITS)) && exits[cmd] )
        art = SPECIAL;  // SpecialExit
    else {
        // Kommandos, die einen in einen anderen Raum bringen
        art = to_int(buf[3]);   // Move-Methode

        // Es zaehlen aber nur Bewegungen, die halbwegs "normal" aussehen,
        // d.h. kein M_TPORT, M_NOCHECK und M_GO muss gesetzt sein.
        if ( art & (M_TPORT | M_NOCHECK) || !(art & M_GO) )
            return 0;
        else
            art = COMMAND;
    }

    // Die UID der Spieler/Seher wird verschluesselt.
    // Schliesslich brauchen wir sie nur fuer statistische Zwecke und nicht,
    // um Bewegungsprofile zu erstellen. crypt() reicht fuer diese Zwecke
    // wohl erstmal.
    uid = getuid(previous_object(1));
    uid = crypt( uid, "w3" );

    // Start, Ziel, Verb, Wizlevel(TP), UID(TP), Art des Ausgangs, Parawelt
    return ({ buf[0], buf[1], cmd, query_wiz_level(previous_object(1)) ? 1 : 0,
                  uid, art, to_int(buf[4]) });
}

// Prueft alle Verbindungen im Array <conns>, welche das gleiche Kommando wie
// <conn> haben, ob sie in unterschiedliche Raeume fuehren. Wenn ja, sind sie
// fuer das Routing nicht nutzbar. Ausnahme: Verbindungen, die in
// unterschiedliche Parawelten fuehren.
// Aendert <conns> direkt.
private void check_and_mark_duplicate_conns_by_conn(mixed conns, mixed conn) {
    foreach(mixed c: conns) {
        if ( c[CONN_CMD] == conn[CONN_CMD]
            && c[CONN_DEST] != conn[CONN_DEST]
            && c[CONN_PARA] == conn[CONN_PARA] )
        {
            c[CONN_FLAGS] |= CFLAG_DUPLICATE;
            conn[CONN_FLAGS] |= CFLAG_DUPLICATE;
        }
    }
}

private void check_and_mark_duplicate_conns(mixed conns) {
    foreach(mixed c: conns)
        check_and_mark_duplicate_conns_by_conn(conns, c);
}


// Neue Verbindung in der Datenbank eintragen
static int insert_paths( mixed *path )
{
    string pre = _get_prefix(path[0]);

    // Falls noch gar kein Eintrag existiert, neu initialisieren
    if ( !mappingp(pathmap[pre]) )
        pathmap[pre] = ([]);

    if ( !pointerp(pathmap[pre][path[0]]) )
        pathmap[pre][path[0]] = ({});

    // Aufbau von 'path':
    // ({ Start, Ziel, Verb, Wizlevel(TP), UID(TP), Art des Ausgangs, Para })
    // Aufbau von 'conn' (s. auch ganz oben)
    // ({ Ziel, Verb, ({ Spieler-UIDs, Seher-UIDs }), Art des Ausgangs,
    //   ({ Letztes Betreten durch Spieler, durch Seher }), Parawelt })

    // Alle Verbindungen des Raumes durchgehen
    mixed conns = pathmap[pre][path[0]];
    // Kommandos der Verbindungen sammeln. Wenn es zwei Verbindungen mit dem
    // gleichen Kommando gibt, sind beide unbenutzbar.
    mixed cmds = m_allocate(10);
    int i;
    for ( i = sizeof(conns); i--; )
    {
        mixed conn = conns[i];
        m_add(cmds,conn[CONN_CMD]); // Kommandos aller Conns merken
        // Wenn Zielraum, Verb und Parawelt passen ...
        if ( conn[CONN_DEST] == path[1] && conn[CONN_CMD] == path[2]
             && conn[CONN_PARA] == path[6] )
        {
            // Wenn schon genug Leute diese Verbindung genutzt haben, einfach
            // nur die Zeit der letzten Benutzung aktualisieren.
            int index = path[3] ? CONN_U_SEERS : CONN_U_PLAYERS; // Seher?
            if ( conn[CONN_USERS][index] == -1 )
            {
                conn[CONN_TIMES][index] = time();
                break;
            }
            // Ansonsten die (neue?) UID hinzufuegen und die Zeit aktualisieren.
            else
            {
                conn[CONN_USERS][index] =
                    conn[CONN_USERS][index] - ({ path[4] }) + ({ path[4] });
                if ( sizeof(conn[CONN_USERS][index]) >= NEEDED )
                    conn[CONN_USERS][index] = -1;

                conn[CONN_TIMES][index] = time();
                break;
            }
        }
    }

    // Falls keine Verbindung gepasst hat, eine neue erzeugen.
    if ( i < 0 ) {
        int index = path[3] ? CONN_U_SEERS : CONN_U_PLAYERS; // Seher?
        mixed uids = ({ ({}), ({}) });
        uids[index] = ({ path[4] });
        mixed times = ({ 0, 0 });
        times[index] = time();

        // Aufbau siehe oben
        mixed conn = ({ path[1], path[2], uids, path[5], times, path[6], 0 });
        conns += ({ conn });
        // wenn die neue Verbindung nen Kommando benutzt, welches schon fuer
        // eine andere Verbindung aus diesem Raum benutzt wird, sind beide
        // nicht nutzbar, da das Ziel nicht eindeutig ist. In diesem Fall
        // muessen alle diese als unbenutzbar fuer das Routing markiert
        // werden.
        if (member(cmds, conn[CONN_CMD])) {
            check_and_mark_duplicate_conns_by_conn(conns,conn);
        }
    }

    pathmap[pre][path[0]] = conns;

    // Ob 0 oder 1 ist eigentlich total egal, da das Ergebnis-Array sowieso
    // verworfen wird.
    return 0;
}

private void add_to_cache(string from, string to, int para, int costs,
                          string *rooms, string *cmds) {
    struct path_s p = (<path_s> costs : costs, rooms: rooms, cmds : cmds,
                         para: para, flags: 0, ttl: time()+86400);
    if (!member(pathcache, from)) {
        pathcache[from] = m_allocate(1);
        pathcache[from][to] = m_allocate(1);
    }
    else if (!member(pathcache[from], to)) {
        pathcache[from][to] = m_allocate(1);
    }
    pathcache[from][to][para] = p;
}

// Diese Funktion berechnet die Kosten einer Verbindung.
// Ein Seherstatus schliesst die Moeglichkeiten von Spielern ein (seher
// kann Spielerpfade problemlos nutzen).
// Wenn eine Verbindung in eine andere Parawelt fuehrt, als in <para>
// angefragt, wird sie erheblich teurer.
protected int _calc_cost(mixed* path, int seer, int para)
{
    mixed* interest=path[CONN_USERS];

    // Basiskosten nach Ausgangsart
    int costs = COST_EXITS[path[CONN_EXITTYPE]];

    // Verbindungen in andere als die gewuenschte Parawelt werden bei der
    // suche nicht beruecksichtigt, aber Verbindung von Para nach Normal und
    // umgekehrt sind erlaubt. Para->Normal soll aber etwas teurer sein, damit
    // es bevorzugt wird, in der angefragten Welt zu bleiben.
    // (Hintergrund: wenn man in Para ist, laeuft man meistens trotzdem durch
    // Normalweltraeume, weil es keine passenden Pararaeume gibt).
    if (para != path[CONN_PARA])
        costs += COST_WORLDCHANGE;

    // wenn schon genug unterschiedliche Leute die Verbindung gelaufen sind,
    // kostet es die Basiskosten. Fuer Seher auch die Spieler einschliessen.
    if (interest[CONN_U_PLAYERS]==-1
        || (seer && interest[CONN_U_SEERS]==-1))
        return costs;
    else
    {
        // Sonst wird die Verbindung teurer.
        // Anzahl Spielernutzer kann nicht -1 sein (s.o.).
        int usercount = sizeof(interest[CONN_U_PLAYERS]);
        // wenn <seer>, wird ggf. die Anzahl an Sehernutzern drauf addiert.
        // Wert fuer Seher kann nicht -1 sein (s.o.)
        if (seer)
            usercount += sizeof(interest[CONN_U_SEERS]);
        switch (usercount)
        {
            case 0:
                costs=1000000;
                break;
            case 1..2:
                costs*=COST_ONE;
            case 3..4:
                costs*=COST_FEW;
            default:
                costs*=COST_MANY;
        }
        return costs;
    }
    DBG(sprintf("Huch, keine Ahnung, Kosten unbestimmbar: %O",path));
    return 1000;
}

// Neuimplementation von Humni, benutzt einfach simpel einen
// Dijkstra-Algorithmus. "Simpel" schreibe ich, bevor ich die
// Funktion schreibe, fluchen werde ich gleich.
static void _search(string fname, string from)
{
    // <from> ist jetzt anzuguckende/zu bearbeitende Raum.

    // Die aktuelle Liste an abgesuchten Raeumen, Raeume, in denen
    // alle Wege bereits eingetragen sind.
    string* rooms_searched=searchlist[fname,SL_VISITED];

    // Zielort
    string to = searchlist[fname,SL_DESTINATION];

    // moegliche Ziele
    mapping targets=searchlist[fname,SL_CANDIDATES];

    // wizlevel
    int seer = searchlist[fname,SL_WIZLEVEL];

    // para
    int para=searchlist[fname,SL_PARA];

    // Noch zu bearbeitende Raeume
    string* to_do=searchlist[fname,SL_TODO];

    DBG("_search() start.");

    while ((get_eval_cost()>900000) && from && (from != to))
    {
        // Fuege den aktuellen Raum (from) als Weg hinzu
        DBG(sprintf("Fuege hinzu: %s",from));
        rooms_searched+=({from});
        // Aus den Raeumen drumrum werden die Ausgaenge gesucht
        // und zur Wegliste hinzugefuegt.

        // Pfade von <from> weg aus der Liste holen
        mixed* paths=pathmap[_get_prefix(from)][from];

        // Liste der Verbindungen von <from> durchgehen
        foreach (mixed conn:paths)
        {
            int newcost;
            // wenn der Raum schonmal gesehen wurde, ist hier nix mehr noetig.
            if (member(rooms_searched,conn[CONN_DEST])>-1)
            {
                DBG(sprintf("Raum %s hab ich schon!",conn[CONN_DEST]));
            }
            else
            {
                // Verbindung je nach Flag nicht nutzen. Ausserdem:
                // wenn die Verbindung in eine Parawelt fuehrt, aber kein
                // Seherstatus gegeben ist, existiert die Verbindung nicht.
                // Weiterhin kommt eine Verbindung nicht in Frage, wenn sie in
                // eine andere Parawelt fuehrt, weil der Weltuebergang nur an
                // bestimmten Orten moeglich ist. Ausnahme davon sind Wechsel
                // zwischen Normalwelt und Parawelt.
                if (
                    (conn[CONN_FLAGS] & CFLAG_DONTUSE)
                    || (conn[CONN_FLAGS] & CFLAG_DUPLICATE)
                    || (!seer && conn[CONN_PARA])
                    || (conn[CONN_PARA] && conn[CONN_PARA] != para)
                    )
                    continue;
                newcost = targets[from,SLT_COSTS]; // kosten bis hierher
                // + Kosten in naechsten Raum.
                newcost += _calc_cost(conn,seer,para);
                // es koennte sein, dass es Verbindungen mit gleichem
                // Start/Ziel gibt, aber unterschiedlichen Kosten. In diesem
                // Fall wuerde das Ziel der aktuellen Verbindung mehrfach
                // hinzugefuegt und das letzte gewinnt. Daher sollte diese
                // Verbindung nur hinzugefuegt werden, wenn noch keine
                // Verbindung in den Zielraum existiert, die billiger ist.
                if (member(targets,conn[CONN_DEST])
                    && newcost >= targets[conn[CONN_DEST],SLT_COSTS])
                    continue;
                // Weg bis hierher plus naechsten Raum
                string* new_way = targets[from,SLT_WAY];
                new_way += ({conn[CONN_DEST]});
                // Kommandos bis hierher plus naechstes Kommando
                string* new_commands = targets[from,SLT_COMMANDS];
                new_commands += ({conn[CONN_CMD]});
                // naechsten Raum in Todo fuer die Pruefung unten vermerken
                to_do += ({conn[CONN_DEST]});
                // Und Kosten/Weg/Kommandos bis zum naechsten Raum unter
                // targets/Kandidaten ablegen.
                targets += ([conn[CONN_DEST]:newcost;new_way;new_commands]);
            }
        }
        // Nachdem die Raeume alle zu den Kandidaten an moeglichen Wegen
        // hinzugefuegt wurden, wird nun der neueste kuerzeste Weg gesucht.
        // Natuerlich nur aus denen, die schon da sind.
        if (sizeof(to_do)>0)
        {
            // Kosten des ersten todo als Startwert nehmen
            string minraum;
            int mincosts=__INT_MAX__;
            // und ueber alle todos laufen um das Minimum zu finden.
            foreach(string raum: to_do)
            {
                if (mincosts>targets[raum,SLT_COSTS])
                {
                    minraum = raum;
                    mincosts=targets[raum,SLT_COSTS];
                }
            }
            DBG(sprintf("Neuer kuerzester Raum: %s",minraum));

            // der billigst zu erreichende Raum wird das neue <from> fuer den
            // naechsten Durchlauf
            from=minraum;
            // und natuerlich aus dem to_do werfen.
            to_do-=({minraum});
            DBG(sprintf("Anzahl untersuchter Raeume: %d, Todo: %d, "
                  "Kandidaten: %d",
                  sizeof(rooms_searched),sizeof(to_do),sizeof(targets)));
        }
        else
        {
            from=0;
        }
    } // while ende

    if (!from)
    {
        // Hmpf...
        TELL_USER("Kein Weg gefunden!");
        if (searchlist[fname, SL_CALLBACK])
            funcall(searchlist[fname,SL_CALLBACK],
                    searchlist[fname,SL_START], to, para,
                    0,0,0);
        m_delete( searchlist, fname );  // suchauftrag entsorgen
        return;
    }
    else if (from==to)
    {
        // JUCHU!
        TELL_USER(sprintf("Weg gefunden! %O,%O,%O",
              targets[to,SLT_COSTS],targets[to,SLT_WAY],
              targets[to,SLT_COMMANDS]));
        add_to_cache(searchlist[fname,SL_START], to, para,
                     targets[to,SLT_COSTS],targets[to,SLT_WAY],
                     targets[to,SLT_COMMANDS]);
        if (searchlist[fname, SL_CALLBACK])
            funcall(searchlist[fname,SL_CALLBACK],
                    searchlist[fname,SL_START], to, para,
                    targets[to,SLT_COSTS],targets[to,SLT_WAY],
                    targets[to,SLT_COMMANDS]);
        m_delete( searchlist, fname );  // suchauftrag entsorgen
        return;
    }

    // Wenn das alles nicht war, waren es die Eval-Ticks. Mist. Spaeter
    // weitersuchen...
    searchlist[fname,SL_VISITED]=rooms_searched;
    searchlist[fname,SL_CANDIDATES]=targets;
    searchlist[fname,SL_TODO]=to_do;
    call_out("_search",2,fname,from);
}


// Die Daten ueber Raeume werden quasi gehasht abgespeichert, damit wir nicht
// so schnell an die Begrenzung bei Arrays und Mappings stossen.
// Dabei ist der 'Hash' der Anfang des Pfades.
private string _get_prefix( string path )
{
    string *tmp;

    tmp = explode( path, "/" );

    // Bei Raeumen unterhalb von /d/* wird /d/region/magiername benutzt
    if ( strstr(path, "/d/") == 0)
        return implode( tmp[0..3], "/" );
    // Ansonsten die ersten beiden Teilpfade, falls soviele vorhanden sind
    else if ( sizeof(tmp) < 4 )
        return implode( tmp[0..1], "/" );
    else
        return implode( tmp[0..2], "/" );
}

// Reset und Datenhaltung
// reset wird alle 3h gerufen, um das Savefile immer mal wieder zu speichern.
// Datenaufraeumen aber nur einmal pro Tag.
public void reset()
{
    if ( time_to_clean_up <= time() )
    {
        // Einmal pro Tag Reset zum Aufraeumen der Datenbank
        rm(DUMPFILE".raeume");
        rm(DUMPFILE".conns");
        time_to_clean_up = time() + 86400;
        expire_path_cache(m_indices(pathcache));
        call_out(#'cleanup_data, 30,
                  sort_array(m_indices(pathmap), #'</*'*/), 0 );

    }
    save_object(SAVEFILE);
    set_next_reset(10800);
}

protected void expire_path_cache(string *startrooms) {
    foreach(string start : startrooms) {
        mapping targets = pathcache[start];
        foreach (string target, mapping paths: targets) {
            foreach(int para, struct path_s p: paths) {
                if (p->ttl < time()
                    || !(p->flags & PFLAG_PERMANENT) )
                    m_delete(paths,para);
            }
            if (!sizeof(paths))
                m_delete(targets, target);
        }
        startrooms-=({start});
        if (!sizeof(targets))
            m_delete(pathcache, start);
        // recht grosser Wert, weil immer komplette Startraeume in einem
        // erledigt werden.
        if (get_eval_cost() < 1000000)
            break;
    }
    if (sizeof(startrooms))
        call_out(#'expire_path_cache, 4, startrooms);
}

// Datenbank aufraeumen
protected void cleanup_data( string *names, int idx )
{
    int size, i, j, k;
    string *rooms;
    mixed *paths;

    size = sizeof(names);

    // Ein bisserl mitloggen, damit wir Schwachstellen im System auch finden.
    if ( !idx ){
        LOG( sprintf("=== %s: Starte cleanup_data(): %O Gebiete, %O Raeume "
                     + "...\n", strftime("%y%m%d-%H%M%S"), size,
                     show_statistic(1, 1) ) );
    }
    else {
        LOG( sprintf("%s: Setze cleanup_data() fort.\n",
              strftime("%y%m%d-%H%M%S") ) );
    }

    // Brav gesplittet, damit es kein Lag gibt.
    // Die Grenze ist recht hoch angesetzt, da immer gleich komplette
    // Teilbaeume aufgeraeumt werden.
    while ( get_eval_cost() > 1100000 && idx < size ){
        rooms = sort_array( m_indices(pathmap[names[idx]]), #'</*'*/ );

        for ( i = sizeof(rooms); i--; ){
            paths = pathmap[names[idx]][rooms[i]];
            int conn_count = sizeof(paths);
            mapping cmds=m_allocate(sizeof(paths));

            for ( j = conn_count; j--; ) {
                mixed conn = paths[j];
                for ( k = 0; k < 2; k++ ) { // Spieler/Sehereintraege
                    // Diese Verbindung hat noch keiner genutzt bisher
                    if ( !conn[CONN_TIMES][k] )
                        continue;
                    // Wenn Verbindung permanent, ueberspringen
                    if (conn[CONN_FLAGS] & CFLAG_PERMANENT)
                        continue;

                    if ( conn[CONN_USERS][k] == -1 // 'bekanntes' Gebiet
                         && time() - conn[CONN_TIMES][k] > TIMEOUT_OLD ){
                        // LOG( sprintf("*** Loesche alte "
                        //             + (k ? "Seher" : "Spieler")
                        //             + "-Verbindung %O.\n", paths[j]) );
                        conn[CONN_USERS][k] = ({});
                        conn[CONN_TIMES][k] = 0;
                    }
                    else if ( conn[CONN_USERS][k] != -1 // 'neues' Gebiet
                              && time() - conn[CONN_TIMES][k] > TIMEOUT_NEW ){
                        conn[CONN_USERS][k] = ({});
                        conn[CONN_TIMES][k] = 0;
                    }
                }

                // Wenn eine Verbindung weder von Spielern noch von Sehern
                // benutzt wurde in letzter Zeit und sie keine permanented
                // ist, Verbindung ganz loeschen.
                if (!(conn[CONN_FLAGS] & CFLAG_PERMANENT)
                    && !conn[CONN_TIMES][CONN_T_PLAYERS]
                    && !conn[CONN_TIMES][CONN_T_SEERS] ) {
                    paths[j..j] = ({}); // conn rauswerfen
                    conn_count--;
                }
                else {
                    // Connections nach Kommandos speichern.
                    if (member(cmds, conn[CONN_CMD]))
                        cmds[conn[CONN_CMD]] += ({conn});
                    else
                        cmds[conn[CONN_CMD]] = ({conn});
                    // und das duplicate flag loeschen, das wird spaeter neu
                    // gesetzt, wenn noetig.
                    // (There is no chance that a concurrent search will check
                    // these connections until they are marked again.)
                    conn[CONN_FLAGS] &= ~CFLAG_DUPLICATE;
                    // Connections dumpen
                    if (!(conn[CONN_FLAGS] & CFLAG_DUPLICATE))
                    {
                      mixed u=({conn[CONN_USERS][CONN_U_PLAYERS],
                              conn[CONN_USERS][CONN_U_SEERS] });
                      write_file(DUMPFILE".conns",sprintf(
                           "%s|%s|%s|%d|%d|%d|%d\n",
                           rooms[i],conn[CONN_DEST],conn[CONN_CMD],
                           conn[CONN_EXITTYPE],conn[CONN_PARA],
                           (u[0]==-1 ? 0 : NEEDED-sizeof(u[0])),
                           (u[0]==-1 ? 0 : NEEDED-sizeof(u[0]))
                           ));
                    }
                }
            }

            // Ein Raum ohne Verbindungen frisst nur Platz in der Datenbank
            if ( !conn_count ){
                // LOG( sprintf("*** Loesche kompletten Raum %O\n", rooms[i]) );
                m_delete( pathmap[names[idx]], rooms[i] );
            }
            else {
                // wenn Kommandos nicht eindeutig sind, dann werden die
                // entsprechenden Verbindungen gekennzeichnet. Alle in Frage
                // kommenden Verbindungen sind jeweils schon zusammensortiert.
                foreach(string cmd, mixed conns : cmds) {
                    if (sizeof(conns)>1)
                        check_and_mark_duplicate_conns(conns);
                }
                // ggf. geaendertes Verbindungsarray wieder ins Mapping
                // schreiben.
                pathmap[names[idx]][rooms[i]] = paths;
                // Raum/Node ins Dump schreiben
                write_file(DUMPFILE".raeume",sprintf(
                      "%s\n",rooms[i]));
            }
        }
        idx++;
    }
    if ( idx >= size ) {
        LOG( sprintf("=== %s: Beende cleanup_data()! Uebrig: %O Raeume.\n",
                     strftime("%y%m%d-%H%M%S"), show_statistic(1, 1) ) );
    }
    else {
        call_out( #'cleanup_data, 2, names, idx );
        LOG( sprintf("%s: WARTE 20s bei Evalcost %O\n",
                     strftime("%y%m%d-%H%M%S"), get_eval_cost()) );
    }
}

// Beim Entladen speichern
public varargs int remove( int silent )
{
    // Vor dem Entfernen noch schnell die Daten sichern
    save_object(SAVEFILE);
    destruct(this_object());

    return 1;
}


// Zum Debuggen. Nur für Tiamak. Bzw. nun fuer Zook, Vanion, Rumata
#define ALLOWED ({"zook","vanion","rumata","zesstra","humni"})
#define MATCH(x,y) (sizeof(regexp(x,y)))
public mapping get_pathmap()
{
  if ( !this_interactive() || !MATCH(ALLOWED,getuid(this_interactive()))
       || !previous_object() || !MATCH(ALLOWED,getuid(previous_object())))
        return ([]);

    return pathmap;
}
public mapping get_pathcache()
{
  if ( !this_interactive() || !MATCH(ALLOWED,getuid(this_interactive()))
       || !previous_object() || !MATCH(ALLOWED,getuid(previous_object())))
        return ([]);

    return pathcache;
}

#undef ALLOWED
#undef MATCH
