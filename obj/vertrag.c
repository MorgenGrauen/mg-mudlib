#pragma strict_types
#pragma combine_strings
#pragma no_shadow
#pragma no_inherit

inherit "std/thing";

#include <properties.h>
#include <language.h>
#include <wizlevels.h>

private int gelesen;
private string unterschrieben;

static string text();

public void create()
{
   if ( !geteuid(this_object()) )
      call_out( "remove", 0 );
   
   if ( !IS_WIZARD(geteuid(this_object())) )
      call_out( "remove", 0 );
   
   if ( !clonep(this_object()) )
      return;
   
   thing::create();
   SetProp( P_SHORT, "Ein Vertrag" );
   SetProp( P_LONG, "@@unterschrieben@@" );
   SetProp( P_NAME, "Vertrag" );
   Set(P_READ_MSG, #'text, F_QUERY_METHOD); 
   SetProp( P_GENDER, MALE );
   SetProp( P_ARTICLE, 1 );
   AddId("vertrag");
   SetProp(P_NEVERDROP, 1);
   AddCmd(({"unterschreibe","unterschreib","unterzeichne"}),"unterschreibe");
   AddCmd(({"zerreisse", "zerreiss"}),"zerreisse");
   
   gelesen = 0;
   unterschrieben = "";
}

static string _query_nodrop()
{
   if (this_player() && IS_WIZARD(this_player())) return 0;
   return "Nein Nein! Dies ist dein Vertrag!\n";
}

static string unterschrieben()
{
   string s;
   
   s = "Dies ist Dein Magier-Vertrag. Du solltest ihn vielleicht ";
   if ( gelesen )
      s += "noch";
   s += "mal lesen.\nEr wurde Dir von " + capitalize(geteuid(this_object()))
      +" ueberreicht.\nWenn Du ihn unterschreibst, wirst Du von "
      + capitalize(geteuid(this_object())) + " gesponsort.\n";
   
   if (this_player() && (unterschrieben == getuid(this_player())) )
      s += "Du hast ihn unterschrieben.\n";
   
   return s;
}


static string text()
{
   gelesen = 1;
   this_player()->More("/etc/WIZRULES",1);
   return "";
}


static int unterschreibe( string str )
{
   notify_fail("Was willst Du denn unterschreiben?\n");
   if ( !id(str) )
      return 0;
   
   if ( !this_player() || this_player() != this_interactive()
        || this_player() != environment() )
      return 0;
   
   if ( !gelesen ){
      write( "Du hast ihn noch nicht gelesen. ERST lesen, DANN schreiben!\n");
      return 1;
   }
   
   write( "Du unterschreibst den Vertrag. Wenn das ein Versehen war, "
          + "zerreiss ihn sofort.\n" );
   unterschrieben = getuid( this_interactive() );
   SetProp( P_NODROP, "Nein, der ist unterschrieben. Du kannst ihn hoechstens"
            + " zerreissen.\n" );
   SetProp( P_NEVERDROP, 1 );
   return 1;
}


static int zerreisse( string str )
{
   notify_fail("Was willst Du denn zerreissen?\n");
   if ( !id(str) ) return 0;
   write( "Du zerreisst den Vertrag.\n" );
   return remove();
}


public int is_unterschrieben()
{
   return environment() && query_once_interactive(environment())
      && getuid(environment()) == unterschrieben;
}

