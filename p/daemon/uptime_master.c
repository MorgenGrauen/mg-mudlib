#pragma strong_types, save_types
#pragma no_clone, no_shadow

#include "/p/daemon/ch.h"
#include "/p/daemon/uptime_master.h"

/// Prototypen ///

void create();
void reset(); 

void SaveMaxUptime();
mapping QuerMaxUptime();

/// Globale Variablen ///
static int next_save_uptime;
static int max_uptime_reached;
static int last_record_boot;

/// Funktionen ///

void create()
{
  seteuid(getuid(this_object()));
  reset();
}

void reset()
{
  call_out("SaveMaxUptime",1);
  set_next_reset(300);
}

// Ist eine Rekord-Uptime erreicht, wird diese hier gespeichert.
void SaveMaxUptime()
{
  int max_shut, max_boot;
  
  // Lohnt es, schon wieder zu pruefen?
  if (time()>next_save_uptime)
  {
    // Initialisieren
    if (next_save_uptime==0)
    {
      DEBUG("Masterinit");
      
      // Zeit berechnen, ab der ein Rekord aufgestellt ist.
      // Vorher braucht nixhts mehr geprueft werden
      sscanf(read_file(MAX_UPTIME)||"0 0","%d %d",last_record_boot, max_shut);
      next_save_uptime=last_reboot_time()+(max_shut-last_record_boot)+1; 
      
    } else {
      DEBUG("Rekord neu speichern.");
	    
      // Neuen Rekord in Datei schreiben.
      catch(rm(MAX_UPTIME);publish);
      write_file(MAX_UPTIME, last_reboot_time()+" "+time());
      
      // Jetzt checken, ob die Uptime grade geknackt wurde ...
      // Wenn ja, dann eine Meldung an den Master
      if(last_reboot_time()!=last_record_boot)
      {
	DEBUG("Rekordmeldung. MB:"+last_record_boot+" LU:"+last_reboot_time());

	// Nur einmal pro Rekord Uptime melden ...
	last_record_boot=last_reboot_time();

	// Hurraaah :)
        CHMASTER->join(RECORD_CHANNEL,this_object());
	CHMASTER->send(RECORD_CHANNEL,this_object(),
	"[<MasteR>:Lars] Wow, eine Rekord-Uptime.\n"
	"[<MasteR>:Merlin] Das erlebt man auch nicht alle Tage ...\n"
	"[<MasteR>:Der GameDriver] Dann wird es wohl Zeit fuer einen Reboot, oder?\n"
	"[<MasteR>:Merlin] Faules Stueck, arbeite weiter!\n"
	"[<MasteR>:Der GameDriver] Och menno!",
	  MSG_EMPTY);
	CHMASTER->leave(RECORD_CHANNEL,this_object());
      }
    }
  }
}

// Gibt Daten zur bisher maximalen Uptime zurueck: (299 Ticks)
// 1.) Wann wurde gebootet
// 2.) Wann endete die Uptime
// 3.) Wann wird dieser Rekord gebrochen?
// 4.) Dauer der Uptime als lesbarer String.
mapping QueryMaxUptime()
{
  int t, tmp, max_boot, max_shut;
  string s;

  sscanf(read_file(MAX_UPTIME)||"0 0","%d %d",max_boot, max_shut);

  t=max_shut-max_boot;

  s="";
  if (t>=86400)
    s+=sprintf("%d Tag%s, ",tmp=t/86400,(tmp==1?"":"e"));
  if (t>=3600)
    s+=sprintf("%d Stunde%s, ",tmp=(t=t%86400)/3600,(tmp==1?"":"n"));
  if (t>60)
    s+=sprintf("%d Minute%s und ",tmp=(t=t%3600)/60,(tmp==1?"":"n"));
  s+=sprintf("%d Sekunde%s",t=t%60,(t==1?"":"n"));

  return (["beginn":max_boot, 
           "ende":max_shut, 
	   "naechster rekord":last_reboot_time()+(max_shut-max_boot), 
	   "rekord als string":s]);
}
