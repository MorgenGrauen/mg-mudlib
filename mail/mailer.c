/* =========================================================================

   Ein Mailobjekt fuer Morgengrauen. 
   (C) 1993-97 Loco@MG. Unter Verwendung des std/post-codes von Jof
   
   Verwendung ausserhalb von Morgengrauen ist gestattet unter folgenden
   Bedingungen:
   - Benutzung erfolgt auf eigene Gefahr. Jegliche Verantwortung wird
     abgelehnt.
   - Auch in veraenderten oder abgeleiteten Objekten muss ein Hinweis auf
     die Herkunft erhalten bleiben.
   - Bitte Loco Bescheid sagen.
   Ein Update-Service besteht nicht. Diese Lizenz kann jederzeit
   zurueckgezogen werden.

   Letzte Aenderungen:
   190494 [Loco]  mail <spieler> fuehrt nicht ins mailmenue anschliessend.
   280494 [Loco]  .mailrc
   020594 [Loco]  system.mailrc, aliase anzeigen, Text retten, selfdestruct
                  in reset() im Fehlerfall, kleinere bugfixes
   070794 [Loco]  Kleinigkeiten (bugfixes und Optimierungen), '+'
   231195 [Loco]  Bereiche bei Speichern und Loeschen
   171295 [Loco]  Bereiche bei forward. Stapelweise Kleinigkeiten.
   191295 [Loco]  Kleinere Zeitoptimierungen, teilweise schiefgelaufen und
                  deswegen doch nicht, minor bugfixes.
   190295 [Loco]  keine folderlist beim hochstarten
   110696 [Loco]  Forwardserver-Unterstuetzung (.forward-aehnlich)
   031296 [Loco]  major update:
                  Erlaube in der Blueprint nichtinteraktives Versenden
                    vorbereiteter mails (z.B. fuer mpa: versende artikel);
		  Zeitoptimierungen durch effektiveres caching;
		  '-';
		  Einige optische Aenderungen;
		  Warnung bei grossen mailfiles;
		  Unterstuetzung von bcc's;
		  d,m,f,s erlauben Listen;
		  Abkuerzungen und '-' bei Folderargumenten
   080297 [Loco]  Zeitoptimierungen, interne Aenderungen, Bugfixes
   280103 [Muadib]Silentflag
   020304 [Vanion]In do_mail() wird jetzt M_NOCHECK gemovet, sonst hat ein
                  Mailer eines Spielers, der voll ist, kein Environment.
   100105 [Zook] Magier mit Level >= 26 bekommen erst spaeter eine
                 Mitteilung (Gebietswartung insb. f. Regionsmagier 
		 erfordert schlicht ein groesseres Postfach). 
   
----------------------------------------------------------------------------

Aufrufe von aussen zur Benutzung:

a) im geklonten Objekt: 
    obj->do_mail()        interaktives Mailmenue
    obj->do_mail(empf)    'halbinteraktiv': _eine_ mail, an Empfaenger str
   Objekt zerstoert sich nach Gebrauch selbst.
    Bei obj->do_mail("-silent") wird der Inhalt des Mailfolders nicht 
    aufgelistet. Ansonsten wie obj->do_mail()
        
b) in der blueprint:      nur non-interactive
    obj->do_mail(empf,titel,text)
       sendet mail mit Titel titel und Inhalt text an Empfaenger empf. 
       Aliase etc. werden von this_interactive() genommen.
       Nur fuer 'trusted objects' zugelassen, siehe #define TRUSTED

=========================================================================== */

#include "post.h"

inherit "/std/thing";
//inherit "std/more";   // wird vom nedit inherited
inherit NEDIT;

#include <properties.h>
#include <language.h>
#include <config.h>
#include <mail.h>
#include <wizlevels.h>
#include <moving.h>
#include <defines.h>
#include <input_to.h>

#undef DEBUG

#define TRUSTED(o) (objectp(o) && BLUE_NAME(o)=="/obj/mpa")
//		    (geteuid(o)=="p:service"|| \
//		     geteuid(o)==geteuid(this_interactive())))

#define MAILFILE(name) (MAILPATH+name[0..0]+"/"+name)
#define MAILFILEO(name) (MAILFILE(name)+".o")
#define MAILDEMON0297    // Maildemon-Version 8.Feb.97 vorhanden?

#define IS_NUMBER(n) (stringp(n) && sizeof(n) && n[0]>='0' && n[0]<='9')

#ifdef DEBUG
#define DEBUGVAR(x) if(find_player("tiamak")) tell_object(find_player("tiamak"),sprintf("Value of x is %O\n",x))
#else
#define DEBUGVAR(x)
#endif

// Zaehlung: akt_nr ab 1, akt_folder ab 0

static string subject, message, receiver, sender, *carbon;
#ifdef MAIL_SUPPORT_BCC
static string* blindcarbon;
#endif
static string name, office_name;
static int done, akt_folder, i, akt_nr, directflag;
static int folder_size,folder_date;                      // cacheverwaltung
static mapping aliases;

mixed *folders;

// Prototypen, die nun mal gebraucht werden

static varargs void SendMail(string text,int flag);
static void input();
static mixed process_names(mixed s);
static void get_carbon_copy(string str);
static varargs string* send_mail(mixed back);
string * unify_array(string * a);
static mapping Read_mailrc(string file);
static varargs void update(int directflag,int nocondflag);
static varargs void ListContent();
string GetFolderName(mixed fol);
static void LagWarning();
string GetReTitle(string s);
static string Message2string(int nr);
int * GetNumbers(mixed s);

protected void create() {
  ::create();
  seteuid(getuid());
  SetProp(P_IDS,({"mailer"}));
  SetProp(P_NAME,"mailer");
  office_name="Morgengrauens Post";
  akt_folder=-1;
  folder_size=folder_date=-42;
  SetProp(P_NODROP,1);
  SetProp(P_SHORT,0);
  SetProp(P_WEIGHT,0);
} 


public varargs void init(object origin) {
  (::init());
  init_rescue();
  add_action("postneustart","post");
  add_action("postneustart","mail");
}


void reset() {
  object pl;
  (::reset());
  if (!name) {remove();return;}
  pl=find_player(name);
  if (!pl || environment()!=pl) {remove();return;}
  if (nedittext && !query_input_pending(pl)) {
    tell_object(pl,"\
*** Hoppla! Du hast noch einen nicht fertiggeschriebenen Brief!\n\
*** Mit ~r kannst Du weiterschreiben.\n");
    return;
  }
  if (query_input_pending(pl)!=this_object())
     {remove();return;}
}


mixed postneustart() {
  if (!this_interactive() || name!=geteuid(this_interactive())
      || query_input_pending(this_interactive())
      || this_interactive()!=this_player()) return 0;
  write("Und weiter gehts...\n");
  if (nedittext) return RescueText();
  return input();
}


string SetOfficeName(string n) {
  return office_name=n;
}


static varargs void write_mail(mixed str, string std_subject, string text) {
  string str2;
  int h;
  
  carbon=process_names(str);
  if (!sizeof(carbon)) { 
    write("Kein Empfaenger angegeben!\n"),(directflag?remove():input());
    return;
  }
  write("Empfaenger: "+implode(map(carbon,#'capitalize/*'*/),", ")+"\n");
  str=carbon[0];
  if (sizeof(carbon)>=2) carbon=carbon[1..<1];
  else carbon=0;
  if ((str2=str) && str2!="" && str2[0]=='\\') str2=str2[1..];
  if (!str) { write("WEM willst Du schreiben?\n"),(directflag?remove():input()); return;
  }
  if (!master()->find_userinfo(str2) && member(str2,'@')==-1)
  {
    write("Mit dem Namen gibt es hier niemanden.\n"),(directflag?remove():input());
    return;
  }
  
  receiver=str;

  if (text) {
    subject=std_subject;
    return SendMail(text,1);  // flag 1: keine CC's bitte.
  }

  //write("Titel");
  //if (std_subject) write(" ("+std_subject+")");
  //write(": ");
  string pr;
  if (std_subject) pr = "Titel ("+std_subject+"): ";
  else pr = "Titel: ";
  subject=std_subject;
  input_to("get_subject",INPUT_PROMPT,pr);
  return;
}


static varargs void get_subject(string str,string pretext) {
  DEBUGVAR(str);
  DEBUGVAR(subject);
  if ((!str||str=="") && !subject)
  {
    input_to("get_subject",INPUT_PROMPT,"Titel (Abbrechen mit ~q): ");
    return;
  }
  if (str=="~q"||str=="~Q") return SendMail(0); // entspricht Abbruch im Editor.
  if (str && str!="") subject=str;
  write("Bitte gib jetzt Deine Nachricht an:\n\
** oder . wenn fertig, ~q fuer Abbruch, ~h fuer eine Hilfsseite\n");
  nedit("SendMail",pretext);
}


static varargs void SendMail(string text,int flag) {
  // flag: 1 = keine CC abfragen.
  if (!text) {
    write("Abbruch! Brief landet im Reisswolf.\n");
    if (directflag) {remove(); return;
    }
    
    subject=receiver=carbon=message=0;
    return input();
  }
  message=text;
  if (flag) return get_carbon_copy(0);
  //write("Cc: ");
  input_to("get_carbon_copy",INPUT_PROMPT, "Cc: ");
  return;
}


static void get_carbon_copy(string str) {	// Aufruf mit 0, wenn keine cc gewuenscht.
  int i,j;
  object p;
  string *oldcarbons,*h,*receivers;
  mapping orignames;
  
  oldcarbons=carbon;
  if (str=="~q") return SendMail(0); // Abbruch, entspricht Abbruch im Editor
  if (!str || str=="") carbon=0;
  else carbon=process_names(str);
  carbon=(oldcarbons ? oldcarbons : ({}))+(carbon ? carbon : ({}));

#ifdef MAIL_SUPPORT_BCC
  blindcarbon=filter(carbon,
      function status (string x)
        {if (x[0]=='-') return(1);
	 return(0);});
  carbon-=blindcarbon;
  blindcarbon=map(blindcarbon,
      function string (string x) 
        {return(x[1..]);} );
#endif

#ifdef MAIL_SUPPORT_BCC
  oldcarbons=({receiver})+carbon+blindcarbon; // speichere alle Originaladressen
#else
  oldcarbons=({receiver})+carbon;
#endif

  /* Forwards auswerten, dabei werden auch ungueltige Adressen gefiltert */
  /* orignames speichert die Zuordnung Zieladressen -> Originaladressen  */

  orignames=([]);
  h=explode(FWSERV->QueryForward(receiver),",");

  DEBUGVAR(h);
  for (j=sizeof(h);j--;)
    orignames[(h[j][0]=='\\'?h[j][1..]:h[j])]=({receiver[0]=='\\'?receiver[1..]:receiver});
  receiver=h[0];
  receivers=h[1..]; // Missbrauch dieser Variable!

  DEBUGVAR(orignames);

  for (i=sizeof(carbon);i--;) {
    h=explode(FWSERV->QueryForward(carbon[i])||"",",");
    for (j=sizeof(h);j--;) {
      h[j]=(h[j][0]=='\\'?h[j][1..]:h[j]);
      orignames[h[j]]=(orignames[h[j]]||({}))+({carbon[i][0]=='\\'?carbon[i][1..]:carbon[i]});
      receivers+=h;
    }
  }
  carbon=receivers;

#ifdef MAIL_SUPPORT_BCC
  receivers=({});
  for (i=sizeof(blindcarbon)-1;i>=0;i--) {
    h=old_explode(FWSERV->QueryForward(blindcarbon[i]),",");
    for (j=sizeof(h)-1;j>=0;j--) {
      h[j]=(h[j][0]=='\\'?h[j][1..]:h[j]);
      orignames[h[j]]=(orignames[h[j]]||({}))+({blindcarbon[i][0]=='\\'?blindcarbon[i][1..]:blindcarbon[i]});
      receivers+=h;
    }
  }
  blindcarbon=receivers;
#endif

  carbon=send_mail();
  receivers=({});

  if (!pointerp(carbon) || !sizeof(carbon)){
    write("Brief NICHT verschickt, da keine Empfaenger gefunden!\n");
  } else { 
    string *a; 
    DEBUGVAR(orignames);
    for (i=0;i<sizeof(carbon);i++){
      DEBUGVAR(carbon[i]);
      /* evtl. abfragen nach query_editing und/oder query_input_to */
      /* Benachrichtige Spieler, die ein forward gesetzt haben */
      a=orignames[carbon[i]];
      if (pointerp(a))
      {
        a=a-({carbon[i]});
        for (j=sizeof(a);j--;) 
          if (p=find_player(a[j])) 
	    tell_object(p,"Ein Postreiter ruft Dir aus einiger Entfernung zu, dass Du neue Post hast!\nDer Brief wurde wunschgemaess weitergeleitet.\n");
      }
      /* Benachrichtige Empfaenger */
#ifndef MAILDEMON0297
      if (p=find_player(carbon[i])) 
        tell_object(p,"Ein Postreiter ruft Dir aus einiger Entfernung zu, dass Du neue Post hast!\n");
#endif
      receivers+=orignames[carbon[i]]||orignames["\\"+carbon[i]]||({});
    }
    DEBUGVAR(carbon);
    write("Abgesandt an: "+implode(unify_array(map(receivers,#'capitalize)),", ")+"\n");//')));
  }
  for (i=sizeof(oldcarbons);i--;)
    if (oldcarbons[i][0]=='\\')
      oldcarbons[i]=oldcarbons[i][1..];
  oldcarbons=oldcarbons-receivers;
  if (sizeof(oldcarbons)) {
    write("Empfaenger unbekannt: "+implode(map(oldcarbons,#'capitalize),", ")+"\nIrgendjemand wirft Dir den zurueckgekommenen Brief zu.\n");//')))
    send_mail(oldcarbons);
  }

  message=receiver=carbon=subject=0;
  if (directflag) {
    if (directflag==1) remove(); // 1: Direktmodus, 2: non-interactive
    return;
  }
  return input();
}


static varargs string* send_mail(mixed back) {
  mixed *mail;
  
  mail=allocate(9);

#ifdef DEBUG
  DEBUGVAR(receiver);
  DEBUGVAR(carbon);
#endif

  if (!pointerp(carbon) || !sizeof(carbon)) carbon=0;
  mail[MSG_FROM]=this_player()->query_real_name();
  mail[MSG_SENDER]=office_name;
  mail[MSG_RECIPIENT]=(back ? mail[MSG_FROM] : receiver);
  mail[MSG_CC]=(back ? 0 : carbon);
#ifdef MAIL_SUPPORT_BCC
  mail[MSG_BCC]=blindcarbon;
#else
  mail[MSG_BCC]=0;
#endif

  mail[MSG_SUBJECT]=(back ? "Zurueck! Empfaenger unbekannt: "+implode(back,", ") : subject);
  mail[MSG_DATE]=ctime(time());
  mail[MSG_ID]="MorgenGrauen:"+time();
  mail[MSG_BODY]=message;
  return MAILDEMON->DeliverMail(mail,NO_SYSTEM_ALIASES|NO_USER_ALIASES);
}


varargs void do_mail(mixed str,string titel,string text) {
  // text und titel angegeben: versende Text, keine weiteren Taetigkeiten.

  mixed i;
  int silent;
  
  if (name) return; /* security flag :-) */
  if (!this_interactive()) return;
  if (!text) {
    name=geteuid(this_interactive());
    move(this_interactive(),M_NOCHECK);
    if (!name) remove();
  }
  aliases=Read_mailrc(ALIASFILE(geteuid(this_interactive())))+
	  Read_mailrc(SYSALIAS);
//  akt_folder=member_array("newmail",folders[0]);
//  if (akt_folder==-1) akt_folder=0;
  if (str) {
    str=lower_case(str);
    
    if(regreplace(regreplace(str," ","",1),"\t","",1)=="-silent")
    {
      silent=1;
    }
    else
    {
      directflag=1;
      if (text) {
        if (this_interactive()!=this_player()) return 0;
        if (!TRUSTED(previous_object())) return
	  write(break_string("WARNUNG!!! Objekt "+object_name(previous_object())+
	  		   " versucht, Mail mit Deinem Absender zu versenden! "
                             "Bitte Erzmagier oder Loco verstaendigen.",78)+
                             "\n");
        directflag=2;
        return write_mail(str,titel,text);
      }
      directflag=1;
      return write_mail(str);
    }
  }
  
  update(0,1);
  if (!pointerp(folders) || sizeof(folders)!=2 || sizeof(folders[0])==0) {
    write("Du hast im Moment keine Post !\n");
    folders=({({}),({})});
  }
  write("Du hast "+sizeof(folders[0])+" Ordner, Liste mit 'i'.\n");

  if(!silent)
  {
    ListContent();
  }
  
  write("Gesamtgroesse Deines Postfachs: "+
	(i=(file_size(MAILFILEO(name))+512)/1024)+" KB.\n");
  // Kleiner Hack als Uebergangsloesung, weil ich die dumme Warnung
  // nicht mehr sehen kann. ;^)
  // 22.10.2000, Tiamak
  if(IS_ARCH(this_interactive())) i=0;

  //
  // Kleiner Hack, damit Regionsmitarbeiter und groessere Magier
  // bei ihren Gebietswartungen nicht staendig mit nervigen
  // Meldungen belaestigt werden:
  //
  // 2005-01-10, Zook
  if (query_wiz_level(this_interactive()) > DOMAINMEMBER_LVL) {
    if (i>700) 
      write("*************************************************************\n"
            "* Dein Postfach ist zu gross! Bitte versuche aufzuraeumen   *\n"
	    "* und damit die Groesse zu reduzieren. Grosse Postfaecher   *\n"
	    "* verursachen unnoetiges Lag fuer die Spieler.              *\n"
	    "*************************************************************\n");
    i=0;
  }

  if (i>500) // Extra-Warnung fuer Catweazle
             // man koennte natuerlich auch eine Channel-Meldung ausspucken
             // mit dem Hinweis, wer das aktuelle Lag verursacht... vielleicht
             // ab 800 KB? ;-)
    write("*****************************************************************\n"
	  "* Dein Postfach hat eine absolut unakzeptable Groesse erreicht. *\n"
	  "* Uebergrosse Postfaecher verursachen unnoetiges Lag fuer alle! *\n"
	  "* Bitte raeume es dringend auf, d.h. loesche alle Briefe, die   *\n"
	  "* Du nicht _unbedingt_ benoetigst, oder lager sie aus und       *\n"
	  "* loesche sie anschliessend. Hauptsache, weg damit.             *\n"
	  "*****************************************************************\n");
  else if (i>300) // Warnung fuer bestimmte Magier und Seher ab 300 KB
    write("WARNUNG! Dein Postfach hat eine bedenkliche Groesse erreicht.\n"
	  "Beachte, dass uebergrosse Postfaecher nicht nur unnoetig Speicher, sondern\n"
	  "insbesondere auch Rechenzeit verbrauchen und damit groesseres Lag verursachen\n"
	  "koennen. Du solltest also dringend aufraeumen und alle nicht unbedingt\n"
	  "notwendigen Briefe loeschen (evtl. natuerlich vorher auslagern.\n");
  else if (i>200) // Hinweis fuer andere.
    write("Der Postbeamte macht dich darauf aufmerksam, dass Dein Postfach bereits\n"
	  "ziemlich voll ist und Du dringend einmal aufraeumen solltest.\n");
  else if (i>100) // Hinweis fuer andere.
    write("Der Postbeamte macht dich darauf aufmerksam, dass Dein Postfach\n"
	  "relativ voll ist.\n");
  

  if ((i=FWSERV->QueryForward(name))!=name)
    write("Du hast einen Nachsendeauftrag gestellt, Deine Post wird an\n"
	  +i+" weitergeleitet.\n");
  return input();
}


static void MediumHelpPage() {
  if (sizeof(folders[0])) 
    write("Aktueller Ordner ist \""+folders[0][akt_folder]+"\"\n");
  write("\n\
Brief <nr> lesen                       '<nr>'                 (lies <nr>)\n\
Aktueller / naechster / letzter Brief  '.' / '+' / '-'\n\
Brief schreiben                        'm <name>'             (schreibe)\n\
Brief beantworten                      'r [nr]'               (beantworte)\n\
Gruppenantwort an alle Empfaenger      'g [nr]'               (gruppe)\n\
Brief(e) loeschen                      'd [nummern]'          (loesche)\n\
Brief(e) weitersenden                  'f [nummern] <name>'   (weiter)\n\
Weitersenden plus eigenen Text         'F [nr] <name>'        (Weiter)\n\
Brief(e) in anderen Ordner verschieben 'v [nummern] <ordner>' (verschiebe)\n\
Mails in diesem Ordner listen          'l' (oder nichts)      (anzeigen)\n\
Aktuellen Ordner wechseln              'c <ordner>'           (oeffne)\n\
Neuen Ordner anlegen                   'n <ordnername>'       (erzeuge)\n\
Leeren Ordner loeschen                 'e <ordner>'           (entferne)\n\
Alle Ordner anzeigen                   'i'                    (ordner)\n\
"+ (IS_WIZARD(this_player()) ? "\
Brief(e) speichern in Datei            's [nummern]'          (speichere)\n\
  "+SAVEFILENAME+"\n" : "")+ "\
Mailaliase anzeigen                    'a'                    (aliase)\n\
Verfuegbare Kommandos zeigen           '?' oder 'h'\n\
Postmenue verlassen                    'q'\n\
Kommando <cmd> ausfuehren              '!<cmd>'\n\
[nummern] bedeutet: [nr|nr-nr [nr|nr-nr ...]]. (Liste von Nr und Bereichen)\n\
Bei der Langform reicht es, die ersten 4 Zeichen eines Kommandos anzugeben.\n\
Falls ein Befehl (z.B. \'v\' nicht funktioniert, pruefe bitte, ob dies ein \n\
clientseitiges Makro bei Dir ist.\n\
");
  return input();
}

static varargs int GetFolders(int nocondflag) {
// nocondflag: no condition, unbedingt neuladen

// Cache-Verwaltung endlich funktionsfaehig [251196]
// IDEE: Uhrzeit & Groesse untersuchen, ausserdem nach Verschieben neuladen.
//       Auch nach automatischem Verschieben (unread -> newmail)!


//  write("DEBUG: GetFolders called, old date "+folder_date+", old size "+folder_size+", nocondflag="+nocondflag+"\n");
  if (!nocondflag && 
      file_time(MAILFILEO(name))==folder_date &&
      file_size(MAILFILEO(name))==folder_size) return 0;

  if (!restore_object(MAILFILE(name))) folders=({({}),({})});
  folder_date=file_time(MAILFILEO(name));
  folder_size=file_size(MAILFILEO(name));
  if (!pointerp(folders) || sizeof(folders)!=2) folders=({({}),({})});
//  write("DEBUG: GetFolders finished, new date "+folder_date+", new size "+folder_size+"\n");
  return 1;
}

static varargs void update(int directflag,int nocondflag) {
  // directflag: Mailer wird im Direktmodus betrieben
  // nocondflag: Unbedingt neuladen

  int i,j,k,newletters;
  mixed *ignored;
  
  if (!GetFolders(nocondflag)) return; // es hat sich nix getan

  if (akt_nr<1) akt_nr=1;

  DEBUGVAR(akt_folder);

  if (akt_folder>=sizeof(folders[0]) || akt_folder<0) {
    akt_folder=member(folders[0], "newmail");
    if (akt_folder==-1) {
      MAILDEMON->MakeFolder("newmail",name);
      GetFolders(1);
      DEBUGVAR(folders[0]);
      akt_folder=member(folders[0], "newmail");
    }
    if (!directflag && akt_folder!=-1) write("Ordner 'newmail' aufgeschlagen.\n");
  }

//  if (!pointerp(folders)) return write("ERROR: folders no array in update\n"); // Kann eigentlich nicht vorkommen
  if (sizeof(folders[0]) && akt_nr>sizeof(folders[1][akt_folder]))
    akt_nr=sizeof(folders[1][akt_folder]);
  j=member(folders[0], "unread");
  if (j==-1) return;
  newletters=0;

  // Testweise eine neue Version, die aber voraussetzt, dass die Position von
  // unread in der Folderliste von /secure/mail waehrend der Aktion
  // nicht veraendert wird.
  // alt ausgeklammert, ausserdem ueberall 0 statt k
  //  k=0;
  // Neue Version wieder testweise drin

  //  while (j != -1 && pointerp(folders[1][j]) && sizeof(folders[1][j])>0) {
  for (k=0;k<sizeof(folders[1][j]);k++) {

    //    write("DEBUG: j="+j+"\n");
    
    if (pointerp(ignored=this_player()->QueryProp(P_IGNORE)) &&
		member(ignored, lower_case(folders[1][j][k][MSG_FROM]+".mail"))>=0) {
      mixed msg;
      msg=folders[1][j][k];
      write("Du laesst einen Brief von "+capitalize(msg[MSG_FROM])+
	    " unbesehen zurueckgehen.\n");
      msg[MSG_BODY]=this_player()->name()+" \
hat diesen Brief ungeoeffnet an Dich zurueckgehen lassen\n\
und moechte nicht mehr von Deinen Briefen belaestigt werden.\n\
Titel: "+msg[MSG_SUBJECT]+"\n\
------ Inhalt: ------------------------\n"+
  msg[MSG_BODY];
      msg[MSG_RECIPIENT]=msg[MSG_FROM];
      msg[MSG_SUBJECT]="Annahme verweigert - zurueck an Absender";
      msg[MSG_CC]=0;
      msg[MSG_BCC]=0;
      MAILDEMON->DeliverMail(msg,NO_SYSTEM_ALIASES|NO_USER_ALIASES);
      if (find_player(msg[MSG_RECIPIENT]))
	tell_object(find_player(msg[MSG_RECIPIENT]),
		    "Ein Postreiter ruft Dir aus einiger Entfernung leicht sauer zu, dass er einen\nzurueckgekommenen Brief fuer Dich hat.\n");
      MAILDEMON->RemoveMsg(0,j,name);
    } else {
    // Testweise durch DeleteUnreadFolder ersetzt (080297)
#ifndef MAILDEMON0297
      MAILDEMON->MoveMsg(0, j, "newmail", name);
#endif
      newletters++;
    }
    // GetFolders(1);
    // j=member_array("unread",folders[0]);
    // Letzte 2 Zeilen in "neuer" Version ersatzlos gestrichen
  }

#ifdef MAILDEMON0297
  MAILDEMON->DeleteUnreadFolder(name);
#else
  MAILDEMON->RemoveFolder("unread",name);
#endif
  if (newletters) {
    if (office_name=="mpa Kurierdienst") 
      write(break_string("Ein Kurier sagt \"Tach, Post!\", drueckt Dir "+
	    ((newletters==1) ? "einen neuen Brief" : newletters+" neue Briefe")
	    +" in die Hand und verschwindet wieder.\n",78));
    else 
      write("Du siehst, wie ein Postbeamter "+
	    ((newletters==1) ? "einen neuen Brief" : newletters+" neue Briefe")
	    +" in Dein Fach legt.\n");
  }
  GetFolders(1); // jetzt ohne unread, damit im endgueltigen Zustand.
  while (akt_folder>=sizeof(folders[0])) akt_folder--;
  if ((!akt_nr)&&sizeof(folders[1][akt_folder])) akt_nr=1;
}


static varargs void ListContent() {
  int i;
  update();
//  DEBUGVAR(folders[0]);
//  DEBUGVAR(folders[1]);
  DEBUGVAR(akt_folder);
  if (!pointerp(folders)||sizeof(folders)!=2||
      !pointerp(folders[0])||!sizeof(folders[0])) {
    write("Du hast keinen einzigen Ordner!\n");
    return;
  }
  write("Ordner "+folders[0][akt_folder]+": ");
  if (!pointerp(folders[1]) || akt_folder>=sizeof(folders[1]) || 
      !pointerp(folders[1][akt_folder])) {
    write("Dieser Ordner ist leer.\n");
    return;
  }
  write(sizeof(folders[1][akt_folder])+" Brief"+
	((sizeof(folders[1][akt_folder])!=1)?"e\n":"\n"));
  for (i=0;i<sizeof(folders[1][akt_folder]);i++){
    write(((i+1==akt_nr) ? "->" : "  ")+
	  sprintf("%3d: (%12s, %s) ",i+1,capitalize(folders[1][akt_folder][i][MSG_FROM]),
		  folders[1][akt_folder][i][MSG_DATE][5..11])+
	  folders[1][akt_folder][i][MSG_SUBJECT]+"\n");
  }
  return;
}


static void ChangeFolder(mixed x) {  
  if (!(x=GetFolderName(x))) return;
  akt_folder=member(folders[0],x);
  write("Du oeffnest den Ordner '"+x+"'.\n");
  if (akt_nr<=0) akt_nr=1;
  if (akt_nr>=sizeof(folders[1][akt_folder])) 
    akt_nr=sizeof(folders[1][akt_folder]);
  ListContent();
}


static void ListFolders() {
  int i;
  update();
  write("Du hast "+sizeof(folders[0])+" Ordner:\n");
  for (i=0;i<sizeof(folders[0]);i++)
    write(sprintf("%2s%3d: %-20s(%3d Briefe)\n",
		  ((i==akt_folder)?"->":"  "),
		  i+1,folders[0][i],sizeof(folders[1][i])));
  write("Gesamtgroesse Deines Postfachs: "+
	((file_size(MAILFILEO(name))+512)/1024)+" KB.\n");
}


static void MakeFolder(string s) {
  int ret;
  if (sscanf(s,"%d",ret)||s[0]<'a'||s[0]>'z') return
     write("Um Probleme zu vermeiden, duerfen Ordner nicht mit Nummern oder Sonderzeichen\nbezeichnet werden.\n");
  if (s=="newmail"||s=="unread") return
    write("Die Ordnernamen 'newmail' und 'unread' sind reserviert.\n");
  ret=MAILDEMON->MakeFolder(s, name);
  if (ret==1) write("Ok, neuer Ordner mit Namen "+s+" angelegt.\n");
  else write("Ein Ordner mit dem Namen existiert bereits.\n");
  return;
}


static int RemoveFolder(string x) {
  int ret;
  if (!x) return -42;    // folder existiert nicht, Fehlermeldung bereits geg.
//  if (intp(x)) x=folders[0][x];

  if (x=="newmail") return
    write("Der Ordnername 'newmail' ist reserviert.\nDieser Ordner darf nicht geloescht werden.\n"),-43;

  ret=MAILDEMON->RemoveFolder(x, name);
  switch (ret) {
  case 1: write("Ordner "+x+" geloescht.\n"); break;
  case -1: write("Kein solcher Ordner.\n"); break;
  case 0: write("Der Ordner war nicht leer - nicht geloescht.\n"); break;
  default: write("Fehler Nummer "+ret+" - was auch immer das heisst...\n"); break;
  }
  return ret;
}


static varargs int DeleteMessage(int *nrs) {
  int ret,x;
  mixed m;
  if ( sizeof(nrs) > 15 ) LagWarning();

  for (x=sizeof(nrs)-1;x>=0;x--) {
    write("Loesche Brief "+(nrs[x]+1)+": ");
    ret=MAILDEMON->RemoveMsg(nrs[x], akt_folder, name);
    switch(ret) {
    case 1: write("Ok.\n"); break;
    case 0: write("Kein solcher Brief im aktuellen Ordner.\n"); break;
    case -1:write("Kein aktueller Ordner.\n"); update(); return ret;
    default: write("MAILDEMON: Interner Fehler Nummer "+ret+"!\n"); break;
    }
  }
  
  return ret;
}

//"

static int MoveMessage(mixed msg,mixed fol) {
  int ret,i;
  
  for (i=0;i<sizeof(msg);i++) {
    ret=MAILDEMON->MoveMsg(msg[i]-i, akt_folder, fol, name);
    switch(ret) {
    case 1:
      write("Brief "+(msg[i]+1)+" verschoben nach "+fol+".\n");
      break;
    case 0: 
      write("So viele Briefe sind nicht im aktuellen Ordner.\n"); return 0;
    case -1:
      write("Seltsamer Fehler - duerfte eigentlich nicht passieren:\n'Kein aktueller Ordner.'\n"); return -1;
    case -3:
      write("Den Zielordner "+fol+" gibt es nicht!\n"); return ret;
    default:
      write("MAILDEMON: MoveMsg Interner Fehler "+ret+". Bitte Erzmagier verstaendigen.\n"); return ret;
    }
  }
  if (akt_nr>=sizeof(folders[1][akt_folder])) 
    akt_nr=sizeof(folders[1][akt_folder])-1;
  
  return ret;
}


static varargs int Reply(int nr,int group) {
  mixed to,dummy;
  if (!pointerp(folders)||!pointerp(folders[0])||
      sizeof(folders[0])<=akt_folder) {
    write("Seltsamer Fehler: Kein aktueller Ordner!\n");
    return 0;
  }
  if (nr<0 || !pointerp(folders[1][akt_folder]) ||
      sizeof(folders[1][akt_folder])<=nr){
    write("Einen Brief mit Nummer "+(nr+1)+" gibt es in diesem Ordner nicht!\n");
    return 0;
  }

  if (sscanf("\n"+lower_case(folders[1][akt_folder][nr][MSG_BODY]),
	     "%s\nreply-to:%s\n",dummy,to)==2) { // Reply-to gesetzt
    while (to[0]==' ') to=to[1..]; // ueberschuessige Leerzeichen entfernen
    while (to[<1]==' ') to=to[0..<2];
  }
  else 
    to=folders[1][akt_folder][nr][MSG_FROM];
  if (group) // Gruppenantwort
    to=({to,
        folders[1][akt_folder][nr][MSG_RECIPIENT]})+
       (pointerp(folders[1][akt_folder][nr][MSG_CC]) ? folders[1][akt_folder][nr][MSG_CC] : ({}))
       -({name});
#ifdef DEBUG
     DEBUGVAR(name);
     DEBUGVAR(to);
#endif
  write_mail(to,GetReTitle(folders[1][akt_folder][nr][MSG_SUBJECT]));
  return 1;
}


static varargs int Forward(mixed to,mixed nr,int appendflag) {
  mixed msg;
  if (!pointerp(folders)||!pointerp(folders[0])||
      sizeof(folders[0])<=akt_folder) {
    write("Seltsamer Fehler: Kein aktueller Ordner!\n");
    return 0;
  }
  if (nr<0 || !pointerp(folders[1][akt_folder]) ||
      sizeof(folders[1][akt_folder])<=nr){
    write("Nicht so viele Briefe in diesem Ordner!\n");
    return 0;
  }
  to=process_names(to);
  receiver=to[0];
  carbon=to[1..];
  subject="Fw: "+folders[1][akt_folder][nr][MSG_SUBJECT];
  message="Weitergesendeter Brief, urspruenglich von: "+
       folders[1][akt_folder][nr][MSG_FROM]+"\n\
-----------------------------\n\
"+folders[1][akt_folder][nr][MSG_BODY]+"\
-----------------------------\n";
  if (!appendflag) return get_carbon_copy(0),1;
  else {
    write("Text kann angehaengt werden\n");
    get_subject(subject,message);
  }
  return 1;
}



static int ForwardArea(mixed to,int * nrs) {
  mixed msg;

  if (!sizeof(nrs)) return 0;
  if (sizeof(nrs)==1) return Forward(to,nrs[0]);
  if (sizeof(nrs)>15) LagWarning();

  to=process_names(to);
  receiver=to[0];
  carbon=to[1..];
  subject="Fw: Gesammelte Briefe ("+dtime(time())[5..23]+")";
  message="";
  for (i=0;i<sizeof(nrs);i++) {
    write("Brief "+(nrs[i]+1)+": ");
    message+=Message2string(nrs[i])+
    "----------------------------------------------------------------------\n";
    write("Angehaengt.\n");
  }
/*
  if (!appendflag) {
*/
    return get_carbon_copy(0),1;
/*  }
  else {
    write("Text kann angehaengt werden\n");
    get_subject(subject,message);
  }
  return 1;
*/
}

//----------


static int ReadMessage(int nr) {
  if (nr<sizeof(folders[1][akt_folder]) && nr>=0)
    akt_nr=nr+1;
  message=Message2string(nr);
  if (!message) return 0;
  this_player()->More(message,0,#'input); //')
  return 1;
}


static string Message2string(int nr) {
  mixed letter;
  string message;
  int x;
  if (!pointerp(folders)||!pointerp(folders[0])||
      sizeof(folders[0])<=akt_folder){
    write("Seltsamer Fehler: Kein aktueller Ordner!\n");
    return 0;
  }
  if (!pointerp(folders[1][akt_folder]) ||
      sizeof(folders[1][akt_folder])<=nr ||
      nr<0) {
    write("Diese Nummer gibt es in diesem Ordner nicht!\n");
    return 0;
  }
  letter=folders[1][akt_folder][nr];
  message=
       "Absender: "+capitalize(letter[MSG_FROM])+"\n"+
       ((letter[MSG_FROM]==letter[MSG_SENDER]) ? "" :
	"Abgesandt aber von: "+capitalize(letter[MSG_SENDER])+"\n") +
       "An: "+capitalize(letter[MSG_RECIPIENT]);
  if (stringp(letter[MSG_CC]) && letter[MSG_CC]!="" ||
      pointerp(letter[MSG_CC]) && sizeof(letter[MSG_CC])) {
    message+="\nCc: ";
    if (!pointerp(letter[MSG_CC])) message+=capitalize(letter[MSG_CC]);    
    else message+=implode(map(letter[MSG_CC],#'capitalize),", ");//'))
  }
  message+="\nDatum: "+letter[MSG_DATE]+"\n"+
/* Sinnlos, oder? "Id: "+letter[MSG_ID]+"\n"+ */
      "Titel: "+letter[MSG_SUBJECT]+"\n\n"+
	letter[MSG_BODY]+"\n\n";
  return message;
}


static void LagWarning() {
  write("\
WARNUNG!!! Diese Aktion kann sehr lange benoetigen. Bitte sparsam verwenden,\n\
  um das Lag fuer alle ertraeglich zu halten. Falls die Aktion mit einem\n\
  Fehler abbricht, waren es wahrscheinlich zu viele Briefe auf einmal.\n\
  Dann kannst Du mit \"mail\" wieder in das Mailmenu einsteigen und solltest\n\
  es mit weniger Briefen versuchen.\n");
}


static varargs int SaveMessage(int * nrs) {
  int x,nr;
  string rest;
  mixed letter;

  if (!IS_WIZARD(this_player())) {
    write("Das koennen nur Magier!\n");
    return 0;
  }
  if (!sizeof(nrs)) {
    write("Speichere nichts.\n");
    return 1;
  }
  
  if ( sizeof(nrs) > 15 ) LagWarning();

  for (nr=0;nr<sizeof(nrs);nr++) {
    write("Speichere Brief "+(nrs[nr]+1)+": ");
    letter=Message2string(nrs[nr]);
    letter+="----------------------------------------------------------------------\n";
    if (!letter) {
      write("Speichern unmoeglich.\n");
      return 0;
    }
    if (!write_file(SAVEFILENAME, letter))
      write("Brief zu lang!\n");
    else
      write("Ok.\n");
  }
  write("Speichern nach "+SAVEFILENAME+" fertig.\nBitte denk dran, diese Datei wieder zu loeschen!\n");
  return 1;
}

// {'}

static void ListAliases() {
  mixed a;
  int i;
  string s;
  a=sort_array(m_indices(aliases),#'>); // ');
  s=( "Definierte Aliase:\n"
      "d.xyz        = Alle Mitarbeiter der Domain xyz\n"
      "freunde      = Deine Freunde (entsprechend Freundschaftsband)\n"
      "me           = "+(this_player()->QueryProp(P_MAILADDR))+"\n");
  for (i=0;i<sizeof(a);i++) 
    if (strstr(aliases[a[i]],"@")==-1) s+=sprintf("%-12s = %s\n",a[i],aliases[a[i]]);
  write(s);
}



/* ------ Das Mailmenue --------------------------------------------------*/


static void mail_cmds(string str) {

  string *strargs;
  int i,nrargs;
  
  update();

  if (!str || str=="" || !(nrargs=sizeof(strargs=old_explode(str[0..0]+lower_case(str[1..])," ")))) {
    ListContent();
    return input();
  }
  strargs[0]=strargs[0][0..3];
  if (IS_NUMBER(strargs[0])) {
    strargs=({"lies",strargs[0]});
    nrargs=2;
  }
  DEBUGVAR(strargs);
  switch (strargs[0]) {
  case "q":                    // quit
  case "quit": 
    remove(); return;
  case "?":                    // Hilfeseite
  case "hilf":
  case "h":
    MediumHelpPage();
    return;
  case "oeff":                 // change folder
  case "c": 
    if (nrargs<2) {
      write("Welchen Ordner willst Du oeffnen (Name, Nummer, +, -)?\n");
      break;
    }
    ChangeFolder(strargs[1]);
    break;
  case "ordn":                 // list folders
  case "i": 
    ListFolders();
    break;
  case "anze":                 // list content
  case "l":
    ListContent();
    break;
  case "alia":                 // list aliases
  case "a":
    ListAliases();
    break;
  case "erze":                 // make new folder
  case "n": 
    if (nrargs<2) {
      write("Bitte als Argument einen Namen fuer den neuen Ordner angeben!\n");
      break;
    }
    MakeFolder(lower_case(strargs[1]));
    break;
  case "entf":                 // delete folder
  case "e": 
    if (nrargs<2) {
      write("Bitte als Argument Name oder Nummer des zu loeschenden Ordners angeben.\n");
      break;
    }
    RemoveFolder(GetFolderName(strargs[1]));
    break;
  case "loes":                 // delete message
  case "d":
    if (nrargs==1) DeleteMessage(({akt_nr-1}));
    else DeleteMessage(GetNumbers(strargs[1..]));
    break;
  case "schr":                 // write mail
  case "m": 
    if (nrargs<2) {
      write("Bitte Empfaenger als Argument angeben!\n");
      break;
    }
    write_mail(strargs[1..]);
    return;
  case "vers":                 // move message to other folder
  case "verl":
  case "v":
    if (nrargs<2 || (nrargs>2 && !IS_NUMBER(strargs[1]))) {
      write("Syntax: v [nr|nr-nr [nr|nr-nr ...]] <ordnername>|<ordnernr>|+|-\n");
      break;
    }
    if (nrargs==2) MoveMessage(({akt_nr-1}),GetFolderName(strargs[1]));
    else MoveMessage(GetNumbers(strargs[1..<2]),GetFolderName(strargs[<1]));
    update(0,1);  // unbedingt neuladen.
    break;
  case "bean":
  case "r":
  case "grup":
  case "g":
    if (nrargs<2) { 
      if (Reply(akt_nr-1,(strargs[0][0]=='g'))) return;
      break;
    }
    if (!IS_NUMBER(strargs[1])) {
      write("Argumentfehler: Bitte Nummer des Briefes angeben, auf den sich die Antwort\n"
	    "beziehen soll. Ohne Argument bezieht sie sich auf den aktuellen Brief.\n");
      break;
    }
    if (nrargs>2) {
      write("Zu viele Argumente. Eine Antwort darf sich nur auf einen Brief beziehen!\n");
      break;
    }
    if (Reply(to_int(strargs[1])-1,(strargs[0][0]=='g'))) return;
    break;
  case "weit":
  case "f":
    if (nrargs<2 || 
	(IS_NUMBER(strargs[nrargs-1])&&sizeof(old_explode(strargs[nrargs-1],"@"))==1)) {
      write("Syntax: f [nr|nr-nr [nr|nr-nr ...]]  empfaenger [empf2 ...]\n");
      break;
    } 
    if (!IS_NUMBER(strargs[1])) {
      if (Forward(strargs[1..],akt_nr-1)) return; 
    }  // return, nicht break: input() wird von get_carbon_copy() aufger.
    else {
      int pos; // letzte Position, an der eine Nummer steht
      
      for (pos=nrargs-1;pos>1&&!IS_NUMBER(strargs[pos]);pos--);
      if (ForwardArea(strargs[(pos+1)..],GetNumbers(strargs[1..pos])))
	return;
    } 
    break;
  case "Weit":
  case "F":
    if (nrargs<2 || (nrargs==2 && IS_NUMBER(strargs[1]))) {
      write("Haeh? Bitte so: F [nr] empfaenger\n");
      break;
    }
    if (!IS_NUMBER(strargs[1])) {
      if (Forward(strargs[1..],akt_nr-1,1)) return;
      break;
    }
    if (IS_NUMBER(strargs[2])||member(strargs[1],'-')>=0) {
      write("Argumentfehler: Wenn Du eigenen Text anhaengen willst, darfst Du nur einen\n"
	    "Brief angeben, nicht mehrere.\n");
      break;
    }
    if (Forward(strargs[2..],to_int(strargs[1])-1,1)) return;
    break;
  case "lies":
    if (nrargs<2) { if (ReadMessage(akt_nr-1)) return; } else
    if (ReadMessage(to_int(strargs[1])-1)) return;
    break;
  case ".":
    if (ReadMessage(akt_nr-1)) return;
    break;
  case "+":
    if (akt_nr==sizeof(folders[1][akt_folder]))
      write("Noch weiter vorwaerts gehts nicht!\nMit 'c +' kannst Du den naechsten Ordner oeffnen.\n");
    else if (ReadMessage(akt_nr)) return;
    break;
  case "-":
    if (akt_nr==1)
      write("Noch weiter zurueck gehts nicht!\nMit 'c -' kannst Du den vorhergehenden Ordner oeffnen.\n");
    else if (ReadMessage(akt_nr-2)) return;
    break;
  case "spei":
  case "s":
    if ((nrargs==2 && !IS_NUMBER(strargs[1]))) {
      write("Syntax: s [nr|nr-nr [nr|nr-nr ...]]\n");
      break;
    }
    if (nrargs==1) (SaveMessage(({akt_nr-1})));
    else (SaveMessage(GetNumbers(strargs[1..])));
    break;
  default:
    write("Kommando nicht verstanden. Eine Hilfsseite bekommst Du mit 'h'.\n");
    break;
  }
  return input();
}


/*------------------------------------------------------------------------*/

static string prompt() {
  string path;
  
  update();
  if (!pointerp(folders)||!pointerp(folders[0])||
     sizeof(folders[0])<=akt_folder)
    path="(kein Ordner)";
  else
    path= "(" + folders[0][akt_folder] + ":" +
      ( sizeof(folders[1][akt_folder]) ?
       akt_nr + "/" + sizeof(folders[1][akt_folder]) :
       "leer") + ")";
  return(path + " [Hilfe mit h] => ");
}


static void input() {
  //prompt();
  input_to("mail_cmds", INPUT_PROMPT, prompt());
  return;
}


static mixed GetAlias(mixed a);

#ifdef MAIL_SUPPORT_BCC
static mixed RecurseProcessNames(mixed a);
#endif


static mixed process_names(mixed s) {
  mixed a1,a2,h;
  int i;
  string domain;

  if (stringp(s)) {
      a1=explode(regreplace(lower_case(s),","," ",1)," ")-({""});
  }
  else a1=s;
  a2=({});
  foreach(string str: a1)
    a2+=explode(str,",");

  a1=({});

//  printf("DEBUG ANFANG: %O\n",a2);

  foreach(string str: a2) { 
      if( !sizeof(str) ) continue;
      if (sscanf(str,"d.%s",domain)) {
	  h=(get_dir("/d/"+domain+"/*")||({}))-({".",".."});
	  // h immer ein Array
	  h=filter(h,#'query_wiz_level);
	  if (sizeof(h))
	    a1+=h;
	  else
	    a1+=({"d."+domain});
      }
      else if (str=="freunde") 
	a1+=("/p/service/tiamak/obj/fbmaster"->get_friends(getuid(this_player()), 8));
      else if (str=="me") 
	a1+=({this_player()->QueryProp(P_MAILADDR)});
      else if (aliases[str]) 
	a1+=GetAlias(str);
#ifdef MAIL_SUPPORT_BCC
    else if (str[0]=='-')
	a1+=map(RecurseProcessNames(str[1..]), function string (string x) {
	    return("-"+x);});
#endif
    else if ( (str[0]>='a' && str[0]<='z') 
	|| (sscanf(str,"%s@%s",domain,domain)) 
	|| str[0]=='\\') 
	a1+=({str});
  }
  
//  printf("DEBUG ENDE: %O\n",a1);

  a1=filter(a1,function int (string x) 
      { return(sizeof(x)>1); } );

  return(map(a1,#'lower_case));
} 


static mixed GetAlias(mixed a) { return process_names(aliases[a]); }
#ifdef MAIL_SUPPORT_BCC
static mixed RecurseProcessNames(mixed a) { return process_names(a); }
#endif


static mapping Read_mailrc(string file) {
  mapping al;
  int i;
  mixed ar;
  string s1,s2;

  if (!(ar=read_file(file))) {
//   write(file+" not readable\n");
    return ([]);
  }
  al=([]);
  ar=explode(ar,"\n");
  for (i=sizeof(ar)-1;i>=0;i--)
    if (sscanf(ar[i],"%s %s",s1,s2)==2) 
      al+=([s1:s2]);
//  printf("Got aliases %O",al);
  return al;
}


string * unify_array(string * a) {
//  int i;
//  for (i=sizeof(a)-1;i>=0;i--) a=a-({a[i]})+({a[i]});
// Rikus 14.02.2001
  a=m_indices(mkmapping(a));
  return a;
}


string GetReTitle(string s) {
  int nr,s2;

  if (!s) s="";
  if (s[0..7]=="Re: Re: ") return "Re^3: "+s[8..];
  else if (sscanf(s,"Re^%d: %s",nr,s2))
    return "Re^"+(nr+1)+": "+s2;
  else return "Re: "+s;
}


int * GetNumbers(mixed s) {
  int i,h1,h2;
  mixed ret;
  
  if (intp(s)) return ({s-1});
  if (stringp(s)) s=({s-1});
  if (!pointerp(s)) return 0;

  ret=({});

  for (i=sizeof(s)-1;i>=0;i--) {
    if (sscanf(s[i],"%d-%d",h1,h2)==2) {
      if (h2-h1>100) {
	write("Nicht so viele auf einmal, bitte.\n");
	return ({});
      }
      for (h1--;h1<h2;h1++) ret=ret-({h1})+({h1});
    }
    else
      ret=ret-({h1=to_int(s[i])-1})+({h1});
  }
  ret=sort_array(ret,#'>); //')
  DEBUGVAR(ret);
  if (ret[0]<0) {
    write("Illegale Nummer: "+(ret[0]+1)+", nichts unter 1 bitte!\n");
    return ({});
  }
  if (ret[<1]>=sizeof(folders[1][akt_folder])) {
    write("Illegale Nummer: "+(ret[<1]+1)+", so gross ist dieser Ordner nicht!\n");
    return ({});
  }
  return ret;
}


/*
int is_number(string s) {
  return (s[0]>='0'&&s[0]<='9');
}
*/


string GetFolderName(mixed fol) {  // int oder string. alles andere -> Fehler!
  mixed h;

  if (fol=="+") fol=akt_folder+1;
  if (fol=="-") fol=akt_folder-1;
  if ((!fol)||(intp(fol))||(IS_NUMBER(fol))) {
    if (!intp(fol)) fol=to_int(fol)-1;
    if (fol<0||fol>=sizeof(folders[0])) 
      return write("Einen Ordner mit Nummer "+(fol+1)+" gibt es nicht.\n"),0;
    return folders[0][fol];
  }
  fol=lower_case(fol);
  if (sizeof(h=regexp(folders[0],"^"+fol))==1) return h[0];
  if (member(folders[0],fol)==-1)
    return write("Einen Ordner mit Namen "+fol+" hast Du nicht.\n"),0;
  return fol;
}

int query_prevent_shadow() { return 1; }
