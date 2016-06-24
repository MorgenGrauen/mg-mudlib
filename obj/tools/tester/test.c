/*
**   Objekt-Tester - Mischung aus Rikus' Detailtester und Rochus' Test-NPC.
**
*****************************************************************************
**
**    Urspruengliche Objekte:
**    ~~~~~~~~~~~~~~~~~~~~~~~
**    /players/rikos/obj/rdet.c           Rikus@MorgenGrauen         25.05.96
**
**    /players/rochus/mon/tester.c        Rochus@MorgenGrauen        18.10.94
**
*****************************************************************************
**
**    Dieses Objekt:                                          [ Version 0.8 ]
**    ~~~~~~~~~~~~~~
**    /players/paracelsus/tools/otest.c   Paracelsus@Morgengrauen    28.02.98
**
**    Die Weitergabe, Verwendung oder Veraenderung ist nur erlaubt, wenn
**    diese Meldung nicht geaendert wird.
**    Die Benutzung erfolgt auf eigene Gefahr, jede Verantwortung wird
**    abgelehnt. Es wird keine Garantie gegeben, dass der Objekttester
**    einwandfrei, vollstaendig oder gar zufriedenstellend arbeitet.
**
*****************************************************************************
**
**    ChangeLog:
**    ~~~~~~~~~~
**    V 0.1    28.02.98 11:00:00   Paracelsus
**
**                                   Rikus' Raumtester kopiert, optimiert.
**
**    V 0.2    28.02.98 11:30:00   Paracelsus
**
**                                   Keine direkte Ausgabe mehr sondern
**                                   stattdessen ein More().
**
**    V 0.3    28.02.98 13:00:00   Paracelsus
**
**                                   Testen nicht mehr auf Raeume beschraenkt.
**
**    V 0.4    28.02.98 16:00:00   Paracelsus
**
**                                   Funktionen aus Rochus' Tester uebernommen
**
**    V 0.5    28.02.98 18:00:00   Paracelsus
**
**                                   Funktionalitaeten von Rikus und Rochus
**                                   aneinander angepasst.
**
**    V 0.6    28.02.98 19:00:00   Paracelsus
**
**                                   Gerueche und Geraeusche eingebaut,
**                                   erste Tests erfolgreich.
**
**    V 0.7    28.02.98 19:30:00   Paracelsus
**
**                                   Spaltenzahl einstellbar.
**
**    V 0.8    01.03.98 19:30:00   Paracelsus
**
**                                   Scannen ganzer Verzeichnisse. Geht jetzt
**                                   zumindest bei Raeumen.
**
**    V 0.9    20.09.98 20:45:00   Paracelsus
**
**                                   Umgestellt von heart_beat() auf
**                                   call_out(), damit this_player()
**                                   erhalten bleibt.
**	       13.10.08		   Chagall
**
**				     _query_details() und
**				     _query_special_details auf
**				     QueryProp() umgestellt
**
****************************************************************************
**
**   Eventuell auftretende Probleme:
**   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**   - Zu hohe eval_cost, wenn zu viele Details vorhanden sind.
**     Groesster bisher getesteter Raum (ohne Fehler) : 63907
**     Scheint also wohl doch kein Problem zu sein.
**
****************************************************************************
**
**   Ideen/ToDo:
**   ~~~~~~~~~~~
**
**   - NPC-Infos testen.
**
****************************************************************************
*/

#include <defines.h>
#include <properties.h>
#include <wizlevels.h>
#include <terminal.h>

#define DATAFILE     "test.dat"
#define DEFAULT_DATA "/obj/tools/tester/test.dat"

#define P_OTEST_TYP "ob-test:typ"
#define P_OTEST_SPL "ob-test:spalten"

#define T_ROCHUS 1
#define T_RIKUS  2

#define T_PAUSE  4

#pragma strong_types

inherit "/std/thing";

// Allgemein benoetigte globale Variablen

int     scantyp,spalten;
mapping all_dets;
mixed   output;
object  master;

// Globale Variablen fuer das scannen ganzer Directories

int     scanrun,scancount,scanpause,*scanerr;
string  scandir,*scanfiles;
object  testob;

string x_long()
{   string re,PRE,END;

    switch (PL->QueryProp(P_TTY))
    {
        case "ansi"  :
        case "vt100" : PRE=BOLD; END=NORMAL; break;
        default      : PRE="";   END="";     break;
    }

    re = break_string(
         "Mit diesem Objektpruefer kannst Du feststellen, welche Details, "+
         "Gerueche oder Geraeusche in einem Raum bzw an einem Objekt noch "+
         "nicht beschrieben sind.",78)+
         "\n"+
         "Syntax: %sotest [detail|smell|sound] {<objekt>[in mir|im raum]|"+
             "hier}\n\n"+
         "        otype [rikus|rochus]\n\n"+
         "        ocolm [1|2|3|4|5|6|7|8|auto]\n\n"+
         "        otdir <verzeichnis> <ausgabefile>%s\n\n"+
         break_string(
         "Mit %sotype%s kann man festlegen, ob man im 'Rikus'- oder im "+
         "'Rochus'-Modus scannen will, mit %socolm%s die anzahl der Ausgabe"+
         "spalten und mit %sotest%s wird dann die Auswertung gestartet. "+
         "Mit dem Kommando %sotdir%s kann man sogar ganze Verzeichnisse "+
         "auf einmal testen lassen. Das dauert natuerlich eine Weile. "+
         "Ausserdem muss ein Verzeichnis '/log/<magiername>/' bestehen.",78)+
         "Derzeitiger Scantyp:    %s'"+(scantyp==T_ROCHUS?"ROCHUS":"RIKUS")+
         "'%s\n"+
         "Derzeitige Spaltenzahl: %s%s%s\n%s";
    return sprintf(re,PRE,END,PRE,END,PRE,END,PRE,END,PRE,END,PRE,END,PRE,
           (spalten>0?sprintf("%d",spalten):"'AUTO'"),END,
           (scanrun?sprintf(
               "\nDerzeit Dirscan in:     %s%s%s\n"+
               "Angenommene Restzeit:   %s(%s) [Fi:%d/%d|Er:%d|Sk:%d]%s\n",
               PRE,scandir,END,PRE,
               time2string("%h:%02m:%02s",(scanrun-1)*4*T_PAUSE),
               (sizeof(scanfiles)-scanrun+1),(sizeof(scanfiles)),
               scanerr[0],scanerr[1],END):""));
}

void create()
{
    if (!clonep(ME))
        return;

    ::create();

// Allgemeine globale Variablen initialisieren

    scantyp   = T_ROCHUS;
    spalten   = 4;
    all_dets  = ([]);
    output    = 0;

// Globale Variablen fuer den Directoryscan initialisieren

    scandir   = 0;
    scanrun   = 0;
    scanfiles = ({});
    scancount = 0;
    scanpause = 0;
    scanerr   = ({0,0});

// Properties

    SetProp(P_SHORT,"Ein Objektpruefer") ;
    Set(P_LONG,#'x_long,F_QUERY_METHOD);
    SetProp(P_NAME,"Objektpruefer");
    SetProp(P_GENDER,MALE);
    SetProp(P_AUTOLOADOBJ,1);
    SetProp(P_VALUE,0);
    SetProp(P_WEIGHT,0);
    SetProp(P_NODROP,1);
    SetProp(P_NEVERDROP,1);

    AddId( ({"pruefer","objektpruefer"}) );

    AddCmd( "otest","search" );
    AddCmd( "otype","scanart" );
    AddCmd( "ocolm","spaltenzahl" );
    AddCmd( "otdir","dirtesten" );
}

void init()
{
    ::init();
    if (PL->QueryProp(P_TESTPLAYER) || IS_LEARNER(PL))
    {
        scantyp=(PL->QueryProp(P_OTEST_TYP)||T_ROCHUS);
        spalten=(PL->QueryProp(P_OTEST_SPL)||4);
        return;
    }
    if (find_call_out("removen") == -1)
      call_out("removen",0);
}

/***************************************************************************
**
**  Die Hilfsfunktionen des Objekttesters.
**
****************************************************************************
*/

void Output(string text)
{
    if (!text || !stringp(text))
        return;
    if (objectp(output))
        output->More(text);
    else if (stringp(output))
        write_file(output,text);
    else
        write(text);
}

int removen()
{
    write("Dieses Tool ist nur fuer Testies und Magier!\n");
    remove();
    return 1;
}

string *gross(string s)
{
    return regexp(regexplode(s,"[^A-Za-z0-9]"),"\\<[A-Z]");
}

string *det_auswerten(string input)
{   string *words,str,*re;
    int    anf,n,i;

    if (scantyp==T_RIKUS)
        return gross(input);

    re = ({});
    if (!input || !stringp(input))
        return re;

    words=regexplode(input,"[^A-Za-z0-9]");
        n=sizeof(words);
    if (!n)
        return re;

    anf=1;
    for (i=0;i<n;i++)
    {
        str=words[i];
        if (str=="" || str==" ")
            continue;
        if (str=="." || str==":" || str=="?" || str=="!")
        {
            anf=1;
            continue;
        }
        if (sizeof(regexp(({str}),"[^A-Za-z0-9]")))
            continue;
        if (sizeof(regexp(({str}),"\\<[0-9]*\\>")))
            continue;
        if (str!=capitalize(str))
            continue;
        if (anf)
        {
            anf=0;
            continue;
        }
        re += ({ str });
    }
    return re;
}

varargs mixed test_details(string str, string def)
{   string *strs,ostr,*strs2,suf;
    int    i;

    if (!str || !stringp(str) || !mappingp(all_dets))
        return 0;

    strs = ({str});
    ostr = str;

    if ((suf=str[<1..<1])=="s" || suf=="n" || suf=="e")
    {
        ostr  = str[0..<2];
        strs += ({ ostr });
        ostr += ("("+suf+")");
    }
    if ((suf=str[<2..<1])=="es" || suf=="en")
    {
        ostr  = str[0..<3];
        strs += ({ ostr });
        ostr += ("("+suf+")");
    }
    for ( strs2=({}),i=sizeof(strs)-1 ; i>=0 ; i--)
        if (member(all_dets,strs[i]))
        {
            if (stringp(def) && all_dets[strs[i]]==def && str!=SENSE_DEFAULT)
                continue;
            strs2 += ({ strs[i] });
	}
    if (sizeof(strs2)>0)
        return strs2;
    return ostr;
}

string *eval_detail_entry(mixed key)
{   mixed  re,h;
    string *ra;
    int    i;

    if (!key || !stringp(key) || !mappingp(all_dets) || !member(all_dets,key))
        return ({});

    re=all_dets[key];
    if (closurep(re))
        re=funcall(re,key);
    if (mappingp(re))
    {
        ra=filter(m_indices(re),#'stringp);
        for ( h=({ re[0] }),i=sizeof(ra)-1 ; i>=0 ; i-- )
            if (stringp(re[ra[i]]))
                h += ({ re[ra[i]] });
            else if (pointerp(re[ra[i]]))
                h += re[ra[i]];
        re=h;
    }
    if (pointerp(re))
        return filter(re,#'stringp);
    if (stringp(re))
        return ({ re });
    return ({});
}

/***************************************************************************
**
**  Die Funktion, die die (Special)Details im Raum untersucht.
**
****************************************************************************
*/

int search_d(object r)
{   int     i,j,room;
    mapping m;
    string  *s,s1,*s2,PRE,END,*special,*zufinden,*details,re,*uebergehen;
    mixed   mi;

    if (!r || !objectp(r))
    {
        write("OTEST-ERROR: Kein (existierendes) Zielobjekt gefunden!\n");
        return 1;
    }

// Die Ausgaben erfolgen zunaechst in einen String.

    re="\n";

// Handelt es sich um einen Raum?

    room = (function_exists("int_long",r) ? 1 : 0 );

// Terminaltyp des Magiers feststellen.

    if (objectp(output))
        switch (output->QueryProp(P_TTY))
        {
            case "ansi"  :
            case "vt100" : PRE=BOLD; END=NORMAL; break;
            default      : PRE="";   END="";     break;
        }
    else
    {
        PRE="";
        END="";
    }

//  Liste der zu uebergehenden Details erzeugen. Die verwendete Methode hat den
//  Vorteil, dass man nach einer Ergaenzung der Liste nicht das Tool neu laden
//  und clonen muss. Das Tool erwartet, die Liste DATAFILE im gleichen
//  Verzeichnis zu finden, in dem auch das Tool liegt.

    s1=implode(explode(old_explode(object_name(ME),"#")[0],"/")[0..<2],"/")+"/"
       DATAFILE;
    if (file_size(s1)<1)
        s1=DEFAULT_DATA;

    uebergehen = regexp(regexplode(read_file(s1),"[^A-Za-z0-9]"),"\\<[a-z]");

// Satz an Details, die auf jeden Fall vorhanden sein sollten, je nachdem
// ob P_INDOORS gesetzt ist oder nicht.

    if (room && r->QueryProp(P_INDOORS))
        zufinden = ({"Boden","Decke","Wand","Waende","Raum"});
    else if (room)
        zufinden = ({"Boden","Himmel","Sonne","Wolke","Wolken"});
    else
        zufinden = ({});

// Alle vorhandenen Details anzeigen.

//    details = sort_array(m_indices(r->_query_details()||([])),#'<);
    details = sort_array(m_indices(r->QueryProp(P_DETAILS)||([])),#'<);

    if (scanrun<1)
    {
        if ((i=sizeof(details))>0)
        {
            for ( s1="",--i; i>=0 ; i-- )
                s1 += (details[i]+(i?"\n":""));

            re += sprintf(sprintf("%sListe der vorhandenen Details:%s\n\n"+
                          "%s\n\n",PRE,END,("%-#78"+(spalten<1?"":
                          sprintf(".%d",spalten))+"s")),s1);
        }
        else
            re += sprintf("%sKeine Details gefunden.%s\n\n",PRE,END);
    }

// Alle vorhandenen SpecialDetails anzeigen.

//    special=sort_array(m_indices(r->_query_special_details()||([])),#'<);
    special=sort_array(m_indices(r->QueryProp(P_SPECIAL_DETAILS)||([])),#'<);

    if (scanrun<1)
    {
        if ((i=sizeof(special))>0)
        {
            for ( s1="",--i ; i>=0 ; i-- )
                s1 += (special[i]+(i?"\n":""));

            re += sprintf(sprintf(
                          "%sListe der vorhandenen SpecialDetails:%s\n\n"+
                          "%s\n\n",PRE,END,("%-#78"+(spalten<1?"":
                          sprintf(".%d",spalten))+"s")),s1);
        }
        else
            re += sprintf("%sKeine SpecialDetails gefunden.%s\n\n",PRE,END);
    }

//  Die Listen der vorhandenen Details und SpecialDetails vereinigen.

    details += special;

//  Details und SpecialDetails auswerten - gar nicht so einfach.

    all_dets = r->Query(P_DETAILS);

    for ( s=({}),i=sizeof(details)-1 ; i>=0 ; i--)
        s += eval_detail_entry(details[i]);

    for ( s2=({}),i=sizeof(s)-1 ; i>=0 ; i-- )
        if (stringp(s[i]))
            if (member(s2,s[i])==-1)
            {
                s2 += ({ s[i] });
                zufinden+=det_auswerten(s[i]);
            }

//  Grossgeschriebene Woerter aus der Langbeschreibung hinzufuegen.

    if (function_exists("int_long",r))
        zufinden += det_auswerten(
            old_explode(r->int_long(master,master),"Es gibt ")[0]);
    else
        zufinden += det_auswerten(old_explode(r->long(),"enthaelt:")[0]);

// Doppelte Eintraege eliminieren.

    for ( s2=({}), i=sizeof(zufinden)-1 ; i>=0 ; i--)
        if (member(s2,zufinden[i])==-1)
            s2 += ({ zufinden[i] });

// Alles klein machen.

    zufinden=map(s2,#'lower_case);

// Bei NPCs/Objekten die IDs rausfiltern.

    if (!room)
        zufinden -= r->QueryProp(P_IDS);

// Die zu uebergehenden Details rausfiltern.

    zufinden=filter((zufinden-uebergehen),#'stringp);

// Schauen, welche Details fehlen und diese Ausgeben.

    for ( s=({}),s2=({}),i=sizeof(zufinden)-1;i>=0;i--)
    {
        if (!(mi=test_details(zufinden[i])))
            continue;
        if (pointerp(mi))
        {
           for (j=sizeof(mi)-1 ; j>=0 ; j--)
               if (member(s,mi[j])==-1)
                   s += ({ mi[j] });
        }
        else if (stringp(mi) && member(s2,mi)==-1)
            s2 += ({ mi });
    }

    s  = sort_array(s, #'<);
    s2 = sort_array(s2,#'<);

    if (scanrun<1)
    {
        if ((i=sizeof(s))>0)
        {
            for ( s1="",--i ; i>=0 ; i-- )
                s1 += (s[i]+(i?"\n":""));

            re += sprintf(sprintf("%sListe der gefundenen Details:%s\n\n"+
                          "%s\n\n",PRE,END,("%-#78"+(spalten<1?"":
                          sprintf(".%d",spalten))+"s")),s1);
        }
        else
            re += sprintf("%sEs gibt keine Details.%s\n\n",PRE,END);
    }

    if ((i=sizeof(s2))>0)
    {
        for ( s1="",--i ; i>=0 ; i-- )
           s1 += (s2[i]+(i?"\n":""));

        re += sprintf(sprintf("%sListe der fehlenden Details:%s\n\n"+
                      "%s\n\n",PRE,END,("%-#78"+(spalten<1?"":
                      sprintf(".%d",spalten))+"s")),s1);
    }
    else
        re += sprintf("%sEs fehlen keine Details.%s\n\n",PRE,END);

    if (scanrun<1)
        re += sprintf("%sFERTIG.%s\n",PRE,END);

// Die eigentliche Textausgabe.

    Output(re);

    return 1;
}

/***************************************************************************
**
**  Hauptfunktion fuer das Testen von Geraeuschen und Geruechen.
**
****************************************************************************
*/

int search_sense(object r, int sense)
{   string re,PRE,END,WAS,*senses,s1,*zufinden,*s2,*s3,def;
    int    i,j;
    mixed  mi;

    if (!r || !objectp(r))
    {
        write("OTEST-ERROR: Kein (existierendes) Zielobjekt gefunden!\n");
        return 1;
    }
    if (sense==SENSE_SMELL)
    {
        all_dets=r->Query(P_SMELLS)||([]);
        WAS="Gerueche";
    }
    else if (sense==SENSE_SOUND)
    {
        all_dets=r->Query(P_SOUNDS)||([]);
        WAS="Geraeusche";
    }
    else
    {
        write("OTEST-ERROR: Illegaler sense-Wert in search_sense()\n");
        return 1;
    }

    re = "\n";

    zufinden = ({SENSE_DEFAULT});

    def = all_dets[SENSE_DEFAULT];

// Terminaltyp des Magiers feststellen.

    if (objectp(output))
        switch (output->QueryProp(P_TTY))
        {
            case "ansi"  :
            case "vt100" : PRE=BOLD; END=NORMAL; break;
            default      : PRE="";   END="";     break;
        }
    else
    {
        PRE="";
        END="";
    }

    senses = sort_array(m_indices(all_dets),#'<);

// Alle vorhandenen Sense-Details anzeigen.

    if (scanrun<1)
    {
        if ((i=sizeof(senses))>0)
        {
            for ( s1="",--i; i>=0 ; i-- )
                s1 += ((senses[i]==SENSE_DEFAULT?"DEFAULT":
                        senses[i])+(i?"\n":""));

            re += sprintf(sprintf("%sListe der vorhandenen %s:%s\n\n"+
                          "%s\n\n",PRE,WAS,END,("%-#78"+(spalten<1?"":
                          sprintf(".%d",spalten))+"s")),s1);
        }
        else
            re += sprintf("%sKeine %s gefunden.%s\n\n",PRE,WAS,END);
    }


//  Sense-Details auswerten - geht wie bei Details.

    for ( s2=({}),i=sizeof(senses)-1 ; i>=0 ; i--)
        s2 += eval_detail_entry(senses[i]);

    for ( s3=({}),i=sizeof(s2)-1 ; i>=0 ; i-- )
        if (stringp(s2[i]))
            if (member(s3,s2[i])==-1)
            {
                s3 += ({ s2[i] });
                zufinden+=det_auswerten(s2[i]);
            }

// Testen, welche Sense-Details fehlen und anzeigen

    for ( s2=({}),s3=({}),i=sizeof(zufinden)-1;i>=0;i--)
    {
        if (!(mi=test_details(zufinden[i],def)))
            continue;
        if (pointerp(mi))
        {
            for (j=sizeof(mi)-1 ; j>=0 ; j--)
                if (member(s2,mi[j])==-1)
                    s2 += ({ mi[j] });
        }
        else if (stringp(mi) && member(s3,mi)==-1)
            s3 += ({ mi });
    }
    s2 = sort_array(s2,#'<);
    s3 = sort_array(s3,#'<);

    if (scanrun<1)
    {
        if ((i=sizeof(s2))>0)
        {
            for ( s1="",--i ; i>=0 ; i-- )
                s1 += ((s2[i]==SENSE_DEFAULT?"DEFAULT":s2[i])+(i?"\n":""));

            re += sprintf(sprintf("%sListe der gefundenen %s:%s\n\n"+
                          "%s\n\n",PRE,WAS,END,("%-#78"+(spalten<1?"":
                          sprintf(".%d",spalten))+"s")),s1);
        }
        else
            re += sprintf("%sEs gibt keine %s.%s\n\n",PRE,WAS,END);
    }

    if ((i=sizeof(s3))>0)
    {
        for ( s1="",--i ; i>=0 ; i-- )
           s1 += ((s3[i]==SENSE_DEFAULT?"DEFAULT":s3[i])+(i?"\n":""));

        re += sprintf(sprintf("%sListe der fehlenden %s:%s\n\n"+
                      "%s\n\n",PRE,WAS,END,("%-#78"+(spalten<1?"":
                      sprintf(".%d",spalten))+"s")),s1);
    }
    else
        re += sprintf("%sEs fehlen keine %s.%s\n\n",PRE,WAS,END);

    if (scanrun<1)
        re += sprintf("%sFERTIG.%s\n",PRE,END);

// Die eigentliche Textausgabe.

    Output(re);

    return 1;
}

/***************************************************************************
**
**  Die Funktion, die die Geraeusche im Raum untersucht.
**
****************************************************************************
*/

int search_n(object r)
{
    if (!r || !objectp(r))
    {
        write("OTEST-ERROR: Kein (existierendes) Zielobjekt gefunden!\n");
        return 1;
    }

    return search_sense(r,SENSE_SOUND);
}

/***************************************************************************
**
**  Die Funktion, die die Gerueche im Raum untersucht.
**
****************************************************************************
*/

int search_m(object r)
{
    if (!r || !objectp(r))
    {
        write("OTEST-ERROR: Kein (existierendes) Zielobjekt gefunden!\n");
        return 1;
    }

    return search_sense(r,SENSE_SMELL);
}

/***************************************************************************
**
**  Die Funktion, die das eingegebene Kommando zerlegt und interpretiert.
**
****************************************************************************
*/

int search(string arg)
{   string *in,dum;
    object targ;
    int    wo;

    if (!(PL->QueryProp(P_TESTPLAYER)) && !IS_LEARNER(PL))
        return removen();

    if (scanrun)
    {
        write("Derzeit wird ein Verzeichnis gescanned. Bitte warten.\n");
        return 1;
    }

    notify_fail("OTEST-Syntax: otest [detail|smell|sound] "+
                 "{<objekt>[in mir|im raum]|hier}\n");

    if (!arg || !stringp(arg) || sizeof(in=old_explode(arg," "))<1)
    {
        output=PL;
        scanrun=0;
        master=PL;
        return search_d(environment(PL));
    }

    if (sizeof(in)>1)
    {
        arg=implode(in[1..]," ");
        if (member(({"hier","raum",""}),arg)!=-1)
            targ=environment(PL);
        else
        {
            if (sscanf(arg,"%s in mir",dum))
            {
                wo=1;
                arg=dum;
            }
            else if (sscanf(arg,"%s im raum",dum))
            {
                wo=-1;
                arg=dum;
            }
            else wo=0;
            if (wo!=-1)
                targ=present(arg,PL);
            if (!objectp(targ) && wo!=1)
                targ=present(arg,environment(PL));
        }
    }
    else
        targ=environment(PL);
    if (!objectp(targ))
    {
        write("OTEST-ERROR: Gewuenschtes Ziel nicht gefunden.\n");
        return 0;
    }

    output=PL;
    scanrun=0;
    master=PL;

    switch(in[0])
    {
        case "de" :
        case "detail" :
        case "details" :
            return search_d(targ);
        case "sm" :
        case "smell" :
        case "smells" :
            return search_m(targ);
        case "so" :
        case "sound" :
        case "sounds" :
            return search_n(targ);
    }
    return 0;
}

/***************************************************************************
**
**  Hier kann man den 'Scantyp' einstellen.
**
****************************************************************************
*/

int scanart(string arg)
{   int h;

    if (!(PL->QueryProp(P_TESTPLAYER)) && !IS_LEARNER(PL))
        return removen();

    if (scanrun)
    {
        write("Derzeit wird ein Verzeichnis gescanned. Bitte warten.\n");
        return 1;
    }

    notify_fail("OTEST-Syntax: otype [rikus|rochus]\n");
    if (!arg || !stringp(arg))
    {
        if (h=PL->QueryProp(P_OTEST_TYP))
            scantyp=h;
        if (!h)
        {
            PL->SetProp(P_OTEST_TYP,scantyp);
            PL->Set(P_OTEST_TYP,SAVE,F_MODE_AS);
        }
        printf("OTEST: Eingestellter Scantyp ist '%s'\n",
            (scantyp==T_ROCHUS?"ROCHUS":"RIKUS"));
        return 1;
    }
    if (member(({"rikus","rochus"}),arg)==-1)
        return 0;
    if (arg=="rochus")
    {
        scantyp=T_ROCHUS;
        PL->SetProp(P_OTEST_TYP,scantyp);
        PL->Set(P_OTEST_TYP,SAVE,F_MODE_AS);
        write("OTEST: Eingestellter Scantyp ist 'ROCHUS'\n");
        return 1;
    }
    scantyp=T_RIKUS;
    PL->SetProp(P_OTEST_TYP,scantyp);
    PL->Set(P_OTEST_TYP,SAVE,F_MODE_AS);
    write("OTEST: Eingestellter Scantyp ist 'RIKUS'\n");
    return 1;
}

/***************************************************************************
**
**  Hier kann man die Spaltenzahl einstellen.
**
****************************************************************************
*/

int spaltenzahl(string arg)
{   int h;

    if (!(PL->QueryProp(P_TESTPLAYER)) && !IS_LEARNER(PL))
        return removen();

    if (scanrun)
    {
        write("Derzeit wird ein Verzeichnis gescanned. Bitte warten.\n");
        return 1;
    }

    notify_fail("OTEST-Syntax: ocolm [1|2|3|4|5|6|7|8|auto]\n");

    if (!arg || !stringp(arg))
    {
        if (h=PL->QueryProp(P_OTEST_SPL))
            spalten=h;
        if (!h)
        {
            PL->SetProp(P_OTEST_SPL,spalten);
            PL->Set(P_OTEST_SPL,SAVE,F_MODE_AS);
        }
        printf("OTEST: Eingestellte Spaltenzahl ist '%s'\n",
            (spalten>0?sprintf("%d",spalten):"'AUTO'"));
        return 1;
    }
    if (arg=="auto")
    {
        spalten=-1;
        PL->SetProp(P_OTEST_SPL,spalten);
        PL->Set(P_OTEST_SPL,SAVE,F_MODE_AS);
        write("OTEST: Eingestellte Spaltenzahl ist 'AUTO'\n");
        return 1;
    }
    h = to_int(arg);
    if (h<1 || h>8)
        return 0;
    spalten=h;
    PL->SetProp(P_OTEST_SPL,spalten);
    PL->Set(P_OTEST_SPL,SAVE,F_MODE_AS);
    printf("OTEST: Eingestellte Spaltenzahl ist %d\n",spalten);
    return 1;
}

/***************************************************************************
**
**  Hier kann man ganze Verzeichnisse durchscannen lassen
**
****************************************************************************
*/

int dirtesten(string arg)
{   string vz,af,u,*dir;
    int    i,si;

    if (!(PL->QueryProp(P_TESTPLAYER)) && !IS_LEARNER(PL))
        return removen();

    if (scanrun)
    {
        write("Derzeit wird ein Verzeichnis gescanned. Bitte warten.\n");
        return 1;
    }

    if (object_name(ME)[0..2]=="/p/")
    {
        write(break_string("Ein Dirscan kann nur durchgefuehrt werden, "+
              "wenn man den Pruefer in sein /players/-Verzeichnis kopiert "+
              "hat oder von einem dort liegenden Objekt inherited.",78,
              "OTEST: "));
        return 1;
    }

    notify_fail("OTEST-Syntax: otdir <verzeichnis> <ausgabefile>\n");
    if (!arg || !stringp(arg) || sscanf(arg,"%s %s",vz,af)!=2)
        return 0;

// Zielfilename testen/festlegen.

    u=getuid(PL);
    if (file_size("/log/"+u+"/")!=-2)
    {
        printf("OTEST: Es existiert kein Verzeichnis '/log/%s/'\n",u);
        return 1;
    }
    for (i=sizeof(af)-1;i>=0;i--)
        if (((si=af[i])<48 && si!=46) || (si>57 && si<65) || si>122 ||
            (si>90 && si<97))
        {
            write("OTEST: Illegaler Filename fuer Ausgabefile.\n");
            return 1;
        }
    u=sprintf("/log/%s/%s",u,af);

// Zu untersuchendes Verzeichnis pruefen/einlesen.

    if (!vz || !stringp(vz) || sizeof(vz)<3 || vz[0]!='/' || vz[<1]!='/')
    {
        write("OTEST: Verzeichnisname muss mit '/' beginnen und aufhoeren.\n");
        return 1;
    }
    for (i=sizeof(af)-1;i>=0;i--)
        if (((si=af[i])<46) || (si>57 && si<65) || si>122 || (si>90 && si<97))
        {
            write("OTEST: Illegaler Filename fuer Verzeichnis.\n");
            return 1;
        }
    dir = get_dir(vz+"*.c");
    if ((si=sizeof(dir))<1)
    {
        write("OTEST: Das gewuenschte Verzeichnis existiert nicht oder ist "+
              "leer.\n");
        return 1;
    }

    output    = u;

    if (stringp(catch
          (i=write_file(output, sprintf("Teststart: %s\n\n",dtime(time())))))
        && i!=1)
    {
        write(break_string(
            sprintf("Fehler beim Schreiben in das File %s:\n%sAborting.",
                output,u),78,"OTEST: ",1));
        output = 0;
        return 1;
    }

    scanrun   = si+1;
    scanfiles = dir;
    scandir   = vz;
    scancount = 0;
    master    = PL;
    scanerr   = ({0,0});

    call_out("x_heart_beat",T_PAUSE);

    printf("OTEST: Scanne %d Files:\n"+
           "       %s => %s\n"+
           "Zeit:  etwa %s\n",si,vz,output,
               time2string("%h:%02m:%02s",si*4*T_PAUSE));

    return 1;
}

/***************************************************************************
**
**  Das Scannen von Verzeichnissen wird ueber x_heart_beat() erledigt.
**
****************************************************************************
*/

void x_heart_beat()
{   int    nr;
    string fn,er;

    call_out("x_heart_beat",T_PAUSE);

    if ((++scancount)>4)
    {
        scancount = 1;
        testob    = 0;
        scanrun--;
    }

    nr=scanrun-2;
    if (nr<0)
    {
        if (environment() && interactive(environment()))
            environment()->Message(break_string("'otdir' beendet.\n"+
                sprintf("Files: %d - Fehler: %d - Skips: %d",
                sizeof(scanfiles),scanerr[0],scanerr[1]),
                78,"OTEST: ",1),MSGFLAG_TELL);

        scanrun   = 0;
        scancount = 0;
        output    = 0;
        scanfiles = ({});
        scandir   = 0;
        testob    = 0;
        scanerr   = ({0,0});

        while(remove_call_out("x_heart_beat")!=-1);

        return;
    }
    fn=scandir+scanfiles[nr];
    if (fn[<2..]==".c")
        fn=fn[0..<3];

    if (scancount==1) // Testen, ob File ladbar
    {
        Output(sprintf("File: %s\n\n",fn));
        er=catch(call_other(fn,"???"));
        if (er && stringp(er))
        {
            Output(sprintf("Error: %s->Abort@Load\n\n",er));
            scanrun--;
            scancount  = 0;
            testob     = 0;
            scanerr[0] = scanerr[0]+1;
        }
        else
            testob = find_object(fn);
        return;
    }
    if (!objectp(testob))
    {
        Output("Objekt nicht gefunden\n-> Abort@Object.\n\n");
        scanrun--;
        scancount  = 0;
        scanerr[0] = scanerr[0]+1;
        return;
    }
    if (!function_exists("int_long",testob))
    {
        Output("Objekt ist kein Raum -> SKIPPING.\n\n");
        scanrun--;
        scancount  = 0;
        testob     = 0;
        scanerr[1] = scanerr[1]+1;
        return;
    }
    switch(scancount)
    {
        case 2 :
            if ((er=catch(search_d(testob))) && stringp(er))
            {
                Output(sprintf("Error: %s-> Abort@Detail.\n\n",er));
                scanrun--;
                scancount  = 0;
                testob     = 0;
                scanerr[0] = scanerr[0]+1;
            }
            return;
        case 3 :
            if ((er=catch(search_sense(testob,SENSE_SMELL))) && stringp(er))
            {
                Output(sprintf("Error: %s-> Abort@Smell.\n\n",er));
                scanrun--;
                scancount  = 0;
                testob     = 0;
                scanerr[0] = scanerr[0]+1;
            }
            return;
        case 4 :
            if ((er=catch(search_sense(testob,SENSE_SOUND))) && stringp(er))
            {
                Output(sprintf("Error: %s-> Abort@Sound.\n\n",er));
                scanrun--;
                scancount  = 0;
                testob     = 0;
                scanerr[0] = scanerr[0]+1;
            }
            return;
    }
    return;
}
