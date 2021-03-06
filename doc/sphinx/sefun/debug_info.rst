debug_info()
============

DEPRECATED
----------
::

SYNOPSIS
--------
::

        #include <debug_info.h>

        mixed debug_info(int flag)
        mixed debug_info(int flag, mixed arg)
        mixed debug_info(int flag, mixed arg2, mixed arg3)

BESCHREIBUNG
------------
::

        Sammelt entsprechend den Angaben in <flag> gewisse intere Debuginfos
        des Treibers. <flag> kann dabei folgende in debug_info.h definierte
        Werte enthalten:

        DINFO_OBJECT   (0): Angezeigt werden Informationen zum in <arg>
            spezifizierten Objekt, zum Beispiel heart_beat,
            enable_commands etc. Die Funktion liefert 0 zurueck.

        DINFO_MEMORY   (1): Angezeigt werden Informationen zu
            Speicherbelegung und -ausnutzung des in <arg> spezifizierten
            Objekts, zum Beispiel Anzahl Strings, Variablen, geerbte Files,
            Objektgroesse etc. Die Funktion liefert 0 zurueck.

        DINFO_OBJLIST  (2): debug_info() liefert Objekte aus der globalen
            Objektliste. Wenn <arg2> nicht angegeben wird, wird das erste
            Element aus der Objektliste gelierfert, wenn <arg2> eine Zahl n
            ist, das n-te Element. Ist <arg2> ein Objekt, werden die
            nachfolgenden Objekte in der Objektliste zurueck geliefert.
            Das optionale Argument <arg3> bezeichnet die Anzahl zurueck
            gelieferter Objekte. Wenn <arg3> 0 ist, wird ein einzelnes
            Objekt zurueck geliefert. Wenn <arg3> eine Zahl m enthaelt, wird
            ein Array mit hoechstens m Elementen zurueck geliefert. Auf
            diese Weise kann ein Array mit saemtlichen Objekten im Spiel
            erzeugt werden, wenn fuer <arg3> __INT_MAX__ gesetzt wird (eine
            entsprechende maximale Arraygroesse vorausgesetzt).

        DINFO_MALLOC   (3): Entsprichend der Eingabe des 'malloc'-Kommandos.
            Es muessen keine weiteren Argumente angegeben werden.

        DINFO_STATUS   (4): Angezeigt wird die Statusinformation des Drivers.
            Optional kann das Argument <arg> die Werte 0, "tables", "swap",
            "malloc" oder andere vom Driver akzeptierte Argumente enthalten.
            Das Resultat ist ein druckbarer String, der die Statusinformation
            enthaelt, oder 0, wenn ein ungueltiges Argument angegeben wurde.

        DINFO_DUMP     (5): Die durch <arg2> angeforderte Information wird
            in ein File geschrieben, das man mit <arg3> angeben kann. Wird
            <arg3> nicht angegeben, wird eine Standarddatei verwendet.
            debug_info() ueberprueft mittels master->valid_write(), ob es
            das File schreiben kann. Falls bereits eine entsprechende Datei
            existiert, wird diese ueberschrieben. Die Funktion liefert 1
            bei Erfolg, 0 sonst.

            <arg2> == "objects": liefert Informationen ueber alle Objekte im
                Spiel und schreibt diese standardmaessig in die Datei
                /OBJ_DUMP, dem valid_write() wird 'objdump' uebergeben.
                Die Datei enthaelt fuer jedes Objekt eine Zeile, in der
                jeweils folgende Informationen aufgelistet sind:
                  - Name des Objekts (object_name)
                  - Groesse im Speicher, gemeinsamer genutzter Speicher nur
                    einmal gezaehlt
                  - Groesse im Speicher, wenn es keine gemeinsam genutzte
                    Daten geben wuerde
                  - Anzahl Referenzen
                  - 'HB', wenn das Objekt einen heart_beat hat, sonst nichts.
                  - der Name der Umgebung oder '--', wenn das Objekt keine
                    Umgebung hat
                  - in Klammern die Anzahl der durch das Objekt verursachten
                    Verarbeitungsschritten (execution ticks)
                  - der Swap-Status:
                      > nichts, wenn das Objekt nicht geswapt wurde
                      > 'PROG SWAPPED', wenn nur das Programm geswapt wurde
                      > 'VAR SWAPPED', wenn nur die Variablen geswapt wurden
                      > 'SWAPPED', wenn beide geswapt wurden
                  - die Zeit, zu der das Objekt geladen wurde.

            <arg2> == "destructed": liefert Informationen ueber alle
                zerstoerten Objekte und schreibt diese standardmaessig in
                die Datei /DEST_OBJ_DUMP, dem valid_write() wird 'objdump'
                uebergeben. Die Datei enthaelt fuer jedes Objekt eine Zeile,
                in der jeweils folgende Informationen aufgelistet sind:
                  - Name des Objekts (object_name)
                  - Anzahl der Referenzen
                  - 'NEW', wenn das Objekt in diesem Verarbeitungszyklus
                    zerstoert wurde, nichts wenn es bereits fruehre zerstoert
                    worden war.

            <arg2> == "opcodes": liefert Nutzungsinformationen ueber die
                opcodes. Standardmaessig wird in die Datei /OPC_DUMP
                geschrieben. valid_write() wird 'opcdump' uebergeben.

            <arg2> == "memory": liefert eine Liste aller allokierten
                Speicherbloecke.
                Standardmaessig wird in die Datei /MEMORY_DUMP geschrieben;
                valid_write() wird 'memdump' uebergeben.  Existiert die
                Datei bereits, werden die neuen Daten angehaengt.
                Wenn der Allokator einen Speicherabzug nicht unterstuetzt,
                wird keine Datei geschrieben und immer 0 zurueckgegeben.
                Diese Funktion ist am nuetzlichsten wenn der Allokator
                mit MALLOC_TRACE und MALLOC_LPC_TRACE kompiliert
                wurde.


        DINFO_DATA    (6): Liefert Rohdaten ueber gewisse, durch <arg2>
            spezifizierte Aspekte des Treibers. Das Resultat der Funktion ist
            ein Array mit der Information oder 0, falls <arg2> keinen
            gueltigen Wert enthalten hat.
            Ist <arg3> eine Zahl, die kleiner ist als die Anzahl Elemente im
            Resultat-Array, liefert die Funktion nur das entsprechende
            Element zurueck.
            Zulaessige Werte fuer <arg2> sind: DID_STATUS, DID_SWAP und
            DID_MALLOC.

            <arg2> == DID_STATUS (0): Liefert die "status" und "status table"
                Information. Folgende Indizes sind definiert:
                  - int DID_ST_BOOT_TIME
                        die Zeit (time()), zu der das Mud gestartet wurde
                  - int DID_ST_ACTIONS
                  - int DID_ST_ACTIONS_SIZE
                        die Anzahl und Groesse im Speicher der allozierten
                        Actions.
                  - int DID_ST_SHADOWS
                  - int DID_ST_SHADOWS_SIZE
                        Anzahl und Groesse im Speicher aller allozierten
                        Shadows.
                  - int DID_ST_OBJECTS
                  - int DID_ST_OBJECTS_SIZE
                        Anzahl und Groesse im Speicher aller Objekte.
                  - int DID_ST_OBJECTS_SWAPPED
                  - int DID_ST_OBJECTS_SWAP_SIZE
                        Anzahl und Groesse im Speicher der geswapten
                        Variablenbloecke der Objekte
                  - int DID_ST_OBJECTS_LIST
                        Anzahl Objekte in der Objektliste
                  - int DID_ST_OBJECTS_NEWLY_DEST
                        Anzahl der frisch zerstoerten Objekte (d.h. Objekte,
                        die in diesem Verarbeitungszyklus zerstoert wurden)
                  - int DID_ST_OBJECTS_DESTRUCTED
                        Anzahl der zerstoerten, aber noch immer referenzierten
                        Objekte, ohne die unter DID_ST_OBJECTS_NEWLY_DEST
                        bereits gezaehlten.
                  - int DID_ST_OBJECTS_PROCESSED
                        Anzahl der gelisteten Objekte, die im letzten
                        Verarbeitungszyklus verarbeitet wurden.
                  - float DID_ST_OBJECTS_AVG_PROC
                        Durchschnittlicher Anteil der pro Zyklus verarbeiteten
                        Objekte, ausgedrueckt in Prozent (0 .. 1.0)
                  - int DID_ST_OTABLE
                        Anzahl eingetragener Objekte in der Objekttabelle
                  - int DID_ST_OTABLE_SLOTS
                        Anzahl von Hashplaetzen, die von jeder Objekttabelle
                        bereitgestellt werden
                  - int DID_ST_OTABLE_SIZE
                        Durch die Objekttabelle belegter Speicher
                  - int DID_ST_HBEAT_OBJS
                        Anzahl Objekte mit einem heart_beat()
                  - int DID_ST_HBEAT_CALLS
                        Anzahl aktiver heart_beat() Zyklen bisher (d.h.
                        Zyklen, in denen mindestens eine heart_beat() Funktion
                        aufgerufen wurde)
                  - int DID_ST_HBEAT_CALLS_TOTAL
                        Gesamtzahl von heart_beat() Zyklen bisher.
                  - int DID_ST_HBEAT_SLOTS
                  - int DID_ST_HBEAT_SIZE
                        Anzahl und Groesse aller allozierten Eintraege in der
                        heart_beat() Tabelle.
                  - int DID_ST_HBEAT_PROCESSED
                        Anzahl heart_beat()s im letzten Zyklus
                  - float DID_ST_HBEAT_AVG_PROC
                        Durchschnittlicher Anteil der pro Zyklus aufgerufenen
                        heart_beat()s, ausgedrueckt in Prozent (0 .. 1.0)
                  - int DID_ST_CALLOUTS
                        Anzahl und Groesse aller laufenden call_out()s
                  - int DID_ST_ARRAYS
                  - int DID_ST_ARRAYS_SIZE
                        Anzahl und Groesse aller Arrays
                  - int DID_ST_MAPPINGS
                  - int DID_ST_MAPPINGS_SIZE
                        Anzahl und Groesse aller Mappings
                  - int DID_ST_PROGS
                  - int DID_ST_PROGS_SIZE
                        Anzahl und Groesse aller Programme
                  - int DID_ST_PROGS_SWAPPED
                  - int DID_ST_PROGS_SWAP_SIZE
                        Anzahl und Groesse der geswapten Programme
                  - int DID_ST_USER_RESERVE
                  - int DID_ST_MASTER_RESERVE
                  - int DID_ST_SYSTEM_RESERVE
                        Momentane Groesse der drei Speicherreserven
                  - int DID_ST_ADD_MESSAGE
                  - int DID_ST_PACKETS
                  - int DID_ST_PACKET_SIZE
                        Anzahl Aufrufe von add_message(), Anzahl und Groesse
                        der versendeten Pakete.
                        Wenn der Driver nicht mit COMM_STAT kompiliert wurde,
                        liefern alle drei Werte immer -1 zurueck.
                  - int DID_ST_APPLY
                  - int DID_ST_APPLY_HITS
                        Anzahl Aufrufen von apply_low(), und wie viele davon
                        Cache-Treffer waren. Wenn der Driver nicht mit
                        APPLY_CACHE_STAT kompiliert wurde, liefern beide
                        Werte immer -1.
                  - int DID_ST_STRINGS
                  - int DID_ST_STRING_SIZE
                        Anzahl unterschiedlicher Strings in der Stringtabelle,
                        sowie ihre Groesse
                  - int DID_ST_STR_TABLE_SIZE
                        Groesse der String Tabelle
                  - int DID_ST_STR_REQ
                  - int DID_ST_STR_REQ_SIZE
                        Gesamte Anzahl von String Allokationen, und ihre
                        Groesse
                  - int DID_ST_STR_SEARCHES
                  - int DID_ST_STR_SEARCH_LEN
                        Anzahl Suchvorgaenge in der Stringtabelle und die
                        Gesamtlaenge des Suchstrings
                  - int DID_ST_STR_FOUND
                        Anzahl erfolgreicher Suchvorgaenge
                  - int DID_ST_STR_ENTRIES
                        Anzahl Eintraege (Hash Ketten) in der String Tabelle
                  - int DID_ST_STR_ADDED
                        Anzahl zur String Tabelle bisher hinzugefuegter
                        Strings
                  - int DID_ST_STR_DELETED
                        Anzahl aus der String Tabelle bisher geloeschter
                        Strings
                  - int DID_ST_STR_COLLISIONS
                        Anzahl zu einer existierenden Hash Kette hinzugefuegte
                        Strings
                  - int DID_ST_RX_CACHED
                        Anzahl gecacheter regulaerer Ausdruecke (regular
                        expressions)
                  - int DID_ST_RX_TABLE
                  - int DID_ST_RX_TABLE_SIZE
                        Anzahl Plaetze in der Regexp Cache Tabelle und
                        Speicherbedarf der gecacheten Ausdruecke
                  - int DID_ST_RX_REQUESTS
                        Anzahl Anfragen fuer neue regexps
                  - int DID_ST_RX_REQ_FOUND
                        Anzahl gefundener regexps in der regexp Cache Tabelle
                  - int DID_ST_RX_REQ_COLL
                        Anzahl angefragter regexps, die mit einer bestehenden
                        regexp kollidierten
                  - int DID_ST_MB_FILE
                        Die Groesse des 'File' Speicherpuffers
                  - int DID_ST_MB_SWAP
                        Die Groesse des 'Swap' Speicherpuffers

            <arg2> == DID_SWAP (1): Liefert "status swap"-Information:
                  - int DID_SW_PROGS
                  - int DID_SW_PROG_SIZE
                        Anzahl und Groesse der geswappten Programmbloecke
                  - int DID_SW_PROG_UNSWAPPED
                  - int DID_SW_PROG_U_SIZE
                        Anzahl und Groesse der nicht geswappten Bloecke
                  - int DID_SW_VARS
                  - int DID_SW_VAR_SIZE
                        Anzahl und Groesse der geswappten Variablenbloecke
                  - int DID_SW_FREE
                  - int DID_SW_FREE_SIZE
                        Anzahl und Groesse der freien Bloecke in der
                        Auslagerungsdatei
                  - int DID_SW_FILE_SIZE
                        Groesse der Auslagerungsdatei
                  - int DID_SW_REUSED
                        Gesamter wiederverwendeter Speicherplatz in der
                        Auslagerungsdatei
                  - int DID_SW_SEARCHES
                  - int DID_SW_SEARCH_LEN
                        Anzahl und Gesamtlaenge der Suchvorgaenge nach
                        wiederverwendbaren Bloecken in der Auslagerungsdatei
                  - int DID_SW_F_SEARCHES
                  - int DID_SW_F_SEARCH_LEN
                        Anzahl und Gesamtlaenge der Suchvorgaenge nach einem
                        Block, der frei gemacht werden kann.
                  - int DID_SW_COMPACT
                        TRUE wenn der Swapper im Compact-Modus laeuft
                  - int DID_SW_RECYCLE_FREE
                        TRUE wenn der Swapper gerade einen freien Block
                        wiederverwendet

            <arg2> == DID_MEMORY (2): Liefert die "status malloc"-Information:
                  - string DID_MEM_NAME
                        Der Name des Allokators: "sysmalloc", "smalloc",
                        "slaballoc"
                  - int DID_MEM_SBRK
                  - int DID_MEM_SBRK_SIZE
                        Anzahl und Groesse der Speicherbloecke, die vom
                        Betriebssystem angefordert wurden (smalloc, slaballoc)
                  - int DID_MEM_LARGE
                  - int DID_MEM_LARGE_SIZE
                  - int DID_MEM_LFREE
                  - int DID_MEM_LFREE_SIZE
                        Anzahl und Groesse der grossen allozierten bzw.
                        freien Bloecke (smalloc, slaballoc)
                  - int DID_MEM_LWASTED
                  - int DID_MEM_LWASTED_SIZE
                        Anzahl und Groesse der unbrauchbaren grossen
                        Speicherfragmente (smalloc, slaballoc)
                  - int DID_MEM_CHUNK
                  - int DID_MEM_CHUNK_SIZE
                        Anzahl und Groesse kleiner Speicherbloecke (chunk
                        blocks; smalloc, slaballoc)
                  - int DID_MEM_SMALL
                  - int DID_MEM_SMALL_SIZE
                  - int DID_MEM_SFREE
                  - int DID_MEM_SFREE_SIZE
                        Anzahl und groesse der allozierten bzw. freien
                        kleinen Speicherbloecke (smalloc, slaballoc)
                  - int DID_MEM_SWASTED
                  - int DID_MEM_SWASTED_SIZE
                        Anzahl und Groesse der unbrauchbar kleinen
                        Speicherfragmente (smalloc, slaballoc)
                  - int DID_MEM_MINC_CALLS
                  - int DID_MEM_MINC_SUCCESS
                  - int DID_MEM_MINC_SIZE
                        Anzahl Aufrufe von malloc_increment(), Anzahl der
                        erfolgreichen Aufrufe und die Groesse des auf diese
                        Art allozierten Speichers (smalloc, slaballoc)
                  - int DID_MEM_PERM
                  - int DID_MEM_PERM_SIZE
                        Anzahl und Groesse permanenter (non-GCable)
                        Allokationen (smalloc, slaballoc)
                  - int DID_MEM_CLIB
                  - int DID_MEM_CLIB_SIZE
                        Anzahl und Groesse der Allokationen durch Clib
                        Funktionen (nur smalloc, slaballoc mit SBRK_OK)
                  - int DID_MEM_OVERHEAD
                        Overhead fuer jede Allokation (smalloc, slaballoc)
                  - int DID_MEM_ALLOCATED
                        Der Speicher, der durch die Speicherverwaltung
                        alloziert wurde, inklusive den Overhead fuer die
                        Speicherverwaltung (smalloc, slaballoc)
                  - int DID_MEM_USED
                        Der Speicher, der durch den Driver belegt ist, ohne
                        den durch die Speicherverwaltung belegten Speicher
                        (smalloc, slaballoc)
                  - int DID_MEM_TOTAL_UNUSED
                        Der Speicher, der vom System zur Verfuegung gestellt,
                        aber vom Treiber nicht benoetigt wird.
                  - int DID_MEM_AVL_NODES          (smalloc, slaballoc)
                        Anzahl der AVL-Knoten, die zur Verwaltung der
                        freien grossen Speicherbloecke verwendet werden
                        (nur smalloc). Dieser Wert kann in Zukunft
                        wieder verschwinden.
                  - mixed * DID_MEM_EXT_STATISTICS (smalloc, slaballoc)
                        Detaillierte Statistiken des Allokators sofern
                        diese aktiviert wurden; 0 anderenfalls.

                        Dieser Wert kann in Zukunft wieder verschwinden.

                        Das Array enthaelt NUM+2 Eintraege, wobei NUM
                        Anzahl der verschiedenen 'kleinen'
                        Blockgroessen ist. Eintrag [NUM] beschreibt
                        die uebergrossen 'kleinen' Bloecke, Eintrag
                        [NUM+1] beschreibt summarisch die 'grossen'
                        Bloecke. Jeder Eintrag ist ein Array mit
                        diesen Feldern:

                        int DID_MEM_ES_MAX_ALLOC:
                          Maximale Anzahl allokierter Bloecke dieser
                          Groesse.

                        int DID_MEM_ES_CUR_ALLOC:
                          Derzeitige Anzahl allokierter Bloecke dieser
                          Groesse.
                          Current number of allocated blocks of this size.

                        int DID_MEM_ES_MAX_FREE:
                          Maximale Anzahl freier Bloecke dieser
                          Groesse.

                        int DID_MEM_ES_CUR_FREE:
                          Derzeitige Anzahl freier Bloecke dieser
                          Groesse.

                        float DID_MEM_ES_AVG_XALLOC:
                          Durchschnittliche Zahl von Allokationen pro
                          Sekunde.

                        float DID_MEM_ES_AVG_XFREE:
                          Durchschnittliche Zahl von Deallokationen pro
                          Sekunde.

                      Die Durchschnittsstatistiken schliessen interne
                      Umsortierungen der Blocklisten nicht ein.


        DINFO_TRACE    (7): Liefert die 'trace' Information aus dem
            Call Stack entsprechend der Spezifikation in <arg2>. Das Resultat
            ist entweder ein Array (dessen Format nachstehend erlaeutert ist)
            oder ein druckbarer String. Wird <arg2> weggelasen entspricht
            dies DIT_CURRENT.

            <arg2> == DIT_CURRENT (0): Momentaner Call Trace
                   == DIT_ERROR   (1): Letzter Fehler Trace (caught oder
                        uncaught)
                   == DIT_UNCAUGHT_ERROR (2): Letzer Fehler Trace, der nicht
                        gefangen werden konnte (uncaught)

            Die Information wird in Form eines Array uebergeben.

            Die Fehlertraces werden nur geaendert, wenn ein entsprechender
            Fehler auftritt; ausserdem werden sie bei einem GC (Garbage
            Collection) geloescht. Nach einem Fehler, der nicht gefangen
            werden konnte (uncaught error), weisen beide Traces auf das
            gleiche Array, sodass der ==-Operator gilt.

            Wenn das Array mehr als ein Element enthaelt, ist das erste
            Element 0 oder der Name des Objekts, dessen heart_beat() den
            laufenden Zyklus begonnen hat; alle nachfolgenden Elemente
            bezeichnen den Call Stack, beginnen mit der hoechsten
            aufgerufenen Funktion.

            Alle Eintraege im Array sind wiederum Arrays mit folgenden
            Elementen:
              - int[TRACE_TYPE]: Der Typ der aufrufenden Funktion
                    TRACE_TYPE_SYMBOL (0): ein Funktionssymbol (sollte nicht
                                           vorkommen)
                    TRACE_TYPE_SEFUN  (1): eine simul-efun
                    TRACE_TYPE_EFUN   (2): eine Efun Closure
                    TRACE_TYPE_LAMBDA (3): eine lambda Closure
                    TRACE_TYPE_LFUN   (4): eine normale Lfun
              - int[TRACE_NAME]
                    _TYPE_EFUN    : entweder der Name der Funktion oder der
                                    Code einer Operator-Closure
                    _TYPE_LAMBDA  : die numerische Lambda-ID
                    _TYPE_LFUN    : der Name der Lfun
              - string[TRACE_PROGRAM]:  Der Name des Programms mit dem Code
              - string[TRACE_OBJECT]:   Der Name des Objekts, fuer das der
                                        Code ausgefuehrt wurde
              - int[TRACE_LOC]:
                    _TYPE_LAMBDA  : Der Offset des Programms seit Beginn des
                                    Closure-Codes
                    _TYPE_LFUN    : Die Zeilennummer.

            <arg2> == DIT_STR_CURRENT (3): Liefert Informationen ueber den
                momentanen Call Trace als druckbarer String.

            <arg2> == DIT_CURRENT_DEPTH (4): Liefert die Zahl der Frames auf
                dem Control Stack (Rekursionstiefe).

        DINFO_EVAL_NUMBER (8): gibt die Nummer der aktuellen Berechnung
            zurueck. Diese Nummer wird fuer jeden vom driver initiierten
            Aufruf von LPC-Code erhoeht, also bei Aufruf von:
              - Kommandos (die per add_action hinzugefuegt wurden)
              - heart_beat, reset, clean_up
              - Aufrufe durch call_out oder input_to
              - master applies, die auf externe Ereignisse zurueckgehen
              - driver hooks genauso
              - Rueckrufen von send_erq
              - logon in interaktiven Objekten

           Dieser Zaehler kann z.B. benutzt werden, um zu verhindern, dass
           bestimmte Aktionen mehrfach innerhalb eines heart_beat()
           ausgefuehrt werden. Eine andere Anwendungsmoeglichkeit sind
           Zeitstempel zur Sortierung zur Sortierung von Ereignissen.

           Es ist zu beachten, dass der Zaehler ueberlaufen kann, insbesondere
           auf 32-bit-Systemen. Er kann demzufolge auch negativ werden.

GESCHICHTE
----------
::

        Seit 3.2.7 liefert DINFO_STATUS die Information zurueck, anstatt sie
            nur auszugeben.
        DINFO_DUMP wurde in 3.2.7 eingefuehrt.
        LDMud 3.2.8 fuegte die Datengroesse des Objekts zum Resultat von
            DINFO_MEMORY hinzu, ausserdem die DINFO_DATA Abfrage und die
            verschiedenen DID_MEM_WASTED Statistiken.
        LDMud 3.2.9 fuegte DINFO_TRACE, das Indizierungs-Feature von
            DINFO_DATA, den 'destructed'-DINFO_DUMP, die DID_MEM_CLIB*,
            die DID_MEM_PERM*, ausserdem DID_ST_OBJECTS_NEWLY_DEST,
            DID_ST_OBJECTS_DEST, DID_MEM_OVERHEAD, DID_MEM_ALLOCATED,
            DID_MEM_USED, DID_MEM_TOTAL_UNUSED, DID_ST_HBEAT_CALLS_TOTAL
            und die found / added / collision Stringstatistiken.
        LDMud 3.2.10 fuegte die Erzeugungszeit zu DINFO_DUMP:"objects" hinzu,
            entfernte DID_MEM_UNUSED aus DINFO_DATA:DID_MEMORY, fuegte
            DINFO_DATA:DID_STATUS DID_ST_BOOT_TIME, DID_ST_MB_FILE und
            DID_ST_MB_SWAP hinzu und entfernte DID_ST_CALLOUT_SLOTS daraus,
            fuegte das dritte Argument zu DINFO_OBJLIST hinzu, und veraenderte
            die Bedeutung von DID_ST_CALLOUT_SIZE und DID_ST_HBEAT_SIZE
            bzw. _SLOTS.
        LDMud 3.3.533 fuegte DID_MEM_AVL_NODES zu DINFO_DATA:DID_MEMORY
            hinzu.
        LDMud 3.3.603 fuegte DID_MEM_EXT_STATISTICS zu DINFO_DATA:DID_MEMORY
            hinzu.
        LDMud 3.3.718 fuegte DIT_CURRENT_DEPTH to DINFO_TRACE hinzu.
        LDMud 3.3.719 fuegte DINFO_EVAL_NUMBER hinzu.

SIEHE AUCH
----------
::

        trace(E), traceprefix(E), malloc(D), status(D), dumpallobj(D)

