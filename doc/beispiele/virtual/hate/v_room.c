inherit "/std/room";

#include <properties.h>
#include <defines.h>

create()
{
        if(IS_BLUE(this_object())) return;
        ::create();
        previous_object()->CustomizeObject();
        call_out(symbol_function("QueryObject", find_object(OBJECTD)), 2);
}
