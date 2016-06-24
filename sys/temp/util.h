// MorgenGrauen MUDlib
//
// sys/thing/util.h -- Headerfile fuer /std/thing/util.c
//
// $Id: util.h,v 1.1 2000/08/15 12:07:30 Paracelsus Exp $

#ifndef __THING_UTIL_H__
#define __THING_UTIL_H__

#endif // __THING_UTIL_H__

#ifdef NEED_PROTOTYPES

#ifndef __THING_UTIL_H_PROTO__
#define __THING_UTIL_H_PROTO__

public void ShowPropList(string *props);
static void PrettyDump(mixed x);
static void DumpArray(mixed *x);
static void DumpMapping(mapping x);
static void DumpKeyValPair(mapping x, mixed key, int size);

#endif // __THING_UTIL_H_PROTO__

#endif // NEED_PROTOTYPES