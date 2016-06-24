// MorgenGrauen MUDlib
//
// kraeuter.h -- Definition fuer Kraeuter 
//
// $Id: fishing.h,v 3.2 2002/12/17 10:01:43 Rikus Exp $

#ifndef __KRAEUTER_H__
#define __KRAEUTER_H__

#define KRAEUTERLISTE      "/sys/items/kraeuter/kraeuterliste.h"

#define PLANTDIR           "/items/kraeuter/"

#define SECURE(x) ("/secure/"+x)
#define KRAEUTERVCSAVEFILE SECURE("ARCH/kraeuterVC")
#define PLANTMASTER        (SECURE("krautmaster"))
#define KRAEUTERVC         PLANTDIR"virtual_compiler"

#define STDPLANT           "/std/items/kraeuter/kraut"
#define PLANTITEM          PLANTDIR"kraut"
#define STDTRANK           "/std/items/kraeuter/trank"
#define TRANKITEM          PLANTDIR"trank"

#define KESSELID           "_lib\nkraeuterkessel"

#define INGREDIENT_ID           0
#define INGREDIENT_DEMON        1
#define INGREDIENT_GENDER       2
#define INGREDIENT_NAME         3
#define INGREDIENT_ADJ          4
#define INGREDIENT_LONG         5
#define INGREDIENT_ROOMDETAIL   6

#define PLANT_ROOMDETAIL   "_lib_p_plantroomdetail"

#define IS_PLANT(x) (member(inherit_list(x), \
                     "/std/items/kraeuter/kraut.c")>=0)

#endif // __KRAEUTER_H__

