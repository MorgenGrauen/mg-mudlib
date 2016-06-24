#ifndef __UPTIME_MASTER__
#define __UPTIME_MASTER__

#define UPTIME_MASTER  "/p/daemon/uptime_master"
#define MAX_UPTIME     "/p/daemon/save/maxuptime"
#define RECORD_CHANNEL "<MasteR>"

#define DEBUG(x)
/*
#define DEBUG(x) if(find_player("vanion")) tell_object(find_player("vanion"),\
		                "UPTIME: "+x+"\n");
*/

#endif /* __UPTIME_MASTER__ */
