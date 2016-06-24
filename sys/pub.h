// MorgenGrauen MUDlib
//
// pub.h -- Headerfile for pubs and pubmaster
//
// $Id: pub.h 4494 2006-09-23 10:14:36Z root $

#ifndef _PUB_H_
#define _PUB_H_

/* Der Pubmaster */
#define PUBMASTER "/secure/pubmaster"

/* Werte fuer die Kneipenformel:

  Alc*Alcdelay+Drink*Drinkdelay+Food*Fooddelay         Value
( --------------------------------------------  +  -------------
          Alcdelay+Drinkdelay+Fooddelay            VALUEDIV+Rate

         Rate                                    Delay
   exp(--------)                           exp(--------)
       RATEDIV1                                WAITDIV1
-  ------------- ) * Factor * ( WAITOFFS + --------------- )
     RATEDIV2                                 WAITDIV2

*/

#define PUB_SOAKMULT 18.0
#define PUB_VALUEDIV 10.0
#define PUB_RATEDIV1  3.5
#define PUB_RATEDIV2  2.5
#define PUB_WAITOFFS  0.9
#define PUB_WAITDIV1 10.0
#define PUB_WAITDIV2  7.5
#define PUB_MAXDELAY 15.0

/* Properties */

// NPCs tanken hier auf die "schnelle" Art
#define P_NPC_FASTHEAL    "npc_fastheal"

// Keine Standard-Getraenke
#define P_NO_STD_DRINK    "no_std_drink"

// Meldung, wenn etwas nicht mehr vorhanden ist
#define P_PUB_UNAVAILABLE "pub_unavailable"

// Meldung, wenn etwas nicht im Menue steht
#define P_PUB_NOT_ON_MENU "pub_not_on_menu"

// Meldung, wenn Spieler nicht genug Geld hat
#define P_PUB_NO_MONEY    "pub_no_money"

// Meldung, wenn der Barkeeper nicht anwesend ist
#define P_PUB_NO_KEEPER   "pub_no_keeper"

/* Sonstige Defines */

// Indices fuer das Menue-Mapping
#define PM_TEXT      0
#define PM_INFO      1
#define PM_RATE      2
#define PM_SERVE_MSG 3
#define PM_REFRESH   4
#define PM_DELAY     5
#define PM_DELAY_MSG 6
#define PM_IDS       7

// Keys fuer den Refresh-Parameter
#define PR_ALL       "All"
#define PR_USER      "User"

// Default-Defines fuer den Availability-Check
#define PR_DEFAULT   "DEF"
#define PR_NONE      "N/A"

// Indices fuer den Refresh-Parameter
#define PRV_AMOUNT   0
#define PRV_REFRESH  1

#endif // _PUB_H_
