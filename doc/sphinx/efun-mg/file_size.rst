file_size
=========

BEISPIELE
---------

  .. code-block:: pike

      Ein Spieler soll seinen Plan abfragen koennen:

      #include <sys/files.h>
      if(file_size("/p/service/loco/plans/"+
                   getuid(this_player())+".plan") <= FSIZE_NOFILE)
      {
        write("Du hast keinen eigenen Plan festgelegt.\n");
        return 1;
      }

      this_player()->More(read_file("/p/service/loco/plans/"+
                          getuid(this_player())+".plan");

