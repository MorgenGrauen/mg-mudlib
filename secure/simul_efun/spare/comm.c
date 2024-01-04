#include <living/comm.h>

// Sendet an alle Objekte in room und room selber durch Aufruf von
// ReceiveMsg().
varargs void send_room(object|string room, string msg, int msg_type,
                       string msg_action, string msg_prefix, object *exclude,
                       object origin=previous_object())
{
  if (stringp(room))
    room=load_object(room);

  // Keine Ausgabe im Netztotenraum. Die Anwesenden fangen damit sowieso
  // nichts an und aufgrund der aufwaendigen Checks besteht die Gefahr, dass
  // die Ticks ausgehen.
  if(object_name(room) == "/room/netztot") return;

  object *dest = exclude ? all_inventory(room) - exclude :
                           all_inventory(room);

  dest->ReceiveMsg(msg, msg_type, msg_action, msg_prefix, origin);
}

varargs void send_debug(object|string wiz, string msg, string msg_prefix)
{
  if (stringp(wiz))
    wiz=find_player(wiz);
  // Mit Absicht hier keine Pruefung auf Magierstatus. Das macht ReceiveMsg.
  // Und: so ist die Moeglichkeit gegeben, Testspieler so einzustellen, dass
  // sie die Debugmeldungen erhalten.
  if (objectp(wiz))
      wiz->ReceiveMsg(msg, MT_DEBUG|MSG_BS_LEAVE_LFS|MSG_DONT_STORE,
                      0, msg_prefix, previous_object());
}

static int _shout_filter( object ob, string pat )
{
    if ( !environment(ob) )
       return 0;

    return sizeof( regexp( ({ object_name( environment(ob) ) }), pat ) );
}

varargs void shout( string s, mixed where ){
    object *u;
    string *pfade;

    if ( !sizeof( u = users() - ({ this_player() }) ) )
       return;

    if ( !where )
       pfade = ({ "/" });
    else if ( intp(where) )
       pfade =
           ({ implode( efun::explode( object_name( environment(this_player()) ),
                                   "/" )[0..2], "/" ) + "/" });
    else if ( stringp(where) )
       pfade = ({ where });
    else
       pfade = where;

    u = filter( u, "_shout_filter", ME, implode( pfade, "|" ) );
    u->ReceiveMsg(s, MT_COMM|MT_FAR|MSG_DONT_WRAP|MSG_DONT_STORE,
                  MA_SHOUT_SEFUN, 0, previous_object());
}


#if __VERSION__ > "3.3.718"
// This sefun replaces the deprecated efun cat().
#define CAT_MAX_LINES 50
varargs int cat(string file, int start, int num)
{
    if (extern_call())
        set_this_object(previous_object());

    int more;

    if (num < 0 || !this_player())
        return 0;

    if (!start)
        start = 1;

    if (!num || num > CAT_MAX_LINES) {
        num = CAT_MAX_LINES;
        more = sizeof(read_file(file, start+num, 1));
    }

    string txt = read_file(file, start, num);
    if (!txt)
        return 0;

    efun::tell_object(this_player(), txt);

    if (more)
        efun::tell_object(this_player(), "*****TRUNCATED****\n");

    return sizeof(txt & "\n");
}
#undef CAT_MAX_LINES
#endif // __VERSION__ > "3.3.718"

#if __VERSION__ > "3.3.719"
// This sefun replaces the deprecated efun tail().
#define TAIL_MAX_BYTES 1000
varargs int tail(string file)
{
    if (extern_call())
        set_this_object(previous_object());

    if (!stringp(file) || !this_player())
        return 0;

    bytes bs = read_bytes(file, -(TAIL_MAX_BYTES + 80),
                          (TAIL_MAX_BYTES + 80));
    // read_bytes() returns 0 if the start of the section given by
    // parameter #2 lies before the beginning of the file.
    // In this case we simply try and read the entire file.
    if (!bytesp(bs) && file_size(file) < TAIL_MAX_BYTES+80)
      bs = read_bytes(file, 0, (TAIL_MAX_BYTES + 80));
    // Exit if still nothing could be read.
    if (!bytesp(bs))
      return 0;

    // convert to string
    string txt = to_text(bs, "UTF-8");

    // cut off first (incomplete) line
    int index = strstr(txt, "\n");
    if (index > -1) {
        if (index + 1 < sizeof(txt))
            txt = txt[index+1..];
        else
            txt = "";
    }

    efun::tell_object(this_player(), txt);

    return 1;
}
#undef TAIL_MAX_BYTES
#endif // __VERSION__ > "3.3.719"

