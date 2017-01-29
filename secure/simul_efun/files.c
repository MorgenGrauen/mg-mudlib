#include "/sys/files.h"

public varargs string read_data(string file, int start, anzahl)
{
    if (!stringp(file))
      raise_error("Bad arg 1 to read_data(): expected 'string'.\n");

    // Wenn Pfad nicht absolut, ist das nen Fehler, daher wird das nicht
    // korrigiert.

    // wenn kein /data/ vorn steht, wird es vorangestellt.
    if (strstr(file,"/"LIBDATADIR"/") != 0)
    {
      file = "/"LIBDATADIR + file;
    }
    // read_file() nicht direkt rufen, sondern vorher als closure ans
    // aufrufende Objekt binden. Sonst laeuft die Rechtepruefung in
    // valid_write() im Master unter der Annahme, dass die simul_efun.c mit
    // ihrer root id was will.
    return funcall(bind_lambda(#'efun::read_file, previous_object()),
                   file, start, anzahl);
}

public varargs int write_data(string file, string str, int flags)
{
    if (!stringp(file))
      raise_error("Bad arg 1 to write_data(): expected 'string'.\n");
    if (!stringp(str))
      raise_error("Bad arg 2 to write_data(): expected 'string'.\n");

    // Wenn Pfad nicht absolut, ist das nen Fehler, daher wird das nicht
    // korrigiert.

    // wenn kein /data/ vorn steht, wird es vorangestellt.
    if (strstr(file,"/"LIBDATADIR"/") != 0)
    {
      file = "/"LIBDATADIR + file;
    }
    // write_file() nicht direkt rufen, sondern vorher als closure ans
    // aufrufende Objekt binden. Sonst laeuft die Rechtepruefung in
    // valid_write() im Master unter der Annahme, dass die simul_efun.c mit
    // ihrer root id was will.
    return funcall(bind_lambda(#'efun::write_file, previous_object()),
                   file, str, flags);
}

// * Bei 50k Groesse Log-File rotieren
varargs int log_file(string file, string txt, int size_to_break)
{
    mixed *st;

    file="/log/"+file;
    file=implode((efun::explode(file,"/")-({".."})),"/");

    if (!funcall(bind_lambda(#'efun::call_other,PO),"secure/master",//')
                "valid_write",file,geteuid(PO),"log_file",PO))
      return 0;
    if ( size_to_break >= 0 & (
        sizeof(st = get_dir(file,2) ) && st[0] >= (size_to_break|MAX_LOG_SIZE)))
        catch(rename(file, file + ".old");publish); /* No panic if failure */

    return(write_file(file,txt));
}

#if !__EFUN_DEFINED__(copy_file)
#define MAXLEN 50000
public nomask int copy_file(string source, string dest)
{

    int ptr;
    string bytes;

    set_this_object(previous_object());
    if (!sizeof(source)||!sizeof(dest)||source==dest||(file_size(source)==-1)||
        (!call_other(master(),"valid_read",source,
                     getuid(this_interactive()||
                   previous_object()),"read_file",previous_object()))||
        (!call_other(master(),"valid_read",source,
                     getuid(this_interactive()||
                   previous_object()),"write_file",previous_object())))
      return 1;
    switch (file_size(dest))
    {
    case -1:
      break;
    case -2:
      if (dest[<1]!='/') dest+="/";
      dest+=efun::explode(source,"/")[<1];
      if (file_size(dest)==-2) return 1;
      if (file_size(dest)!=-1) break;
    default:
      if (!rm(dest)) return 1;
      break;
    }
    do
    {
      bytes = read_bytes(source, ptr, MAXLEN); ptr += MAXLEN;
      if (!bytes) bytes="";
      write_file(dest, bytes);
    }
    while(sizeof(bytes) == MAXLEN);
    return 0;
}
#endif //!__EFUN_DEFINED__(copy_file)


