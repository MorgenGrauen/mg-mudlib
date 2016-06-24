// (c) by Padreic (Padreic@mg.mud.de)

// Auch wenn dieses Objekt noch keinerlei tieferen Sinn hat, so ist es doch
// bereits ein vollstaendiges Objekt (bereits eine einzige Funktion reicht
//  fuer ein LPC-Object aus).

// Die 3 Funktionen in diesem Objekt haben darueber hinaus noch eine
// ganz besondere Bedeutung. Jede dieser 3 Funktionen wird vom Game-Driver
// im laufenden Betrieb bei besonderen Anlaessen aufgerufen.

protected void create()
// Diese Funktion wird aufgerufen sobald dieses Objekt geclont wird.
{
   // gibt an den Spieler der dieses Objekt clont eine Meldung aus.
   tell_object(this_player(), "Hello World!\n");
}

void init()
// Diese Funktion wird aufgerufen, sobald ein Spieler oder ein NPC in
// Reichweite dieses Objektes kommt: Also entweder wenn ein Lebewesen in
// das Objekt bewegt wird oder umgekehrt oder wenn ein Lebewesen in die
// Umgebung eines Objektes kommt oder umgekehrt.
{
   // gibt wieder eine Meldung an den Spieler aus...
   tell_object(this_player(), ("Hello again :).\n");
}

void reset()
// Diese Funktion wird im beruemten Reset aufgerufen, den Du ja sicher auch
// schon als Spieler kennengelernt hast (Also alle 30-60 Minuten).
// Ausnahme: Wenn das Objekt laengere Zeit mit keinem anderen Objekt
// kommuniziert hat, wird diese Funktion vorlaeufig nicht mehr aufgerufen!
{
  // Hier kann man nun auch irgendetwas machen. Ein write("..") haette
  // hier jedoch keinen Effekt, da kein Spieler diese Funktion ausgeloest
  // hat und daher nicht bekannt ist, an wen die Meldung denn gehen soll...
}
