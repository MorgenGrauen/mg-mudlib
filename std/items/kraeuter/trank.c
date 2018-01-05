//TODO:

#pragma strong_types,rtt_checks

inherit "/std/thing";

#include <properties.h>
#include <defines.h>
#include <items/kraeuter/kraeuter.h>
#include <items/kraeuter/trankattribute.h>
#include <hook.h>
#include <class.h>
#include <new_skills.h>
#include <wizlevels.h>

#ifndef BS
#  define BS(x)             break_string(x, 78)
#endif

#define allowed(x) (object_name(x)==PLANTMASTER)
#define DRINK_POTION "lib_kraeutertrank_trinken"
// for debug
#define private public

// Ablaufzeit des Tranks, ab dann keine Wirkung mehr
private nosave int expiry;
// Ablaufzeit der wirkungen des Trankes (0, wenn Trank nicht getrunken)
private nosave int duration;
// Trankattribute, vom Krautmaster schon skaliert, gekappt, beschraenkt auf
// die jeweils max. positiven Effekte
private nosave mapping data;
// Klassen, die bei Schutz oder verstaerktem Schaden betroffen sind. Werte
// ist der Schutz- oder Attackebonus/-malus.
private nosave mapping att_classes;
private nosave mapping prot_classes;
// max. Summe von Giftstufen (P_POISON und P_LEVEL/10 bei CL_POISON).
private nosave int prot_poison;
// max. geheilte Summe von Krankheitslevel (P_LEVEL/5 in CL_DISEASE).
private nosave int prot_disease;
// nach Wirkung absteigend sortierte Liste der Effekte, wird beim ersten
// Aufruf von DetermineStrongesEffect() befuellt.
private nosave string* sorted_effects;

mixed _query_data() {return data;}
int _set_data(mixed d) {data=d; expiry = __INT_MAX__; return data!=0;}
private string effect2colour();

protected void create()
{
  if (object_name(this_object()) == __FILE__[0..<3])
  {
    set_next_reset(-1);
    return;
  }
  ::create();
  SetProp(P_GENDER, FEMALE);
  SetProp(P_NAME, "Glasflasche");
  SetProp(P_NAME_ADJ, ({"klein"}));
  SetProp(P_VALUE, 10);
  SetProp(P_WEIGHT, 100);
  SetProp(P_KILL_NAME, "Ein Kraeutertrank");
  SetProp(P_KILL_MSG, "%s hat sich wohl beim Anruehren vertan.");
  AddId(({"glasflasche", "flasche"}));
  AddAdjective(({"klein", "kleine"}));
  AddCmd("trink|trinke&@ID", "cmd_trinken",
    "Was willst Du trinken?");
}

protected void create_super()
{
  set_next_reset(-1);
}

static string _query_short()
{
  if (!clonep(ME))
    return "Eine kleine Glasflasche";
  return Name(WEN, 0)+(data==0 ? "" : " (gefuellt)");
}

static string _query_long()
{
  if (data==0)
    return break_string(
      "Eine kleine leere Glasflasche, die mit einem Korken verschlossen "
      "ist.\n"
      "Flaschen wie diese werden ueblicherweise verwendet, um darin "
      "magische Traenke abzufuellen.", 78, 0, BS_LEAVE_MY_LFS);

  return break_string(
    "Eine kleine Glasflasche, die mit einer "+effect2colour()+"en "
    "Fluessigkeit gefuellt ist.\n"
    "Sie ist mit einem Korken verschlossen, um den Inhalt der "
    "Flasche zu schuetzen.",78, 0, BS_LEAVE_MY_LFS);
}

private string num2desc(int bumms)
{
  switch(abs(bumms))
  {
    case 0..499:
      return "ein wenig";
    case 500..999:
      return "so einiges";
    case 1000..1499:
      return "erheblich";
    case 1500..2000:
      return "unglaublich";
  }
  return "ungenehmigt viel"; // kommt hoffentlich nicht vor.
}

// Ermittelt den betragsmaessig staerksten wirksamen Effekt, d.h. wenn ein 
// Trank eine starke negative Wirkung und eine mittelstarke positive Wirkung 
// hat, wird hier die negative Wirkung ermittelt.
varargs private string DetermineStrongestEffect(int pos)
{
  // globale Werteliste befuellen, wenn da noch nichts drinsteht.
  if ( !pointerp(sorted_effects) ) {
    sorted_effects = sort_array(m_indices(data) & T_KRAUT_EFFECTS,
      function int (string a, string b) {
        return (abs(data[a])<=abs(data[b]));
      });
  }

  // Zur Indizierung des Arrays muss <pos> bei Null starten, es wird
  // aber mit der Bedeutung einer Ordinalzahl (erste, zweite, dritte, ...)
  // uebergeben. Daher um 1 reduzieren.
  --pos;

  string ret;

  // Im Array muss mindestens ein Eintrag stehen, sonst gibt's gar keinen
  // Effekt.
  if ( sizeof(sorted_effects) )
  {
    // Wenn der angefragte Index ausserhalb der Arraygrenzen liegt, wird
    // angenommen, dass der erste bzw. letzte Eintrag gesucht waren.
    if ( pos < 0 )
      ret = sorted_effects[0];
    else if ( pos >= sizeof(sorted_effects) )
      ret = sorted_effects[<1];
    else
      ret = sorted_effects[pos];
  }
  return ret;
}

// Liefert zu dem maximal wirksamen Effekt die Farbe des Trankes zurueck.
private string effect2colour()
{
  // Ist die Haltbarkeit schon abgelaufen, wird der Trank farblos.
  if ( time() > expiry )
    return "farblos";

  // Namen des staerksten Effekts holen.
  string effect = DetermineStrongestEffect(1);
  mapping colours = ([
    T_CARRY:              "trueb braun",
    T_DAMAGE_ANIMALS:     "blutrot",
    T_DAMAGE_MAGIC:       "oktarinfarben",
    T_DAMAGE_UNDEAD:      "anthrazitfarben",
    T_FLEE_TPORT:         "schwefelgelb",
    T_FROG:               "schlammgruen",
    T_HEAL_DISEASE:       "perlmuttfarben",
    T_HEAL_POISON:        "gruen",
    T_HEAL_SP:            "blau",
    T_HEAL_LP:            "scharlachrot",
    T_PROTECTION_ANIMALS: "metallisch grau",
    T_PROTECTION_MAGIC:   "violett",
    T_PROTECTION_UNDEAD:  "strahlend weiss",
    T_SA_SPEED:           "orangefarben",
    T_SA_SPELL_PENETRATION: "stahlblau",
    T_SA_DURATION:        "pinkfarben",
  ]);
  string ret = colours[effect];
  return stringp(ret) ? ret : "farblos";
}

// Wird gerufen, wenn die Wirkung des Trankes ablaufen soll.
private void terminate_effects()
{
  tell_object(environment(),
      "Die letzten Wirkungen des Kraeutertrankes klingen ab.\n");
  remove(1);
}

// Von den Hooks H_HOOK_ATTACK_MOD und H_HOOK_DEFEND gerufen, erhoeht oder
// verringert den Schaden gegen Lebenwesen bestimmter Klassen (Keys in
// <classes>). Der Malus/Bonus steht als Wert zum jeweiligen Key in dem
// Mapping.
mixed hcallback(object hookSource, int hookid, mixed hookData)
{
    if (hookSource != environment())
        return ({H_NO_MOD,hookData});
    switch(hookid)
    {
      case H_HOOK_ATTACK_MOD:
        foreach(string class, int modval : att_classes)
        {
          if (hookData[SI_ENEMY]->is_class_member(class))
          {
            // Yeah. Treffer. Schaden erhoehen oder verringern... ;)
            hookData[SI_SKILLDAMAGE] += modval;
            // Ende. keine weiteren Klassen pruefen.
            return ({H_ALTERED, hookData});
          }
        }
        break;
      case H_HOOK_DEFEND:
        // hookData: ({dam,dam_type,spell,enemy})
        foreach(string class, int modval : prot_classes)
        {
          if (hookData[3]->is_class_member(class))
          {
            // Yeah. Treffer. Schaden erhoehen oder verringern... ;)
            hookData[0] += modval;
            // Ende. keine weiteren Klassen pruefen.
            return ({H_ALTERED, hookData});
          }
        }
        break;
      case H_HOOK_INSERT:
        // Wenn die Giftschutzkapazitaet noch ausreicht, wird das reinkommende
        // Objekt zerstoert (und die Kapazitaet reduziert).
        // hookData: neues object
        if (prot_poison > 0
            && hookData->is_class_member(CL_POISON))
        {
          // kostet ein Zehntel des Levels, aber min. 1.
          int p=hookData->QueryProp(P_LEVEL) / 10 + 1;
          if (p < prot_poison)
          {
            hookData->remove(1);
            prot_poison-=p;
            return ({H_CANCELLED, hookData});
          }
        }
        // Wenn die Krankheitsschutzkapazitaet noch ausreicht, wird das reinkommende
        // Objekt zerstoert (und die Kapazitaet reduziert).
        // hookData: neues object
        if (prot_disease > 0
            && hookData->is_class_member(CL_DISEASE))
        {
          // kostet ein Fuenftel des Levels, aber min. 1.
          int lvl = hookData->QueryProp(P_LEVEL) / 5 + 1;
          if (lvl < prot_disease)
          {
            hookData->remove(1);
            prot_disease-=lvl;
            return ({H_CANCELLED, hookData});
          }
        }
        break;
      case H_HOOK_POISON:
        // hookData: poisonval
        // Alle Giftlevel werden reduziert auf 0 und von prot_poison
        // abgezogen. Wenn das 0 ist, endet der Giftschutz.
        if (prot_poison>0)
        {
          if (hookData < prot_poison)
          {
            prot_poison-=hookData;
            hookData = 0;
          }
          else
          {
            hookData -= prot_poison;
            prot_poison=0;
          }
          return ({H_ALTERED, hookData});
        }
        break;
    }
    return ({H_NO_MOD, hookData});
}

private int reg_hook(int hook, int hooktype)
{
  // Wenn schon registriert, zaehlt das auch als Erfolg.
  if (environment()->HIsHookConsumer(hook, #'hcallback))
    return 1;
  int res = environment()->HRegisterToHook(hook, #'hcallback,
                 H_HOOK_OTHERPRIO(0), hooktype, duration);
  if (res <= 0)
  {
    // wenn andere Fehler als -7 (zuviele Hooks registriert) vorkommen:
    // Fehlermeldung ausgeben
    if (res != -7)
      tell_object(environment(),break_string(sprintf(
          "Technischer Hinweis, den Du an einen Magier weitergeben "
          "solltest: Beim Registrieren des Hooks %d gab es Fehler: %d\n",
          hook, res),78));
    return 0;
  }
  return 1;
}

// effekt: Wirkungswert des Tranks (muss negativ sein)
// type: 1 fuer Gift, 0 fuer Krankheit
private void ticktack(int effekt, int type)
{
  // Schaden tickt alle 5 Sekunden
  int delay = 5;
  // Der halbe Betrag des negativen Effekts wird als Schaden am
  // Spieler verursacht.
  // Berechnung: (Schaden pro Sekunde) * (Delay in Sekunden)
  // in float rechnen ist hier sinnvoll, inkl. aufrunden (durch die +0.5)
//  int dam = to_int(((-0.5*effekt)/duration)*delay + 0.5);
  int dam = to_int(0.5*abs(effekt)/data[T_EFFECT_DURATION]*delay + 0.5);

  if (type)
    tell_object(environment(),
      break_string("Gift pulsiert brennend durch Deine Adern.",78));
  else
    tell_object(environment(),
      break_string("Du fuehlst Dich schwach und elend, eine Erkrankung "
      "zehrt an Deinen Kraeften.",78));

  environment()->do_damage(dam, this_object());
  call_out(#'ticktack, delay, effekt, type);
}

private int act_attr_heal_poison(int effekt)
{
  int erfolgreich;
  tell_object(environment(), break_string(
     "Du fuehlst, wie der Trank wie Feuer durch Deinen Koerper schiesst "
     "und kruemmst Dich vor Schmerzen. Doch Momente spaeter laesst die "
     "Tortur nach.",78));

  // max. 40 Giftlevel heilen...
  prot_poison = effekt / 50;

  if (prot_poison < 0)
  {
    tell_object(environment(), BS(
      "Bah! Der Trank schmeckt widerlich bitter. Wenn der mal nicht "
      "giftig war."));
    call_out(#'ticktack, 5, effekt, 1); // 1 => Gift
    return 1;
  }
  
  // ab jetzt nur noch positive Wirkungen.

  // P_POISON zuerst.
  int poison = environment()->QueryProp(P_POISON);
  if (poison)
  {
    if (poison <= prot_poison)
    {
      prot_poison -= poison;
      environment()->SetProp(P_POISON,0);
      if (!environment()->QueryProp(P_POISON))
        ++erfolgreich;
    }
    else
    {
      poison -= prot_poison;
      prot_poison=0;
      environment()->SetProp(P_POISON, poison);
      // Wenn erfolgreich, direkt Meldung und raus.
      if (environment()->QueryProp(P_POISON) == poison)
      {
        tell_object(environment(), break_string(
           "Ueberrascht stellst Du fest, dass Du Dich "
           "besser fuehlst - der Trank hat Deine Vergiftung offenbar "
           "gelindert.",78));
        return 1;
      }
    }
  }

  // wenn Trank immer noch positiv (also noch WIrkung uebrig)
  if (prot_poison > 0)
  {
    // Als naechstes Objekte suchen.
    object *ob = filter_objects(all_inventory(environment()),
                                "is_class_member", CL_POISON);
    foreach(object o: ob)
    {
      // Entgiften kostet ein Zehntel des Levels, aber min. 1.
      poison = o->QueryProp(P_LEVEL);
      if (poison <= prot_poison*10)
      {
        prot_poison -= poison/10 + 1;
        o->SetProp(P_LEVEL, 0);
        if (o->remove())
          ++erfolgreich;
      }
      else
      {
        poison -= prot_poison * 10;
        prot_poison = 0;
        o->SetProp(P_LEVEL, poison);
        if (o->QueryProp(P_LEVEL) == poison)
          ++erfolgreich;
      }
      if (prot_poison <= 0)
        break;
    }
  }

  if (erfolgreich)
  {
    tell_object(environment(), break_string(
      "Ueberrascht stellst Du fest, dass Du Dich viel besser fuehlst - der "
      "Trank wirkt offenbar gegen Vergiftungen.",78));
  }
  else
  {
    tell_object(environment(), break_string(
      "Eine Ahnung sagt Dir, dass der Trank irgendeine positive "
      "Wirkung hat."));
  }

  // ggf. an die Hooks registrieren, wenn noch Schutzwirkung uebrig ist.
  if (prot_poison > 0)
  {
    // Rueckgabewerte von HRegisterToHook speichern...
    int *res = ({ reg_hook(H_HOOK_POISON, H_DATA_MODIFICATOR) });
    res += ({ reg_hook(H_HOOK_INSERT, H_HOOK_MODIFICATOR) });
    // Wenn alle versuchten Registrierungen erfolgreich waren...? Ansonsten
    // andere Meldung... Ich bin noch nicht gluecklich, das hier so explizit
    // auszugeben, aber ich weiss gerade sonst nicht, wie man drauf kommen
    // soll, dass es ein Problem gibt.
    if (sizeof(res) == sizeof(res & ({1})))
      tell_object(environment(),
         "Vielleicht haelt diese Wirkung ja sogar noch etwas an?\n");
    else
    {
      // zumindest ein erfolg?
      if (member(res, 1) > -1)
        tell_object(environment(),
            "Vielleicht haelt ein Teil dieser Wirkung ja sogar noch etwas an?\n");
    }
  }
  return 1;
}

private int act_attr_heal_disease(int effekt)
{
  int erfolgreich;

  // max. 40 Krankheitslevel heilen...
  prot_disease = effekt / 50;

  if (prot_disease > 0)
  {
     tell_object(environment(), break_string(
       "Du fuehlst, wie der Trank in Deinem Bauch eine wohlige Waerme "
       "verbreitet und laechelst unwillkuerlich.",78));
   
    // Objekte suchen.
    object *ob = filter_objects(all_inventory(environment()),
                                "is_class_member", CL_DISEASE);
    foreach(object o: ob)
    {
      // Heilen kostet ein Fuenftel des Levels, aber min. 1.
      int disease = o->QueryProp(P_LEVEL);
      if (disease <= prot_disease*5)
      {
        prot_disease -= disease/5 + 1;
        o->SetProp(P_LEVEL, 0);
        if (o->remove())
          ++erfolgreich;
      }
      else
      {
        disease -= prot_disease * 5;
        prot_disease = 0;
        o->SetProp(P_LEVEL, disease);
        if (o->QueryProp(P_LEVEL) == disease)
          ++erfolgreich;
      }
      if (prot_disease <= 0)
        break;
    }
  }
  else
  {
    tell_object(environment(), BS(
      "Der Trank schmeckt eklig faulig. Dein Magen rebelliert umgehend. "
      "Du kannst Deinen Brechreiz gerade noch unterdruecken, fuehlst "
      "Dich aber krank."));
    call_out(#'ticktack, 5, effekt, 0); // 0 => Krankheit
    return 1;
  }

  if (erfolgreich)
  {
    tell_object(environment(), break_string(
      "Entspannt durchatmend stellst Du fest, dass Du Dich viel besser fuehlst - der "
      "Trank wirkt offenbar gegen Krankheiten.",78));
  }
  else
  {
    tell_object(environment(), break_string(
      "Eine Ahnung sagt Dir, dass der Trank irgendeine positive "
      "Wirkung hat."));
  }

  // ggf. an die Hooks registrieren.
  if (prot_disease > 0)
  {
    // Registrierung erfolgreich...? Ansonsten andere Meldung... Ich bin
    // noch nicht gluecklich, das hier so explizit auszugeben, aber ich
    // weiss gerade sonst nicht, wie man drauf kommen soll, dass es ein
    // Problem gibt.
    if (reg_hook(H_HOOK_INSERT, H_HOOK_MODIFICATOR)==1)
      tell_object(environment(),
         "Vielleicht haelt diese Wirkung ja sogar noch etwas an?\n");

  }
  return 1;
}

private string num2desc_fight(int bumms)
{
  switch(abs(bumms))
  {
    case 0..499:
      return "ein wenig";
    case 500..999:
      return "spuerbar";
    case 1000..1499:
      return "deutlich";
    case 1500..2000:
      return "erheblich";
  }
  return "ungenehmigt viel"; // kommt hoffentlich nicht vor.
}

// AN: Tiere sind: CL_ANIMAL, CL_FISH, CL_FROG, CL_INSECT, CL_MAMMAL,
// CL_MAMMAL_LAND, CL_MAMMAL_WATER, CL_REPTILE, CL_ARACHNID, CL_BIRD
private int act_attr_dam_animals(int effekt)
{
  if (reg_hook(H_HOOK_ATTACK_MOD, H_DATA_MODIFICATOR) == 1)
  {
    if (!mappingp(att_classes)) att_classes=m_allocate(1);
    att_classes[CL_ANIMAL] = effekt/20;
    if ( effekt > 0 )
      tell_object(environment(), break_string(
        "Du spuerst in Dir ein seltsames Verlangen aufsteigen, auf die Jagd "
        "zu gehen - als wuerde Artemis persoenlich Deine Angriffe "
        + num2desc_fight(effekt)
        + " verbessern."),78);
    else 
      tell_object(environment(), break_string(
        "Dein Verlangen, auf die Jagd zu gehen, ist auf einmal sehr "
        "gedaempft, denn Du spuerst, dass Artemis Dich im Kampf gegen "
        "ihre Schuetzlinge "
        + num2desc_fight(effekt)
        + "schwaechen wuerde.",78);
    return 1;
  }
  return 0;
}

// AN: Magische Wesen sollen sein: CL_ELEMENTAL, CL_ILLUSION, CL_SHADOW
// CL_DRAGON, CL_DEMON, CL_SHAPECHANGER, CL_HARPY
private int act_attr_dam_magical(int effekt)
{
  if (reg_hook(H_HOOK_ATTACK_MOD, H_DATA_MODIFICATOR) == 1)
  {
    if (!mappingp(att_classes)) att_classes=m_allocate(4);
    att_classes[CL_DRAGON] = att_classes[CL_ELEMENTAL]
                            = att_classes[CL_SHADOW]
                            = att_classes[CL_ILLUSION]
                            = effekt/20;
    tell_object(environment(), break_string(
        "Merkwuerdig. Du hast gerade das Gefuehl, als fiele Dir der "
        "Kampf gegen von Hekate beschenkte Wesen "
        + num2desc_fight(effekt)
        + (effekt<0?" schwerer":" leichter."),78));
    return 1;
  }
  return 0;
}

// AN: Untote sollen sein: CL_SKELETON, CL_GHOUL, CL_GHOST, CL_VAMPIRE
// CL_ZOMBIE, CL_UNDEAD
// Bloed ist nur, dass CL_UNDEAD alle anderen enthaelt bis auf CL_GHOST.
// Also lassen wir die Unterscheidung und schrittweise Freischaltung
// erst einmal sein.
private int act_attr_dam_undead(int effekt)
{
  if (reg_hook(H_HOOK_ATTACK_MOD, H_DATA_MODIFICATOR) == 1)
  {
    // Zombies, Skelette, Ghule...
    if (!mappingp(att_classes)) att_classes=m_allocate(1);
    att_classes[CL_UNDEAD] =  effekt/20;
    tell_object(environment(), break_string(
        "Auf einmal hast Du den Eindruck, dass die Kreaturen des "
        "Hades Deinen Angriffen "
        + num2desc_fight(effekt)
        + (effekt<0?" mehr":" weniger")+" entgegenzusetzen haben.",78));
    return 1;
  }
  return 0;
}

private int act_attr_prot_animals(int effekt)
{
  if (reg_hook(H_HOOK_DEFEND, H_DATA_MODIFICATOR) == 1)
  {
    if (!mappingp(prot_classes)) prot_classes=m_allocate(1);
    prot_classes[CL_ANIMAL] = effekt/20;
    if ( effekt>0 ) 
    {
      tell_object(environment(), break_string(
        "Du hast das Gefuehl, dass Artemis ihre schuetzende Hand "
        + num2desc_fight(effekt)
        + " ueber Dich haelt.",78));
    }
    else
    {
      tell_object(environment(), break_string(
        "Du hast das Gefuehl, dass Artemis Dich im Kampf gegen ihre "
        "Schuetzlinge "
        + num2desc_fight(effekt)
        + " schwaechen wird.",78));
    }
    return 1;
  }
  return 0;
}

private int act_attr_prot_magical(int effekt)
{
  if (reg_hook(H_HOOK_DEFEND, H_DATA_MODIFICATOR) == 1)
  {
    if (!mappingp(prot_classes)) prot_classes=m_allocate(4);
    prot_classes[CL_DRAGON] = prot_classes[CL_ELEMENTAL]
                            = prot_classes[CL_SHADOW]
                            = prot_classes[CL_ILLUSION]
                            = effekt/20;
    tell_object(environment(), break_string(
        "Du hast das Gefuehl, dass von Hekate beschenkte Wesenheiten Dir "
        +num2desc_fight(effekt)
        +(effekt<0?" mehr":" weniger")+" anhaben koennen.",78));
    return 1;
  }
  return 0;
}

private int act_attr_prot_undead(int effekt)
{
  if (reg_hook(H_HOOK_DEFEND, H_DATA_MODIFICATOR) == 1)
  {
    // Zombies, Skelette, Ghule...
    if (!mappingp(prot_classes)) prot_classes=m_allocate(1);
    prot_classes[CL_UNDEAD] =  effekt/20;
    tell_object(environment(), break_string(
        "Du bist Dir ploetzlich sicher, Angriffen der Kreaturen des Hades "
        + num2desc_fight(effekt)
        + (effekt<0?" schlechter":" besser")+" widerstehen zu koennen.",78));
    return 1;
  }
  return 0;
}


private int act_attr_tragen(int effekt)
{
  if ( IS_LEARNER(environment()) )
    tell_object(environment(), sprintf("effekt: %d\n",effekt));
  SetProp(P_WEIGHT, QueryProp(P_WEIGHT) - effekt*4);
  tell_object(environment(),
      BS("Du fuehlst Dich ploetzlich "+num2desc(effekt)
        + (effekt>0 ? " entlastet" : " belastet") + "."));
  return 1;
}

// Berechnet eine max. Verzoegerung der Wirkung abhaengig von der Wirkung und
// einer Wirkschwelle. Der rel. Abstand von der Wirkschwelle (relativ zum max.
// moeglichen Abstand) wird hierbei genutzt. Ausserdem ist die max.
// Verzoegerung natuerlich die Wirkungsdauer des Trankes.
// <duration> muss im Trank schon gesetzt sein.
private int calc_max_delay(int effekt, int wirkschwelle)
{
  int abstand = abs(effekt - wirkschwelle);
  if (!duration) duration = time()+600;
  if ( IS_LEARNER(environment()) )
    printf("calc_max_delay: %d\n",((duration-time()) * abstand) /
      (2000-abs(wirkschwelle)));
  return ((duration-time()) * abstand) / (2000-abs(wirkschwelle));
}

//TODO: die Zeitverzoegerung ist nen netter Effekt, aber zeitvergzoegerte
//Tports sind oefter keine gute Idee -> noch pruefen
//TODO: Die Zeitverzoegerung bei NO_TPORT_OUT auch nochmal pruefen
private int act_attr_flee_tport(int effekt)
{
  // effekt > 0 teleportiert sofort zu halbwegs sicheren Orten
  // -750 <= effekt < 0 teleportiert auch sofort zu nicht so dollen Orten
  // -2000 <= effekt < -750 teleportiert spaeter zu bloeden Orten
  if (effekt < -750 && effekt >= -2000)
  {
    // Verzoegerung ausrechnen und dies hier per call_out nochmal mit einem
    // effekt rufen, der eine instantane Reaktion ausloest.
    // effekt - 2000 ist ein Hack, damit nicht nochmal verzoegert wird.
    call_out(#'act_attr_flee_tport,
        random(calc_max_delay(effekt, -750))+1, effekt - 2000);
    tell_object(environment(),
        "Deine Umgebung fuehlt sich nach und nach unwirklicher an.\n");
    return 1;
  }

  // Effekte instantan ausloesen.

  // wenn hier kein P_NO_TPORT erlaubt ist, mal gucken, ob wir spaeter noch
  // zeit haben.
  if (environment(environment())->QueryProp(P_NO_TPORT)==NO_TPORT_OUT)
  {
    tell_object(environment(),
        BS("Irgendetwas haelt Dich an diesem Ort fest."));
    int delay = duration - time();
    // wenn noch genug Restzeit, nochmal versuchen, sonst nix machen.
    if (delay>10)
    {
      // AN/TODO: Kann das nicht auch ziemlich lang sein?
      call_out(#'act_attr_flee_tport, random(delay), effekt);
    }
    return 0;
  }

  // den Hack von oben rueckgaengig machen, wir brauchen den originalen
  // Effektwert zurueck.
  if (effekt < -2000)
    effekt += 2000;

  string* dests;
  if ( effekt > 0 )
  {
    switch(effekt)
    {
      case 0..499:
        dests = ({"/d/inseln/zesstra/vulkanweg/room/r1",
                  "/d/fernwest/li/xian/lab2/grab2",
                  "/d/ebene/miril/schloss/heide11",
                  "/d/polar/alle/room/weg4_15",
                  "/d/dschungel/arathorn/tempel/room/t4-5",
                  "/d/anfaenger/arathorn/minitut/room/huette_"+
                    environment()->query_real_name(),
                 });
        break;
      case 500..749:
        dests = ({"/d/ebene/bertram/ebene/wasser8",
                  "/d/ebene/room/gebuesch2_3",
                  "/d/polar/alle/room/eiswueste/eiswueste[4,6]",
                 });
        if (environment()->QueryProp(P_REAL_RACE)!="Dunkelelf")
          dests += ({"/d/unterwelt/padreic/kneipe/kneipe"});
        break;
      case 750..999:
        dests = ({"/d/gebirge/silvana/cronoertal/room/tf4",
                  "/d/inseln/schiffe/floss",
                  "/d/polar/humni/hexen/room/leuchtkammer",
                  "/d/polar/gabylon/temong/rooms/anlegestelle",
                 });
        break;
      case 1000..1249:
        dests = ({"/d/fernwest/shinobi/konfu_quest/room/insel4",
                  "/d/fernwest/ulo/mura/tokoro/haus4",
                  "/d/ebene/room/Halle/shalle14",
                 });
        break;
      case 1250..1499:
        dests = ({"/d/gebirge/silvana/cronoertal/room/tf4",
                  "/gilden/zauberer",
                  "/d/gebirge/georg/room/w11"
                 });
        break;
      case 1500..1749:
        dests = ({"/gilden/bierschuettler",
                  "/gilden/kaempfer",
                  "/d/wald/gundur/hobbitdorf/schrein",
                  "/d/vland/morgoth/room/city/rathalle",
                 });
        break;
      default:
        dests = ({environment()->QueryProp(P_START_HOME)||
                    environment()->QueryDefaultHome(),
                  environment()->QueryDefaultHome(),
                  "/d/ebene/room/PortVain/po_haf2",
                  "/d/gebirge/room/he3x3",
                  "/d/ebene/room/huette",
                 });
        break;
    }
  }
  else if ( effekt < 0 )
  {
    switch(effekt)
    {
      case -499..0:
        dests = ({"/d/polar/bendeigid/rooms/neskaya/neskay12",
                  "/players/ketos/gl/room/gl1x1",
                  "/d/inseln/zesstra/vulkanweg/room/r10",
                  "/d/vland/morgoth/room/kata/ktanke",
                  "/d/ebene/zardoz/burg/feuerrau",
                 });
        break;
      case -999..-500:
        dests = ({"/d/ebene/room/Hoehle/hg6",
                  "/d/gebirge/silvana/warok/room/l3/wa_3x7",
                  "/d/vland/morgoth/room/kata/xkat03",
                  "/d/vland/morgoth/room/kata/kata5",
                  "/d/wald/yoru/ort/dravi/weg5",
                  "/d/wald/atamur/room/e83",
                 });
        break;
      case -1499..-1000:
        dests = ({"/d/polar/bendeigid/rooms/pass/pass_e1",
                  "/d/ebene/room/gebuesch",
                  "/d/gebirge/silvana/warok/room/l1/wa_1x6",
                  "/d/vland/morgoth/room/kata/gkat17",
                  "/d/wald/atamur/room/e12",
                 });
        if (environment()->QueryProp(P_REAL_RACE)=="Dunkelelf")
          dests += ({"/d/unterwelt/padreic/kneipe/kneipe"});
        break;
      case -1749..-1500:
        dests = ({"/d/dschungel/wurzel/t2",
                  "/d/vland/morgoth/room/kata/ukat26",
                  "/d/vland/morgoth/room/kata/h12",
                  "/d/gebirge/boing/sm/l5/m5x2",
                 });
        if (environment()->QueryProp(P_GUILD)=="chaos")
          dests+=({"/gilden/klerus"});
        break;
      default:
        dests = ({"/d/ebene/rochus/room/sp10",
                  "/d/ebene/rochus/quest_3player/room/schacht10",
                 });
        if ( IS_SEER(environment()) )
          dests += ({"/d/wald/paracelsus/froom/sch_6e",
                     "/d/wald/paracelsus/froom/sch_9x",
                     "/d/wald/paracelsus/froom/sch2_6d",
                    });
        break;
    }
  }
  tell_object(environment(),
      BS("Eine Kraft zerrt an Dir, die Welt verschwimmt..."));
  if (environment()->move(dests[random(sizeof(dests))],M_TPORT) == MOVE_OK)
    tell_object(environment(),
        "Einen Moment spaeter bist Du ganz woanders.\n");
  else
    tell_object(environment(),
        "Aber sonst passiert nichts.\n");
  return 1;
}

private int act_attr_change_dimension(int effekt)
{
  // nur effekt >= 1000 aendern die Dimension instantan. ;-)
  if (effekt > 0 && effekt < 1000)
  {
    // Verzoegerung ausrechnen und dies hier per call_out nochmal mit einem
    // effekt rufen, der eine instantane Reaktion ausloest.
    call_out(#'act_attr_change_dimension,
        random(calc_max_delay(effekt,1000))+1, 1000);
    tell_object(environment(),BS("Um Dich herum wird alles "
        "langsam grauer.\n"));
    return 1;
  }
  // nur -600 < effekt < 0 aendern die Dimension instantan ;-)
  if (effekt < -600)
  {
    // Verzoegerung ausrechnen und dies hier per call_out nochmal mit einem
    // effekt rufen, der eine instantane Reaktion ausloest.
    call_out(#'act_attr_change_dimension,
        random(calc_max_delay(effekt, -600))+1, -601);
    tell_object(environment(),BS("Um Dich herum wird alles "
        "langsam grauer.\n"));
    return 1;
  }
  // Effekte instantan ausloesen.
  // wenn hier kein Para-Trans erlaubt ist, mal gucken, ob wir spaeter noch
  // zeit haben.
  if (environment(environment())->QueryProp(P_NO_PARA_TRANS))
  {
    int delay = duration - time();
    // wenn noch genug Restzeit, nochmal versuchen, sonst nix machen.
    if (delay>10)
    {
      call_out(#'act_attr_change_dimension, random(delay), effekt);
      tell_object(environment(), BS("Die Welt um Dich wird ein "
            "wenig grauer."));
    }
    return 0;
  }
  if ( effekt > 0 )
  {
    if ( environment()->QueryProp(P_PARA) > 0 )
    {
      environment()->SetProp(P_PARA, 0);
      tell_object(environment(), BS("Fuer einen kurzen Moment siehst Du nur "
          "ein graues Wabern um Dich herum, bevor die Welt wieder "
          "normal aussieht.\n"));
    }
    else
    {
      tell_object(environment(), BS("Fuer einen kurzen Moment sieht alles um "
            "Dich herum grau aus."));
    }
  }
  else if ( effekt < 0 )
  {
    if ( !environment()->QueryProp(P_PARA) )
    {
      environment()->SetProp(P_PARA, 1);
       tell_object(environment(), BS("Fuer einen kurzen Moment siehst Du nur "
          "ein graues Wabern um Dich herum, bevor die Welt wieder "
          "normal aussieht. Aber es bleibt ein ungutes Gefuehl.\n"));
    }
    else {
      tell_object(environment(), BS("Fuer einen kurzen Moment sieht alles um "
            "Dich herum grau aus."));
    }
  }
  return 1;
}

private int act_attr_defrog(int effekt)
{
  // nur effekt > 1000 entfroscht instantan. ;-)
  if (effekt > 0 && effekt < 1000)
  {
    // Verzoegerung ausrechnen und dies hier per call_out nochmal mit einem
    // effekt rufen, der eine instantane Reaktion ausloest.
    call_out(#'act_attr_defrog, random(calc_max_delay(effekt,1000))+1, 1000);
    tell_object(environment(),BS(
        "Du hoerst ploetzlich lautes Gequake, was langsam immer leiser "
        "wird.\n"));
    return 1;
  }
  // nur -500 < effekt < 0 froscht instantan ;-)
  if (effekt < -500)
  {
    // Verzoegerung ausrechnen und dies hier per call_out nochmal mit einem
    // effekt rufen, der eine instantane Reaktion ausloest.
    call_out(#'act_attr_defrog, random(calc_max_delay(effekt, -500))+1, -501);
    tell_object(environment(),BS(
        "Du hoerst ploetzlich ganz leisess Gequake, was langsam immer lauter "
        "wird.\n"));
    return 1;
  }

  // Effekte instantan ausloesen.
  if ( effekt > 0 )
  {
    if ( environment()->QueryProp(P_FROG) )
    {
      environment()->SetProp(P_FROG,0);
      tell_object(PL, "Du fuehlst Dich deutlich weniger gruen.\n");
    }
    else
    {
      tell_object(environment(), break_string("Die Welt um Dich herum verliert "
        "ploetzlich alle gruenen Farbtoene, die bald darauf allmaehlich "
        "zurueckkehren.",78));
    }
  }
  else if ( effekt < 0 ) {
    if ( !environment()->QueryProp(P_FROG) ) {
      environment()->SetProp(P_FROG, 1);
      tell_object(environment(), "Quak!\n");
    }
    else {
      tell_object(environment(), break_string("Deine Sicht verschwimmt, alles wird "
        "intensiv gruen. Merkwuerdig. Zum Glueck ist das schnell wieder "
        "vorbei.",78));
    }
  }
  return 1;
}

private int act_attr_heal_lp(int effekt)
{
  if (effekt > 0)
  {
    tell_object(environment(),
        BS("Du fuehlst Dich schlagartig "+num2desc(effekt)
          + " besser."));
    environment()->restore_hit_points(effekt/10);
  }
  else
  {
    tell_object(environment(),
        BS("Du fuehlst Dich schlagartig "+num2desc(effekt)
          + " schlechter."));
    environment()->do_damage(-effekt/10);
  }
  return 1;
}

private int act_attr_heal_sp(int effekt)
{
  if (effekt > 0)
  {
    tell_object(environment(),
        BS("Du fuehlst Dich schlagartig "+num2desc(effekt)
          + " konzentrierter."));
    environment()->restore_spell_points(effekt/10);
  }
  else
  {
    tell_object(environment(),
        BS("Du fuehlst Dich schlagartig "+num2desc(effekt)
          + " unkonzentrierter."));
    environment()->reduce_spell_points(-effekt/10);
  }
  return 1;
}


private int modify_sa(string sa, int effekt)
{
  string msg;
  switch (sa)
  {
    case SA_SPEED:
      msg = "Du wirst gerade " + num2desc(effekt)
            +(effekt>0 ? " schneller." : " langsamer.");
      effekt = (effekt * 30) / 2000;
      break;
    case SA_DURATION:
      msg = "Du wirst gerade " + num2desc(effekt)
            +(effekt>0 ? " ausdauernder."
                       : " weniger ausdauernd.");
      effekt = (effekt * 25) / 2000;
      break;
    case SA_SPELL_PENETRATION:
      msg = "Deine geistige Durchsetzungskraft hat sich gerade "
            + num2desc(effekt)
            +(effekt>0 ? " verbessert." : " verschlechtert.");
      effekt = (effekt * 30) / 2000;
      break;
  }
  if (environment()->ModifySkillAttribute(sa, effekt, duration-time()) ==
        SA_MOD_OK)
  {
    tell_object(environment(),BS(msg));
    return 1;
  }
  return 0;
}

private int act_attr_change_sa_speed(int effekt)
{
  return modify_sa(SA_SPEED, effekt);
}
private int act_attr_change_sa_duration(int effekt)
{
  return modify_sa(SA_DURATION, effekt);
}
private int act_attr_change_sa_spell_penetration(int effekt)
{
  return modify_sa(SA_SPELL_PENETRATION, effekt);
}


// Spieler MUSS das environment() sein!
private void effekt()
{
  // Als erstes die Wirkungsdauer verwursten, einige Effekte brauchen
  // <duration>.
  // Wann laufen die Effekte denn spaetenstens ab?
  duration = time() + data[T_EFFECT_DURATION];
  call_out(#'terminate_effects, data[T_EFFECT_DURATION]);

  // nur echte wirkungen beruecksichtigen, keine "Metadaten"
  mapping effects = data & T_KRAUT_EFFECTS;
  // fuer Spieler wird der Paratrans nicht stattfinden.
  if (!IS_SEER(environment()))
    m_delete(data, T_CHANGE_DIMENSION);

  // Waehrend der Wirkung ist dieses Objekt schonmal unsichtbar.
  SetProp(P_INVIS, 1);
  // Gewicht nullen, bevor die Wirkungen aktiviert werden, da die
  // Wirkung von T_CARRY ansonsten wieder deaktiviert wuerde. 
  SetProp(P_WEIGHT, 0);
  // neue, leere Flasche ins Inventar des Spielers bewegen.
  clone_object(TRANKITEM)->move(environment(),M_NOCHECK|M_SILENT);

  // auftrennen in positive und negative Effekte. Keys mit Wert 0 werden
  // ignoriert.
  mapping peff = filter(effects, function int (string k, int val)
                        {return val > 0;});
  mapping neff = filter(effects, function int (string k, int val)
                        {return val < 0;});
  // erst die positiven, dann die negativen Wirkungen aktivieren
  // fuer jede Wirkung wird eine lfun act_<trankattribut>() gerufen, z.B.
  // act_attr_tragen() (-> act_T_CARRY() )
  mapping notactivated =
          filter(peff, function int (string k, int val)
              {return !funcall(symbol_function("act_"+k,this_object()), val);})
         +
          filter(neff, function int (string k, int val)
              {return !funcall(symbol_function("act_"+k,this_object()), val);});
  // Meldungen ausgeben ueber nicht aktivierte Wirkungen?
  // TODO
}

static int cmd_trinken(string str, mixed *param)
{
  if (environment(this_object()) != PL)
  {
    write("Aus auf dem Boden liegenden Flaschen trinkt es sich so "
       "schlecht!\n");
  }
  else if (data==0)
  {
    write("Die Flasche ist leer, Du muesstest erst etwas hineinfuellen.\n");
  }
  else if (time()>expiry) 
  {
    write(BS("Irgendwie passiert nichts. Hattest Du vielleicht doch nur "
      "klares Wasser abgefuellt?"));
    data=0;
  }
  else {
    // Die Sperrzeit muss hier separat berechnet werden, weil eine
    // Anpassung der Wirkungsdauer im Krautmaster dazu fuehren wuerde,
    // dass es keine instantan wirkenden Traenke mehr gaebe.
    int blocktime = max(60+random(60), data[T_EFFECT_DURATION]);
    if(environment()->check_and_update_timed_key(blocktime, DRINK_POTION)==-1)
    {
      write(BS("Du oeffnest die Flasche und trinkst ihren Inhalt aus."));
      say(BS(PL->Name(WER, 0)+" oeffnet eine Flasche und trinkt sie in einem "
        "Schluck aus."));
      effekt();
      // TODO: reicht das hier aus, oder muss das noch an anderen Stellen 
      // eingebaut werden?
      RemoveId(({"trank","kraeutertrank"}));
    }
    else {
      tell_object(environment(), BS(
        "Der letzte Trank wirkt noch ein wenig nach. Du kannst Deinem "
        "Magen nicht so bald schon den naechsten zumuten."));
    }
  }
  return 1;
}

public nomask int Fill(object *plants)
{
  if (!pointerp(plants)) return -1;
  if (data) return -2;  // schon voll
  data = PLANTMASTER->make_potion(plants);
  AddId(({"trank","kraeutertrank"}));
  if (mappingp(data))
  {
    // Pflanzen zerstoert der Master, wenns geklappt hat.
    expiry=time()+data[T_EXPIRE];
    SetProp(P_WEIGHT, 300);
    return 1;
  }
  return -3;
}

void NotifyPlayerDeath(object vic, object killer, int exp)
{
  call_out("remove",1);
}
