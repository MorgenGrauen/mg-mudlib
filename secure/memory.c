/*!
// MorgenGrauen Mudlib
//
// secure/memory.c  -- zum Speichern von Daten ueber die gesamte Mud-Uptime
//
// Memory speichert seine Daten als Pointer in der extra-wizinfo. Andere 
// Programme koennen Ihre Daten im Memory ablegen. Die Daten bleiben dadurch 
// auch ueber ein Update/Reload der Blueprint erhalten.
//
// Die Daten werden auf Klassenebene behandelt. Jeder Clone eines Programms
// (Blueprint) darf auf die gespeicherten Daten des Programms schreibend und
// lesend zugreifen. Andere Programme haben keinen Zugriff auf diese Daten.
//
// Nur Objekte in /secure/memory_lib bzw. /secure/memory_nolib eingetragen 
// sind, haben Rechte den Memory zu nutzen.
//
// Die Idee, sensible Daten in der extra_wizinfo abzuspeichern entstammt 
// /secure/memory.c aus der Wunderland Mudlib von Holger@Wunderland
*/
#include <config.h>
#include <wizlevels.h>
#include <files.h>

#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma no_inherit
#pragma verbose_errors
#pragma combine_strings
#pragma pedantic
//#pragma range_check
#pragma warn_deprecated


/* Global variable declaration */
/*! \brief Liste mit Programmen, die den Memory nutzen duerfen 
 Die Variable wird entweder im create() oder durch manuellen Aufruf 
 von RereadProgramLists() aus /secure/memory_lib und /secure/memory_nolib
 befuellt.
 */
string *known_programs;

/* Function declaration - public interface */
void create();

// Payload Interface
int   Save  (string key, mixed data);
mixed Load  (string key);
int   Remove(string key);
int   HaveRights();

// Administrative Interface
varargs int RereadProgramLists(int silent);

// Debugging Interface
varargs void ShowData(string user, string key);

/* Function definition */ 
/*! Objekt initialisieren */
void create() {
  mapping info;
  seteuid(getuid(this_object()));

  // Wizinfo-Pointer holen
  if(!pointerp(info=get_extra_wizinfo(0)))
    raise_error("WIZINFO nicht lesbar. secure/simul_efun muss neu geladen werden.!\n");

  // Mein Feld der Wizinfo bei Bedarf initialisieren
  if(!mappingp(info[MEMORY_BUFF]))
    info[MEMORY_BUFF]=([]);

  RereadProgramLists(1);
}

/*!
 Wenn das aufrufende Programm fuer die Speichernutzung zugelassen ist,
 wird hier der Hash zurueckgegeben unter dem seine Daten abgespeichert
 sind. Der Hash ist derzeit der Filename des Blueprint.
*/
private string get_caller_hash() {
  if (member(known_programs, program_name(previous_object())[..<3]) != -1) 
    return program_name(previous_object())[..<3];
  else 
    return 0;
}

/*! 
 Die Hilfsfunktion liest eine Liste mit Dateinamen und gibt die Programme 
 als Array of Strings zurueck. Alle Zeilen die nicht mit einem Slash 
 anfangen, werden dabei ignoriert.
 \param file Die auszulesende Datei.
*/
private mixed read_list(string file)
{
  mixed data;
  if (!stringp(file) || file_size(file) == FSIZE_NOFILE )
    return ({});

  // Daten laden
  data = explode(read_file(file)||"", "\n");

  // Alle Zeilen die nicht mit / beginnen entfernen ...
  data=regexp(data,"^/.+");

  // ".c" und eventuelle Leerzeichen in jeder Zeile werden entfernt
  data= map(data, 
     function string (string x) {return regreplace(x,"(.c)* *$","",1);});
  return data;
}

/*! 
 Wenn die Liste mit Programmen geaendert wurde, kann sie hier per Hand
 neu eingelesen werden.
 \param silent Varargs Flag, um Bildschirmausgabe zu unterdruecken
*/
varargs int RereadProgramLists(int silent)
{
  // Objekte laden, die zur Mudlib gehoeren
  known_programs = read_list("/secure/memory_lib");

  // Objekte laden, die nicht zur Mudlib gehoeren
  known_programs+=read_list("/secure/memory_nolib");

  if(!silent) printf ("%O\n", known_programs);

  return sizeof(known_programs);
}

/*! Zeiger auf den Speicher holen */
private mapping get_memory_pointer(){
  mixed info;

  // Die Fehlermeldungen sind etwas ausfuehrlicher, um dem Debugger
  // einen Hinweis auf Reparaturmoeglichkeiten zu geben
  if(!pointerp(info=get_extra_wizinfo(0)))
    raise_error("Wizinfo nicht ladbar. Entweder ich hab keine Rechte "
                "oder das Mud hat ein echtes Problem!\n");

  if(!mappingp(info[MEMORY_BUFF]))
    raise_error("Ich finde meine Daten in der Wizinfo nicht. Ein EM kann "
                "versuchen, mich neu zu laden, aber was die alten Daten "
                "betrifft, frei nach Catty: Keine Arme, keine Kekse!\n");
  // Da info[MEMORY_BUFF] immer ein Mapping ist (siehe Create), wird hier ein 
  // Pointer uebergeben. Dadurch werden Details zum Aufbau der extra_wizinfo
  // nur in dieser Funktion benoetigt.
  return info[MEMORY_BUFF]; 
}

/*!
  Pruefen, ob previous_object() Nutzungsrechte fuer den Memory hat.
  \return Gibt 1 zurueck, wenn der Aufrufende berechtigt ist, das 
          Objekt zu nutzen, sonst 0
*/
int HaveRights(){
  return stringp(get_caller_hash());
}

/*!
 Daten im Memory ablegen
 \param key ID unter der die Daten gespeichert werden
 \param var Zu speicherndes Datum
*/
int Save(string key, mixed var) {
  string who;
  mapping memory;

  // Hash des Aufrufers holen. Hat der keinen, ist er nicht befugt,
  // den Memory zu nutzen.
  if(!stringp(who=get_caller_hash())) return 0;

  // Wenn kein Key uebergeben wird, kann auch nichts gespeichert werden.
  if(!stringp(key)) return 0;

  // (hoffentlich) globalen memory_pointer holen
  memory = get_memory_pointer();

  // Wenn keine Daten da sind, muss man auch nichts anlegen. Rueckgabewert
  // ist dennoch 1, weil Load bei unbekannten Keys auch 0 liefert, die 
  // Null wird also korrekt gespeichert, irgendwie. Das muss aber nach dem
  // get_memory_pointer() passieren, da sonst das Korrekte Funktionieren von
  // Load nicht sichergestellt ist.
  if(var==0) return 1;

  // Erster Aufruf durch PO? Dann muss sein Speicherbereich initialisiert 
  // werden
  if(!member(memory,who))
    memory[who]=([]);

  // Endlich! Daten koennen gespeichert werden.
  memory[who][key]=var;

  return 1;
}

/*!
 Daten ausgeben/referenzieren
 \param key Id unter der die Daten gespeichert sind.
*/
mixed Load(string key) {
  string who;
  mapping memory;

  // Hash des Aufrufers holen. Hat der keinen, ist er nicht befugt,
  // den Memory zu nutzen.
  if(!stringp(who=get_caller_hash())) return 0;

  // Wenn kein Key uebergeben wird, kann auch nichts gespeichert werden.
  if(!stringp(key)) return 0;

  // (hoffentlich) globalen memory_pointer holen
  memory = get_memory_pointer();

  // Das Objekt hat noch nie Save() aufgerufen.
  if(!member(memory,who))
    return 0;

  // Unter diesem Key ist noch nichts gespeichert.
  if(!member(memory[who],key))
    return 0;

  // Gespeicherten Wert zurueckgeben
  return memory[who][key];
}

/*! 
 Ein Eintrag oder alle Eintraege werden geloescht
 \param key Id unter der die Daten gespeichert sind. 
            Wird kein Key uebergeben, werden alle Daten des Objekts 
            geloescht.
*/
varargs int Remove(string key) {
  string who;
  mapping memory;

  // Hash des Aufrufers holen. Hat der keinen, ist er nicht befugt,
  // den Memory zu nutzen.
  if(!stringp(who=get_caller_hash())) return 0;

  // (hoffentlich) globalen memory_pointer holen
  memory = get_memory_pointer();

  // Das Objekt hat noch nie Save() aufgerufen.
  if(!member(memory,who)) return 0;

  // Wenn kein Key uebergeben wird, wird alles zum Objekt gehoerende geloescht.
  if(stringp(key)){
    // Unter diesem Key ist nichts gespeichert.
    if(!member(memory[who],key)) return 0;

    // Wert unter Key fuer das File loeschen
    m_delete(memory[who], key);

  } else {
    // Alles fuer das File loeschen
    m_delete(memory, who);
  }

  return 1; 
}

varargs void ShowData(string user, string key)
{
  // Bekannte Objekte
  if (!ELDER_SECURITY )
    printf("I'm fine. Thanks for asking.\n");
  else {
    printf("known_programs:\n%O\n\n", known_programs);
    // EM duerfen sich auch noch zuscrollen lassen, wenn sie wollen
    if (ARCH_SECURITY && user)
    {
      mapping data = get_memory_pointer();
      if (!member(data,user))
        printf("memory: Keine Daten fuer %s vorhanden.\n",user);
      else if (user && key)
        printf("memory: Daten fuer User %s, Key %s:\n%O\n",user, key,
                  data[user][key]);
      else
        printf("memory: Daten fuer User %s:\n%O\n",user,data[user]);
    }
    else
    {
      mapping memory;
      memory = get_memory_pointer();
      // Andere bekommen eine anonymisierte Version
      printf("memory: (Nur Feldnamen, keine Daten)\n");
      foreach (string program, mapping data: memory ) {
        if ( mappingp(data) && sizeof(data) ){
          printf(break_string(
                   implode(m_indices(data),";\n"), 78,  
                   sprintf("  %-50s: ",program)),1);
        }
        else
        printf("  %-50s: - leer -\n", program);
      }
    }
  }
  return;
}

