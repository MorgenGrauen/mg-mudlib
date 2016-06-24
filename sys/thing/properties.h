// MorgenGrauen MUDlib
//
// thing/properties.h -- contains property definitions and prototypes
//
// $Id: properties.h,v 3.5 2003/08/07 15:09:33 Rikus Exp $

#ifndef __THING_PROPERTIES_H__
#define __THING_PROPERTIES_H__

// special defines

#define F_VALUE         0
#define F_MODE          1
#define F_SET_METHOD    2
#define F_QUERY_METHOD  3
#define F_MODE_AS       4
#define F_MODE_AD       5

#define SAVE            64
#define PROTECTED       128  // only this_object() can change the values
#define SECURED         256  // like PROTECTED, but never resetable
#define NOSETMETHOD     512  // Keine Set-Methode => Nicht mofifizierbar
#define SETMNOTFOUND	1024 // Keine Set-Methode gefunden
#define QUERYMNOTFOUND	2048 // Keine Query-Methode gefunden
#define QUERYCACHED	4096
#define SETCACHED	8192

#endif // __THING_PROPERTIES_H__

#ifdef NEED_PROTOTYPES

#ifndef __THING_PROPERTIES_H_PROTO__
#define __THING_PROPERTIES_H_PROTO__

// prototypes

// (E)UID-Methods
//static mixed _query_uid();
//static mixed _query_euid();

// Property-Initialisation
protected void InitializeProperties();

// direct property access
public varargs mixed Set(string name, mixed Value, int Type, int extern);
public varargs mixed Query(string name, int Type);

// filtered property access
public mixed SetProp(string name, mixed Value);
public mixed QueryProp(string name);

// global property handling
public void SetProperties(mapping props);
public mapping QueryProperties();

// misc/interna
public mixed *__query_properties();
public void _set_save_data(mixed data);
public mixed _get_save_data();

#endif // __THING_PROPERTIES_H_PROTO__

#endif // NEED_PROTOTYPES
