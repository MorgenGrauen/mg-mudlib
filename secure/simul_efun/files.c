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
