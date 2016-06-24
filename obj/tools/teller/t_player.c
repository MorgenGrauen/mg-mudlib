// ----------------------------------------------------------------------
// Zwischenobjekt zum Lesen der Spielerdaten auch von nichteingeloggten.
// ----------------------------------------------------------------------
#include "teller.h"

inherit "std/player/base";

create() {
	used_attributes_offsets = ([]);
}

string name;
string race;
string hands;
mapping autoload;

_query_name() { return name; }
playername() { return name; }
_set_name( str ) { return name=str; }

_query_race()
{
	if( race ) return race;
		
	return "/secure/master"
		->query_player_object(lower_case(name))
		->QueryProp(P_RACE);
}

_query_hands()
{
	if( hands ) return hands;
	return ({ " nicht", 0 });
}

_query_autoload()
{
	return autoload;
}

Load( str )
{
	restore_object( "/save/"+str[0..0]+"/"+str );
}

Notlogged() { return TRUE; }

QueryMoney()
{
	if( !mappingp(autoload) )
		return 0;
	return autoload["/items/money"];
}

QueryEnemies()
{
        return ({({}),({})});
}

QueryAttributeOffset( arr ) { return "?"; }
