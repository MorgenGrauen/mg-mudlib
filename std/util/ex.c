// MorgenGrauen MUDlib
//
// util/ex.c -- ex compatible editor
//
// $Id: ex.c 9142 2015-02-04 22:17:29Z Zesstra $

#pragma strong_types
#pragma save_types
#pragma pedantic
#pragma range_check
#pragma no_clone

inherit "/std/util/input";

private nosave mapping ctrl = ([]);
private nosave mixed buf;

#include <sys_debug.h>
#undef ARGS
#include <wizlevels.h>
#include <player.h>

#define MODE	0
#  define CMD		0
#  define INP		1
#define TEXT	1
#define LINE	2
#define FUNC	3
#define ARGS	4
#define FLAG	5
#  define NUM		1
#define CHG	6

#define YANK	"@YANK"

int evaluate(string in);

varargs int error(string msg, string arg)
{
  if(stringp(arg)) write(arg+": "+msg+"\n");
  else write(msg+"\n");
}

string substitute(string s, string regex, string n, string extra)
{
  mixed del; int i;
  if((i = sizeof(del = regexplode(s, regex))) < 2) return s;
  if(member(extra, 'g') == -1) i = 1;
  else i -= 2;
  while(i>0) {
    del[i] = n;
    i -= 2;
  }
  return implode(del, "");
}

// saveText() -- save edited text
//
int saveText()
{
  string text;
  text = implode(ctrl[buf][TEXT][1..], "\n")+"\n";
  error(sprintf("%O, %d Zeilen, %d Zeichen", 
	       buf ? buf : "", sizeof(ctrl[buf][TEXT])-1,
	       sizeof(text)));
  if(ctrl[buf][FUNC]) apply(ctrl[buf][FUNC], ctrl[buf][ARGS], text);
  ctrl = m_copy_delete(ctrl, buf);
  ctrl = m_copy_delete(ctrl, buf+"!");
  buf = 0;
  return 1;
}

// cmdPrompt() -- create command prompt
//
string cmdPrompt(int mode, mixed text, int line, int maxl)
{
  return ":";
}

// inputPrompt() -- create input prompt
//
string inputPrompt()
{
  if(ctrl[buf][FLAG] & NUM) return sprintf("[%02d] ", ctrl[buf][LINE]);
  return "";
}

string AddNum(string s)
{
  string r;
  r = inputPrompt()+s;
  ctrl[buf][LINE]++;
  return r;
}

void showLines(int from, int to)
{
  write(implode(map(ctrl[buf][TEXT][from..to], #'AddNum), "\n")+"\n");
  ctrl[buf][LINE] = from;
}

// commandMode() -- handle commands
//
int commandMode(string in)
{
  int from, to, swap;
  string cmd;

  if(!in) return 0;
  if(in[0..1] == "se") {
    ctrl[buf][FLAG] ^= NUM;
    return 0;
  }
    
  if(sscanf(in, "%d,%d%s", from, to, cmd) != 3)
    if(sscanf(in, "%d%s", from, cmd) != 2)
      if(!stringp(in)) return error("Kein Editorkommando", in);
      else { cmd = in; from = to = ctrl[buf][LINE]; }
    else to = from;

  if(from < 0) from = sizeof(ctrl[buf][TEXT])-1+from;
  if(to < 1) to = sizeof(ctrl[buf][TEXT])-1+to;
  if(to && to < from) return error("Erste Adresse groesser als die Zweite");
  if(from) ctrl[buf][LINE] = from;
  if(from > sizeof(ctrl[buf][TEXT])-1 ||
     to > sizeof(ctrl[buf][TEXT])-1) 
    return error("Nicht so viele Zeilen im Speicher");
  if(!to) to = from;
  switch(cmd[0])
  {
  case 'h':
    write("ex: Kommandohilfe\n\n"
	 "Alle Kommandos funktionieren nach folgendem Prinzip:\n"
	 "Adresse Kommando Argumente\n"
	 "Adressen werden gebildet durch: ZeileVon,ZeileBis\n"
	 "(ZeileBis kann weggelassen werden, samt Komma)\n"
	 "Kommandos:\n"
	 "  q,x -- Editor verlassen\n"
	 "  r   -- Datei einfuegen\n"
	 "         r<datei>\n"
	 "  a   -- Text hinter der aktuellen Zeile einfuegen\n"
	 "  i   -- Text vor der aktuellen Zeile einfuegen\n"
	 "  d   -- Zeilen loeschen\n"
	 "  y   -- Zeilen zwischenspeichern\n"
	 "  pu  -- Zwischengespeicherte Zeilen einfuegen\n"
	 "  s   -- Substitution von Text in Zeilen\n"
	 "         s/AlterText/NeuerText/\n"
          "  p,l -- Zeilen anzeigen\n"
	 "  z   -- Eine Seite anzeigen\n");
    break;
  case 'q':
    if(ctrl[buf][CHG])
      if(!(sizeof(cmd) > 1 && cmd[1]=='!'))
        return error("Datei wurde geaendert! Abbrechen mit q!");
      else ctrl[buf] = ctrl[buf+"!"];
  case 'x':
    if(saveText()) return 1;
  case 'r':
    if(!IS_WIZARD(this_player()))
      return error("Kommando gesperrt", cmd[0..0]);
    if(file_size(cmd[1..]) < 0) 
      return error("Datei nicht gefunden", "\""+cmd[1..]+"\"");
    ctrl[buf][TEXT] = ctrl[buf][TEXT][0..from] 
		  + explode(read_file(cmd[1..]), "\n")
	           + ctrl[buf][TEXT][(from == to ? to+1 : to)..];
    ctrl[buf][CHG] = 1;
    break;
  case 'a':
    ctrl[buf][MODE] = INP;
    write("Texteingabe: (. beendet zum Kommandomodus, ** beendet ganz)\n");
    input(#'inputPrompt, 0, #'evaluate); 
    ctrl[buf][CHG] = 1;
    return 1;
  case 'i':
    ctrl[buf][MODE] = INP;
    ctrl[buf][LINE]--;
    write("Texteingabe: (. beendet zum Kommandomodus, ** beendet ganz)\n");
    input(#'inputPrompt, 0, #'evaluate);
    ctrl[buf][CHG] = 1;
    return 1;
  case 'd':
    ctrl[buf][TEXT][from..to] = ({});
    ctrl[buf][CHG] = 1;
    break;
  case 'y':
    ctrl[YANK] = ctrl[buf][TEXT][from..to];
    if(to - from) error(to-from+1+" Zeilen gespeichert");
    break;
  case 's':
  {
    mixed reg, new, extra, scmd;
    if(sizeof(scmd = old_explode(cmd[1..], cmd[1..1])) < 2)
      return error("Substitution fehlgeschlagen");
    reg = scmd[0]; new = scmd[1];
    if(sizeof(scmd) > 2) extra = scmd[2];
    else extra = "";
    ctrl[buf][TEXT][from..to] = map(ctrl[buf][TEXT][from..to], 
				      #'substitute, 
                                          reg, new, extra);
    showLines(from, to);
    ctrl[buf][CHG] = 1;
    break;
  }
  case 'z':
    showLines(ctrl[buf][LINE], 
	     ctrl[buf][LINE]+this_player()->QueryProp(P_SCREENSIZE));
    ctrl[buf][LINE] += this_player()->QueryProp(P_SCREENSIZE);
    break;
  case 'p':
    if(sizeof(cmd) > 1 && cmd[1] == 'u')
    {
      if(!pointerp(ctrl[YANK])) return error("Keine Zeilen im Speicher");
      if(from >= ctrl[buf][LINE]) ctrl[buf][TEXT] += ctrl[YANK];
      else
        ctrl[buf][TEXT][0..from] + ctrl[YANK] + ctrl[buf][TEXT][from+1..];
      ctrl[buf][LINE] += sizeof(ctrl[YANK]);
      ctrl[YANK] = 0;
      showLines(ctrl[buf][LINE], ctrl[buf][LINE]);
      break;
    }
  case 'l':
  default:
    if(!from) 
    {
      error("Kein Editorkommando", sprintf("%c", cmd[0]));
      return 0;
    }
    showLines(from, to);
  }
  input(#'cmdPrompt, 0, #'evaluate);
  return 1;
}

// inputMode() -- handle input mode commands
//
int inputMode(string in)
{
  switch(in)
  {
  case ".": 
    ctrl[buf][MODE] = CMD;
    ctrl[buf][LINE]--;
    input(#'cmdPrompt, 0, #'evaluate);
    break;
  case "**":
    return saveText();
  default:
    if(!in) /* do nothing now */;
    if(ctrl[buf][LINE] < sizeof(ctrl[buf][TEXT])-1)
      ctrl[buf][TEXT] = ctrl[buf][TEXT][0..ctrl[buf][LINE]-1] + ({ in }) 
	        + ctrl[buf][TEXT][ctrl[buf][LINE]..];
    else ctrl[buf][TEXT] += ({ in });
    ctrl[buf][LINE]++;
    input(#'inputPrompt, 0, #'evaluate);
    break;
  }
  return 1;
}

// evaluate() -- evaluate input (in) in context (ctrl[buf])
//
int evaluate(string in)
{
  switch(ctrl[buf][MODE])
  {
    case INP: return inputMode(in);
    case CMD: return commandMode(in);
    default : return 0;
  }
}

void StartEX(string in, mixed c)
{
  if(!in || in == "j" || in == "J") ctrl[buf] = c;
  else if(in && (in != "n"  || in != "N"))
    return 0;
  ctrl[buf+"!"] = ctrl[buf];
  input(#'cmdPrompt, 0, #'evaluate);
}

varargs int ex(mixed text, closure func, mixed fargs, string buffer)
{
  int c, l;
  mixed ct;
  if(!text) text = "";
  c = sizeof(text);
  l = sizeof(text = explode(text, "\n")) - 1;
  ct = ({ CMD, text, 0, func, fargs, 0, 0});
  if(!ctrl[buffer]) StartEX(0, ct);
  else input(sprintf("Es existiert bereits Text! Verwerfen? [j]"),
	    0, #'StartEX, ct);
  return 1;
}
