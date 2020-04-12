// MorgenGrauen MUDlib
#pragma strong_types,save_types,rtt_checks
#pragma no_clone,no_inherit,no_shadow
#pragma pedantic, range_check

#include <files.h>
#include <config.h>

// Variablen fuer das Savefile von Jof, was dieses Objekt schreibt.
string name = "jof";
string password = "";
int level = 100;
string shell = "/std/shells/magier";

public varargs int remove(int s)
{
  destruct(this_object());
  return 1;
}

protected void create()
{
# if !defined(__TESTMUD__) && MUDHOST==__HOST_NAME__
  remove();
  return;
# endif
  seteuid(getuid(this_object()));
  if(file_size("/players/jof") == FSIZE_NOFILE)
  {
    mkdir("/players/jof");
  }
  if(file_size("/players/jof/workroom.c") == FSIZE_NOFILE)
  {
    copy_file("/std/def_workroom.c","/players/jof/workroom.c");
  }
  if (!master()->find_userinfo("jof"))
  {
    save_object(SECURESAVEPATH+"j/jof");
  }

  if(file_size("/room/konstrukt.c") > 0)
  {
    load_object("/room/konstrukt");
  }
  master()->clear_cache();
  remove();
}
