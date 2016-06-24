// MorgenGrauen MUDlib
/** @file
  Konstanten, Prototypen und Properties fuer sockelbare Items.
* Langbeschreibung...
* @author Zesstra + Arathorn
* @date xx.05.2008
* @version $Id$
*/

/* Changelog:
*/

#ifndef _SOCKETS_H_
#define _SOCKETS_H_

/** Speichert die Sockel und deren Inhalt, die ein Item hat.
  ([ TYP1: ({ITEM1, ITEM2}),
     TYP2: ({-1}) ]) \n
     ITEM1 = ([ P_SOCKET_PROPS, 
                BLUE_NAME: load_name(),
                DESCRIPTION: name(WER) ])
  @internal
 */
#define P_SOCKETS      "_sockets"

/** Speichert ein Mapping von Prop-Modifikatoren.
  Die in diesem Mapping enthaltenen Props werden beim Einsetzen im Item mit
  dem Sockel um den angegebenen Wert modifiziert.
  */
#define P_SOCKET_PROPS "_socket_props"

/** Sockeltyp, in den das Item eingesetzt werden kann.
  Alle anderen Sockel sind inkompatibel.
  */
#define P_SOCKET_TYPE  "_socket_type"

/** Einschraenkungen, denen der gesockelte Gegenstand nach dem Einsetzen
    zusaetzlich unterliegt. 
    Wird P_RESTRICTIONS hinzugefuegt. Mapping a la P_RESTRICTIONS.
 */
#define P_SOCKET_RESTR_USE    "_socket_restr_use"

/** Einschraenkungen, denen das Lebewesen unterliegt, welches das Item in 
  den Sockel einbauen soll/will.
  Mapping a la P_RESTRICTIONS.
  */
#define P_SOCKET_RESTR_MOUNT  "_socket_restr_mount"

// Sockeltypen
// TODO: gueltige Sockeltypen definieren festlegen, welche Props sie aendern
// duerfen.
#define VALID_SOCKET_TYPES ({})

// Rueckgabewerte
#define SOCKET_OK                1
#define SOCKET_NO_OBJECT        -1
#define SOCKET_NO_DATA          -2
#define SOCKET_INVALID_TYPE     -3
#define SOCKET_NO_SOCKETS       -4
#define SOCKET_NONE_AVAILABLE   -5
#define SOCKET_NO_EXPERTISE     -6
#define SOCKET_ITEM_INUSE       -7

// sym. Konstante fuer freien Sockel
#define SOCKET_FREE             -1

#endif // _SOCKETS_H_

#ifdef NEED_PROTOTYPES
#ifndef _SOCKETS_H_PROTOTYPES_
#define _SOCKETS_H_PROTOTYPES_
// public Interface 
public int   MountSocketItem(object item);
public int   TestSocketItem(object item);
public mixed GetSocketInfo();

#endif // _SOCKETS_H_PROTOTYPES_
#endif // NEED_PROTOTYPES

