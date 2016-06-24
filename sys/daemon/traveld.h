#ifndef __TRAVELD_H__
#define __TRAVELD_H__

#define TRAVELD          "/p/daemon/traveld"
#define TRAVELD_SAVEFILE "/p/daemon/save/traveld"

#ifdef NEED_PROTOTYPES
#ifndef __TRAVELD_H_PROTO__
#define __TRAVELD_H_PROTO__

public void AddStop(string ship, string harbour);
public void RemoveTransporter(object ship);
public varargs mixed HasTransporter(object harbour, string ship);
public mixed RouteExists(object ship, string dest);
public mapping QueryShips();
public mapping QueryAllHarbours();

#endif // __TRAVELD_H_PROTO__

#endif // NEED_PROTOTYPES

#endif // __TRAVELD_H__
