// MorgenGrauen MUDlib
//
// living/put_and_get.c -- taking and putting things
//
// $Id: put_and_get.c 8755 2014-04-26 13:13:40Z Zesstra $

/*
  Grundlegend neu strukturiert von Amynthor im April-Juni 2007

Die eigentlichen Funktionen:

  private string put_or_get(object o, object dest)
    Bewegt ein einzelnes Objekt mit automatisch bestimmter Method. Gibt im
    Erfolgsfall 0 zurueck, sonst die auszugebende Fehlermeldung.

  varargs int drop(object o, mixed msg)
  varargs int put(object o, object dest, mixed msg)
  varargs int pick(object o, mixed msg)
  varargs int give(object o, object dest, mixed msg)
  varargs int show(object o, object dest, mixed msg)
    Der Spieler nimmt/legt/gibt/zeigt/laesst ein Objekt fallen, wobei die
    entsprechenden oder optional abweichende (im Format von P_XXX_MSG) oder
    gar keine (msg == 1) Meldungen ausgegeben werden. Es liegt in der
    Verantwortung des Rufenden, sinnvolle Werte zu uebergeben; insbesondere
    wird nicht geprueft, ob sich die Objekte in der Reichweite des Spielers
    befinden. Gibt 1 zurueck, wenn das Objekt bewegt wurde, sonst 0.

Hilfsfunktionen:

  private object *__find_objects(string *tokens, object env, int is_source)
  object *find_objects(string what, object env, int is_source)
    Sucht im Raum und im Spieler (oder alternativ in der angegebenen Umgebung)
    nach den in tokens/what bezeichneten Objekten. is_source bestimmt die
    erwartete grammatische Form (0 fuer "topf auf herd" und 1 fuer "topf von
    herd", siehe Manpage).

  varargs int drop_objects(string str, mixed msg)
  varargs int put_objects(string str, int casus, string verb, mixed msg)
  varargs int pick_objects(string str, mixed msg, int flag)
  varargs int give_objects(string str, mixed msg)
  varargs int show_objects(string str, mixed msg)
    Ein Befehl wie "wirf waffen weg" resultiert in einem Aufruf von
    drop_objects("waffen"). Diese Funktionen sind hauptsaechlich fuer die
    Behandlung der jeweiligen Kommandos vorgesehen, koennen jedoch auch fuer
    eigene Befehle verwendet werden. put_objects() erwartet ausserdem den
    Kasus ("Du kannst nichts an DER Pinwand befestigen.") und das verwendete
    Verb in der Gundform ("befestigen"). Das Flag fuer pick_objects() gibt
    an, ob das Objekt auch einfach herumliegen darf ("nimm ...") oder nicht
    ("hole ..."). Gibt bei Erfolg 1, sonst 0 zurueck.

  object *moved_objects()
  object moved_where()
    Gibt die eben fallengelassenen/gesteckten/... Objekte zurueck und wohin
    sie gesteckt/wem sie gegeben/gezeigt wurden. Fuer den Fall, dass man
    anschliessend noch etwas mit ihnen machen moechte.

Die einzelnen Kommandos:
  static int fallenlassen(string str)
  static int werfen(string str)
  static int legen(string str)
  static int stecken(string str)
  static int holen(string str)
  static int nehmen(string str)
  static int geben(string str)
    Minimale Wrapper fuer XXX_objects(), entfernen "fallen", "weg" bzw. "ab"
    aus den Argumenten und setzen entsprechende Standard-Fehlermeldungen.
    
  protected void add_put_and_get_commands()
    Registriert obige Funktionen per add_action().

Aus reinen Kompatibilitaetsgruenden weiterhin enthalten:

  object* find_obs(string str, int meth)
  int pick_obj(object ob)
  int drop_obj(object ob)
  int put_obj(object ob, object where)
  int give_obj(object ob, object where)
    siehe Manpages

*/

/*
  21. Okt 1998 komplette neu programmierung von put_and_get.c (Padreic)
- die Gruppenauswahlen alles, waffen und ruestungen sind jetzt immer moeglich
  die Gruppen sind sehr leicht erweiterbar und man sollte sich nicht scheuen
  davon gebrauch zu machen...
- mit "in mir" und "im raum" kann man den abzusuchenden Raum selbst eingrenzen
- mit "alle|jede|jeden|jedes <id>" kann man auch ganze objektgruppen auswaehlen
*/

#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <language.h>
#include <thing/description.h>
#include <thing/properties.h>
#include <moving.h>
#include <container.h>
#undef NEED_PROTOTYPES

#include <defines.h>
#include <properties.h>
#include <wizlevels.h>

#define TME(str) tell_object(this_object(), \
    break_string(str, 78, 0, BS_LEAVE_MY_LFS))
#define TOB(ob,str) tell_object(ob, break_string(str, 78, 0, BS_LEAVE_MY_LFS))
#define SAY(str) tell_room(environment(), \
    break_string(str, 78, 0, BS_LEAVE_MY_LFS), ({this_object()}))
#define SAY2(obs, str) tell_room(environment(), \
    break_string(str, 78, 0, BS_LEAVE_MY_LFS), ({this_object()}) + obs)
#define NF(str) _notify_fail(break_string(str, 78, 0, BS_LEAVE_MY_LFS))

private nosave closure cl;
private nosave string wen0, wen1, wer0;
private nosave object *last_moved_objects;
private nosave object last_moved_where;


/*********************** Die eigentlichen Funktionen ************************/

private string put_or_get(object o, object dest)

/* Bewegt ein einzelnes Objekt <o> in das Zielobjekt <dest>. Verwendet dazu
 * je nach Umstaenden (Ziel ist der Spieler, die Umgebung, ein Lebewesen) den
 * entsprechenden Wert fuer <method>. Gibt im Erfolgsfall 0 zurueck, erstellt
 * sonst die auszugebende Fehlermeldung und gibt diese zurueck.
 */

{
    int method, ret;
    string str;

    //if (living(o))
    //    raise_error(sprintf("Lebendes Argument fuer put_or_get: %O\n", o));

    if (dest == this_object())          /* pick */
        method = M_GET;
    else if (dest == environment())     /* drop */
        method = M_PUT;
    else if (living(dest))              /* give */
        method = M_GIVE;
    else {                              /* put */
        method = M_PUT | M_GET;
        if (first_inventory(o))
            return o->Name(WER, 1) + " ist nicht leer!";
    }

    if ((ret = o->move(dest, method)) > 0)
        return 0;

    switch (ret) {
        case ME_TOO_HEAVY:
            if (dest == this_object())
                if (QueryProp(P_GHOST))
                    return "Als Geist kannst Du nichts mitnehmen.";
                else
                    return "Du kannst " + wen1 + " nicht mehr tragen.";

            if (stringp(str = dest->QueryProp(P_TOO_HEAVY_MSG)))
                return capitalize(replace_personal(str, ({o, dest}), 1));

            if (living(dest)) {
                if (dest->QueryProp(P_GHOST))
                    return "Als Geist kann " + dest->name(WER, 1) +
                           " nichts mitnehmen.";
                else
                    return dest->Name(WER, 1) + " kann " +
                           wen0 + " nicht mehr tragen.";
            }

            if (dest == environment())
                return (stringp(str = dest->Name(WER, 1)) && sizeof(str) ?
                        str : "Der Raum") + " wuerde dann zu schwer werden.";

            return capitalize(wer0 + " passt in " + dest->name(WEN, 1) +
                              " nicht mehr rein.");

        case ME_CANT_BE_DROPPED:
            if (o && stringp(str = o->QueryProp(P_NODROP)))
                return str;

            if (dest == environment())
                return "Du kannst " + wen1 + " nicht wegwerfen!";

            if (living(dest))
                return "Du kannst " + wen1 + " nicht weggeben.";

            return "So wirst Du " + wen1 + " nicht los...";

        case ME_CANT_BE_TAKEN:
            if (o && stringp(str = o->QueryProp(P_NOGET)))
                return str;

            if (stringp(str = environment(o)->QueryProp(P_NOLEAVE_MSG)))
                return capitalize(
                    replace_personal(str, ({o, environment(o)}), 1));

            //if (dest != environment())
            //    return "Du kannst " + wen1 + " nicht einmal nehmen.";

            return "Du kannst " + wen1 + " nicht nehmen.";

        case ME_CANT_BE_INSERTED:
            if (stringp(str = dest->QueryProp(P_NOINSERT_MSG)))
                return capitalize(replace_personal(str, ({o, dest}), 1));

            if (dest == environment())
                return "Das darfst Du hier nicht ablegen.";

            if (dest == this_object())
                return "Du kannst " + wen1 + " nicht nehmen.";

            if (living(dest))
                return "Das kannst Du " + dest->name(WEM, 1) + " nicht geben.";

            return capitalize(wen0 + " kannst Du dort nicht hineinstecken.");

        case ME_CANT_LEAVE_ENV:
            // ME_CANT_LEAVE_ENV kann nur auftreten, wenn o ein Environment
            // hat, deshalb kein Check dadrauf
            if (stringp(str = environment(o)->QueryProp(P_NOLEAVE_MSG)))
                return capitalize(
                    replace_personal(str, ({o, environment(o)}), 1));

            if (environment(o) != this_object())
                return "Du kannst " + wen1 + " nicht nehmen.";

            if (dest == environment())
                return "Du kannst " + wen1 + " nicht wegwerfen!";

            if (living(dest))
                return "Du kannst " + wen1 + " nicht weggeben.";

            return "So wirst Du " + wen1 + " nicht los...";

        case ME_TOO_HEAVY_FOR_ENV:
            if (stringp(str = dest->QueryProp(P_ENV_TOO_HEAVY_MSG)))
                return capitalize(replace_personal(str, ({o, dest}), 1));

            if (environment(dest) == this_object())
                return dest->Name(WER, 1) +
                       " wuerde Dir dann zu schwer werden.";

            return (stringp(str = environment(dest)->Name(WER, 1))
                        && sizeof(str) ? str : "Der Raum") +
                    " wuerde dann zu schwer werden.";

        case TOO_MANY_OBJECTS:
            if (stringp(str = dest->QueryProp(P_TOO_MANY_MSG)))
                return capitalize(replace_personal(str, ({o, dest}), 1));

            if (dest == this_object())
                return "Soviele Gegenstaende kannst Du unmoeglich tragen!";

            if (dest == environment())
                return "Dafuer ist hier nicht mehr genug Platz.";

            if (living(dest))
                return dest->Name(WER, 1) + " kann "  + wen0 +
                       " nicht mehr tragen.";

            return "Dafuer ist nicht mehr genug Platz in " +
                   dest->name(WEM, 1) + ".";

        default:
            if (dest == this_object())
                return "Du kannst " + wen1 + " nicht nehmen.";

            if (dest == environment())
                return "Du kannst " + wen1 + " nicht wegwerfen!";

            if (living(dest))
                return "Du kannst " + wen1 + " nicht weggeben.";

            return capitalize(wen0 + " kannst Du dort nicht hineinstecken.");
    }
    return 0; // NOT REACHED
}


/* varargs int drop(object o, mixed msg)
 * varargs int put(object o, object dest, mixed msg)
 * varargs int pick(object o, mixed msg)
 * varargs int give(object o, object dest, mixed msg)
 * varargs int show(object o, object dest, mixed msg)
 *
 * Der Spieler nimmt/legt/gibt/zeigt/laesst ein Objekt fallen, wobei die
 * entsprechenden oder optional abweichende (im Format von P_XXX_MSG) oder
 * gar keine (msg == 1) Meldungen ausgegeben werden. Es liegt in der
 * Verantwortung des Rufenden, sinnvolle Werte zu uebergeben; insbesondere
 * wird nicht geprueft, ob sich die Objekte in der Reichweite des Spielers
 * befinden. Gibt 1 zurueck, wenn das Objekt bewegt wurde, sonst 0.
 */

varargs int drop(object o, mixed msg)
{
    string str;

    // vorher speichern, falls das Objekt zerstoert wird
    cl = symbol_function("name", o);
    wen0 = funcall(cl, WEN, 0);
    wen1 = funcall(cl, WEN, 1);
    wer0 = 0;

    if (!msg)
        msg = o->QueryProp(P_DROP_MSG);

    if (str = put_or_get(o, environment())) {
        TME(str);
        return 0;
    }

    if (!msg) {
        TME("Du laesst " + wen1 + " fallen.");
        SAY(Name(WER,1) + " laesst " + wen0 + " fallen.");
    } else if (pointerp(msg))
        switch (sizeof(msg)) {
          // Wenn es zwei Strings gibt, geht die 2. ans Environment
          case 2:
            SAY(replace_personal(msg[1], ({this_object(), o||wen0}), 1));
          case 1:
            TME(replace_personal(msg[0], ({this_object(), o||wen1}), 1));
            break;
          default:
            raise_error(sprintf(
                "Falsches Format fuer P_DROP_MSG: %O\n", o||wen1));
        }

    return 1;
}

varargs int put(object o, object dest, mixed msg)
{
    string str;

    // Falls das jemand von aussen ruft und Schrott uebergibt...
    //if (living(dest))
    //    raise_error(sprintf("Lebendes Ziel fuer put(): %O\n", dest));
    if (dest == environment())
        raise_error("Ziel fuer put() ist Umgebung des Spielers\n");

    // vorher speichern, falls das Objekt zerstoert wird
    cl = symbol_function("name", o);
    wen0 = funcall(cl, WEN, 0);
    wen1 = funcall(cl, WEN, 1);
    wer0 = funcall(cl, WER, 0);

    if (!msg)
        msg = o->QueryProp(P_PUT_MSG);

    if (str = put_or_get(o, dest)) {
        TME(str);
        return 0;
    }

  
    if (!msg) {
        TME("Du steckst " + wen1 + " in " + dest->name(WEN, 1) + ".");
        if (environment())
	  SAY(Name(WER, 1) + " steckt " + wen0 +
	      " in " + dest->name(WEN, 0) + ".");
    }
    else if (pointerp(msg)) {
        switch (sizeof(msg)) {
          case 2:
            if (environment())
	      SAY(replace_personal(msg[1], ({this_object(), o||wen0, dest}), 1));
          case 1:
            TME(replace_personal(msg[0], ({this_object(), o||wen1, dest}), 1));
            break;
          default:
            raise_error(sprintf(
                "Falsches Format fuer P_PUT_MSG: %O\n",o||wen1));
        }
    }

    return 1;
}

varargs int pick(object o, mixed msg)
{
    string str;

    // vorher speichern, falls das Objekt zerstoert wird
    cl = symbol_function("name", o);
    wen0 = 0;
    wen1 = funcall(cl, WEN, 1);
    wer0 = 0;

    if (!msg)
        msg = o->QueryProp(P_PICK_MSG);

    if (str = put_or_get(o, this_object())) {
        TME(str);
        return 0;
    }

    if (!msg) {
        TME("Du nimmst " + wen1 + ".");
        SAY(Name(WER, 1) + " nimmt " + wen1 + ".");
    } else if (pointerp(msg))
        switch (sizeof(msg)) {
          case 2:
            SAY(replace_personal(msg[1], ({this_object(), o||wen1}), 1));
          case 1:
            TME(replace_personal(msg[0], ({this_object(), o||wen1}), 1));
            break;
          default:
            raise_error(sprintf(
                "Falsches Format fuer P_PICK_MSG: %O\n", o||wen1));
        }

    return 1;
}

varargs int give(object o, object dest, mixed msg)
{
    string zname, gname;
    string str;

    // Falls das jemand von aussen ruft und Schrott uebergibt...
    if (!living(dest))
        raise_error(sprintf("Totes Ziel fuer give(): %O\n", dest));

    zname = dest->name(WEM, 1);
    gname = Name(WER, 1);

    // vorher speichern, falls das Objekt zerstoert wird
    cl = symbol_function("name", o);
    wen0 = funcall(cl, WEN, 0);
    wen1 = funcall(cl, WEN, 1);
    wer0 = 0;

    if (!msg)
        msg = o->QueryProp(P_GIVE_MSG);

    if (str = put_or_get(o, dest)) {
        TME(str);
        return 0;
    }

    if (!msg) {
        TME("Du gibst " + zname + " " + wen1 + ".");
        TOB(dest, gname + " gibt Dir " + wen0 + ".");
        SAY2(({dest}), gname + " gibt " + zname + " " + wen0 + ".");
    } else if (pointerp(msg))
        switch (sizeof(msg)) {
          case 3:
            TOB(dest, replace_personal(
                msg[2], ({this_object(), o||wen0, dest||zname}), 1));
          case 2:
            SAY2(({dest, this_object()}), replace_personal(
                 msg[1], ({this_object(), o||wen0, dest||zname}), 1));
          case 1:
            TME(replace_personal(
                msg[0], ({this_object(), o||wen1, dest||zname}), 1));
            break;
          default:
            raise_error(sprintf(
                "Falsches Format fuer P_GIVE_MSG: %O\n", o||wen1));
        }

    if (!query_once_interactive(dest))
        dest->give_notify(o);

    return 1;
}

varargs int show(object o, object whom, mixed msg)
{
    string zname, gname;
    string wen0, wen2, long;

    zname = whom ? whom->name(WEM, 1) : "allen";
    gname = Name(WER, 1);

    if (!msg)
        msg = o->QueryProp(P_SHOW_MSG);

    if (environment(o) == this_object() ||
        environment(environment(o)) == this_object()) {
        wen0 = o->name(WEN, 0);

        /* Der Akkusativ muss mit dem unbestimmten Artikel gebildet werden,
         * damit eventuelle Adjektive die richtige Endung besitzen.
         * (ein kleines Schwert -> kleines Schwert -> Dein kleines Schwert)
         */

        if (o->QueryProp(P_ARTICLE) && member(wen0, ' ') >= 0) {
            int obgender = o->QueryProp(P_GENDER);
            int obnum = o->QueryProp(P_AMOUNT) > 1 ? PLURAL : SINGULAR;

            // Wichtig: P_AMOUNT ist 0 fuer Objekte, die nicht von unit erben.
            // Da unit.c kein P_AMOUNT==0 zulaesst, nehmen wir diesen Fall als
            // singular an. *Rumata

            wen2 = wen0[member(wen0, ' ')..];
            wen0 = QueryPossPronoun(o, WEN, obnum) + wen2;

            if (obnum == PLURAL || obgender == FEMALE)
                wen2 = "Deine" + wen2;
            else if (obgender == MALE)
                wen2 = "Deinen" + wen2;
            else
                wen2 = "Dein" + wen2;
        } else
            wen2 = wen0;
    } else
        wen2 = wen0 = o->name(WEN, 1);

    // vorher speichern, falls das Objekt im catch_tell() zerstoert wird
    long = o->long(4);

    if (!msg) {
        TME("Du zeigst " + zname + " " + wen2 + ".");
        if (!whom)
            SAY(gname + " zeigt Dir " + wen0 + ".");
        else {
            TOB(whom, gname + " zeigt Dir " + wen0 + ".");
            SAY2(({whom}), gname + " zeigt " + zname + " " + wen0 + ".");
        }
    } else if (pointerp(msg))
        switch (sizeof(msg)) {
          case 3:
            if (whom)
                TOB(whom, replace_personal(
                    msg[2], ({this_object(), o||wen0, whom||zname}), 1));
            else
                SAY(replace_personal(
                    msg[2], ({this_object(), o||wen0, whom||zname}), 1));
          case 2:
            if (whom)
                SAY2(({whom, this_object()}), replace_personal(
                     msg[1], ({this_object(), o||wen0, whom||zname}), 1));
          case 1:
            TME(replace_personal(
                msg[0], ({this_object(), o||wen2, whom||zname}), 1));
            break;
          default:
            raise_error(sprintf(
                "Falsches Format fuer P_SHOW_MSG: %O\n", o||wen0));
        }

    if (!whom)
        SAY(long);
    else {
        TOB(whom, long);
        if (!query_once_interactive(whom))
            whom->show_notify(o);
    }

    return 1;
}


/***************************** Hilfsfunktionen *****************************/

/* private object *__find_objects(string *tokens, object env, int is_source);
 * object *find_objects(string what, object env, int is_source);
 *
 * Sucht im Raum und im Spieler (oder alternativ in der angegebenen Umgebung)
 * nach den in tokens/what bezeichneten Objekten. is_source bestimmt die
 * erwartete grammatische Form (0 fuer "topf auf herd" und 1 fuer "topf von
 * herd", siehe Manpage).
 */
 
private object *__find_objects(string *tokens, object env, int is_source)
{
    object ob, *obs;

    // is_source == 0: Objekt soll nicht bewegt werden ("topf auf herd")
    //              1: Objekt soll bewegt werden ("topf von herd")
    //              2: intern

    if (!env && sizeof(tokens) > 1 && tokens[<1] == "hier") {
        tokens = tokens[..<2];
        env = environment();
    }
    else if (!env && sizeof(tokens) > 2 && tokens[<2] == "in")
    {
        if (tokens[<1] == "mir" ||
            tokens[<1] == "dir") {
            tokens = tokens[..<3];
            env = this_object();
        }
        else if (tokens[<1] == "raum") {
            tokens = tokens[..<3];
            env = environment();
        }
    }

    for (int i = sizeof(tokens)-1; i > 1; i--) {
        if (env)
            ob = present(implode(tokens[i..], " "), env);
        else
            ob = present(implode(tokens[i..], " "), environment()) ||
                 present(implode(tokens[i..], " "), this_object());

        if (!ob)
            continue;

        if (living(ob)) {
            NF("Aber " + ob->name(WER, 1) + " lebt doch!");
            continue;
        }

        if (ob->QueryProp(P_CNT_STATUS)) {
            NF("Aber " + ob->name(WER, 1) + " ist doch geschlossen!");
            continue;
        }

        if (is_source != 0 &&
            tokens[i-1] == ob->QueryProp(P_SOURCE_PREPOSITION))
            return ob->present_objects(implode(tokens[..i-2], " "));

        if (tokens[i-1] == ob->QueryProp(P_PREPOSITION))
            return __find_objects(tokens[..i-2], ob, is_source ? 2 : 0);

        NF("Du kannst nichts " + tokens[i-1] + " " +
           ob->name(WEM, 1) + " nehmen.");
    }

    if (is_source == 2)
        return ({});

    if (env)
        return env->present_objects(implode(tokens, " "));

    if (environment() &&
        sizeof(obs = environment()->present_objects(implode(tokens, " "))))
        return obs;

    return present_objects(implode(tokens, " "));
}

object *find_objects(string what, object env, int is_source)
{
  if (!stringp(what) || !sizeof(what))
    return ({});
  return __find_objects(explode(what, " "), env, is_source);
}


/* varargs int drop_objects(string str, mixed msg);
 * varargs int put_objects(string str, int casus, string verb, mixed msg);
 * varargs int pick_objects(string str, int flag, mixed msg);
 * varargs int give_objects(string str, mixed msg);
 * varargs int show_objects(string str, mixed msg);
 *
 * Ein Befehl wie "wirf waffen weg" resultiert in einem Aufruf von
 * drop_objects("waffen"). Diese Funktionen sind hauptsaechlich fuer die
 * Behandlung der jeweiligen Kommandos vorgesehen, koennen jedoch auch fuer
 * eigene Befehle verwendet werden. put_objects() erwartet ausserdem den
 * Kasus ("Du kannst nichts an DER Pinwand befestigen.") und das verwendete
 * Verb in der Gundform ("befestigen"). Das Flag fuer pick_objects() gibt
 * an, ob das Objekt auch einfach herumliegen darf ("nimm ...") oder nicht
 * ("hole ..."). Gibt bei Erfolg 1, sonst 0 zurueck.
 */

varargs int drop_objects(string str, mixed msg)
{
    object *obs;

    if (!sizeof(obs = find_objects(str, this_object(), 1)))
        return 0;

    foreach (object o: obs) {
        if (objectp(o))
            drop(o, msg);

        if (get_eval_cost() < 100000) {
            TME("Den Rest behaeltst Du erst mal.");
            last_moved_objects = obs[..member(obs, o)];
            last_moved_where = 0;
            return 1;
        }
    }

    last_moved_objects = obs;
    last_moved_where = 0;
    return 1;
}

varargs int put_objects(string str, int casus, string verb, mixed msg)
{
    object *obs, dest, *no_move;
    
    if (!stringp(str) || !sizeof(str)) return 0;

    string *tokens = explode(str, " ");
    int allow_room = 1;
    int allow_me = 1;

    if (sizeof(tokens) > 1 && tokens[<1] == "hier") {
        tokens = tokens[..<2];
        allow_me = 0;
    } else if (sizeof(tokens) > 2 && tokens[<2] == "in")
        if (tokens[<1] == "mir" ||
            tokens[<1] == "dir") {
            tokens = tokens[..<3];
            allow_room = 0;
        } else if (tokens[<1] == "raum") {
            tokens = tokens[..<3];
            allow_me = 0;
        }

    for (int i = sizeof(tokens)-1; i > 1; i--) {
        if (!(dest = allow_room && present(implode(tokens[i..], " "),
                                           environment())) &&
            !(dest = allow_me && present(implode(tokens[i..], " "),
                                         this_object())))
            continue;

        if (living(dest)) {
            NF("Aber " + dest->name(WER, 1) + " lebt doch!");
            continue;
        }
/*
        if (verb == "legen" && !dest->QueryProp(P_TRAY)) {
            NF("Du kannst nichts auf " + dest->name(WEN, 1) + " legen.");
            continue;
        }
*/
        if (verb == "stecken" && !dest->QueryProp(P_CONTAINER)) {
            NF("Du kannst in " + dest->name(WEN, 1) + " nichts reinstecken.");
            continue;
        }

        if (dest->QueryProp(P_CNT_STATUS)) {
            NF("Aber " + dest->name(WER, 1) + " ist doch geschlossen!");
            continue;
        }

        if (tokens[i-1] != dest->QueryProp(P_DEST_PREPOSITION)) {
            NF("Du kannst nichts " + tokens[i-1] + " " +
               dest->name(casus, 1) + " " + verb + ".");
            continue;
        }

        if (!sizeof(obs = __find_objects(tokens[..i-2], 0, 1) - ({ dest }))) {
            NF("WAS moechtest Du " + tokens[i-1] + " " +
               dest->name(casus, 1) + " " + verb + "?");
            return 0;
        }

        if (sizeof(no_move = obs & all_inventory(dest))) {
            TME(capitalize(CountUp(map_objects(no_move, "name", WER, 1))) +
                (sizeof(no_move) == 1 ? " ist" : " sind") +
                " doch bereits in " + dest->name(WEM,1) + ".");
            if (!sizeof(obs -= no_move))
                return 0;
        }

        foreach (object o: obs) {
            if (objectp(o))
                put(o, dest, msg);

            if (get_eval_cost() < 100000) {
                TME("Den Rest laesst Du erst mal, wo er ist.");
                last_moved_objects = obs[..member(obs, o)];
                last_moved_where = dest;
                return 1;
            }
        }

        last_moved_objects = obs;
        last_moved_where = dest;
        return 1;
    }
    
    return 0;
}

varargs int pick_objects(string str, int flag, mixed msg)
{
    object *obs;

    if (((int)QueryProp(P_MAX_HANDS)) < 1){
        NF("Ohne Haende kannst Du nichts nehmen.");
        return 0;
    }

    if (!sizeof(obs = find_objects(str, 0, 1) - all_inventory()
	  - (flag ? all_inventory(environment()) : ({}))))
        return 0;

    foreach (object o: obs) {
        if (objectp(o))
            pick(o, msg);

        if (get_eval_cost() < 100000) {
            TME("Den Rest laesst Du erst mal liegen.");
            last_moved_objects = obs[..member(obs, o)];
            last_moved_where = 0;
            return 1;
        }
    }

    last_moved_objects = obs;
    last_moved_where = 0;
    return 1;
}

varargs int give_objects(string str, mixed msg)
{
    object *obs, dest;
    
    if (!stringp(str) || !sizeof(str)) return 0;

    string *tokens = explode(str, " ");

    if (((int)QueryProp(P_MAX_HANDS)) < 1){
        NF("Ohne Haende kannst Du nichts weggeben.");
        return 0;
    }

    for (int i = 0; i < sizeof(tokens)-1; i++) {
        if (!(dest = present(implode(tokens[..i], " "), environment())))
            continue;

        if (!living(dest)) {
            NF("Aber " + dest->name(WER, 1) + " lebt doch gar nicht!");
            dest = 0;
            continue;
        }

        if (!sizeof(obs = __find_objects(tokens[i+1..], 0, 1))) {
            NF("WAS moechtest Du " + dest->name(WEM, 1)+" geben?");
            dest = 0;
        } else
            break;
    }

    if (!dest) {
        int pos;

        if ((pos = strrstr(str, " an ")) >= 0) {
            dest = present(str[pos+4..], environment());
            // zu gebende Objekte in Env + Living suchen
            obs = find_objects(str[..pos-1], 0, 1);
        }
    }

    if (!dest || !living(dest) || !sizeof(obs))
        return 0;

    foreach (object o: obs) {
        if (objectp(o))
            give(o, dest, msg);

        if (get_eval_cost() < 100000) {
            TME("Den Rest behaeltst Du erst mal.");
            last_moved_objects = obs[..member(obs, o)];
            last_moved_where = dest;
            return 1;
        }
    }

    last_moved_objects = obs;
    last_moved_where = dest;
    return 1;
}

varargs int show_objects(string str, mixed msg)
{
    object *obs, whom;
    
    if (!stringp(str) || !sizeof(str))
      return 0;

    string *tokens = explode(str, " ");

    for (int i = 0; i < sizeof(tokens)-1; i++) {
        if (whom = present(implode(tokens[..i], " "), environment())) {
            if (!living(whom)) {
                NF("Aber " + whom->name(WER, 1) + " lebt doch gar nicht!");
                continue;
            }
        } else {
            if (i != 0 || tokens[0] != "allen")
                continue;

            if (!sizeof(filter(all_inventory(environment()) -
                    ({ this_object() }), #'living))) {
                NF("Hier ist niemand, dem Du etwas zeigen koenntest!");
                continue;
            }
        }

        if (whom == this_object()) {
            NF("Dazu solltest Du dann besser 'schau' benutzen!\n");
            continue;
        }

        if (!sizeof(obs = __find_objects(tokens[i+1..], this_object(), 0)) &&
            !sizeof(obs = __find_objects(tokens[i+1..], 0, 0)
                            - ({ this_object(), whom }))) {
            NF("WAS moechtest Du " + (whom ? whom->name(WEM, 1) : "allen") +
               " zeigen?");
            continue;
        }

        foreach (object o: obs) {
            if (objectp(o))
                show(o, whom, msg);

            if (get_eval_cost() < 100000) {
                TME("Das reicht erst mal.");
                last_moved_objects = obs[..member(obs, o)];
                last_moved_where = whom;
                return 1;
            }
        }

        last_moved_objects = obs;
        last_moved_where = whom;
        return 1;
    }

    return 0;
}

object *moved_objects(void)
{
    return last_moved_objects;
}

object moved_where(void)
{
    return last_moved_where;
}


/************************* Die einzelnen Kommandos **************************/

/* static int fallenlassen(string str)
 * static int werfen(string str)
 * static int legen(string str)
 * static int stecken(string str)
 * static int holen(string str)
 * static int nehmen(string str)
 * static int geben(string str)
 *   Minimale Wrapper fuer XXX_objects(), entfernen "fallen", "weg" bzw. "ab"
 *   aus den Argumenten und setzen entsprechende Standard-Fehlermeldungen.
 *   
 * protected void add_put_and_get_commands()
 *   Registriert obige Funktionen per add_action().
 */

static int fallenlassen(string str)
{
    if (QueryProp(P_GHOST)) {
        _notify_fail("Als Geist kannst Du nichts fallenlassen.\n");
        return 0;
    }

    if (!str || str[<7..] != " fallen") {
        _notify_fail("Lass etwas FALLEN, oder was meinst Du?\n");
        return 0;
    }

    _notify_fail("WAS moechtest Du fallenlassen?\n");
    return drop_objects(str[0..<8]);
}

static int werfen(string str)
{
    if (QueryProp(P_GHOST)) {
        _notify_fail("Als Geist kannst Du nichts wegwerfen.\n");
        return 0;
    }

    if (!str || str[<4..] != " weg") {
        _notify_fail("Wirf etwas WEG, oder was meinst Du?\n");
        return 0;
    }

    _notify_fail("WAS moechtest Du loswerden?\n");
    return drop_objects(str[0..<5]);
}

static int legen(string str)
{
    if (QueryProp(P_GHOST)) {
        _notify_fail("Als Geist kannst Du nichts weglegen.\n");
        return 0;
    }

    if (!str) {
        _notify_fail("Lege etwas AB, oder was meinst Du?\n");
        return 0;
    }

    if (str[<3..] == " ab") {
        _notify_fail("WAS moechtest Du ablegen?\n");
        return drop_objects(str[0..<4]);
    }

    if (str[<4..] == " weg") {
        _notify_fail("WAS moechtest Du weglegen?\n");
        return drop_objects(str[0..<5]);
    }

    _notify_fail("WAS moechtest Du WOHIN legen?\n");
    return put_objects(str, WEN, "legen");
}

static int stecken(string str)
{
    if (QueryProp(P_GHOST)) {
        _notify_fail("Das kannst Du in Deinem immateriellen Zustand nicht.\n");
        return 0;
    }

    _notify_fail("WAS moechtest Du WOHIN stecken?\n");
    return put_objects(str, WEN, "stecken");
}

static int holen(string str)
{
    if (QueryProp(P_GHOST)) {
        _notify_fail("Als Geist kannst Du nichts nehmen.\n");
        return 0;
    }

    _notify_fail("WAS moechtest Du aus WAS holen?\n");
    return pick_objects(str, 1);
}

static int nehmen(string str)
{
    if (QueryProp(P_GHOST)) {
        _notify_fail("Als Geist kannst Du nichts nehmen.\n");
        return 0;
    }

    _notify_fail("WAS moechtest Du nehmen?\n");
    return pick_objects(str, 0);
}

static int geben(string str)
{
    if (QueryProp(P_GHOST)) {
        _notify_fail("Als Geist kannst Du nichts weggeben.\n");
        return 0;
    }

    _notify_fail("WEM moechtest Du WAS geben?\n");
    return give_objects(str);
}

static int zeigen(string str)
{
    if (QueryProp(P_GHOST)) {
        _notify_fail("Als Geist kannst Du niemandem etwas zeigen.\n");
        return 0;
    }

    _notify_fail("WEM moechtest Du WAS zeigen?\n");
    return show_objects(str);
}

protected void add_put_and_get_commands(void)
{
    add_action("fallenlassen", "lass");
    add_action("fallenlassen", "lasse");
    add_action("werfen",       "wirf");
    add_action("werfen",       "werf");
    add_action("werfen",       "werfe");
    add_action("legen",        "leg");
    add_action("legen",        "lege");
    add_action("stecken",      "steck");
    add_action("stecken",      "stecke");
    add_action("holen",        "hol");
    add_action("holen",        "hole");
    add_action("nehmen",       "nimm");
    add_action("nehmen",       "nehm");
    add_action("nehmen",       "nehme");
    add_action("geben",        "gebe");
    add_action("geben",        "gib");
    add_action("zeigen",       "zeig");
    add_action("zeigen",       "zeige");
}


/********** Aus reinen Kompatibilitaetsgruenden weiterhin enthalten *********/

object* find_obs(string str, int meth)
// gibt ein array zurueck mit allen Objekten die mit str angesprochen werden
{
   object inv;
   if (!str) return 0;
   if (str[<7..]==" in mir") {
     inv=ME;
     str=str[0..<8];
   }
   else if (str[<8..]==" in raum") {
     if (meth & PUT_GET_DROP) { // man kann nichts aus dem Raum wegwerfen
       _notify_fail("Du kannst nichts wegwerfen, das Du gar nicht hast.\n");
       return 0;
     }
     inv=environment();
     str=str[0..<9];
   }
   else if (meth & PUT_GET_DROP) inv=ME; // Raum bei drop uninteressant
   // else kein besonderes inv ausgewaehlt also inv=0
   if (!sizeof(str))
     return 0; // hier passt die bereits gesetzte _notify_fail
   else {
     object *obs;
     string con;
     if (sscanf(str, "%s aus %s", str, con)==2 ||
         sscanf(str, "%s in %s", str, con)==2 ||
         sscanf(str, "%s von %s", str, con)==2 ||
         sscanf(str, "%s vom %s", str, con)==2) {
       if (!inv) {
         if (!environment() || !(inv=present(con, environment())))
            inv=present(con, ME); // sowohl im env als auch im inv suchen
       }
       else inv=present(con, inv); // nur in ausgewaehltem inv suchen
       if (inv==ME) inv=0;
       if (!inv || !(inv->short())) {
         _notify_fail(break_string("Du hast hier aber kein '"+capitalize(con)
                                 +"'.",78));
         return 0;
       }
       if (living(inv)) {
         _notify_fail(break_string("Aber "+inv->name(WER,1)+" lebt doch!",78));
         return 0;
       }
       // wieso man aus Objekten die von std/tray abgeleitet werden etwas
       // nehmen koennen soll, versteh ich zwar nicht so ganz...
       if (!(inv->QueryProp(P_CONTAINER)) && !(inv->QueryProp(P_TRAY))) {
         _notify_fail(break_string("Du kannst nichts aus "+inv->name(WEM,1)
                                 +" nehmen.",78));
         return 0;
       }
       if (inv->QueryProp(P_CNT_STATUS)) { // Container ist geschlossen
         _notify_fail(break_string("Aber "+inv->name(WER,1)
                                 +" ist doch geschlossen.", 78));
         return 0;
       }
     }
     else if (inv==ME && (meth & PUT_GET_TAKE)) { // nichts aus sich nehmen
       _notify_fail("Du kannst nichts nehmen, "
                    "was Du schon bei Dir traegst.\n");
       return 0;
     }
     if (!inv && (meth & PUT_GET_TAKE))
       inv=environment(); // nichts nehmen was man schon hat

     if (!inv) {
       if (environment()) {
         obs=(environment()->present_objects(str)||({}));
         if (!sizeof(obs)) obs+=(ME->present_objects(str)||({}));
       }
       else obs=(ME->present_objects(str) || ({}));
     }
     else obs=(inv->present_objects(str) || ({}));
     return obs-({ ME });
   }
   return(0);
}

int pick_obj(object ob)
{
  object env;

  if (!ob || ob == this_object() || environment(ob) == this_object()) return 0;
  if ((env=environment(ob)) != environment()) {
    if (!env->QueryProp(P_CONTAINER) && !env->QueryProp(P_TRAY)) {
      TME("Du kannst nichts aus " + env->name(WEM,1) + " nehmen.");
      return 1;
    }
    else if (env->QueryProp(P_CNT_STATUS)) {  // Container ist geschlossen
      TME("Aber " + env->name(WER, 1) + " ist doch geschlossen.");
      return 1;
    }
  }
  if (ob->IsUnit() && ob->QueryProp(P_AMOUNT)<0) {
    TME("Du kannst nicht mehr nehmen als da ist.");
    return 1;
  }
  pick(ob);
  return 1;
}

int drop_obj(object ob)
{
  if (!ob || ob==this_object() || environment(ob)!=this_object()) return 0;
  drop(ob);
  return 1;
}

int put_obj(object ob, object where)
{
  object env;

  if (ob == this_object() || ob == where || environment(ob) == where) return 0;
  env=environment(ob);
  if (!where->QueryProp(P_CONTAINER)) {
    TME("Du kannst in " + where->name(WEN,1) + " nix reinstecken.");
    return 1;
  }
  if (where->QueryProp(P_CNT_STATUS)) {  // Container ist geschlossen
    TME("Aber " + where->name(WER, 1) + " ist doch geschlossen.");
    return 1;
  }
  if (env!=environment(this_object()) && env!=this_object()) {
    _notify_fail("Da kommst du so nicht ran.\n");
    return 0;
  }
  put(ob, where);
  return 1;
}

int give_obj(object ob, object where)
{
  object env;

  if (environment(ob)!=this_object()) {
    TME("Das solltest Du erstmal nehmen.");
    return 1;
  }
  if (!ob || ob == this_object() || ob == where ||
      environment(where)!=environment())
    return 0;
  if (environment(ob) == where) {
    _notify_fail("Das Ziel ist in dem zu gebenden Object enthalten!\n");
    return 0;
  }
  if (environment(ob)!=this_object()) {
    TME("Das hast Du nicht.");
    return 1;
  }
  give(ob, where);
  return 1;
}
