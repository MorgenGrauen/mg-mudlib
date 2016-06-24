// MorgenGrauen MUDlib
//
// VR_COMPILER.C -- virtual room compiler
//
// $Date: 2002/08/28 09:57:18 $
// $Revision: 6605 $
/* $Log: vr_compiler.c,v $
 * Revision 1.3  2002/08/28 09:57:18  Rikus
 * auf strong_types umgestellt
 *
 * Revision 1.2  1994/07/27 14:48:18  Jof
 * suid
 *
 * Revision 1.1  1994/02/01  19:47:57  Hate
 * Initial revision
 *
*/

// principle:
// 	- inherit this object into your own 'virtual_compiler.c'
//  - customize Validate() and CustomizeObject() for you own sake
//  
//  * Validate() checks if a room filename given as argument (without path)
//    is valid and returns this filename with stripped '.c'!!
//  * CustomizeObject() uses the previous_object()->Function() strategy to
//    customize the standard object (for example to set a description)
//
// Properties: P_VALID_NAME, P_MIN_X, P_MAX_X, P_MIN_Y, P_MAX_Y

#pragma strong_types

inherit "/std/virtual/v_compiler";

#define NEED_PROTOTYPES

#include <thing/properties.h>
#include <defines.h>
#include <v_compiler.h>
#include "/obj/virtual/vr_compiler.h"
#include <sys_debug.h>

void create()
{
  ::create();
	SetProp(P_VALID_NAME, "raum");
	seteuid(getuid());
}

string Validate(string file)
{
	int x,y;
	string path, name;

  file = ::Validate(file);
	if((sscanf(file, "%s[%d,%d]", name, x, y) == 3) &&
			name == QueryProp(P_VALID_NAME) &&
		  (x >= QueryProp(P_MIN_X) && x <= QueryProp(P_MAX_X)) &&
		  (y >= QueryProp(P_MIN_Y) && y <= QueryProp(P_MAX_Y))
		)
		return file;
}

mixed CustomizeObject()
{
	string path, file, name;
	int x,y;

	if(!(file = ::CustomizeObject())) return 0;

	path = QueryProp(P_COMPILER_PATH);
	sscanf(file, "%s[%d,%d]", name, x, y);
	name = QueryProp(P_VALID_NAME);
	if(y < QueryProp(P_MAX_Y))
		previous_object()->AddExit("norden", path+"/"+name+"["+(x  )+","+(y+1)+"]");
	if(y > QueryProp(P_MIN_Y))
		previous_object()->AddExit("sueden", path+"/"+name+"["+(x  )+","+(y-1)+"]");
	if(x < QueryProp(P_MAX_X))
		previous_object()->AddExit("osten" , path+"/"+name+"["+(x+1)+","+(y  )+"]");
	if(x > QueryProp(P_MIN_X))
		previous_object()->AddExit("westen", path+"/"+name+"["+(x-1)+","+(y  )+"]");
}
