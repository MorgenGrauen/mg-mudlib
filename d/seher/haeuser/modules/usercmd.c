//  usercmd.c -- Modul fuer benutzerdefinierte Befehle
//
//  (c) 1995 Wargon@MorgenGrauen
//
// $Id: usercmd.c,v 1.4 2003/11/15 13:48:46 mud Exp $
//

#define NEED_PROTOTYPES
#include "../haus.h"
#include <properties.h>
#include <wizlevels.h>
#include <thing/properties.h>

private string ucFilter(string str);
private string ucText(string str);
private void splitCmd(string *cmd, string *verb, string *para);

create()
{
  Set(H_COMMANDS, SAVE, F_MODE);
  Set(H_COMMANDS, ([]));

  AddCmd("", "userCommands", 1);
}

/*
 * AddUserCmd: eigenes Kommando hinzufuegen
 *  cmd: String oder Array von Strings. Enthaelt entweder nur die Verben oder
 *         die kompletten Befehle (Verb + Parameter). Siehe auch pa.
 *  pa:  Zahl oder Array von Strings. Falls pa eine Zahl ist, so enthaelt cmd
 *         die kompletten Befehle (samt Parametern). Ansonsten enthaelt pa die
 *         Parameter fuer das entsprechende Verb in cmd.
 *         Ist pa = ({ "@NF@"}) , so handelt es sich um ein notify_fail, das an
 *         die Beschreibung des Verbs (falls vorhanden) angehaengt wird.
 *  me:  Text fuer den Ausfuehrenden. Der Text muss schon geparsed worden
 *         sein!
 *  oth: Text fuer die Umstehenden oder 0. Der Text muss schon geparsed wor-
 *         den sein!
 */
varargs void
AddUserCmd(mixed cmd, mixed pa, string me, string oth)
{
  int v,p;
  mapping cmds, desc;
  string *verb, *para, txt;

  cmds = Query(H_COMMANDS);
  if (stringp(cmd))
    verb = ({ cmd });
  else
    verb = cmd;

  if (intp(pa))
    splitCmd(verb[0..], &verb, &para);
  else {
    if (stringp(pa))
      para = ({ pa });
    else if (pointerp(pa))
      para = pa;
    for (desc = ([]), p=sizeof(para)-1; p>=0; p--)
      desc += ([ para[p] : me; oth ]);
  }

  for (v = sizeof(verb)-1; v>= 0; v--) {
    if (member(cmds, verb[v])) {
      if (intp(pa))
        cmds[verb[v]] += ([para[v] : me; oth ]);
      else
        cmds[verb[v]] += desc;
    }
    else {
      if (intp(pa))
        cmds += ([ verb[v] : ([para[v] : me; oth ]) ]);
      else
        cmds += ([ verb[v] : desc ]);
    }
  }
  Set(H_COMMANDS, cmds);
}

/*
 * RemUserCmd: Kommando(s) wieder entfernen
 *  com: String oder Array von Strings. Enthaelt das zu loeschende Kommando
 *         (Verb oder Verb + Parameter).
 *  all: Falls != 0, so werden die Verben aus com komplett mit allen Para-
 *         metern geloescht.
 */
varargs void
RemUserCmd(mixed com, int all)
{
  mapping cmd, tmp;
  string *verb, *para;
  int v, p;

  cmd = Query(H_COMMANDS);
  splitCmd(stringp(com) ? ({com}) : com, &verb, &para);

  if (all)
    for (v=sizeof(verb)-1; v>=0; v--)
      cmd = m_copy_delete(cmd, verb[v]);
  else {
    for (v=sizeof(verb)-1; v>=0; v--) {
      if (tmp = cmd[verb[v]]) {
        for (p=sizeof(para)-1; p>=0; p--)
          tmp = m_copy_delete(tmp, para[p]);
        cmd[verb[v]] = tmp;
      }
    }
  }
  Set(H_COMMANDS,cmd);
}

/*
 * userCommands: Auswertung der benutzerdefinierten Befehle.
 */
static int
userCommands(string str)
{
  mapping ucmd, uparm;
  string *parts, text;
  int i;

  ucmd = QueryProp(H_COMMANDS);
  str = this_player()->_unparsed_args(1)||"";
  if (uparm = ucmd[query_verb()]) {
    if (member(uparm, str)) {
      text = ucText(uparm[str,0]);
      if (sizeof(old_explode(text, "\n")) >= this_player()->QueryProp(P_SCREENSIZE))
        this_player()->More(capitalize(text)[0..<2]);
      else
        write(capitalize(text));

      if (uparm[str,1])
        say(ucText(uparm[str,1]));

      if (member(m_indices(QueryProp(P_EXITS)), query_verb()) == -1)
        return 1;
    }
    else {
      if (str && str != "" && text = uparm["@NF@"])
        notify_fail(implode(old_explode(text,"@F"),str));
    }
  }
  return 0;
}

/*** Functions private to this module... ***/

/*
 * ucFilter: Ersetzen von Name, Personal- und Possessivpronomen.
 *  Gibt den ersetzten String zurueck.
 */
private string
ucFilter(string str)
{
  int p,g;

  switch(str[0..1]) {
    case "@W": // Name in entsprechendem Fall...
      return this_player()->name(to_int(str[2..2]));
    case "@P": // Personalpronomen in entprechendem Fall...
      return this_player()->QueryPronoun(to_int(str[2..2]));
    case "@B": // Possesivpronomen in entprechendem Fall...
      p = to_int(str[4..4]);
      g = to_int(str[3..3]);
      return this_player()->QueryPossPronoun(g, to_int(str[2..2]), p);
  }
  return str;
}

/*
 * ucText: Rassen- und geschlechtsspezifische Teile bearbeiten sowie
 *  Namen und Pronomina einsetzen.
 *
 *  str enthaelt den String, der beim Beschreiben des Befehls
 *  eingegeben wurde. Bis auf den Default-Text sind alle Teile
 *  optional. Der String kann folgenden Aufbau haben:
 *  ------ schnipp ------
 *  Default-Text, der ausgegeben wird, wenn die folgenden spezielleren
 *  Texte nicht auftreten oder auf den Ausfuehrenden nicht zutreffen
 *  @NAME:nameA
 *  Text, der ausgegeben wird, wenn der Spieler "nameA" das Kommando
 *  eingegeben hat
 *  @NAME:nameB
 *  Und so weiter fuer Spieler "nameB" etc...
 *  @RA
 *  Text, der ausgegeben wird, wenn der Ausfuehrende auf der Erlaube-
 *  Liste des Hauses steht.
 *  @RE
 *  Text, der ausgegeben wird, wenn der Ausfuehrende ein Elf
 *  ist. Ebenso gibt es @RD fuer Dunkelelfen, @RF fuer Felinen,
 *  @RH fuer Hobbits, @RM fuer Menschen, @RG fuer Goblins  und @RZ 
 *  fuer Zwerge.
 *  ------ schnapp ------
 *
 *  Der Default-Text muss immer am Anfang stehen. Die anderen Bloecke
 *  koennen in beliebiger Reihenfolge folgen. Die Reihenfolge, in der
 *  die Bloecke betrachtet werden, ist dabei folgende:
 *  - zuerst werden @NAME-Bloecke untersucht
 *  - dann wird @RA (Erlaube-Liste) getestet
 *  - danach wird die Rasse ueberprueft (@RD/@RE/@RF/@RH/@RM/@RZ/@RG)
 *  - zuletzt wird der Default-Text betrachtet
 *
 *  Innerhalb jedes der Bloecke kann man noch mit @G zwischen
 *  maennlichen und weiblichen Vertretern unterscheiden (bei @NAME:
 *  macht das aber wenig Sinn). Beispiel:
 *  ------ schnipp ------
 *  @RZ
 *  Der Zwerg war maennlich
 *  @G
 *  Der Zwerg war weiblich
 *  ------ schnapp ------
 *
 *  Die Funktion gibt den fuer den Ausfuehrenden zutreffenden Text
 *  zurueck.
 */
private string
ucText(string str)
{
  string *parts, *names, *lnames;
  int i, n;

  // Text nach Namen- und Rassentrennern aufteilen
  parts = regexplode(str, "(@NAME:[A-Za-z1-9]*\n)|(@R[A-Z]\n)");
  i = -1;

  if (sizeof(parts) > 1) {

    // Zuerst wird nach Namenstrennern gesucht
    names = regexp(parts, "@NAME:");

    if (sizeof(names) > 0) {
      // ein kleiner Umweg, da der Name im Eingabestring nicht
      // notwendigerweise in Kleinbuchstaben vorliegt.
      lnames = map(names, #'lower_case);
      n = member(lnames, "@name:"+getuid(this_player())+"\n");

      if (n >= 0) {
        i = member(parts, names[n]);
      }
    }

    // Kein passender Namenstrenner gefunden: Erlaube-Liste
    // ueberpruefen
    if (i<0 && allowed_check(this_player())) {
      i=member(parts, "@RA\n");
    }

    // Weder Namenstrenner noch Erlaube-Liste passen: Rasse
    // ueberpruefen
    if (i<0) {
      switch(this_player()->QueryProp(P_RACE)) {
        case "Zwerg":
          i=member(parts, "@RZ\n");
          break;
        case "Elf":
          i=member(parts, "@RE\n");
          break;
        case "Feline":
          i=member(parts, "@RF\n");
          break;
        case "Hobbit":
          i = member(parts, "@RH\n");
          break;
        case "Dunkelelf":
          i = member(parts, "@RD\n");
          break;
        case "Goblin":
          i = member(parts, "@RG\n");
          break;
        case "Ork": 
          i = member(parts, "@RO\n");
          break;
        default:
          i=member(parts, "@RM\n");
          break;
      }
    }
    
    // Den richtigen Teil des Strings herauspicken
    if (i>-1)
      str = parts[i+1];
    else
      str = parts[0];
  }
  if (sizeof(parts = old_explode(str, "@G\n"))==2)
    str = parts[(this_player()->QueryProp(P_GENDER) == MALE ? 0 : 1)];

  parts = regexplode(str, "(@W[0-3]|@P[0-3]|@B[0-3][0-2][0-1])");
  parts = map(parts, #'ucFilter);
  return implode(parts, "");
}

/*
 * splitCmd: Komplettes Kommando in Arrays von Verben und Parametern zerlegen.
 *  cmd: Array von Strings. Dieses Array enthaelt die aufzuspaltenden Befehle.
 *  verb: Referenz auf ein Array von Strings fuer die Verben.
 *  para: Referenz auf ein Array von Strings fuer die Parameter.
 */
private void splitCmd(string *cmd, string *verb, string *para)
{
  int c, sp;

  for (verb = ({}), para = ({}), c = sizeof(cmd)-1; c>=0; c--) {
    if ((sp=member(cmd[c], ' ')) >= 0) {
      verb += ({ cmd[c][0..sp-1] });
      para += ({ cmd[c][sp+1..] });
    }
    else {
      verb += ({ cmd[c] });
      para += ({ "" });
    }
  }
}

// $Log: usercmd.c,v $
// Revision 1.4  2003/11/15 13:48:46  mud
// @RD als Trenner fuer Dunkelelfen
//
// Revision 1.3  2000/12/03 17:15:40  mud
// ucText: Neuer Trenner @NAME:foo, mit dem man in Seherhausbefehlen
// Texte fuer bestimmte Spieler vorsehen kann.
//
// Revision 1.2  2000/08/20 20:38:40  mud
// @RF als Trenner fuer Felinen
//
// Revision 1.1.1.1  2000/08/20 20:22:42  mud
// Ins CVS eingecheckt
//
// Revision 1.4  1996/04/19  23:10:52  Wargon
// @RA fuer Leute mit Erlaubnis
//
// Revision 1.3  1995/10/31  12:59:52  Wargon
// @RH fuer Hobbits
//
// Revision 1.2  1995/06/22  19:48:31  Wargon
// Bugfix in userCommands()
//
// Revision 1.1  1995/04/21  09:22:50  Wargon
// Initial revision
//
