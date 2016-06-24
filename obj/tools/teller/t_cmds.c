// ----------------------------------------------------------------------
// Builtinkommandos der Teller.
// ----------------------------------------------------------------------
#include "teller.h"
inherit T_BASE;

#include <moving.h>
#include <attributes.h>
#include <terminal.h>
#include <wizlevels.h>

static void do_rinv( object env, int depth );
static scan_obj( object player, object obj );
static void mk_waitfor( mixed waitfor );
static void mk_autoload( mixed autoload );
static int do_roomupdate( int destflag, int noitems );
static int do_cleanof( int strong );

// from teller.c
static int do_cmd( string str );

static int cmd_clear()
{
	stack = ({});
	return TRUE;
}

static int cmd_pop()
{
	if( !sizeof(stack) )
		return error( "pop: Der Stack ist leer" );
	pop();
	return TRUE;
}

static int cmd_top()
{
	if( !sizeof(stack) )
		return error( "top: Der Stack ist leer" );
	write( "TOS= " );
	dump_obj( top(), 5 );
	return TRUE;
}

static int cmd_swap()
{
	mixed tmp;

	if( sizeof(stack)<2 )
		return error( "swap: Keine 2 Elemente auf dem Stack" );
	tmp = stack[0];
	stack[0] = stack[1];
	stack[1] = tmp;
	return TRUE;
}

static int cmd_dup()
{
	if( !sizeof(stack) )
		return error( "dup: Der Stack ist leer" );
	push(top());
	return TRUE;
}

static int cmd_here()
{
	push(environment(PL));
	return TRUE;
}

static int cmd_stack()
{
  int i;
  if( !sizeof(stack) )
    return memo( "Der Stack ist leer" );

  for( i=0; i<sizeof(stack); i++ )
  {
    printf( "%2d: ", i );
    dump_obj( stack[i], 4 );
  }
  return TRUE;
}

static int cmd_inv()
{
  int i;
  object ob;

	if( !becomes_obj() )
		return error( "inv: TOS ist kein Objekt" );
  write( "Inventar von " );
  dump_obj( top(), 13 );
  for( i=0, ob=first_inventory(top()); ob; ob=next_inventory(ob), i++ )
  {
    printf( "%2d. ", i );
    dump_obj( ob, 4 );
  }
  return TRUE;
}

static int cmd_rekinv()
{
	if( !becomes_obj() )
		return error( "rekinv: TOS ist kein Objekt" );
	write( "Inventar von " );
	dump_obj( top(), 13 );

	do_rinv( top(), 2 );
	return TRUE;
}

static void do_rinv( object env, int depth )
{
	int i;
	object ob;

	for( i=0, ob=first_inventory(env); ob; ob=next_inventory(ob), i++ )
	{
		printf( "%*d. ", depth, i );
		dump_obj( ob, 2+depth );
		do_rinv( ob, depth+2 );
	}
}

static int cmd_me()
{
	push( PL );
	return TRUE;
}

// Uebernommen aus dem Teddy (mit freundlicher Genehmigung von Sir).
static int cmd_scan()
{
	object obj;

	if( !becomes_pl() && ( !objectp(top()) || !living(top()) ) )
	{
		if( stringp(top()) && file_size( "/save/"+top()[0..0]+"/"+top()+".o") > 0 )
		{
			obj = clone_object( T_PLAYER );
			obj->Load( top() );
			obj->SetProp( P_NAME, capitalize( pop() ) );
			scan_obj( TRUE, obj );
			destruct( obj );
			return TRUE;
		}
		return error( "scan: TOS ist kein Lebewesen" );
	}

	scan_obj( query_once_interactive( top() ), pop() );
	return TRUE;
}

static int WizLevel( object obj )
{
	if( obj->Notlogged() )
		return query_wiz_level( lower_case(obj->playername()) );
	else
		return query_wiz_level( obj );
}

static string IpName( object obj )
{
	string ip_name, ip_num, nm;

	if( obj->Notlogged() )
	{
		// Aus Umstellungsgruenden werden CALLED_FROM_IP und IP_NAME
		// abgefragt. IP_NAME ist neuer.

		nm = lower_case(obj->playername());
		ip_name = obj->QueryProp(P_CALLED_FROM_IP);
		if( !ip_name ) ip_name = obj->Query(P_IP_NAME);
		return ip_name + " ("
			+ dtime(get_dir("/save/"+nm[0..0]+"/"+nm+".o",4)[0]) +")";
	}
	else
	{
		nm = lower_case( obj->name(RAW) );
		ip_name = query_ip_name( obj );
		if( ip_name == "" || !ip_name )
		{
			ip_name = obj->QueryProp(P_CALLED_FROM_IP);
			if( !ip_name ) ip_name = obj->Query(P_IP_NAME);
			return ip_name + " ("
				+ dtime(get_dir("/save/"+nm[0..0]+"/"+nm+".o",4)[0]) +")";
		}
		return ip_name + " [" + query_ip_number(obj) + "]";
	}
}

string IdleTime( object obj )
{
	if( obj->Notlogged() ) return "-nicht da-";
	if( query_ip_number(obj) ) return ""+query_idle(obj);
	return "-netztot-";
}

static scan_obj( object player, object obj )
{
	string title, level, gender, room, testpl,
		weapon, armour, quest, stat_str, *arr;
	int i,ac;
	object weaponobj, *list, *gegner;
	mixed *hands, *quests, *stats;

	// 1.Zeile : Name Titel - Rasse - [ Wizlevel ]
	title = obj->QueryProp(P_TITLE);

  if( !player )
    level = "Monster" ;
  else if( WizLevel( obj ) < WIZARD_LVL )
	{
		if( testpl=obj->QueryProp( P_TESTPLAYER ) )
		{
			if( stringp(testpl) )
				level = "("+testpl+")";
			else
				level = "Testspieler";
		}
		else if( WizLevel( obj ) >= SEER_LVL )
			level = "Seher";
		else
			level = "Spieler" ;
	}
  else if( WizLevel( obj ) >= GOD_LVL )
    level = "MudGott" ;
  else if( WizLevel( obj ) >= ARCH_LVL )
    level = "Erzmagier" ;
  else if( WizLevel( obj ) >= LORD_LVL )
    level = "Regionsmagier" ;
  else
    level = "Magier" ;

  if( !obj->short() )
    level += ", unsichtbar" ;
  if( obj -> QueryProp( P_FROG ) )
    level += ", Frosch" ;
  if( obj->QueryProp( P_GHOST ) )
  	level += ", tot";
	if( obj->Notlogged() )
		level += ", ausgeloggt";
	if(obj->QueryProp(P_SECOND) )
		level +=", Zweitie";

	if( environment(obj) )
		room = object_name(environment( obj ));
	else
		room = "-nirgends-";

  printf( "%s %s %s[ %s ].\nBefindet sich in %s.\n",
		obj->name(RAW), title? title : "",
		stringp(obj->QueryProp(P_RACE)) ? "- "+obj->QueryProp(P_RACE)+" - " : "",
		level, room ) ;

	// 1 abc Zeile : Host,Email,Snooper
	if( player )
	{
		printf( "Host.......: %s\n", IpName(obj) );
		printf( "E-Mail.....: %s.\n", obj->QueryProp(P_MAILADDR) );
		if( !obj->Notlogged() && query_snoop(obj) )
			printf( "Snooper....: %s.\n", capitalize(getuid(query_snoop(obj))) );

		printf( "Vorsicht...: %11d Kurzmodus.: %11s Magierblick....: %11s.\n",
			obj->QueryProp(P_WIMPY), obj->QueryProp(P_BRIEF) ? "-an-" : "-aus-",
			obj->QueryProp(P_WANTS_TO_LEARN) ? "-an-" : "-aus-" );
		printf( "Idlezeit...: %11s Alter.....: %11s Verheiratet mit: %-11s.\n",
			IdleTime(obj), time2string("%5d:%02h:%02m",obj->QueryProp(P_AGE)*2),
			(stringp(obj->QueryProp(P_MARRIED)) ? obj->QueryProp(P_MARRIED) : "-" )
		);
	}

  // 2.Zeile : HP, SP und XP
  printf( "Lebenspkt..: [%4d/%4d] Magiepkt..: [%4d/%4d].\n" +
    "Questpunkte: [%4d/%4d] Erfahrung.: %11d.\n",
    obj->QueryProp(P_HP), obj->QueryProp(P_MAX_HP),
		obj->QueryProp(P_SP), obj->QueryProp(P_MAX_SP),
		obj->QueryProp(P_QP), "/secure/questmaster"->QueryMaxQP(),
		obj->QueryProp(P_XP) );

  // 3.Zeile : FOOD, DRINK, ALCOHOL
  printf( "Nahrung....: [%4d/%4d] Fluessigk.: [%4d/%4d] " +
    "Alkohol........: [%4d/%4d].\n",
    obj->QueryProp(P_FOOD), obj->QueryProp(P_MAX_FOOD),
    obj->QueryProp(P_DRINK), obj->QueryProp(P_MAX_DRINK),
    obj->QueryProp(P_ALCOHOL), obj->QueryProp(P_MAX_ALCOHOL) ) ;

  // 4.Zeile : Geschlecht, Alignment, Level
  switch( obj->QueryProp(P_GENDER) )
  {
    case FEMALE : gender = "weiblich   " ; break ;
    case MALE   : gender = "maennlich  " ; break ;
    default     : gender = "neutrum    " ; break ;
  }
  printf(
		"Geschlecht.: %s Charakter.: %11d (Magier)Stufe..: [%4s/%4d].\n", 
    gender, obj->QueryProp(P_ALIGN),
		player ? WizLevel(obj)+"" : "-", obj->QueryProp(P_LEVEL) );

	// 5.Zeile : Geld, Gewicht, Playerkills
	printf( "Geld.......: %11d Traegt....: %11d Playerkills....: %11d.\n",
		obj->QueryMoney(), obj->query_weight_contents(),
		obj->QueryProp(P_KILLS) );

  // 6.Zeile : stati
  stats = obj->QueryProp(P_ATTRIBUTES) ;
	arr = m_indices( stats );
  stat_str = "" ;
  for( i = 0; i < sizeof( arr ); i++ ) {
    stat_str += capitalize(arr[ i ]) + "[" + stats[arr[ i ]];
    if( ac = obj->QueryAttributeOffset(arr[i]) ) {
    	stat_str += "+" + ac;
    }
    stat_str += "], ";
  }

  if( stat_str == "" )
    stat_str = "Keine" ;
  else
    stat_str = stat_str[0..sizeof( stat_str ) - 3] ;
  printf( "Attribute..: %s.\n", stat_str ) ;

  // 7.Zeile : Waffe( Dateiname )[ AC ]
  // 8.Zeile : Ruestung(en)[ WC ]
	weaponobj=obj->QueryProp(P_WEAPON);
	if( weaponobj )
    weapon = weaponobj->name(RAW) + " (" + 
      object_name( weaponobj ) + ") [" +
      weaponobj->QueryProp(P_WC) + "]" ;
	else
	{
		hands = obj->QueryProp(P_HANDS);
		weapon = sprintf( "kaempft%s [%d]", hands[0], hands[1] );
	}
	ac = 0;
	list = obj->QueryProp(P_ARMOURS);
	armour = "";
	for( i = 0; i < sizeof( list ); i++ )
	{
     armour += ( list[i]->name(RAW) + "[" +
         list[i]->QueryProp(P_AC) + "]" + ", ") ;
		 ac += list[i]->QueryProp(P_AC);
	}

  if( armour == "" )
    armour = "Keine  " ;

	arr = old_explode( break_string( armour[0..<3]+sprintf(" =>[%d]",
		ac+obj->QueryProp(P_BODY) ), 65 ), "\n" ) ;
	armour = arr[ 0 ] ;
	for( i = 1; i < sizeof( arr ); i++ )
		armour += "\n             " + arr[ i ] ;
  printf( "Waffe......: %s.\nRuestung...: %s.\n", weapon, armour ) ;

	gegner = obj->QueryEnemies();
	if( pointerp(gegner) )
	{
		gegner = gegner[0];
		for( i=0; i<sizeof(gegner); i++ )
		{
			if( i==0 ) printf( "Gegner.....: "); else printf( "             " );
			if( !objectp(gegner[i]) )
				printf( "<%O>\n", gegner[i] );
			else
				printf( "%s (%s)\n", gegner[i]->name(WER,0), object_name(gegner[i]) );
		}
	}

	mk_waitfor( obj->QueryProp(P_WAITFOR) );

	mk_autoload( obj->QueryProp(P_AUTOLOAD) );

	return TRUE;
}

static void mk_waitfor( mixed waitfor )
{
	string str;
	int i;

	if( !pointerp(waitfor) || sizeof(waitfor)==0 )
		return;
	str = "Waiting for: ";
	for( i=sizeof(waitfor)-1; i>0; i-- )
		str += waitfor[i] + ", ";
	str += waitfor[0];
	write( str+"\n" );
}
	
static void mk_autoload( mixed autoload )
{
	string str, *objlist;
	int i;

	if( !mappingp(autoload) )
		return;
	str = "Autoload...: ";
	objlist = m_indices(autoload);
	for( i=sizeof(objlist)-1; i>=0; i-- )
	{
		str += "\"" + objlist[i] + "\"\n";
		if( i>0 )
			str += "             ";
	}
	write( str );
}

static void print_memory_line( string key, object data, int flag )
{
  printf( " %-10s%s ", key, (flag ? ">" : "=") );
  dump_obj( data, 13 );
}
	
static int cmd_memory()
{
  int i;
  if( !sizeof(memory) )
    return memo( "Keine Variablen definiert" );

  walk_mapping( memory, #'print_memory_line );
  return TRUE;
}

static int cmd_array()
{
  mixed *array;
  mixed ob;

	if( !sizeof(stack) )
		return error( "array: Der Stack ist leer" );
  array = ({});
  while( sizeof(stack) && (ob=pop()) && ob!=";" )
    array = ({ob}) + array;
  push( array );
	return TRUE;
}

static int cmd_split()
{
  mixed *array;
  int i;

	if( !pointerp(top()) )
		return error( "split: TOS ist kein Array" );
  array=pop();
  if( sizeof(stack) )
    push( ";" );
  for( i=0; i<sizeof(array); i++ )
    push(array[i]);
	return TRUE;
}

static int cmd_player()
{
	object ob;
	string str;

	str = top();
	if( !stringp(str) )
		return error( "player: TOS ist kein String" );
	ob = becomes_pl();
	if( !ob )
		return error( "player: Keinen Spieler namens \""+str+"\" gefunden" );
	//pop();
	//push(ob);
	return TRUE;
}

static int cmd_object()
{
	object ob;
	string err,fnam;

	if( !stringp(top()) )
		return error( "object: TOS ist kein String" );
	ob = find_object(top());
	if( !ob )
	{
		if( !(fnam=this_player()->find_file(top(),".c")) )
			return error( "object: Kein Objekt namens \""+top()+"\" gefunden" );
		if( err=(catch(call_other(fnam,"?"))) )
			return error( "object: Fehler beim Laden: "+err[1..<3] );
		ob = find_object(fnam);
	}
	pop();
	push(ob);
	return TRUE;
}

static int cmd_living()
{
	object ob;
	if( !stringp(top()) )
		return error( "object: TOS ist kein String" );
	ob = find_living(top());
	if( !ob )
		return error( "object: Kein Objekt namens \""+top()+"\" gefunden" );
	pop();
	push(ob);
	return TRUE;
}

static int cmd_say()
{
	mit_say = !mit_say;
	if( mit_say )
		memo( "Meldungen an Mitspieler an" );
	else
		memo( "Meldungen an Mitspieler aus" );
	return TRUE;
}

static int cmd_names()
{
	mit_namen = !mit_namen;
	if( mit_namen )
		memo( "Namen werden angezeigt" );
	else
		memo( "Namen werden nicht angezeigt" );
	return TRUE;
}

static int cmd_secureinv()
{
	secureinv = !secureinv;
	if( secureinv )
		memo( "Inventory wird ueberwacht" );
	else
		memo( "Inventory wird nicht ueberwacht" );
	set_heart_beat(secureinv);
	return TRUE;
}

static int cmd_logaccess()
{
	dologaccess = !dologaccess;
	if( dologaccess )
		memo( "Zugriffe werden gemeldet" );
	else
		memo( "Zugriffe werden nicht gemeldet" );
	return TRUE;
}

static int cmd_destruct_bang()
{
	if( !becomes_obj() )
		return error( "destruct: TOS ist kein Objekt" );
	destruct(pop());
	return TRUE;
}

static int cmd_destruct()
{
	if( !becomes_obj() )
		return error( "remove: TOS ist kein Objekt" );
	memo( "destruct: TOS wird 'removed'!" );
	top()->remove();
	if( top() )
		memo( "destruct: TOS lebt noch." );
	else
		pop();
	return TRUE;
}

static int cmd_remove()
{
	if( !becomes_obj() )
		return error( "remove: TOS ist kein Objekt" );
	top()->remove();
	if( top() )
		memo( "destruct: TOS lebt noch." );
	else
		pop();
	return TRUE;
}

static int cmd_update()
{
	object blue;

	if( !becomes_obj() )
		return error( "update: TOS ist kein Objekt" );
	blue = find_object(old_explode(object_name(top()),"#")[0]);
	blue->remove();
	if( blue )
		memo( "update: TOS lebt noch" );
	else
		pop();
	return TRUE;
}

static int cmd_update_bang()
{
	if( !becomes_obj() )
		return error( "update: TOS ist kein Objekt" );
	destruct(find_object(old_explode(object_name(pop()),"#")[0]));
	return TRUE;
}

static int cmd_roomupdate()
{
	return do_roomupdate( FALSE, FALSE );
}

static int cmd_roomupdate_bang()
{
	return do_roomupdate( TRUE, FALSE );
}

static int cmd_extroomupdate()
{
	return do_roomupdate( FALSE, TRUE );
}

static int cmd_extroomupdate_bang()
{
	return do_roomupdate( TRUE, TRUE );
}

// Hilfsfunktionen zum Filtern von Items
static object *collect_items;
static void collect( object* data ) { collect_items += ({ data[0] }); }

static int do_roomupdate( int destflag, int noitems )
{
	object tmproom,newroom;
	object *inv;
	string errmsg;
	string *file;
	object *items;
	int i;

	if( !becomes_obj() )
		return error( "roomupdate: TOS ist kein Objekt" );
	file = old_explode( object_name( top() ), "#" );
	if( sizeof(file) > 1 )
		return error( "roomupdate: TOS ist keine Blueprint" );
	if( file[0] == "/room/void" )
		return error( "roomupdate: Die `void' darf nicht geupdatet werden" );

	// ----- Rettung	
	tell_room( top(),
		"Der Raum verschwimmt vor Deinen Augen, um sich zu erneuern.\n"
	);
	tmproom = clone_object( "/room/void" );

	if( noitems )
	  // Nur Spieler kommen raus.
		inv = filter( all_inventory(top()), #'query_once_interactive );
	else
	{ // Dinge, die P_ITEMS sind, bleiben da!
		collect_items = ({});
		map( top()->QueryProp(P_ITEMS), #'collect ); 
		inv = all_inventory(top()) - collect_items;
	}

	for( i=sizeof(inv)-1; i>=0; i-- )
		inv[i]->move( tmproom, M_NOCHECK | M_SILENT | M_NO_SHOW );

	// ----- Vernichtung
	if( destflag )
		destruct( pop() );
	else
	{
		top()->remove();
		if( top() )
			memo( "roomupdate : TOS ist nicht verschwunden." );
		else
			pop();
	}

	// ----- Neuerschaffung
	errmsg = catch( call_other( file[0], "?" ) );
	if( errmsg )
	{
		tell_room( tmproom, "Der Raum verbleicht in ein Nichts.\n" );
		push( file[0] );
		return error( "updateroom: " + errmsg[1..<2] );
	}

	// ----- Restaurierung
	newroom = find_object( file[0] );
	for( i=sizeof(inv)-1; i>=0; i-- )
		if( objectp(inv[i]) ) // Objekte koennten sich beim ersten move zerstoeren.
			inv[i]->move( newroom, M_NOCHECK | M_SILENT | M_NO_SHOW );
	tell_room( newroom, "Die Konturen werden wieder scharf.\n" );
	destruct( tmproom );
	return TRUE;
}

static int cmd_clone()
{
	if( !stringp(top()) )
		return error( "clone: TOS ist kein String" );
	if( file_size(top()+".c")<=0 )
		return error( "clone: Kein solches File" );
	push(clone_object(pop()));
	//do_move( top(), environment(PL) );
	//top()->move(PL,M_GET|M_SILENT);
	return TRUE;
}

static int cmd_move()
{
	object ob;

	if( !becomes_obj() )
		return error( "move: Ziel ist kein Objekt" );
	ob = pop();
	if( !becomes_obj() )
		return error( "move: Kein solcher Gegenstand" );
	do_move( pop(), ob );
	return TRUE;
}

static int cmd_cleanof_bang()
{
	return do_cleanof( TRUE );
}

static int cmd_cleanof()
{
	return do_cleanof( FALSE );
}

static int do_cleanof( int strong )
{
	object *inv;
	int i;
	string clean_id;

	if( !stringp(top()) )
		return error( "cleanof: TOS ist kein String" );
	clean_id = pop();
	if( !becomes_obj() )
	{
		push( clean_id );
		return error( "cleanof: Kein Objekt zum Leeren" );
	}
	for( i=0, inv=all_inventory(pop()); i<sizeof(inv); i++ )
		if( inv[i]->id(clean_id) )
		{
			if( strong )
				destruct( inv[i] );
			else
				inv[i]->remove();
		}
	return TRUE;
}

static int cmd_snoopers()
{
	object* u, snooper;
	int i, flag;

	flag = 0;
	u = users();
	for( i=0; i<sizeof(u); i++ )
	{
		if( snooper = query_snoop(u[i]) )
		{
			flag = 1;
			printf( "%s wird gesnooped von: %s.\n",
				capitalize(getuid(u[i])), capitalize(getuid(snooper)) );
		}
	}
	if( !flag )
		memo( "Momentan wird niemand gesnooped" );
	return TRUE;
}

static int cmd_ping()
{
	object pl;

	if( !becomes_pl() )
		return error( "ping: TOS ist kein Spieler" );

	pl=pop();
	call_out( "ping", 0, ({ pl, 5 }) );
	return TRUE;
}

static void ping( mixed* argv )
{
	if( !argv[0] || --argv[1] < 0 ) return;
	tell_object( argv[0], BEEP+PL->name(WER)+" pingt Dich an.\n" );
	call_out( "ping", 1, argv );
}

static void do_calloutinfo( mixed* call )
{
	int l,i;

	if( pointerp(call) )
	{
		printf( "%5d:%O->%O(", call[2], call[0], call[1]);
		if( (l=sizeof(call))>3 ) {
			for( ; l>=3 && !call[--l]; ) ;
			for( i=3; i<=l; i++ ) printf( "%O%s", call[i], (i==l)?"":"," );
		}
		write(")\n");
	}
}

static int cmd_callouts_bang()
{
	mixed *calls;
	object obj;
	string name;
	int i,j;

	calls = call_out_info();
	if( !pointerp(calls) || !sizeof(calls) )
	{
		memo( "Keine Callouts vorhanden" );
		return TRUE;
	}
	map( calls, #'do_calloutinfo );
	return TRUE;
}

static void do_calloutinfo2( mixed* call, string str )
{
	string s;
	int i,l;

	if( pointerp(call) )
	{
		s = sprintf( "%5d:%O->%O(", call[2], call[0], call[1]);
		if( sizeof(explode(s,str)) > 1 )
		{
			write( s );
			if( (l=sizeof(call))>3 ) {
				for( ; l>=3 && !call[--l]; ) ;
				for( i=3; i<=l; i++ ) printf( "%O%s", call[i], (i==l)?"":"," );
			}
			write(")\n");
		}
	}
}

static int cmd_callouts()
{
	mixed *calls;
	object obj;
	string str;
	int i,j;

	if( !stringp(top()) )
		return error( "TOS ist kein String" );
	str = pop();
	calls = call_out_info();
	if( !pointerp(calls) || !sizeof(calls) )
	{
		memo( "Keine Callouts vorhanden" );
		return TRUE;
	}
	map( calls, #'do_calloutinfo2, str );
	return TRUE;
}

static int cmd_heartbeats()
{
	mixed *beats;
	int i;
	object env;
	string enam;

	beats = heart_beat_info();
	if( !pointerp(beats) || !sizeof(beats) )
	{
		memo( "Keine Heartbeats vorhanden" );
		return TRUE;
	}
	for( i=0; i<sizeof(beats); i++ )
	{
		env = environment(beats[i]);
		enam = env ? object_name(env) : "-- nirgends --";
		printf( "%-35s %-35s\n", object_name(beats[i]), enam );
	}
	return TRUE;
}

static int cmd_wer()
{
	object* ppl;
	string* pl;
	int i;

	ppl = sort_array( users(), lambda( ({ 'x, 'y }),
		({ #'<, ({ #'query_ip_number, 'x }), ({ #'query_ip_number, 'y }) })
	));
	pl = ({});
	for( i=0; i<sizeof(ppl); i++ )
	{
		pl += ({ sprintf( "%'.'-14s %-15s %3d %s \n",
				capitalize(geteuid(ppl[i])),
				query_ip_number(ppl[i]),
				query_wiz_level(ppl[i])>0 ? query_wiz_level(ppl[i])
					: ppl[i]->QueryProp(P_LEVEL),
				query_wiz_level(ppl[i])>0 ? "W" : "P"
		) });
	}
	write( implode(pl,"") );
	return TRUE;
}

static int cmd_debuginfo()
{
	if( !becomes_obj() )
		return error( "dinfo: TOS ist kein Objekt" );
	debug_info( 0, pop() );
	return TRUE;
}

static int cmd_pretty()
{
	pretty = !pretty;
	if( pretty )
		memo( "Schoenmodus an" );
	else
		memo( "Schoenmodus aus" );
	return TRUE;
}

static int cmd_doprofile()
{
	do_profile=!do_profile;
	if( do_profile )
		memo( "Profile wird geladen" );
	else
		memo( "Profile wird nicht geladen" );
	return TRUE;
}

static int cmd_evaluate()
{
	string str;
	if( !sizeof(stack) ) return error( "evaluate: Stack ist leer" );
	if( !stringp(top()) ) return error( "evaluate: TOS ist kein String" );
	str = pop();
	return do_cmd( str );
}

static void write_memory( string nam, string str, int flag, string file )
{
	if( flag ) write_file( file, nam + " = " + str + "\n" );
}

static int cmd_dump()
{
	string file;

	if( !sizeof(stack) || !stringp(top()) )
		file = "/players/"+getuid(PL)+"/.memory.o";
	else
		file = pop();
	rm( file );
	write_file( file, "# Dump des Tellerstapels vom " + dtime(time()) + "\n" );
	write_file( file, "# Owner = "+capitalize(getuid(PL))+"\n" );
	walk_mapping( memory, #'write_memory, file );
	return TRUE;
}

static int restore_line( string line )
{
	string nam,str;
	if( sscanf( line, "%s = %s", nam, str ) != 2 )
		return error( "restore: Fehler im file" );
	memory += ([ nam: str; 1 ]);
  return 1;
}

static int cmd_restore()
{
	string str, *lines;

	if( !sizeof(stack) || !stringp(top()) )
		str = "/players/"+getuid(PL)+"/.memory.o";
	else
		str = pop();

	if(file_size(str)<=0)
		return error( "restore: kann '"+str+"' nicht laden" );
	lines = regexp( old_explode( read_file(str), "\n" ), "^[^#]" );
	map( lines, #'restore_line );
	return TRUE;
}

static int cmd_if()
{
	if( sizeof(stack) < 3 )
		return error( "if: zuwenig Argumente" );
	if( !pop() )
		cmd_swap();
	pop();
	return TRUE;
}
