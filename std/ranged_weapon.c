// MorgenGrauen MUDlib
//
// ranged_weapon.c -- Standardobjekt fuer Fernkampfwaffen
//
// $Id: ranged_weapon.c 8820 2014-05-14 19:31:46Z Zesstra $

// Gebraucht wird Munition mit einer MUN_* id. Dieses Objekt sollte ausserdem
// P_DAM_TYPE setzen sowie P_SHOOTING_WC und ggf. P_HIT_FUNC. In der Regel
// sollten diese Objekte units sein, da groessere Anzahlen davon gebraucht
// werden.
#pragma strong_types
#pragma save_types
#pragma no_clone
#pragma pedantic
#pragma range_check

inherit "/std/living/skill_utils";
inherit "/std/weapon";

#include <ranged_weapon.h>

#include <properties.h>
#include <defines.h>
#include <combat.h>
#include <new_skills.h>
#include <unit.h>

static int last_shoot;

protected void create()
{
    ::create();

    SetProp(P_WEAPON_TYPE, WT_RANGED_WEAPON);
    SetProp(P_STRETCH_TIME, 1);
    SetProp(P_AMMUNITION, MUN_ARROW);
    SetProp(P_SHOOTING_WC, 70);
    SetProp(P_RANGE, 50);
    SetProp(P_INFO, "Syntax: schiesse WAS auf WEN?\n");
    // fuer den Fall, das man den Bogen als Knueppel nutzt...
    SetProp(P_WC, 30);
    SetProp(P_QUALITY, 100);

    AddCmd( ({"schiess", "schiesse"}), "cmd_shoot");
}
      
static int shoot_dam(mapping shoot)
// ausgegliedert, damit man diese Funktion ggf. ueberschreiben kann.
{   mixed   res;
    object  hitfunc;
    closure usk;

    // diesselbe Formel wie bei Nahkampfwaffen, nur das hier DEX anstatt STR
    // benutzt wird, und die WC der Munition noch addiert wird
    shoot[SI_SKILLDAMAGE] = random(1+( 2*(shoot[P_WC]+shoot[P_SHOOTING_WC])
                                     + 10*PL->QueryAttribute(A_DEX) )/3);
    // HitFunc der Munition addieren
    if ( objectp(hitfunc=(shoot[P_AMMUNITION]->QueryProp(P_HIT_FUNC))) )
      shoot[SI_SKILLDAMAGE] += (hitfunc->HitFunc(shoot[SI_ENEMY]));

    usk=symbol_function("UseSkill",PL);

    // Schuss-Skill der Gilde fuer diesen Munitionstyp anwenden
    if ( mappingp(res=funcall(usk,SHOOT(shoot[P_WEAPON_TYPE]),
                              deep_copy(shoot))) )
      SkillResTransfer(res, &shoot);

    // Allgemeinen Schuss-Skill der Gilde anwenden
    if ( mappingp(res=funcall(usk,SK_SHOOT,deep_copy(shoot))) )
      SkillResTransfer(res, &shoot);

    return shoot[SI_SKILLDAMAGE];
}

static string FindRangedTarget(string str, mapping shoot)
{   string wen;
    mixed  area;
    int    psr;

    if ( sscanf(str, "%s auf %s", str, wen)!=2 )
    {
        shoot[SI_ENEMY]=PL->SelectFarEnemy(PL->PresentEnemies(),1,4);
        str = shoot[P_WEAPON_TYPE];
    }
    else if ( objectp(shoot[SI_ENEMY]=present(wen, environment(PL))) )
    {
        return str;
    }
    else if ( ( (area=(environment(PL)->QueryProp(P_TARGET_AREA)))
               || (area=environment(environment(PL))) )
             && ( objectp(area)
                 || ( stringp(area)
                     && objectp(area=find_object(area)) ) )
             && (psr=environment(PL)->QueryProp(P_SHOOTING_AREA)) )
    {
        shoot[SI_ENEMY]=present(wen, area);
        if ( shoot[SI_ENEMY] && (psr>QueryProp(P_RANGE)) )
        {
            write(break_string((shoot[SI_ENEMY]->Name(WER))+
                " ist leider ausserhalb der Reichweite "+name(WESSEN, 1)+".",
               78));
            return 0;
         }        
    }

    if ( !shoot[SI_ENEMY] )
    {
        write("Es ist kein Feind vorhanden auf den Du schiessen koenntest.\n");
        return 0;
    }

    return str;
}

static int cmd_shoot(string str)
{   int    dam;
    string no_at;
    object quiver;
    mapping shoot;

    if ( PL->QueryProp(P_GHOST) )
    {
        write("Deine immateriellen Finger gleiten durch die Waffe hindurch.\n");
        return 1;
    }
   
    if ( !QueryProp(P_NOGET) )
    {
        // Katapulte oder aehnliches die von Magiern aufgestellt werden
        // muessen natuerlich nicht gezueckt werden :).
        if ( !QueryProp(P_WIELDED) )
        {
            notify_fail(break_string("Du solltest "+name(WEN, 1)+
                                     " dazu schon zuecken.", 78));
            return 0;
        }

        if ( (PL->InFight()) && ((PL->PresentPosition())<2) )
        {
            write(break_string(
                "Du solltest Dich erst etwas weiter zurueckziehen, um mit "+
                name(WEM, 1)+" schiessen zu koennen!", 78));
            return 1;
        }
    }

    if ( ( (QueryProp(P_EQUIP_TIME)+QueryProp(P_STRETCH_TIME)*2) > time() )
        || ( (last_shoot+QueryProp(P_STRETCH_TIME)) > absolute_hb_count() ) )
    {
        write(break_string("Du kannst mit "+name(WEM, 1)+
                           " noch nicht wieder schiessen.", 78));
        return 1;
    }

    if ( (PL->QueryProp(P_DISABLE_ATTACK))>0 )
    {
        write(break_string("Solange Du noch gelaehmt bist, kannst Du "+
                           name(WEN, 1)+" nicht benutzen!", 78));
        return 1;
    }

    if ( PL->QueryProp(P_ATTACK_BUSY) )
    {
        write("Nicht so hektisch! So schnell bist Du nicht.\n");
        return 1;
    }

    // P_ATTACK_BUSY natuerlich auch setzen...
    PL->SetProp(P_ATTACK_BUSY,1);

    // Info-Mapping erzeugen
    shoot = ([ P_WEAPON       : ME,
               P_WEAPON_TYPE  : QueryProp(P_AMMUNITION),
               P_STRETCH_TIME : QueryProp(P_STRETCH_TIME),
               P_WC           : QueryProp(P_SHOOTING_WC),
               SI_SPELL       : 0
            ]);

    if ( !stringp(str) || str=="" )
      str=shoot[P_WEAPON_TYPE];
    else if (str[0..3]=="mit ")
      str=str[4..];
    else if (str[0..3]=="auf ")
      str=shoot[P_WEAPON_TYPE]+" "+str;

    if ( !(str=FindRangedTarget(str,shoot)) )
      return 1;

    if ( shoot[SI_ENEMY]->QueryProp(P_GHOST) )
    {
        write(break_string("Aber "+(shoot[SI_ENEMY]->name(WER, 1))+
                           " ist doch ein Geist!", 78));
        return 1;
    }
    else if ( no_at=(shoot[SI_ENEMY]->QueryProp(P_NO_ATTACK)) )
    {
        if ( stringp(no_at) )
          write(no_at);
        else
          write(break_string("Du kannst "+(shoot[SI_ENEMY]->name(WEN, 1))+
                             " nicht angreifen.", 78));
        return 1;
    }
    else if ( shoot[SI_ENEMY]==PL )
    {
        write("Du kannst doch nicht auf Dich selbst schiessen!\n");
        return 1;
    }

    if ( !(shoot[P_AMMUNITION]=present(str, PL))
        && ( !objectp(quiver=PL->QueryArmourByType(AT_QUIVER))
            || !(shoot[P_AMMUNITION]=present(str, quiver)) ) )
    {
        if ( str==shoot[P_WEAPON_TYPE] )
            write(break_string("Du hast keine passende Munition bei Dir, um "+
                               "mit "+name(WEM, 1)+" schiessen zu koennen.",
                               78));
        else
          write(break_string("Du hast kein '"+str+"' bei Dir.", 78));
        return 1;
    }

    if ( !shoot[P_AMMUNITION]->id(shoot[P_WEAPON_TYPE]) )
    {
        write(break_string((shoot[P_AMMUNITION]->Name(WER))+" kannst Du mit "+
                           name(WEM, 1)+" nicht verschiessen.", 78));
        return 1;
    }

    shoot[P_AMMUNITION]->SetProp(U_REQ, 1);

    shoot[SI_SKILLDAMAGE_MSG]  = shoot[SI_SKILLDAMAGE_MSG2]
                               = shoot[P_AMMUNITION]->name(WEN);
    shoot[SI_SKILLDAMAGE_TYPE] = ( shoot[P_AMMUNITION]->QueryProp(P_DAM_TYPE)
                                  || ({DT_PIERCE}) );

    shoot[P_SHOOTING_WC]  = shoot[P_AMMUNITION]->QueryProp(P_SHOOTING_WC);

    dam=shoot_dam(shoot);

    // Textausgabe
    if (shoot[SI_ENEMY])
    {
      tell_object(PL,break_string(
          "Du schiesst "+shoot[SI_SKILLDAMAGE_MSG]+" auf "+
          shoot[SI_ENEMY]->name(WEN, 1)+".", 78) );
      tell_room(environment(PL), break_string(
          PL->Name(WER)+" schiesst "+shoot[SI_SKILLDAMAGE_MSG2]+" auf "+
          shoot[SI_ENEMY]->name(WEN, 1)+".", 78),
          ({ shoot[SI_ENEMY], PL }) );
      if ( environment(PL)!=environment(shoot[SI_ENEMY]) )
        tell_room(environment(shoot[SI_ENEMY]),break_string(
            PL->Name(WER)+" schiesst "+shoot[SI_SKILLDAMAGE_MSG2]+" auf "+
            shoot[SI_ENEMY]->name(WEN, 1)+".", 78),
            ({ shoot[SI_ENEMY] }) );
      tell_object(shoot[SI_ENEMY], break_string(
          PL->Name(WER)+" schiesst "+shoot[SI_SKILLDAMAGE_MSG2]+
          " auf Dich ab.",78) );

      shoot[SI_ENEMY]->Defend(dam,shoot[SI_SKILLDAMAGE_TYPE],shoot[SI_SPELL],PL);
    }
    // Munition verbrauchen
    if ( shoot[P_AMMUNITION]->IsUnit() )
      shoot[P_AMMUNITION]->AddAmount(-1);
    else
      shoot[P_AMMUNITION]->remove();

    // es wird an dieser stelle absolute_hb_count anstelle von time() benutzt
    // um dem Spieler wirklich die vollen 2 sek Zeit zu geben um zu reagieren
    // ohne das er dadurch einen Schaden hat (wie er es z.b. bei time() haette)
    last_shoot=absolute_hb_count();

    return 1;
}

