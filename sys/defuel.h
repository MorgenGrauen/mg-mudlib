// MorgenGrauen MUDlib
//
// defuel.h -- Header fuer defuel.c (Enttanken)
//
// $Id: defuel.h,v 1.3 2003/11/11 10:59:06 Muadib Exp $

#ifndef __DEFUEL_H__
#define __DEFUEL_H__

//--------------------------------------------------------------------------
//   Statistik
#define FUELSTAT	"/d/erzmagier/muadib/fuelstat/fuelstat"   


//--------------------------------------------------------------------------
//   Limit - #defines
//   Ab wieviel kann man ueberhaupt enttanken? (rassenabhaengig)
#define   P_DEFUEL_LIMIT_FOOD       "defuel_limit_food"
#define   P_DEFUEL_LIMIT_DRINK      "defuel_limit_drink"

//--------------------------------------------------------------------------
//   Time - #defines
//   Wielange muss man zwischen dem Enttanken verdauen? (rassenabhaengig)
#define   P_DEFUEL_TIME_FOOD        "defuel_time_food"
#define   P_DEFUEL_TIME_DRINK       "defuel_time_drink"

//--------------------------------------------------------------------------
//   Amount - #defines
//   Wieviel Prozent der aktuellen P_FOOD/P_DRINK Werte kann man enttanken?
//   (rassenabhaenhig)
#define   P_DEFUEL_AMOUNT_FOOD      "defuel_amount_food"
#define   P_DEFUEL_AMOUNT_DRINK     "defuel_amount_drink"

// Returnwerte der zu verwendeten Interfaces
#define   NO_DEFUEL                  0  // leer
#define   DEFUEL_TOO_LOW            -1  // nicht leer, aber zu wenig
#define   DEFUEL_TOO_SOON           -2  // letztes entanken ist noch nicht 
                                        // lange genug her
#endif // __DEFUEL_H__
