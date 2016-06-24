// ----------------------------------------------------------------------
// Tellerstapel, ein Stackorientiertes magiertool.
// Rumata 15.09.93
// ----------------------------------------------------------------------
#include "teller.h"
inherit "std/secure_thing";
inherit T_CMDS;
inherit T_EFUN;
inherit "std/more";

#include "/secure/wizlevels.h"

nosave mapping tcommands;
nosave string owner_name;

void logaccess( string s );
static void do_msg( string s, object o );
static string|int parseNext( string str, mixed auxstack, string funname );
static mixed nextAlpha( string s );
static varargs void restore_stack( string* argv, int args );
static int do_call( string fun, int mit_return );
static int do_call_efun( string fun, int mit_return );
static int do_lpc( string str, int mit_return );
static int do_cmd( string str );
int error( string s );

static void tAddCmd( mixed cmd )
{
	int i;

	//if (find_player("rikus")) tell_object(find_player("rikus"),sprintf("%O\n",cmd));
	if( !pointerp(cmd) )
    m_add(tcommands, cmd, cmd);
	else
		for( i=0; i<sizeof(cmd); i++ )
      m_add(tcommands, cmd[i], cmd[0]);
}

void create()
{
	if( IS_BLUE(ME) ) return;
	if( !find_player(getuid()) ) return;
	owner_name = find_player(getuid())->name(WESSEN,1);

	secure_thing::create();
	t_cmds::create();
	SetProp( P_NAME, "Tellerstapel" );
	SetProp( P_GENDER, MALE );
	AddId( ({"teller","stapel","tellerstapel"}) );
	SetProp( P_NODROP, "Wozu denn?\n" );
	SetProp( P_NEVERDROP, 1 );
	SetProp( P_READ_MSG, "Die Gravur besagt: 'Aus der Rumata-Manufaktur'\n" );
	tcommands = ([]);
	
	tAddCmd( "say" );
	tAddCmd( ({"names","nam"}) );
	tAddCmd( "secureinv" );
	tAddCmd( "logaccess" );
	tAddCmd( "pretty" );
	tAddCmd( "doprofile" );
	tAddCmd( ({"clear","clr"}));
	tAddCmd( "pop" );
	tAddCmd( "dup" );
	tAddCmd( "swap" );
	tAddCmd( ({"evaluate","eval"}) );

	tAddCmd( "here" );
	tAddCmd( "me" );

	tAddCmd( ({"memory","mem"}) );
	tAddCmd( "dump" );
	tAddCmd( "restore" );
	tAddCmd( "top" );
	tAddCmd( "wer" );
	tAddCmd( ({"stack","stk"}) );
	tAddCmd( ({"snoopers","snoop"}) );
	tAddCmd( "callouts" );
	tAddCmd( ({"callouts_bang","callouts!"}) );
	tAddCmd( ({"heartbeats","beats"}) );
	tAddCmd( "inv" );
	tAddCmd( ({"rekinv","rinv"}) );
	tAddCmd( "scan" );
	tAddCmd( "if" );

	tAddCmd( ({"array","arr"}) );
	tAddCmd( ({"split","spl"}) );
	tAddCmd( ({"player","pl"}) );
	tAddCmd( ({"living","lv"}) );
	tAddCmd( ({"object","ob"}) );

	tAddCmd( ({"debuginfo","dinfo"}) );
	tAddCmd( ({"clone","cln"}) );
	tAddCmd( ({"update","upd"}) );
	tAddCmd( ({"update_bang","update!","upd!"}) );
	tAddCmd( ({"move","mv"}) );
	tAddCmd( ({"destruct","dest","des"}) );
	tAddCmd( ({"remove","rem"}) );
	tAddCmd( ({"destruct_bang","destruct!","dest!","des!","remove!","rem!"}) );
	tAddCmd( ({"roomupdate","rupdate","rupd","rup"}) );
	tAddCmd( ({"roomupdate_bang","roomupdate!","rupdate!","rupd!","rup!"}) );
	tAddCmd( ({"extroomupdate","rupdate*","rupd*","rup*"}) );
	tAddCmd( ({"extroomupdate_bang","roomupdate*!","rupdate*!","rupd*!","rup*!",
		"rup!*" }) );
	tAddCmd( ({"cleanof","clnof"}) );
	tAddCmd( ({"cleanof_bang","cleanof!","clnof!"}) );

	tAddCmd( "ping" );
}

void init()
{
	//logaccess( "init" );
	::init();
	if ( !IS_WIZARD(PL) || environment()!=PL )
		return;
	add_action( "parse", ",", 1 );
	add_action( "jonglier", "jonglier", 1 );
	add_action( "message", "message" );
	add_action( "message", "msg" );
}

short()
{
	logaccess( "short" );
	return "Ein Stapel Teller.\n";
}

id( str )
{
	logaccess( "id" );
	return ::id(str);
}

name( casus, dem, force )
{
	logaccess( "name" );
	return ::name( casus, dem, force );
}

long()
{
	string answer;
	
	answer =
		 "Du betrachtest "+owner_name+" magischen Tellerstapel. Die einzelnen Teller sind\n"
		+"bis auf eine kleine Inschrift auf dem Boden weiss. Wenn Du hiermit\n"
		+"jonglierst, kann ALLES passieren.\n";

	if( getuid(PL) == getuid() )
	{
		answer +=
			( mit_say
				? "Die Teller sind beim Jonglieren sichtbar.\n"
				: "Die Teller sind beim Jonglieren unsichtbar.\n"
			) + ( mit_namen
				? "Die Teller nennen die Dinge beim Namen.\n"
				: "Die Teller nennen die Dinge beim Filenamen.\n"
			) + ( pretty
				? "Die Teller haben ein verschnoerkeltes Muster.\n"
				: "Die Teller sind geradezu schmucklos.\n"
			) + ( dologaccess
				? "Die Teller spueren Zugriffe.\n"
				: ""
			) + ( secureinv
				? "Die Teller bewachen Dein Inventar.\n"
				: ""
			) + ( do_profile
				? "Die Teller werden beim Einloggen aktiv.\n"
				: ""
			);
	} 
	return answer; 	
}

void _set_autoloadobj( mixed val )
{
	if( !pointerp(val) ) return;
	val += ({ 0,0,0,0,0,0 });
	mit_namen = val[0];
	mit_say = val[1];	
	set_heart_beat( secureinv = val[2] );
	dologaccess = val[3];
	pretty = val[4];
	do_profile = val[5];
	if( do_profile ) call_out( "_load_profile", 0, this_player() );
}

mixed _query_autoloadobj()
{
	return ({ mit_namen, mit_say, secureinv, dologaccess, pretty,
		do_profile });
}

void _load_profile( object pl )
{
	string pf,errmsg;

	if( geteuid() && pl && geteuid(pl)==geteuid()
		&& file_size(pf="/players/"+geteuid()+"/.profile.c")>0
		&& (errmsg=catch(call_other(pf,"setup",pl)))
	)
		printf( "Error loading profile: %O\n", errmsg );
}

void logaccess( string str )
{
	if( RPL && dologaccess && getuid()!=getuid(RPL) && find_player(getuid())
		&& previous_object() && getuid(previous_object()) != ROOTID
		&& query_wiz_level(getuid())>=query_wiz_level(getuid(RPL))
	)
		tell_object( find_player(getuid()), "MEMO: " + str + "() von "
			+ RPL->name(WEM) + " via ["
			+ object_name(previous_object())+"].\n"
		);
	if( secureinv && find_player(getuid()) && !member(heart_beat_info(),ME) )
	{
		tell_object( find_player(getuid()), "MEMO: heart_beat restartet!.\n" );
		set_heart_beat(TRUE);
	}
}

// ----------------------------------------------------------------------
// Hilfe-Funktionen
// ----------------------------------------------------------------------
static int secure()
{
	int i;
	if( process_call() || secure_level()<query_wiz_level(RPL) ) return 0;
	for( i=0; previous_object(i)==RPL; i++ )
		;
	return (!previous_object(i) || previous_object()==ME)
		 && getuid()==getuid(RPL) && IS_WIZARD(RPL);
}

int jonglier( string str )
{
	string arg;

	logaccess( "jongliere" );
	if( !str || id(str) || sscanf( str, "mit %s", arg ) == 1 )
	{
		if( !secure() )
			write(
				"Dieser Tellerstapel ist nichts fuer Dich. Suche Dir einen eigenen.\n"
			);
		else
			write(
			  "Der Jonglierbefehl lautet \";\" oder \",\".\n"
			+	"\";hilfe <befehl>\" gibt Dir Hilfestellung.\n"
			);
		return TRUE;
	}
  return 0;
}
			
static int zeige_hilfe( string str )
{
	if( !stringp(str) ) str = "general";
	str = implode( old_explode( HELP(str), "/../" ), "/" );
	if( file_size(str) > 0 )
	{
		More( str, 1 );
		return TRUE;
	}
	write( "Zu diesem Thema gibt es keine Hilfe.\n" );
	return TRUE;
}

int message( string str )
{
	string pl;
	object dest;

	if( !secure() ) return FALSE;

	if( str )
	{
		if( sscanf( str, "to %s", pl ) == 1 )
			str = pl;
		if( !(dest = find_player(str) ) )
		{
			write( capitalize(str) + " ist nicht eingeloggt.\n" );
			return TRUE;
		}
	}
	else
		dest = 0;

	do_msg( "<Mitteilung von "+PL->name(WEM)+">", dest );
	write( "Mitteilung"
		+ ( dest ? " an " + dest->name(WEN) : "" )
		+ " kann mit '**' beendet werden.\n" );

	input_to( "more_message", 0, dest );
	return TRUE;
}

static void do_msg( string str, object obj )
{
	if( obj )
		tell_object( obj, str + "\n" );
	else
		say( str + "\n" );
}

static int more_message( string str, object obj )
{
	if( str != "**" )
	{
		do_msg( str, obj );
		input_to( "more_message", 0, obj );
	}
	else
		do_msg( "<Ende der Mitteilung>", obj );
	return TRUE;
}

// ----------------------------------------------------------------------
// Parse-Funktionen
// ----------------------------------------------------------------------
int parse( string str )
{
	int i;
	string arg, rest;

	if( !secure() ) return FALSE;

	// Die Hilfe faellt aus der allgemeinen Syntax heraus !
	if( query_verb()[1..] == "hilfe" || query_verb()[1..] == "help" )
		return zeige_hilfe( str );

	if( str=this_player()->_unparsed_args() )
		str = query_verb()[1..]+" "+str;
	else
		str = query_verb()[1..];

	fehler_passiert = FALSE;
	while( !fehler_passiert && str && str!="" )
		str = parseNext( str, ({}), "-ROOT-" );

	if( mit_say && !PL->QueryProp(P_INVIS) )
	{
		if( !fehler_passiert )
			say( PL->name(WER)+" jongliert mit "
				+ PL->QPP(MALE,WEM,PLURAL)+" Tellern.\n" );
		else
			say( PL->name(WEM)+" faellt beim Jonglieren ein Teller herunter.\n" );
	}
	if( !fehler_passiert )
		write( "OK.\n" );
	return TRUE;
}

static string|int parseNext( string str, mixed auxstack, string funname )
{
	mixed *res;
	string arg, rest;
	bool mit_return;
	int num;

	while( str!="" && str[0]==' ' )
		str = str[1..];
	if( str=="" )
		return FALSE;

	switch( str[0] )
	{
		case ';': // ---------------------------------------- Kommandobeginn
		case ',':
			push( ";" );
			return str[1..];
		case '\"': // --------------------------------------- Stringkonstante
			if( sscanf( str, "\"%s\"%s", arg, rest )==2 )
			{
				/* test auf numerisch fehlt hier noch */
				push_str( arg );
				return rest;
			}
			if( !sscanf( str, "\"%s\"", arg ) )
				return error( "String ohne Ende" );
			push_str( arg );
			return FALSE;
		case '#': // ---------------------------------------- Zahl
			res = nextAlpha( str[1..] );
			if( sscanf( res[0], "%d", num ) == 1 )
			{
				push( num );
				return res[1];
			}
			return error( "'#': keine erlaubte Zahl" );
		case '^': // ---------------------------------------- Hole env
			if( !becomes_obj() )
				return error( "'^': TOS ist kein Objekt" );
			if( !environment(top()) )
				return error( "'^': TOS hat kein Environment" );
			push(environment(pop()));
			return str[1..];
		case '.': // ---------------------------------------- Hole aus inv
			if( !becomes_obj() )
				return error( "'.': TOS ist kein Objekt" );
			res = nextAlpha( str[1..] );
			calcinv( res[0] );
			return res[1];
		case '<': // ---------------------------------------- Recall
			if( !sscanf( str, "<%s", arg ) || arg=="" )
				return error( "'<': Variablenname fehlt" );
			res = nextAlpha( str[1..] );
			do_recall(res[0]);
			return res[1];
		case '>': // ---------------------------------------- Store
			if( !sscanf( str, ">%s", arg ) || arg=="" )
				return error( "'>': Variablenname fehlt" );
			res = nextAlpha( str[1..] );
			do_store(res[0]);
			return res[1];
		case '-': // ---------------------------------------- Call mit '-'
			str = str[1..];
			if( mit_return = (str[0] == '-') )
				str = str[1..];
			res = nextAlpha( str[1..] );
			switch( str[0] )
			{
				case '>':
					if( do_call( res[0], mit_return ) )
						return res[1];
					break;
				case '*':
					if( do_call_efun( res[0], mit_return ) )
						return res[1];
					break;
				default:
					return error( "'-': '>' oder '*' erwartet" );
			}
			return FALSE;
		case '\'': // --------------------------------------- Call
			str = str[1..];
			if( mit_return = (str[0] == '\'') )
				str = str[1..];
			res = nextAlpha( str );
			if( do_call( res[0], mit_return ) )
				return res[1];
			return FALSE;
		case '`': // --------------------------------------- Call Efun
			str = str[1..];
			if( mit_return = (str[0] == '`') )
				str = str[1..];
			res = nextAlpha( str );
			if( do_call_efun( res[0], mit_return ) )
				return res[1];
			return FALSE;
		case '@': // --------------------------------------- Evaluate
			str = str[1..];
			if( mit_return = (str[0] == '@') )
				str = str[1..];
			if( do_lpc( str, mit_return ) )
				return FALSE;
			return FALSE;
		case '!': // ------------------------------------------- META
			if( sscanf(str,"!%d%s",num,arg)==2 )
			{
				if( num>=sizeof(auxstack) )
					return error( "'"+funname+"': zu weing Argumente" );
				push( auxstack[num] );
			}
			else
			{
				res = nextAlpha( str[1..] );
				memory += ([ res[0]: res[1]; 1 ]);
				return FALSE;
			}
			return arg;
		default: // ----------------------------------------- default
			res = nextAlpha( str );
			do_cmd(res[0]);
			return res[1];
	}
	return memo( "Etwas Seltsames ist passiert" );
}

static int do_cmd( string str )
{
	string fun, filename, *spl;
	mixed* oldstack;
	int i,max,num;

	if( member(memory,str) )
	{
		fun = memory[str,0];
		if( !memory[str,1] ) // normale Variablen
		{
			push( fun );
			return TRUE;
		}
		oldstack = stack;
		stack = ({});
		spl = regexplode( fun, "![0-9][0-9]*" );
		max = -1;
		for( i=1; i<sizeof(spl); i+=2 )
			if( sscanf(spl[i],"!%d",num) && max<num ) max = num;

		while( !fehler_passiert && fun && fun!="" )
			fun = parseNext( fun, oldstack, str );
		if( fehler_passiert )
		{
			stack = oldstack;
			return FALSE;
		}
		stack = stack + oldstack[(max+1)..];
		return TRUE;
	}
  else if (fun=tcommands[str])
		return call_other(ME,"cmd_"+fun);

	if( sscanf( str, "%d", i ) )
		push( i );
	else
	{
		filename = MASTER->_get_path(str,getuid());
		if( filename[<1] == '.' )
			filename = filename[0..<2];
		if( file_size( filename+".c" ) != -1 )
			push( filename );
		else
			push( str );
	}
	return TRUE;
}

static mixed nextAlpha( string str )
{
	int pos;

	for( 
		pos=0;
		pos<sizeof(str) && member( " #\";,^.><", str[pos] ) == -1;
		pos++
	)
		;

	if( pos==sizeof(str) )
		return ({ str, 0 });
	else
		return ({ str[0..pos-1], str[pos..] });
}

static varargs void restore_stack( string* argv, int args )
{
	int i;
	if( !args )
		args = sizeof(argv);
	if( sizeof(stack) ) push( ";" );
	for( i=0; i<args; i++ )
		push(argv[i]);
}

static int do_call( string fun, int mit_return )
{
	int args;
	string err;
	mixed result;
	mixed arg, *argv;

	argv = ({});
	while( sizeof(stack) && (arg=pop())!=";" )
		argv = ({arg}) + argv;
	if( !becomes_obj(argv) )
	{
		restore_stack( argv );
		return error( "call: Funktion nicht an Objekt gerichtet" );
	}
	if( !function_exists(fun,argv[0]) )
	{
		restore_stack(argv);
		return error( "call: Funktion nicht gefunden" );
	}

	args = sizeof(argv);
	argv += ({ 0,0,0,0,0 });
	err=(catch(result=call_other(argv[0],fun,
		argv[1],argv[2],argv[3],argv[4],argv[5])));
	if( err )
	{
		restore_stack( argv, args );
		return error( "call: "+err[1..<2] );
	}
	else
	{
		write( "=> " );
		dump_obj( result, 3 );
		if( mit_return )
			push( result );
	}
	return TRUE;
}

static int do_call_efun( string fun, int mit_return )
{
	int args;
	string err;
	mixed result;
	mixed arg, *argv;

	argv = ({});
	while( sizeof(stack) && (arg=pop())!=";" )
		argv = ({arg}) + argv;

	if( !function_exists( "efun_"+fun ) )
		return error( "call efun: \""+fun+"\" nicht benutzbar" );
	fun = "efun_"+fun;

	args = sizeof(argv);
	argv += ({ 0,0,0,0,0 });
	err=(catch(result=call_other(ME,fun,
			argv[0],argv[1],argv[2],argv[3],argv[4])));
	if( err )
	{
		restore_stack( argv, args );
		return error( "call efun: "+err[1..<2] );
	}
	else
	{
		write( "=> " );
		dump_obj( result, 3 );
		if( mit_return )
			push( result );
	}
	return TRUE;
}

static int do_lpc( string str, int mit_return )
{
	int args, val;
	string err;
	string file, cmd, pre, post;
	mixed result;
	mixed arg, *argv;

	argv = ({});
	while( sizeof(stack) && (arg=pop())!=";" )
		argv = argv + ({arg});

	file = "/players/"+getuid()+"/.teller";
	cmd = "#include <language.h>\n#include <properties.h>\n"
		+ "#include <moving.h>\n#include <defines.h>\n#include <terminal.h>\n"
		+ "#include <wizlevels.h>\n#include <ansi.h>\ncreate(){}doit(){";

	while( str!="" && sscanf( str, "%s@%s", pre, post ) == 2 )
	{
		if( sscanf( str, "%s@%d@%s", pre, val, post ) == 3 )
		{
			if( sizeof(argv)<=val || val<0 )
			{
				restore_stack( argv, args );
				return error( "lpc: Illegaler Index auf den Stack" );
			}
			cmd += pre + DumpObj(argv[val]);
			str = post;
		}
		else
			if( sscanf( str, "%s@%s", pre, post ) == 2 )
			{
				cmd += pre + "@";
				str = post;
			}
	}
	cmd += str;
	rm( file+".c" );
	write_file( file+".c" , cmd+";}\n" );
	if( find_object(file) ) destruct( find_object(file) );
	err=(catch(result=call_other(file,"doit")));
	if( err )
	{
		//rm( file+".c" );
		restore_stack( argv, args );
		return error( "lpc: "+err[1..<2] );
	}
	else
	{
		//rm( file+".c" );
		write( "=> " );
		dump_obj( result, 3 );
		if( mit_return )
			push( result );
	}
	return TRUE;
}
