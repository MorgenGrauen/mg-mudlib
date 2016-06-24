#define INVPATH "/std/player/invmaster/"
#define DB(x)
/*
#define DB(x) if (find_player("rikus")) \
                   tell_object(find_player("rikus"), \
                     break_string(x, 78, "INVMASTER: ", 1));
*/
#define P_INVMASTER_CONFIG "invmaster_config"
#define COLORNAMES ({"Schwarz","Rot","Gruen","Gelb",\
                     "Blau","Magenta","Tuerkis","Weiss","Keine"})
#define COLORCODES (["schwarz":0,\
                     "rot":1,\
                     "gruen":2,\
                     "gelb":3,\
                     "blau":4,\
                     "magenta":5,\
                     "tuerkis":6,\
                     "weiss":7,\
                     "keine":8,\
                     "hintergrund":8])
#define default_config ([0:0, 1:1, 2:2, 3:3, 4:4, 5:5, 6:6, 7:7, 8:0])
