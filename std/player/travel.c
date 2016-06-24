
/* 'reise' handling
 *
 * Ueberarbeitete und 
 * erweiterte Version: Tilly@MorgenGrauen, 10.01.02
 * Basierend auf     : base.c@SilberLand,
 *                     Revision 3.55, Woody@SilberLand, 11.05.99
 */   
#pragma strong_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#define NEED_PROTOTYPES
#include <player.h>
#include <living/moving.h>
#include <thing/properties.h>
#undef NEED_PROTOTYPES

#include <properties.h>
#include <config.h>
#include <wizlevels.h>
#include <moving.h>
#include <living.h>
#include <attributes.h>
#include <defines.h>
#include <new_skills.h>
#include <combat.h>
#include <transport.h>

void create(){}

private string _traveller(string *s)
{
  switch (sizeof(s))
  {
    case 1 : return s[0];
    case 2 : return s[0]+" oder "+s[1];
  }
  return efun::implode(s[0..<2],", ")+" oder "+s[<1];
}

static int reise(string str)
{
  mixed  t, ship, dstr;
  string mit, nach, s1, s2;
  int    i;

  _notify_fail("Syntax: reise mit <transportmittel> nach <zielort>\n\n"
               "Weitere Syntaxen moeglich, bitte 'hilfe reise' lesen.\n");

  t = QueryProp(P_TRAVEL_INFO);
  
  if (!pointerp(t) || (sizeof(t) < 4) || !objectp(t[0]) || !objectp(t[1]))
  {
    SetProp(P_TRAVEL_INFO, t = 0);
  }
/* * * * 
 * REISE
 * * * */
  if (!str)
  {
    if (!t)
    {
      write("Du hast keine Reiseroute festgelegt.\n");
    }
    else if (t[0] == environment())
    {
      if (environment()->id("Transporter"))
      {
        write(sprintf("Du reist mit %s%s.\n",environment()->name(WEM,1), 
                                             t[2]?" "+t[3]:""));
      }
      else
      {
        write(sprintf("Du wirst mit %s%s reisen.\n",t[1]->name(WEM,1), 
                                                   t[2]?" "+t[3]:""));
      }
    }
    else
    {
       write(sprintf("Deine letzte Route (mit %s%s) ist hier nicht wirksam.\n",
                     t[1]->name(WEM,1),
                     t[2]?" "+t[3]:""));
    }
    return 1;
  }

  str = lower_case( _unparsed_args() );
/* * * * * * * 
 * REISE ROUTE
 * * * * * * */
  if (str == "route")
  {
    string *harbours;
    
    if (environment()->id("Transporter"))
    {
      if (environment()->QueryProp(P_NO_TRAVELING))
      {
        write(break_string(
         capitalize(environment()->name(WER,1))+" hat gar keine "
         "Reiseroute. Wo Dich das wohl hinfuehrt?",78));
        return 1;
      }

      harbours = environment()->QueryHarbours(1);
      
      if (!sizeof(harbours) || !stringp(harbours[0]))
      {
        write("Die Reiseroute "+environment()->name(WESSEN,1)+" ist "
             +"leider nicht bekannt.\n");
      }
      else
      {
        write(break_string(capitalize(efun::implode(harbours," - "))+".",78,
         "Reiseroute "+environment()->name(WESSEN,1)+": ",BS_INDENT_ONCE));
      }
      return 1;
    }
    if (environment()->QueryProp(P_NO_TRAVELING))
    {
      write(break_string("Hier kannst Du leider keine Reiseroute waehlen. "
                         "Was nicht heisst, dass hier kein Transportmittel "
                         "verkehrt.",78));
      return 1;
    }
    if (!pointerp(ship = TRAVELD->HasTransporter(environment())))
    {
      _notify_fail("Hier verkehrt kein Transportmittel.\n");
      return 0;
    }

    write("Hier verkehren folgende Transportmittel \n"
          "--------------------------------------- \n");
         
    for (i = 0; i < sizeof(ship); i++)
    {    
      if (!ship[i]->Query(P_SHORT)) continue;
      
      harbours = ship[i]->QueryHarbours(1);

      if (sizeof(harbours) && stringp(harbours[0]))
      {
        write(break_string(efun::implode(harbours," - ")+".",78,
                           ship[i]->Query(P_SHORT)+": ",BS_INDENT_ONCE));
      }
      else
      {
        write(ship[i]->Query(P_SHORT)+": Route unbekannt.\n");
      }
    }
    return 1;
  }
/* * * * * * * 
 * REISE AUS
 * REISE NICHT
 * * * * * * */
  if (member((["aus","nicht"]),str))
  {
    if (!t)
    {
      write("Du hattest keine Reiseroute eingestellt.\n");
    }
    else
    {
      write("Du loeschst Deine Reiseroute.\n");
    }
    SetProp(P_TRAVEL_INFO, 0);
    return 1;
  }

  str = regreplace(str,"\\<(zu|zum|zur|ins|ans)\\>","nach",0);
/* * * * * * * * *
 * REISE MIT NACH
 * REISE NACH MIT
 * * * * * * * * */
  if ((sscanf(str, "mit %s nach %s", mit, nach) == 2) ||
      (sscanf(str, "nach %s mit %s", nach, mit) == 2))
  {
    _notify_fail("Hier kannst Du leider keine Reiseroute waehlen.\n");

    if (environment()->QueryProp(P_NO_TRAVELING))
    {
      return 0;
    }
    if (!sizeof(nach))
    {
      _notify_fail("Syntax: reise mit <transportmittel> nach <zielort>\n"
                   "        reise nach <zielort> mit <transportmittel>\n");
      return 0;
    }
    if (environment()->id("Transporter"))
    {
      if (environment()->id(mit))
      {
        command("reise nach "+nach);
        return 1;
      }
      else
      {
        _notify_fail("Beende erstmal Deine aktuelle Reise mit "+
                      environment()->name(WEM,1)+".\n");
        return 0;
      }
    }
    if (!pointerp(ship = TRAVELD->HasTransporter(environment(), mit)))
    {
      _notify_fail("So ein Transportmittel verkehrt hier nicht.\n");
      return 0;
    }
    for (i = sizeof(ship) -1 ; i >= 0; i--)
      if (!ship[i]->HasRoute(nach))
      {
        ship[i] = 0;
      }

    ship -= ({0});

    if (pointerp(t) && objectp(t[1]) && (member(ship,t[1]) != -1))
    {
      ship = ({ t[1] });
    }
    if (sizeof(ship) > 1)
    {
      if (object_name(environment()) == ship[0]->HasRoute(nach)[0])
      {
        _notify_fail("Aber da bist Du doch bereits.\n");
        return 0;
      }
     write("Dorthin kannst Du mit "+CountUp(map_objects(ship,"name",WEM))
           +"reisen.\n");
 

      dstr = filter( filter_objects(ship,"short"), lambda( ({'x}),
             ({ #'==, ({#'environment, 'x}), environment() }) ) );

      if (sizeof(dstr))
      {
        ship = dstr[0];
      }
      else
      {
        ship = ship[0];
      }

      dstr = ship->HasRoute(nach);

      write(sprintf("Du entscheidest Dich fuer %s und reist %s.\n",
            ship->name(WEN,1),dstr[1]));
    }
    else if (sizeof(ship) < 1)
    {
      _notify_fail("Nach '"+capitalize(nach)+"' kann Dich das angegebene "
                  +"Transportmittel leider nicht bringen.\n");
      return 0;
    }
    else
    {
      ship = ship[0];
      dstr = ship->HasRoute(nach);

      if (object_name(environment()) == dstr[0])
      {
        _notify_fail("Aber da bist Du doch bereits.\n");
        return 0;
      }
      if (t && stringp(t[2]))
      {
        if (t[2] == dstr[0])
        {
          _notify_fail("Aber das tust Du doch bereits.\n");
          return 0;
        }
      }
      write(sprintf("Ok, Du reist nun mit %s %s.\n",
                     ship->name(WEM,1),dstr[1]));
    }
    if (environment(ship)==environment() && ship->short())
    {
      ship->Enter(this_object());
    }
    SetProp(P_TRAVEL_INFO, ({ environment(), ship, dstr[0], dstr[1] }) );
    return 1;
  }
/* * * * * * *
 * REISE NACH
 * * * * * * */
  if (sscanf(str,"nach %s",nach))
  {
    _notify_fail("Hier kannst Du leider keine Reiseroute waehlen.\n");

    if (environment()->QueryProp(P_NO_TRAVELING))
    {
      return 0;
    }
    if (environment()->id("Transporter"))
    {
      if (!dstr = environment()->HasRoute(nach))
      {
        _notify_fail("Dorthin kann Dich "+environment()->name(WER,1)+
                     " leider nicht bringen.\n");
        return 0;
      }
      if (t && stringp(t[2]))
      {
        if (t[2] == dstr[0])
        {
          _notify_fail("Aber das tust Du doch bereits.\n");
          return 0;
        }
      }
      write(sprintf("Ok, Du reist jetzt mit %s %s.\n",
                    environment()->name(WEM,1),dstr[1]));
      
      if (IS_WIZARD(this_object()))
      {
        write("Als Magier nimmst Du natuerlich die Abkuerzung.\n");
        move(dstr[0],M_NOCHECK);
        return 1;
      }
      SetProp(P_TRAVEL_INFO,({ environment(), 
                               environment(), 
                               dstr[0], 
                               dstr[1] }) );

      if (object_name(environment(ship = environment())) == dstr[0] && 
          ship->short())
      {
        environment()->Leave(this_object());

        if (environment() != ship)
        {
          SetProp(P_TRAVEL_INFO, 0);
        }
      }
      return 1;
    }
    if (!pointerp(ship = TRAVELD->HasTransporter(environment())))
    {
      _notify_fail("Von hier aus kannst Du nicht reisen.\n");
      return 0;
    }
    for (i = sizeof(ship) - 1; i >= 0; i--)
      if (!ship[i]->HasRoute(nach))
      {
        ship[i] = 0;
      }

    ship -= ({ 0 });
 
    if (pointerp(t) && objectp(t[1]) && (member(ship,t[1]) != -1))
    {
      ship = ({ t[1] });
    }
    if (sizeof(ship) > 1)
    {
      if (object_name(environment()) == ship[0]->HasRoute(nach)[0])
      {
        _notify_fail("Aber da bist Du doch bereits.\n");
        return 0;
      }

      write(break_string("Dahin kannst Du mit "
           +_traveller(map_objects(ship, "name", WEM))+" gelangen.",78));

      dstr = filter(filter_objects(ship,"short"),lambda( ({'x}),
            ({ #'==, ({#'environment, 'x}), environment() }) ) );

      if (sizeof(dstr))
      {
        ship = dstr[0];
      }
      else
      {
        ship = ship[0];
      }

      dstr = ship->HasRoute(nach);

      write(sprintf("Du waehlst %s und reist %s.\n",ship->name(WEN,1),
                                                    dstr[1]));
    }
    else if (sizeof(ship) < 1)
    {
      _notify_fail("Nach '"+capitalize(nach)+"' kann Dich leider keines der "
                  +"hier verkehrenden Transportmittel bringen.\n");
      return 0;
    }
    else
    {
      ship = ship[0];
      dstr = ship->HasRoute(nach);

      if (object_name(environment()) == dstr[0])
      {
        _notify_fail("Aber da bist Du ja bereits.\n");
        return 0;
      }
      else if (t && stringp(t[2]))
      {
        if (t[2] == dstr[0])
        {
          _notify_fail("Aber das tust Du doch bereits.\n");
          return 0;
        }
      }
      write(sprintf("Ok, Du reist nun mit %s %s.\n",ship->name(WEM),
                                                    dstr[1]));
    }
    if (IS_WIZARD(this_object()))
    {
      write("Als Magier nimmst Du natuerlich die Abkuerzung.\n");
      move(dstr[0],M_NOCHECK);
      return 1;
    }
    if (environment(ship)==environment() && ship->short())
    {
      ship->Enter(this_object());
    }
    SetProp(P_TRAVEL_INFO, ({ environment(), ship, dstr[0], dstr[1] }) );
    return 1;
  }
/* * * * * *
 * REISE MIT
 * * * * * */
  if (sscanf(str, "mit %s", mit))
  {
    _notify_fail("Hier kannst Du leider keine Reiseroute waehlen.\n");

    if (environment()->QueryProp(P_NO_TRAVELING))
    {
      return 0;
    }
    if (environment()->id("Transporter"))
    {
      if (environment()->id(mit))
      {
        _notify_fail("Aber das tust Du doch bereits.\n");
        return 0;
      }
      else
      {
        _notify_fail("Beende erstmal Deine aktuelle Reise mit "+
                      environment()->name(WEM,1)+".\n");
        return 0;
      }
    }
    if (t && objectp(t[1]) && t[1]->id(mit) && t[0] == environment())
    {
      _notify_fail("Aber das tust Du doch bereits.\n");
      return 0;
    }
    if (!pointerp(ship = TRAVELD->HasTransporter(environment(),mit)))
    {
      _notify_fail("So ein Transportmittel verkehrt hier nicht.\n");
      return 0;
    }
    if (sizeof(ship) > 1)
    {
      write("'"+capitalize(mit)+"' koennte "
               +_traveller(map_objects(ship,"name",WER))+" sein.\n");

      dstr = filter(filter_objects(ship,"short"),lambda( ({'x}),
            ({ #'==, ({#'environment, 'x}), environment() }) ) );

      if (sizeof(dstr))
      {
        ship = dstr[0];
      }
      else
      {
        ship = ship[0];
      }
      write(sprintf("Du waehlst %s.\n", ship->name(WEN,1)));
    }
    else if (sizeof(ship) < 1)
    {
      notify_fail("So ein Transportmittel verkehrt hier nicht.\n");
      return 0;
    }
    else
    {
      ship = ship[0];
      write(sprintf("Du reist nun mit %s.\n",ship->name(WEM,1)));
    }
    if (environment(ship)==environment() && ship->short())
    {
      ship->Enter(this_object());
    }
    if (pointerp(t) && stringp(t[2]) && stringp(t[3]) &&
        member(ship->QueryHarbours(),t[2]) != -1)
    {
      write("Du behaeltst Dein bisheriges Reiseziel ("+t[3]+") bei.\n");
      SetProp(P_TRAVEL_INFO, ({ environment(), ship, t[2], t[3] }) );
    }
    else
    {
      SetProp(P_TRAVEL_INFO, ({ environment(), ship, 0, 0 }) );
    }
    return 1;
  }
  return 0;
}

