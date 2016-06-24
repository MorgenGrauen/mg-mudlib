// MorgenGrauen MUDlib
//
// OBJECT.H -- persistent object handling
//
// $Date: 1995/03/31 13:30:33 $
// $Revision: 1.1 $
/* $Log: object.h,v $
 * Revision 1.1  1995/03/31  13:30:33  Hate
 * Initial revision
 *
 * Revision 1.1  1994/03/20  17:07:28  Hate
 * Initial revision
 *
 */
 
#ifndef __OBJECT_H__
#define __OBJECT_H__

// defines
#define OBJECTD       "/p/daemon/objectd"
#define OBJECTD_SAVE  "/p/daemon/save/objectd"

#endif // __OBJECT_H__

#ifdef NEED_PROTOTYPES

#ifndef __OBJECT_H_PROTO__
#define __OBJECT_H_PROTO__

// prototypes
mixed AddObject(string obj, string env);
mixed RemoveObject(string obj, string env);
varargs void QueryObject(mixed env);


#endif // __OBJECT_H_PROTO__

#endif	// NEED_PROTOYPES
