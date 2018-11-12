// MorgenGrauen MUDlib
//
// container/vitems.h -- managing virtually present automatic items
//

#ifndef __CONTAINER_VITEMS_H__
#define __CONTAINER_VITEMS_H__

#define P_VITEMS "p_lib_vitems"

// VI_REFRESH* haben aehnliche Bedeutung wie bei AddItem
#define VI_REFRESH_NONE      1
#define VI_REFRESH_DESTRUCT  2
#define VI_REFRESH_REMOVE    3
#define VI_REFRESH_ALWAYS    4
#define VI_REFRESH_MOVE_HOME 5
#define VI_REFRESH_INSTANT   6

// Prefix fuer einen Propertynamen, welcher anzeigt, dass die Prop vor dem
// Seten resettet (d.h. komplett ueberschrieben werden soll).
#define VI_RESET_PREFIX      '-'
#define VI_PROP_RESET(x)      ("-"+x)

#endif // __CONTAINER_VITEMS_H__

#ifdef NEED_PROTOTYPES

#ifndef __CONTAINER_VITEMS_H_PROTO__
#define __CONTAINER_VITEMS_H_PROTO__
public varargs void RemoveVItem(string key);
public varargs void AddVItem(string key, int refresh, mapping shadowprops,
                             string path, mapping props);
public object *GetVItemClones();
public object present_vitem(string complex_desc);

#endif // __CONTAINER_VITEMS_H_PROTO__

#endif // NEED_PROTOTYPES

