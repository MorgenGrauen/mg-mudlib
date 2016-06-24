// MorgenGrauen MUDlib
//
// mini_props.c -- Abgespeckte Verwaltung von Props
//                 Aus Sicherheitsgruenden darf hier nicht die "Vollversion"
//                 mit echten Properties stehen
//
// $Id$
#pragma strict_types,save_types
#pragma no_clone
#pragma no_shadow
#pragma pedantic,range_check,warn_deprecated

#define NEED_PROTOTYPES
#include <thing/properties.h>
#undef NEED_PROTOTYPES

nosave mapping prop;

private void InitProps()
{
    prop = m_allocate(2,1);
}


public varargs mixed Query( string str, int type )
{
    if ( !mappingp(prop) )
        InitProps();

    return prop[str];
}

public varargs mixed Set(string name, mixed value, int type, int extern)
{
    if ( !mappingp(prop) )
        InitProps();

    return prop[name] = value;
}

