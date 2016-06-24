// Autor: Rumata@gmx.de
// ... sozusagen meine private rasse ...
//
// MorgenGrauen MUDlib
//
// shells/magier.c -- magier shell
//
// $Id: baum.c 8675 2014-02-18 20:39:54Z Zesstra $

#pragma strong_types,save_types

inherit "/std/shells/magier";

//#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <properties.h>
#include <wizlevels.h>
#include <language.h>
#include <moving.h>
#include <attributes.h>
#include <combat.h>
#include <defines.h>
#include <ansi.h>
#include <udp.h>
#include <new_skills.h>

static int _wurzel;

void create()
{
  if (!clonep() || object_name(this_object()) == __FILE__[0..<3]) {
      set_next_reset(-1);    
      return;
  }

  ::create();
	_wurzel = -1;
}

static int wurzel_an( string arg ) {
	string num;
	int count;

	if( this_object() != this_interactive() ) return 0;
	notify_fail( "WAS willst Du schlagen?\n" );
	if( !arg || sscanf( arg, "wurzel%s", num ) == 0 ) return 0;

	if( sscanf( num, " %d", count ) == 0 ) {
		notify_fail( "schage wurzel <zahl>\n" );
		return 0;
	}

	_wurzel = count;
	say( Name(WER) + " schlaegt hier Wurzeln.\n" );
	write( "Du schlaegst nun Wurzeln.\n" );
	return 1;
}

static int wurzel_aus( string arg ) {
	
	if( this_object() != this_interactive() ) return 0;
	notify_fail( "WAS willst du loesen?\n" );
	if( !arg ) return 0;
	if( member( ({"wurzel","wurzeln"}),	arg ) == -1 ) return 0;
	
	_wurzel = -1;
	say( Name(WER) + " loest " + QueryPronoun(FEMALE,WEN,PLURAL)
			 + " Wurzeln aus dem Boden.\n" );
	write( "Du loest Deine Wurzeln aus dem Boden.\n" );
	return 1;
}

int wurzel_info( string arg ) {
	if( this_object() != this_interactive() ) return 0;
	switch( _wurzel ) {
	case -1:
		write( "Deine Wurzeln sind lose.\n" );
		break;
	case 0:
		write( "Deine Wurzeln sitzen fest.\n" );
		break;
	case 1:
		write( "Noch eine Bewegung und Du sitzt fest.\n" );
		break;
	default:
		printf( "Noch %d Bewegungen, und Du sitzt fest.\n", _wurzel );
	}
	return 1;
}

static mixed _query_localcmds() {
	return ::_query_localcmds() + ({
		({ "schlag"    , "wurzel_an"  ,1,ARCH_LVL }),
		({ "loes"      , "wurzel_aus" ,1,ARCH_LVL }),
		({ "wurzelinfo", "wurzel_info",0,ARCH_LVL })
	});
}

varargs int move( mixed dest, int method, string dir,
									string textout, string textin )
{
	if( _wurzel == 0 ) {
		return ME_TOO_HEAVY;
	}
	if( _wurzel > 0 ) { _wurzel--; }
	return ::move( dest, method, dir, textout, textin );
}

static int new_quit() {
	_wurzel = -1;
	return ::new_quit();
}
