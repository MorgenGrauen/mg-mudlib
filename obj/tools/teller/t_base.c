// ----------------------------------------------------------------------
// Die Basis- und Hilfsfunktionen der Teller.
// Stack und Memory
// ----------------------------------------------------------------------
#include "teller.h"

#include <moving.h>
#include <terminal.h>

static mixed *stack;
static mapping memory;
static object *oldinv;
static bool mit_namen;
static bool fehler_passiert;
static bool mit_say;
static bool secureinv;
static bool dologaccess;
static bool pretty;
static bool do_profile;

static mixed top();
static mixed pop();
static void push( mixed ob );

static void dump_obj( mixed ob, int indent );

void create()
{
	stack = ({});
	memory = ([]);
	mit_namen = TRUE;
	fehler_passiert = FALSE;
	mit_say = TRUE;
	secureinv = FALSE;
	dologaccess = FALSE;
	pretty = TRUE;
}

// ----------------------------------------------------------------------
// Hilfsfunktionen
// ----------------------------------------------------------------------

static int error( string msg )
{
	write( "Fehler in "+msg+".\n" );
	fehler_passiert = TRUE;
	return FALSE;
}

static int memo( string msg )
{
	write( "MEMO: "+msg+".\n" );
	return FALSE;
}

static int calcinv( string str )
{
	object* obs;
	object ob;
	int val;

	if( sscanf( str, "%d", val ) == 1 )
	{
		obs = all_inventory(top());
		if( val<0 || val>=sizeof(obs) )
			return error( "'.': Kein Objekt mit dieser Nummer" );
		pop();
		push(obs[val]);
		return TRUE;
	}

	if( !(ob=present(str,top())) )
		return error( "'.': Kein Objekt namens \""+str+"\" gefunden" );
	pop();
	push(ob);
	return TRUE;
}

static mixed push_str( string str )
{
	return
		push( implode(old_explode( implode(old_explode( implode(old_explode(
					" "+str+" ",
				"\\n"),"\n"), "\\e"),ESC), "\\b"),"\b")
		[1..<2] );
}

static void do_move( object ob, mixed dest )
{
	int weight;

	weight = ob->QueryWeight();
	if( environment(ob) )
		environment(ob)->AddWeight(-weight);
	ob->move(dest,M_NOCHECK);
	dest->AddWeight(weight);
}

void heart_beat()
{
	object *newinv;
	object owner;
	int i,j;

	if( owner=find_player(getuid()) )
	{
		newinv = sort_array( all_inventory(owner), lambda( ({'a,'b}),
			({ #'<,
				({ #'object_name, 'a }),
				({ #'object_name, 'b })
			})
		) );
		if( pointerp(oldinv) )
		{
			for( i=0,j=0; i<sizeof(newinv) && j<sizeof(oldinv) ; )
			{
				if( newinv[i] == oldinv[j] )
				{ i++; j++; }
				else if( object_name(newinv[i]) > object_name(oldinv[j]) )
				{
					tell_object(owner,
						"MEMO: \""+object_name(newinv[i])+"\" entered inventory.\n" );
					i++;
				}
				else
					j++;
			}
			for( ; i<sizeof(newinv) ; i++ )
				tell_object( owner,
					"MEMO: \""+object_name(newinv[i])+"\" entered inventory.\n" );
		}
	}
	oldinv = newinv;
}

// ----------------------------------------------------------------------
// Dumpfunktionen
// ----------------------------------------------------------------------

static void dump_array( mixed* ob, int indent )
{
	int i;
	for( i=0; i<sizeof(ob); i++ )
	{
		printf( "%*s", indent, "" );
		dump_obj( ob[i], indent );
	}
}

static void dump_mapping( mapping ob, int indent )
{
	mixed *index;
	mixed key;
	int i,j,values;

	index = m_indices( ob );
	values = get_type_info(ob)[1];
	for( i=0; i<sizeof(index); i++ )
	{
		key = index[i];
		printf( "%*s", indent, "" );
		dump_obj( key, indent );
		for( j=0; j<values; j++ )
		{
			printf( "%*s: ", indent+1, "" );
			dump_obj( ob[key,j], indent+3 );
		}
	}
}

static void dump_obj( mixed ob, int indent )
{
	if( !pretty )
	{
		printf( "%O\n", ob );
		return;
	}
	if( intp(ob) )
		write( ob + "\n" );
	else if( floatp(ob) )
		printf( "%1.5f\n", ob );
	else if( stringp(ob) )
	{
		if ( ob==";" )
			write( ";\n" );
		else
		{
			write( "\"" + implode(old_explode( implode(old_explode( implode(old_explode(
							" "+ob+" ",
						 "\n"),"\\n"), ESC), "\\e"), "\b"),"\\b")
						 [1..<2] + "\"\n"
			);
		}
	}
	else if( objectp(ob) )
	{
		if( mit_namen )
			write( object_name(ob)+" ("+ob->name(WER,0)+")\n" );
		else
			write( object_name(ob)+"\n" );
	}
	else if( mappingp(ob) )
	{
		write( "([\n" );
		dump_mapping( ob, indent + 2 );
		printf( "%*s\n", indent + 2, "])" );
	}
	else if( pointerp(ob) )
	{
		write( "({\n" );
		dump_array( ob, indent+2 );
		printf( "%*s\n", indent+2 , "})" );
	}
	else
		printf( "%O\n", ob );
}

// ----------------------------------------------------------------------
// Speicherfunktionen
// ----------------------------------------------------------------------

static void do_recall( mixed arg )
{
	if( member(memory,arg) )
		push( memory[arg,0] );
}

static void do_store( mixed arg )
{
	if ( sizeof(stack) )
	{
		if( !top() )
			memory = m_delete(memory,arg);
		else
			memory += ([ arg: top(); 0 ]);
	}
	else
		memo( "Es wurde kein Wert in \""+arg+"\" gespeichert" );
}

// ----------------------------------------------------------------------
// Stack-Funktionen
// ----------------------------------------------------------------------

static void push( mixed ob )
{
	stack = ({ ob }) + stack;
}

static mixed pop()
{
	mixed answer;

	if( !sizeof( stack ) )
		return FALSE;
	answer = stack[0];
	stack = stack[1..];
	return answer;
}	

static mixed top()
{
	if( sizeof(stack) )
		return stack[0];
  return 0;
}

static varargs int becomes_obj( mixed argv)
{
	if( !pointerp(argv) ) // default ist der stack !
		argv = stack;
	if( !sizeof(argv) )
		return FALSE; 
	if( stringp(argv[0]) && !catch(call_other(argv[0],"?")) )
	{
	  argv[0] = find_object(argv[0]);
		return TRUE;
	}
	else
		return objectp(argv[0]);
}

static int isSubStr( object pl, string str, int len )
{
	return getuid(pl)[0..len] == str;
}

static varargs mixed becomes_pl( mixed argv)
{
	object pl;
	object* pllist;
	string str;
	int len;

	if( !argv ) argv = stack;
	if( !sizeof(argv) || ( becomes_obj() && !interactive(argv[0]) ) )
		return FALSE;
	if( stringp(argv[0]) )
	{
		str = lower_case( argv[0] );
		pl = 0;
		if( str[<1..<1] == "*" )
		{	
			str = str[0..<2];
			len = sizeof(str) - 1;
			pllist = filter( users(), #'isSubStr, str, len );
			if( sizeof(pllist) == 1 )
			{
				pl=pllist[0];
				argv[0] = pl;
			}
		}
		if( !pl && pl=find_player(argv[0]) )
			argv[0] = pl;
		return pl;
	}
	return argv[0];
}

static mixed DumpMapp( mixed ob, int i);

static mixed DumpObj( mixed ob )
{
  string ans;
  int i;

  if( intp(ob) )
    return ""+ob;
  else if( stringp(ob) )
    return "\""+implode(explode(ob,"\n"),"\\n")+"\"";
  else if( objectp(ob) )
    return "\""+object_name(ob)+"\"";
  else if( mappingp(ob) )
  {
    ans="([";
    for( i=0; i<sizeof(ob)-1; i++ )
      ans += DumpMapp(ob,i)+",";
    // rely on right value of i
    return ans+DumpMapp(ob,i)+"])";
  }
  else
  {
    ans="({";
    for( i=0; i<sizeof(ob)-1; i++ )
      ans += DumpObj(ob[i])+",";
    // rely on right value of i
    return ans+DumpObj(ob[i])+"})";
  }
}

static mixed DumpMapp( mixed ob, int i)
{
  int j,vz;
  string ans;

  vz = get_type_info(ob)[1];
  ans = DumpObj(m_indices(ob)[i])+":";
  for( j=0; j<vz-1; j++ )
    ans += DumpObj(ob[m_indices(ob)[i],j])+";";
  // rely on value of j
  return ans + DumpObj(ob[m_indices(ob)[i],j]);
}
