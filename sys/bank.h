// MorgenGrauen MUDlib
//
// bank.h -- Definitionen fuer die Zentralbank bzw. Laeden
//
// $Id: bank.h 6749 2008-03-12 19:45:27Z Zesstra $

#ifndef __BANK_H__
#define __BANK_H__

#define ZENTRALBANK "/p/daemon/zentralbank"

// Diese Properties lassen sich nur abfragen!
#define P_SHOP_PERCENT_LEFT "shop_percent_left"    // Aktuelle Prozente in
#define P_STORE_PERCENT_LEFT "store_percent_left"  // Laden und Speicher
#define P_CURRENT_MONEY "current_money"       // Vermoegen des Ladens

#define P_STORE_CONSUME  "store_consume"    /* default 20%, immer mind. 1% */
#define P_MIN_STOCK      "min_stock"    /* mindest Vorrat an Gegenstaenden */

#endif // __BANK_H__

// Konstanten fuer die Implementation
#ifdef NEED_PROTOTYPES
#ifndef __BANK_H_PROTOTYPES__
#define __BANK_H_PROTOTYPES__

#define BANK_DEFAULT_PERCENT 20 // So viele % der Einzahlung gehen aufs Konto
#define SHOP_PERCENT_LEFT 30 // So viele % kriegt der Laden fuer Objekte
			     // gutgeschrieben, die er zerstoert

#define STORE_PERCENT_LEFT 25 // Dito fuer den Store

#endif // __BANK_H_PROTOTYPES__

#endif // NEED_PROTOTYPES

