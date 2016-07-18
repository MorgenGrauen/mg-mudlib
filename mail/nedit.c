#pragma strict_types

/* nedit.c
   Editor fuer news, mail usw.
   Version 1 (C) 1993 Loco
   Version 2 (C) 1995 Wargon

   Verwendung ausserhalb von Morgengrauen ist gestattet unter folgenden
   Bedingungen:
   - Benutzung erfolgt auf eigene Gefahr. Jegliche Verantwortung wird
     abgelehnt.
   - Auch in veraenderten oder abgeleiteten Objekten muss ein Hinweis auf
     die Herkunft erhalten bleiben.
   Ein Update-Service besteht nicht.

   Anwendung:
    #inherit "/mail/nedit";      (zusaetzlich zu anderen inherits)

     nedit(string funcname);                    oder
    nedit(string funcname,string pretext);
    Nach Beendigung des Editors wird die Funktion funcname aufgerufen.
    Sie bekommt als Argument einen string mit dem fertigen editierten Text
    uebergeben bzw. 0, wenn der Editor mit ~q abgebrochen wurde.
    Optionales Argument pretext ist der zu editierende Text. Wenn 0 oder nicht
    uebergeben, wird ein neuer Text begonnen.
*/

#include <properties.h>
#include <wizlevels.h>
#include <defines.h>
#include <input_to.h>

#define TP this_player()

#define F_OVR 1  // Overwrite-Modus
#define F_BLK 2  // Blocksatz

#define MAX_LINES 1000

static mixed nedittext, nexitfunc, editor_used;
static int bstart, bend;  // Anfang und Ende des Blocks
static int cur;           // Aktuelle Zeile
static int len;           // Laenge des Gesamttextes
static int flags;          // Overwritemodus

static int get_edit_line(string str);
static int ShowWritten(int f, int l, int num);
static string killctrl( string str );
static int ShowHelp();
static int delLine(int l);
static void delBlock();
static int input_func();
varargs static void moveBlock(int start, int end, int real);

void init_rescue() {
  add_action("RescueText","~r");
}

private string nedit_prompt()
{
  if (sizeof(nedittext) <= MAX_LINES)
    return("]");
  else {
    nedittext = nedittext[0..MAX_LINES-1];
    len = MAX_LINES;
    if (cur >= MAX_LINES)
      cur = MAX_LINES-1;
    if (bstart >= MAX_LINES)
      bstart = MAX_LINES-1;
    if (bend >= MAX_LINES)
      bend = MAX_LINES-1;
    return sprintf("*** Mehr als %d Zeilen! Text wurde abgeschnitten! ***\n]", MAX_LINES);
  }
}

static varargs int nedit(string exitfunc,string pretext) {
  if (editor_used) {
    write("Offensichtlich schreibt hier schon jemand dran. Sorry.\n"+
                        "(Falls du es selbst bist, tippe ~r zur Wiederaufnahme einer verlorenen Sitzung)\n");
    return -1;
  }
  if (!pretext) nedittext=({""});
  else nedittext=explode(pretext,"\n");
  bstart = 0;
  bend = cur = len = sizeof(nedittext)-1;
  nexitfunc=exitfunc;
  flags = 0;
  editor_used=(string)TP->query_real_name();
  if (pretext)
    get_edit_line("~z");
  else {
    //nedit_prompt();
    input_to("get_edit_line", INPUT_PROMPT, nedit_prompt());
  }
  return 1;
}

static int get_edit_line(string str) {
  int err;
  int spaces;
  int fflag;
  int sl;

  if (!str) str="";
  str=killctrl(str); /* kleiner hack wegen CTRL-chars. */
  fflag = 0;

  sl = sizeof(str);

  if (str=="**" || str=="~." || str==".") {
    editor_used=0;
    str = implode(nedittext, "\n");
    nedittext = 0;
    call_other(this_object(),nexitfunc,str);
    return 1;
  }
  if (str[0..0]== "~" && sl >= 2) {
    if (sl == 2) {
      switch(str[0..1]) {
      // Abbruch:
      case "~q":
        editor_used = 0;
        return call_other(this_object(), nexitfunc, 0);
      // Temporaer rausgehen:
      case "~!":
        write("Mit ~r kannst Du weiterschreiben.\n");
        init_rescue();
        return 1;
      // Gesamten Text anzeigen
      case "~R":
        fflag = 1;
      case "~r":
        return ShowWritten(0, len, fflag);
      // Ausschnitt um Cursor anzeigen
      case "~Z":
        fflag = 1;
      case "~z":
        int f = (cur > 5) ? cur-5 : 0;
        int l = (cur < len-5) ? cur + 5 : len;
        return ShowWritten(f, l, fflag);
      // Hilfeseite anzeigen:
      case "~h":
        return ShowHelp();
      // Zeile ueber Cursor loeschen:
      case "~d":
        return delLine(cur-1);
      // Block loeschen:
      case "~D":
        delBlock();
        write( "Block geloescht.\n" );
        return input_func();
      // Overwrite-Modus toggeln:
      case "~v":
        flags ^= F_OVR;
        printf("%smodus eingeschaltet.\n",
              (flags & F_OVR) ? "Ueberschreib" : "Einfuege");
        return input_func();
      // Blocksatz toggeln:
      case "~b":
        flags ^= F_BLK;
        printf("%ssatz aktiviert.\n",
              (flags & F_BLK) ? "Block" : "Flatter");
        return input_func();
      // Statusinfo anzeigen:
      case "~s":
        printf("Zeile: %d | Laenge: %d | BStart: %d | BEnde: %d | "
               "Modus: %s/%s.\n",
                cur, len, bstart, bend, (flags&F_OVR)?"Ueber.":"Einfg.",
                (flags&F_BLK)?"Blocksatz":"Flattersatz");
        return input_func();
      // Block verschieben:
      case "~m":
        moveBlock(bstart, bend, 1);
        return input_func();
      // Block umformatieren:
      case "~F":
        int bs = bstart;
        str = implode(nedittext[bstart..bend], " ");
        delBlock();
        bstart = bend = cur = bs;
        fflag = -1;
        write("Block wurde umformatiert.\n");
        break;
      // Zeile umformatieren:
      case "~f":
        str = nedittext[cur];
        nedittext = nedittext[0..cur-1] + nedittext[cur+1..];
        if (cur <= bstart)
          bstart--;
        if (cur <= bend)
          bend--;
        len --;
        fflag = -1;
        write("Zeile wurde umformatiert.\n");
        break;
      } // switch
    } // if (sl == 2)
    else {
      // Cursorbewegungen an bestimmte Stellen:
      if (str[0..1] == "~c" && sl == 3) {
        fflag = 1;
        switch(str[2]) {
          case 'u': spaces = -1;           break;
          case 'd': spaces = 1;            break;
          case 't': spaces = -len;         break;
          case 'b': spaces = len;          break;
          case 's': spaces = bstart - cur; break;
          case 'e': spaces = bend - cur;   break;
          default:  spaces = 0;
                    fflag = 0;
                    break;
        }
      }
      // Cursorbewegung mit Zeilennummern:
      if (spaces || sscanf(str, "~c%d", spaces) == 1) {
        if (fflag>0 || str[2..2] == "+" || str[2..2] == "-")
          cur += spaces;
        else
          cur = spaces-1;

        if (cur < 0)
          cur = 0;
        else if (cur > len)
          cur = len;

        printf("%s\n",nedittext[cur]);
        return input_func();
      }
      // Blockgrenzen setzen:
      if ((err = (str[0..2] == "~bs")) || (str[0..2]=="~be")) {
        string out, p;
        int pos;

        int valid = -1;

        if (sl == 3)
          pos = valid = cur;
        else if (sscanf(str[3..], "%d%s", pos, p) == 2 && p=="")
          valid = pos--;

        if (valid >= 0) {
          if (err) {
            bstart = pos;
            if (pos > bend)
              bend = len;
            out = "Blockanfang";
          }
          else {
            if (pos < bstart)
              bstart = 0;
            bend = pos;
            if (len && bend == len)
              bend--;
            out = "Blockende";
          }
          printf("%s gesetzt.\n", out);
          return input_func();
        }
      }

      // Ersetzen:
      if ((sizeof(str) >= 8) && str[0..1] == "~s") {
        string *s1, *s2;
        int m;

        m = (str[2] == ' ') ? 3 : 2;

        if (sizeof(s1 = explode(str[m..], str[m..m])) == 4) {
          s2 = explode(nedittext[cur], s1[1]);
          if (sizeof(s2) > 1) {
            s2[1] = s2[0]+s1[2]+s2[1];
            nedittext[cur] = implode(s2[1..], s1[1]);
            if (s1[3] == "p")
              printf("%s\n", nedittext[cur]);
            else
              write("OK.\n");
            //nedit_prompt();
          }
          else {
            printf("\"%s\" nicht gefunden!\n", s1[1]);
            //nedit_prompt();
          }

          input_to("get_edit_line", INPUT_PROMPT, nedit_prompt());
          return 1;
        }
      }
    } // if (sl > 2)
  } // if (str[0..0] == "~")

  spaces=(sizeof(str) && (str[0]==' ' || str[0]=='\t'));
  if (spaces) str="$"+str;  /* Kleiner hack wegen fuehrenden Leerzeichen */
  str=break_string(str,78,0,(flags&F_BLK) ? BS_BLOCK|BS_NO_PARINDENT : 0);
  if (spaces) str=str[1..<1];
  if ( ((str[0..1]=="~r" && sizeof(str)>2) || str[0..1]=="~i") && 
       IS_LEARNER(TP) ) 
  {
    str=str[2..<2];
    if (str[0..0]==" ") str=str[1..<1];
    if (!str || catch(err=file_size(str=(string)"/secure/master"->_get_path(str,getuid(TP)))) || err<0) {
      write("File nicht gefunden.\n");
      //nedit_prompt();
      input_to("get_edit_line", INPUT_PROMPT, nedit_prompt());
      return 1;
    }
    str=read_file(str);
    if (!str){
      write("Zu gross!\n");
      //nedit_prompt();
      input_to("get_edit_line", INPUT_PROMPT, nedit_prompt());
      return 1;
    }
    write("Ok.\n");
  }
  if (str=="" || !str) str="\n";
  {
    string *x;
    int sx;

    x = explode(str, "\n")[0..<2];
    sx = sizeof(x);

    if (flags&F_OVR && !fflag) {
      nedittext = nedittext[0..cur-1] + x + nedittext[cur+1..];
      sx--;
    }
    else
      nedittext = nedittext[0..cur-1] + x + nedittext[cur..];

    if (cur < bstart)
      bstart += sx;
    if (cur <= bend)
      bend += (sx + fflag);
    cur += (sx + fflag + ((flags&F_OVR) ? 1:0));
    len += sx;

    // Kann beim Umformatieren des letzten Abschnitts vorkommen.
    if (nedittext[len] != "") {
      nedittext += ({ "" });
      len++;
    }
  }
  //nedit_prompt();
  input_to("get_edit_line", INPUT_PROMPT, nedit_prompt());
  return 1;
}

static int delLine(int l)
{
  string pr;
  if (l < 0)
    pr="Da ist nix mehr zum Loeschen!\n]";
  else {
    if (bstart >= l)
      bstart--;
    if (bend >= l)
      bend--;
    if (cur >= l)
      cur--;
    len--;
    nedittext=(nedittext[0..l-1]+nedittext[l+1..]);
    write("Vorherige Zeile geloescht.\n");
    pr=nedit_prompt();
  }
  input_to("get_edit_line", INPUT_PROMPT, pr);
  return 1;
}

static void delBlock()
{
  if (cur > bstart) {
    if (cur <= bend)
      cur = bstart;
    else
      cur -= (bend - bstart + 1);
  }
  if (bend == len)
    nedittext = nedittext[0..bstart-1];
  else
    nedittext = nedittext[0..bstart-1] + nedittext[bend+1..];

  bend = len = sizeof(nedittext)-1;
  bstart = 0;
  if (cur > len)
    cur = len;
}

varargs static void moveBlock(int start, int end, int real)
{
  int blen;
  string *block;

  if (cur >= start && cur <= end)
    write("Aber der Cursor steht innerhalb des Blocks!\n");
  else {
    block = nedittext[start..end];
    blen = sizeof(block)-1;
    delBlock();
    nedittext = nedittext[0..cur-1] + block + nedittext[cur..];
    if (real) {
      bstart = cur;
      bend = cur + blen;
    }
    len += (blen+1);
    write("OK.\n");
  }
}

#define UMLAUT ([ \
    'ä': "ae", \
    'ö': "oe", \
    'ü': "ue", \
    'Ä': "Ae", \
    'Ö': "Oe", \
    'Ü': "Ue", \
    'ß': "ss"  ])

static string killctrl( string str )
{
    int i, j;
    string tmp;

    tmp = "";
    
    for ( j = sizeof(str), i = 0; i < j; i++ )
        tmp += (UMLAUT[str[i]] || str[i..i]);

    tmp = regreplace( tmp, "[^ -~\t]", "", 1 );
    
    if (sizeof(tmp) &&  tmp[0] < 32 )
        return tmp[1..];
  
    return tmp;
}

mixed RescueText() {
  if (!nedittext || !editor_used)
    return notify_fail("Du hast nix angefangen zu schreiben!\n"),0;
  if (TP->query_real_name()!=editor_used)
    return notify_fail("Hier schreibt "+capitalize(editor_used)+"!\n"),0;
  if (query_input_pending(TP))
    return notify_fail("Du schreibst gerade schon irgendwas. Sorry...\n"),0;
  return ShowWritten(0, len, 0);
}

static int ShowWritten(int f, int l, int num) {
  string s, t, c, p, in;
  int i;

  if (num) {
    if (l >= 100) {
      p = "%3d%s%s";
      in = " ";
    }
    else {
      p = "%2d%s%s";
      in = "  ";
    }
  }
  else
    in = "";

  for (t="", i=l-1; i>=f; i--) {
    if (i == cur)
      c = ( ((i == bend) || (i == bstart)) ? "#" : "*");
    else if (i==bstart || i==bend)
      c = ">";
    else c = ":";

    if (num)
      t = (sprintf(p, i+1, c, nedittext[i])[0..75] + "\n" + t);
    else
      t = (c + nedittext[i]+"\n" + t);
  }

  if (l==len) {
    if (len == cur)
      c = ( (len == bend) ? "#" : "*");
    else if (len==bend)
      c = ">";
    else c = " ";
  }
  else c= " ";

  s="Das hast Du bisher geschrieben:\n"+in+" \
--------------------------\n\
"+t+in+c+"\
--------------------------";
  this_player()->More(s,0,symbol_function("input_func",this_object()));
  return 1;
}

static int ShowHelp() {
  this_player()->More("\
--------------------------\n\
Der Editor versteht folgende Befehle:\n\
--- Dateikommandos:\n\
~h            diese Hilfsseite\n\
~r / ~R     zeigt an, was Du bisher geschrieben hast\n\
~z / ~Z     zeigt den Textausschnitt um den Cursor herum\n\
"+(IS_WIZARD(TP) ? "\
~i filename fuegt eine Datei in den Text ein (auch ~r filename)\n\
" : "" )+ "\
~q            bricht ab\n\
** oder .   beendet sauber\n\
!<cmd>            fuehrt <cmd> aus, wie wenn Du gerade nicht schreiben wuerdest\n\
~!            verlaesst den Editor voruebergehend\n\
~s            Statusinformationen anzeigen\n\
~b            Wechselt zwischen Flatter- (default) und Blocksatz\n\
--- zeilenorientierte Kommandos:\n\
~d            loescht die letzte Zeile (Text-, nicht Eingabezeile)\n\
~v            wechselt zwischen Einfuege- (default) und Ueberschreibmodus\n\
~s !s1!s2!  Ersetzt das erste Vorkommnis des Strings s1 durch den String s2\n\
            in der aktuellen Zeile.\n\
            Statt durch ! koennen die Strings auch durch beliebige andere\n\
            Zeichen getrennt werden, die weder in s1 noch in s2 vorkommen.\n\
~f            Formatiert die aktuelle Zeile neu\n\
--- Cursorkommandos:\n\
~cu / ~cd   Cursor um eine Zeile nach oben/unten bewegen\n\
~ct / ~cb   Cursor an Anfang/Ende des Textes bewegen\n\
~cs / ~ce   Cursor an Blockanfang/Blockende bewegen\n\
~c<nr>            Cursor nach Zeile <nr> bewegen\n\
~c+<nr>     Cursor um <nr> Zeilen nach unten bewegen\n\
~c-<nr>     Cursor um <nr> Zeilen nach oben bewegen\n\
--- blockorientierte Kommandos:\n\
~bs/~bs<nr> setzt Blockanfang auf Cursorposition bzw. auf Zeile <nr>\n\
~be/~be<nr> setzt Blockende auf Cursorposition bzw. auf Zeile <nr>\n\
~F            formatiert den Block neu\n\
~D            loescht den markierten Block\n\
~m            verschiebt den markierten Block an die Cursorposition\n\
---\n\
Alles andere gilt als Text. Ueberlange Zeilen werden auf eine maximale\n\
Laenge von 78 Zeichen umgebrochen.\n\
Nach ~!, oder wenn man waehrend des Schreibens netztot wird, kann man mit\n\
 ~r wieder in den Editor einsteigen.\n\
--------------------------\n\
", 0, symbol_function("input_func", this_object()));
  return 1;
}

static int input_func() {
  //nedit_prompt();
  input_to("get_edit_line", INPUT_PROMPT, nedit_prompt());
  return 1;
}
