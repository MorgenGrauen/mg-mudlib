// MorgenGrauen MUDlib
//
// shells/feline.c -- Feline Shell
//
// $Id: feline.c 8487 2013-05-21 19:15:52Z Zesstra $

#pragma strong_types,save_types

inherit "/std/player/base";

#include <attributes.h>
#include <combat.h>
#include <health.h>
#include <new_skills.h>
#include <properties.h>
#include <language.h>
#include <wizlevels.h>
#include <defuel.h>


void create()
{   
    if (!clonep() || object_name(this_object()) == __FILE__[0..<3]) {
      set_next_reset(-1);    
      return;
    }

    int   i,g;
    mixed h;

    base::create();

// Startraum/Kapelle setzen
    SetDefaultHome("/d/dschungel/paracelsus/room/fkapelle");
    SetDefaultPrayRoom("/d/dschungel/paracelsus/room/fkapelle");

// Besondere rassenspezifische Properties (bei denen es nichts macht, wenn
// sie nach jedem Einloggen neu gesetzt werden):

    SetProp(P_ATTRIBUTES_OFFSETS, // Summe 4 statt 6 wg. SA_SPEED
    ([
        A_STR :  1,
        A_INT :  2,
        A_DEX :  2,
        A_CON : -1 
    ]) );
    SetProp(P_AVERAGE_SIZE,200);
    SetProp(P_AVERAGE_WEIGHT,85000);
    SetProp(P_BODY,15);
    SetProp(P_SKILL_ATTRIBUTE_OFFSETS,([SA_SPEED:120]));
    SetProp(P_MATERIAL_KNOWLEDGE,
    ([
        MATGROUP_WOOD    :  60,
        MATGROUP_JEWEL   : 100,
        MATGROUP_EATABLE :  30
    ]) );

    SetProp(P_MAGIC_RESISTANCE_OFFSET,
    ([
        MT_ANGRIFF     :  200,
        MT_ILLUSION    : -500,
        MT_VERWANDLUNG :  500,
        MT_PSYCHO      : 1000 
    ]) );

    SetProp(P_RESISTANCE_STRENGTHS,
    ([
        DT_WATER : -0.1,
        DT_ACID  :  0.1,
        DT_COLD  :  0.1 
    ]) );

// Lebenspunkte werden langsamer als normal regeneriert
    SetProp(P_HP_DELAY,HEAL_DELAY+1);

// Magiepunkte werden schneller als normal regeneriert
    SetProp(P_SP_DELAY,HEAL_DELAY-1);

// Gift wirkt etwas langsamer als normal
    SetProp(P_POISON_DELAY,POISON_DELAY+1);

// Getraenke werden etwas schneller als normal abgebaut
    SetProp(P_DRINK_DELAY,DRINK_DELAY-1);

// Essen wird etwas schneller als normal abgebaut ...
    SetProp(P_FOOD_DELAY,FOOD_DELAY-1);
    SetProp(P_MAX_FOOD,140);

// Es gibt einige Sachen, die sollen nur beim ersten Einloggen gesetzt werden.
// Andere muessen nachtraeglich - aber nur einmal - gemacht werden, weil sich
// etwas geaendert hat.
    switch( QueryProp(P_SHELL_VERSION) )
    {
        case 0 :

            g=QueryProp(P_GENDER);

            if ( !(i=QueryProp(P_SIZE)) || (i<(g==FEMALE?165:170)) 
                || (i>(g==FEMALE?225:230)) )
            {
                SetProp(P_SIZE, (g==FEMALE?195:200)
                    + random(16) - random(16) + random(16) - random(16) );
            }

            if( !(i=QueryProp(P_WEIGHT)) || (i<(g==FEMALE?70000:77000)) 
               || (i>(g==FEMALE?88000:95000)) || (i==75000) )
            {
                SetProp(P_WEIGHT, (g==FEMALE?70000:77000)
                    + random(4501) + random(4501) + random(4501) + random(4501) );
            }


            SetProp(P_MATERIAL,([
                MAT_MISC_LIVING : 90,
                MAT_PELT        :  8,
                MAT_HORN        :  2
            ]) );

            if ( !IS_SEER(this_object()) )
            {
                SetProp(P_MSGIN,"schleicht herein");
                SetProp(P_MSGOUT,"schleicht");
                SetProp(P_MMSGIN,"erscheint mit einem grellen Blitz");
                SetProp(P_MMSGOUT,"verschwindet mit einem grellen Blitz");
                SetProp(P_HANDS,({" mit scharfen Krallen",40, ({DT_RIP}) }));
            }
            else
            {
                if ( !pointerp(h=QueryProp(P_HANDS)) || (sizeof(h)<1) )
                    h = ({" mit scharfen Krallen",40, ({DT_RIP}) });
                else
                    h = ({h[0],40, ({DT_RIP}) });
                SetProp(P_HANDS, h);
            }

            if ( !pointerp(h=QueryProp(P_CHANNELS)) )
                SetProp(P_CHANNELS,({"katzenkrieger"}));
            else if ( member(h,"katzenkrieger")==-1 )
                SetProp(P_CHANNELS, h + ({"katzenkrieger"}) );

            Set(P_SIZE,SAVE,F_MODE_AS);
            Set(P_MATERIAL,SAVE,F_MODE_AS);
            Set(P_WEIGHT,SAVE,F_MODE_AS);

        default :

            SetProp(P_SHELL_VERSION,1);
    }
  SetProp(P_DEFUEL_LIMIT_FOOD,70);
  SetProp(P_DEFUEL_LIMIT_DRINK,40);
  SetProp(P_DEFUEL_TIME_FOOD,400);
  SetProp(P_DEFUEL_TIME_DRINK,300);
  SetProp(P_DEFUEL_AMOUNT_FOOD,0.55);
  SetProp(P_DEFUEL_AMOUNT_DRINK,0.4);

}

// Diese Rasse kann derzeit gewaehlt werden:
int QueryAllowSelect() { return 1; }

// Rassenbezeichnung
string _query_race()
{
    return "Feline";
}

string _query_real_race()
{
    return "Feline";
}

// Die Rassenbeschreibung, die man beim ersten Einloggen abrufen kann.
string _query_racedescr()
{
    return break_string(
        "Felinen sind aufrecht gehende Katzenwesen.\n"+
        "Ihre Heimat ist der Dschungel. Kaum jemand duerfte sich dort "+
        "besser zurechtfinden als sie. Bedingt durch diese Umgebung "+
        "haben sie im Laufe der Zeit eine Vorliebe fuer elegante Hoelzer "+
        "und funkelnde Edelsteine entwickelt. Sie sind zwar nicht so "+
        "'raffgierig' wie Zwerge, aber dennoch sollte man besser nicht "+
        "versuchen, einem Felinen einen Edelstein wegzunehmen. Sie "+
        "benutzen die Edelsteine sehr gerne, um sich damit zu schmuecken. "+
        "Felinen betreiben sogar einen regelrechten Koerperkult, "+
        "insbesondere wenn es darum geht, das Fell oder die Krallen zu "+
        "faerben. Edelsteine kommen da als Accessoires gerade recht.\n"+
        "Auch im Kampf gegen einen Felinen sollte man sehr vorsichtig "+
        "sein, da Felinen ihre geringe Ausdauer durch eine hohe "+
        "Geschwindigkeit sowie Geschick und Intelligenz wettmachen. "+
        "Auch die Spitzen Krallen sind da nicht zu verachten und so "+
        "mancher Gegner musste schon als Ersatz fuer einen Kratzbaum "+
        "herhalten.",78,0,1);
}

// Geschlechtsabhaengiges Rassenbezeichnungs-Array
string *_query_racestring()
{
    if (QueryProp(P_GENDER)==2)
        return ({"Felinin","Felinin","Felinin","Felinin"});
    return ({"Feline","Felinen","Felinen","Felinen"});
}

// Regeneration der Lebenspunkte ist von der Umgebung abhaengig
// Im Wald und Dschungel geht es schneller, in der Wueste und im
// Polargebiet dagegen langsamer.
int _query_hp_delay()
{
    int re;
    string fn;

    re = Query(P_HP_DELAY);
    if (environment() && !(environment()->QueryProp(P_INDOORS)) &&
        fn=object_name(environment()))
    {
        if (fn[0..12]=="/d/dschungel/" || fn[0..7]=="/d/wald/")
            re--;
        else if (fn[0..9]=="/d/wueste/" || fn[0..8]=="/d/polar/")
            re++;
    }
    return re;
}

// Regeneration der Magiepunkte ist von der Umgebung abhaengig
// Im Wald und Dschungel geht es schneller, in der Wueste und im
// Polargebiet dagegen langsamer.
int _query_sp_delay()
{
    int re;
    string fn;

    re = Query(P_SP_DELAY);
    if (environment() && !(environment()->QueryProp(P_INDOORS)) &&
        fn=object_name(environment()))
    {
        if (fn[0..12]=="/d/dschungel/" || fn[0..7]=="/d/wald/")
            re--;
        else if (fn[0..9]=="/d/wueste/" || fn[0..8]=="/d/polar/")
            re++;
    }
    return re;
}

string _query_default_guild(){
  return "katzenkrieger";
}

mixed RaceDefault(string arg)
{
    if (!arg)
      return 0;
    switch(arg)
    {
        case P_HANDS :
            return ({" mit scharfen Krallen",40, ({DT_RIP}) });
        case P_BODY :
            return 15;
    }
    return base::RaceDefault(arg);
}
