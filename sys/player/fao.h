// MorgenGrauen MUDlib
//

#ifndef __PLAYER_FAO_H__
#define __PLAYER_FAO_H__

#define FAO_MASTER      "/d/ebene/miril/fraternitas/secure/faomaster" // die zentrale stelle des fao

// die Liste der verfügbaren Portale fuer das erste portalgeschenk
#define FAO_PORTALS1_LIST ({2,3,4,5,6,7,8,9,10,13,14,15,16,19,20,21,25,29,33})
// die Liste der verfügbaren Portale fuer das zweite portalgeschenk
#define FAO_PORTALS2_LIST FAO_PORTALS1_LIST+({1,11,12,17,18,24,26,27,28,31,34,36,37,38})
// die anzahl der zu waehlenden portale fuer das erste portalgeschenk
#define FAO_PORTALS1_NUM  5
// die anzahl der zu waehlenden portale fuer das zweite portalgeschenk
#define FAO_PORTALS2_NUM  5

// properties
#define P_FAO           "fraternitasdonoarchmagorum"  // enthaelt die doni
#define P_FAO_PORTALS   P_FAO+"PORTALS" // enthaelt die dem spieler uebergebenen portale

// bitmasks
#define FAO_TITLE   0x01
#define FAO_PORTALS1    0x02
#define FAO_PORTALS2  0x04
#define FAO_SEER_PORTAL 0x08
#define FAO_MEMBER 0x10

// hilfreiche makros
#define FAO_HAS_TITLE_GIFT(x)   (x!=0 && query_once_interactive(x) \
    && ((int)x->QueryProp(P_FAO)&FAO_TITLE)!=0)
#define FAO_HAS_PORTALS1_GIFT(x)  (x!=0 && query_once_interactive(x) \
    && ((int)x->QueryProp(P_FAO)&FAO_PORTALS1)!=0)
#define FAO_HAS_PORTALS2_GIFT(x)  (x!=0 && query_once_interactive(x) \
    && ((int)x->QueryProp(P_FAO)&FAO_PORTALS2)!=0)
#define FAO_HAS_PORTALS_GIFT(x)   (FAO_HAS_PORTALS1_GIFT(x) || FAO_HAS_PORTALS2_GIFT(x))
#define FAO_MAY_USE_PORTAL(x,p)   (intp(p) && ((FAO_HAS_PORTALS1_GIFT(x) && \
        member(FAO_PORTALS1_LIST,p)!=-1 )||(FAO_HAS_PORTALS2_GIFT(x) && \
        member(FAO_PORTALS2_LIST,p)!=-1) ) && \
    (pointerp((int*)x->QueryProp(P_FAO_PORTALS)) \
     && member((int*)x->QueryProp(P_FAO_PORTALS),p)!=-1) )
#define FAO_HAS_SEER_PORTAL_GIFT(x)     (x!=0 && query_once_interactive(x) \
    && ((int)x->QueryProp(P_FAO)&FAO_SEER_PORTAL)!=0)
#define FAO_IS_MEMBER(x) (x!=0 && query_once_interactive(x) \
    && ((int)x->QueryProp(P_FAO)&FAO_MEMBER)!=0)
#endif // __PLAYER_FAO_H__

