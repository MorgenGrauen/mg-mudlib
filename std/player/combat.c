// MorgenGrauen MUDlib
//
// player/combat.c -- combat statistics
//
// $Id: combat.c 9008 2015-01-06 17:20:17Z Zesstra $
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

inherit "/std/living/combat";
inherit "/std/player/pklog";

#include <thing/properties.h>
#include <properties.h>
#include <wizlevels.h>
#include <combat.h>
#include <new_skills.h>

#define ME this_object()
#define STATMASTER "/p/service/rochus/guildstat/master"

private nosave closure mod_def_stat;
private nosave string *plAttacked = ({});

protected void create() {
  combat::create();
  // P_NO_ATTACK ist nicht fuer Spieler gedacht. Ausnahme: Spieler ist
  // Geist, dann setzt das Spielerobjekt aber selber.
  Set(P_NO_ATTACK, SECURED|NOSETMETHOD, F_MODE_AS);
  
  Set(P_HELPER_NPC, PROTECTED, F_MODE);
  SetProp(P_HELPER_NPC, ([]) );
}

// Maske fuer alle moeglichen Klassen von Helfer-NPC
#define CLASS_MASK 0x1fffffff
/** registriert den NPC als Helfer von diesem Spieler.
  @param[in] npc object Helfer-NPC
  @param[in] flags int Bitfeld von Flags
  @return int 1, falls der Helfer-NPC registriert wurde und noch nicht
  registriert war.
  @attention Nutzt aus, dass QueryProp(P_HELPER_NPC) _keine_ Kopie des
  Mappings in der Prop liefert.
  */
public int RegisterHelperNPC(object npc, int flags) {
  if (!objectp(npc))
    raise_error(sprintf( "Wrong argument 1 in RegisterHelperNPC(). "
          "Expected <object>, got %.100O\n", npc));
  if (!intp(flags) || flags < 1)
     raise_error(sprintf( "Wrong argument 2 in RegisterHelperNPC(). "
          "Expected positive <int>, got %O\n", flags));
  
  mapping helpers = QueryProp(P_HELPER_NPC);
  
  // schon registrierte sind witzlos.
  if (member(helpers, npc))
    return 0;
  
  // auf exklusive Helfer pruefen.
  foreach(object helper, int fl: helpers) {
    // flags identisch? Dann Klasse und Exklusivitaet identisch
    if (fl == flags)
      return 0;
    // oder einer von beiden exklusiv und beide in der gleichen Klasse?
    else if ( ((fl & EXCLUSIVE_HELPER) || (flags & EXCLUSIVE_HELPER))
              && ((fl & CLASS_MASK) == (flags & CLASS_MASK)) )
      return 0;
  }
  // scheint wohl ok zu sein. Registrieren und Prop im NPC setzen.
  helpers += ([ npc: flags ]);
  npc->SetProp(P_HELPER_NPC, ({ this_object(), flags }) );
  // momentan unnoetig, da helpers keine Kopie ist.
  // SetProp(P_HELPER_NPC, helpers);

  return 1;
}
#undef CLASS_MASK

/** de-registriert den NPC als Helfer von diesem Spieler.
  @param[in] npc object Helfer-NPC
  @return int 1, falls der Helfer-NPC registriert war und entfernt wurde.
  @attention Nutzt aus, dass QueryProp(P_HELPER_NPC) _keine_ Kopie des
  Mappings in der Prop liefert.
 */
public int UnregisterHelperNPC(object npc) {
  if (!objectp(npc))
    raise_error(sprintf("Wrong argument in UnregisterHelpernNPC(). "
          "Expected <object>, got %.100O\n", npc));

  mapping helpers = QueryProp(P_HELPER_NPC);
  if (member(helpers, npc)) {
    m_delete(helpers, npc);
    // momentan unnoetig, da helpers keine Kopie ist.
    // SetProp(P_HELPER_NPC, helpers);
    npc->SetProp(P_HELPER_NPC, 0);
    return 1;
  }
  return 0;
}

/** Feind eintragen.
  * Traegt ob als Feind ein. Dies allerdings nur, wenn ob kein Spieler ist
  * oder beide Spieler (dieses Objekt und ob) in der Schattenwelt sind oder
  * beide Spieler Testspieler sind.
   @param[in] ob potentieller Feind.
   @return int 1, falls ob als _neuer_ Feind eingetragen wurde.
   */
public int InsertEnemy(object ob) {
  // wenn ob ein Spieler ist und nicht sowohl ich als auch ob in der
  // Schattenwelt sind, wird ob nicht als Feind eingetragen.
  if (query_once_interactive(ob)
      && (strstr(object_name(environment(ob)),"/d/schattenwelt/")!=0
          || strstr(object_name(environment(ME)),"/d/schattenwelt/")!=0)
      && (!QueryProp(P_TESTPLAYER) || !ob->QueryProp(P_TESTPLAYER))
     )
  {
    return 0;
  }
  return ::InsertEnemy(ob);
}

/** Hat dieser Spieler den Spieler pl angegriffen?.
  @param[in] pl object zu pruefender Spieler
  @return int 1, falls dieser Spieler pl angegriffen hat.
  @attention Nebeneffekt: bereinigt den internen Speicher von UIDs von
  zerstoerten Spielern und solchen, die keine Feinde mehr sind.
  */
public int QueryPlAttacked(object pl) {
  object ob;

  if ( !objectp(pl) )
    return 0;

  foreach(string plname: plAttacked) {
    if ( !( ob=(find_player(plname)||find_netdead(plname)) )
        || ( !IsEnemy(ob) && !(ob->IsEnemy(ME)) ) )
      plAttacked -= ({plname}); // ja, das geht. ;-)
  }
  return (member( plAttacked, getuid(pl) ) >= 0);
}

/** kill - Kampf starten.
 * Fuegt ob der Feindesliste hinzu.
 */
public int Kill(object ob) {

  if (!objectp(ob)) return 0;

  // dies dient nur dazu, das plAttacked mapping zu bereinigen.
  // TODO: besser machen. ;-)
  if ( query_once_interactive(ob) && !IsEnemy(ob)) {
    QueryPlAttacked(ME);
    ob->QueryPlAttacked(ob); // aktualisieren ...
  }

  int res = combat::Kill(ob);

  // falls ob nen Spieler ist, pruefen, ob es ein Spieler-Spieler-Angriff
  // ist.
  // Dabei ggf. loggen und Magier verstaendigen.
  if (query_once_interactive(ob) && CheckPlayerAttack(ME, ob, 0))
  {
    if (res == -4) // feind wurde nicht eingetragen
      tell_object(ME, "Ein goettlicher Befehl hindert Dich am Kampf.\n");
    else
      plAttacked += ({ getuid(ob) });
  }

  return res;
}

public int Defend(int dam, string|string* dam_type, int|mapping spell, object enemy) {
  int delta_hp,res;

  if (query_once_interactive(ME)
      && !IS_LEARNER(ME)
      && !objectp(get_type_info(mod_def_stat,2))) {
    object ma;
    if (!objectp(ma=find_object(STATMASTER)))
      return ::Defend(dam,dam_type,spell,enemy);
    // Statistik nur aufrufen falls Master geladen
    mod_def_stat=symbol_function("ModifyDefendStat",ma);
  }

  if (closurep(mod_def_stat))
    delta_hp=QueryProp(P_HP);

  res=::Defend(dam,dam_type,spell,enemy);

  if (closurep(mod_def_stat)) {
    delta_hp-=QueryProp(P_HP);
    if (delta_hp<0)
      delta_hp=0;
    funcall(mod_def_stat,
            QueryProp(P_GUILD),
            QueryProp(P_GUILD_LEVEL),
            dam-10*delta_hp,
            dam_type,
            spell);
  }

  return res;
}

// Spieler koennen als Geist nicht kämpfen
// TODO: pruefen, ob das Setzen und Loeschen der Prop in set_ghost() nicht
// auch ausreichen wuerde. In dem Fall muesste man aber P_NO_ATTACK auch
// speichern, da P_GHOST gespeichert wird...
static mixed _query_no_attack()
{
    if ( QueryProp(P_GHOST) )
        return 1;

    return Query(P_NO_ATTACK);
}
