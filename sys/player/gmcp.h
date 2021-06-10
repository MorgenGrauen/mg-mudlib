// MorgenGrauen MUDlib
//
// player/gmcp.h -- Stuff for GMCP handling
//
// $Id$

#ifndef __GMCP_H__
#define __GMCP_H__

// properties

#endif // __GMCP_H__

#ifdef NEED_PROTOTYPES

#ifndef __GMCP_H_PROTO__
#define __GMCP_H_PROTO__

// prototypes
protected int GMCP_Status(string module="Core");
protected int GMCP_Char(mapping data);
protected int GMCP_Channel(string msg, string channel, string sender);
protected int GMCP_Room();

#endif // __GMCP_H_PROTO__

#endif  // NEED_PROTOYPES

