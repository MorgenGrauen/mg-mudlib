#include "/sys/files.h"

#define PO        efun::previous_object(0)

public varargs string read_data(string file, int start, int anzahl)
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
    // Wenn file schon mit /log/ anfaengt, schreiben wir nicht noch eins
    // davor.
    if (strstr(file,"/"LIBLOGDIR"/") != 0)
    {
      file="/log/"+file;
    }

    // Achtung: es ist verfuehrerisch, diese Pruefung zu entfernen und
    // stattdessen, den Aufruf von write_file() an den Aufrufer zu binden.
    // Dies funktioniert aber nicht, weil log_file gesonderte Behandlung in
    // valid_write() erfaehrt.
    file=implode((efun::explode(file,"/")-({".."})),"/");
    if (!funcall(bind_lambda(#'efun::call_other,PO),"secure/master",//')
                "valid_write",file,geteuid(PO),"log_file",PO))
      return 0;

    // pruefen, ob das Verzeichnis existiert - sonst anlegen
    string dir=file[..strrstr(file,"/")-1];
    if (file_size(dir) != -2) // FSIZE_DIR
    {
      if (!mkdirp(dir))
        return 0;
    }

    // Wenn zu gross, rotieren.
    if ( size_to_break >= 0 & (
        sizeof(st = get_dir(file,2) ) && st[0] >= (size_to_break|MAX_LOG_SIZE)))
        catch(rename(file, file + ".old");publish); /* No panic if failure */

    // Die Zugriffspruefung hier laeuft mit Rechten der simul_efuns...
    // Rechtepruefung oben
    return(write_file(file,txt));
}

#undef PO

