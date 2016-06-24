// Mit dem Praecompiler lassen sich symbolische Konstanten definieren. Dies
// z.B. sinnvoll, wenn eine Zahl eine bestimmte Bedeutung hat. Z.b. hat die
// Zahl 42 an sich keine fixe Bedeutung, hier ist die jedoch die Antwort!
#define ANTWORT 42
// Schreibt man nun irgendwo im Code ANTWORT, ersetzt der Praecompiler jedes
// Vorkommen von ANTWORT durch 42.

// auch komplexere Konstanten sind moeglich, z.B. ein Argument eines Defines:
#define ROOM(x) ("/d/ebene/zesstra/advent/room/"+x)
// Dies dient meist, um sich Schreibarbeit zu sparen. Formuliert man nach
// diesem #define ein ROOM("weg1"), erzeugt der Praecompiler hieraus:
// ("/d/ebene/zesstra/advent/room/weg1")

// Weiteres Beispiel:
#define BS(x) break_string(x, 78)
// erzeugt also aus BS("langer text") ein break_string("langer text",78)

// Es gehen auch mehrere Argumente:
#define BS(x,y) break_string(x, 78, y)
// BS("langer text", 78, "Zesstra sagt: ")
//   -> break_string("langer text", 78, "Zesstra sagt: ")

// Solche Defines lassen sich schachteln:
#define HOME(x) ("/d/ebene/zesstra/advent/"+x)
#define ROOM(x) (HOME("room/"+x))
// Ein ROOM("weg1") wird nun durch (HOME("room/"+"weg1")) ersetzt
// und dieses wiederum durch ein 
// (("/d/ebene/zesstra/advent/"+"room/"+"weg1")), was letztendlich ein
// (("/d/ebene/zesstra/advent/room/weg1")) ist.

// Letzteres wird haeufig benutzt bei der Erstellung von Gebieten um sich bei
// Umzuegen Arbeit zu sparen: zieht das Gebiet um (z.B. von /players/zesstra/
// nach /d/ebene/zesstra/advent/, reicht es aus, HOME zu aendern und man muss
// nicht 150mal den kompletten Pfad im Code aendern.

// moechte man ein Define ueber mehrere Zeilen definieren, muss der
// Zeilenumbruch mit einem \ escaped  werden:
#define LOG(x) log_file("bla",\
    "Dies ist ein laengerer Text.\n")
// WICHTIG: Hinter dem \ darf KEIN Leerzeichen, Tab o.ae. stehen!

// Tips:
// * Damit man Defines schnell als solche erkennt, sollten alle Defines
//   grossgeschrieben werden: z.B. PL vs. pl
// * von komplexen Defines wird abgeraten, weil sie schnell Uebersicht 
//   und Effizienz ruinieren koennen
// * einige haeufig benutzte Defines finden sich in <defines.h>
// * der Driver definiert bestimmte Defines automatisch. Eine Liste
//   findet sich in /doc/driver/predefined

