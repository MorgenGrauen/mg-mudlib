// MorgenGrauen MUDlib
//
// money.h -- Konstanten und Pfade rund ums Geld
//
// $Id: moneyhandler.h,v 3.1 1997/02/12 13:29:09 Wargon Exp $

#ifndef __LIB_MONEY_H__
#define __LIB_MONEY_H__

#include <container/moneyhandler.h>
#include <bank.h>

#define GELD       "/items/money"
#define BOERSE     "/items/boerse"
#define SEHERKARTE "/items/seercard"

#define GELDID       "\ngeld"
#define BOERSEID     "\ngeldboerse"
#define SEHERKARTEID "\ngeldkarte"
// Id fuer eine Boerse mit Geld drin...
#define GELDBOERSE_MIT_GELD "\ngeldboerse_mit_geld"
// ID fuer aktive Geldkarte
#define SEHERKARTEID_AKTIV "\ngeldkarte_aktiv"

#endif

#ifdef NEED_PROTOTYPES
#ifndef __LIB_MONEY_H_PROTO__
#define __LIB_MONEY_H_PROTO__

#endif
#endif
