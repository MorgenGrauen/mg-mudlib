#pragma strong_types,save_types,rtt_checks
#pragma no_clone,no_inherit,no_shadow
#pragma pedantic, range_check

#include <rooms.h>
#include <files.h>
#include <properties.h>
#include <config.h>

inherit "/std/room";

protected void add_testmud_gexits();

protected void create() {
  ::create();

  SetProp(P_INT_LONG,
    break_string(
      "Du befindest Dich im \"Konstrukt\", einem weissen Nichts eines "
      "jungfraeulichen Universums. Dieser Ort wartet darauf, dass Du ihn "
      "mit Deiner Phantasie auffuellst. Von hier aus kann man wohl durch "
      "blossen Willen andere Orte erreichen, die es in diesem jungen "
      "Universum schon gibt.", 78));
  SetProp(P_INT_SHORT, "Im Konstrukt");
  SetProp(P_LIGHT, 1);

  AddDetail(({"konstrukt","nichts"}),
    break_string("Es ist weiss - und sonst Nichts.", 78));
  AddDetail("universum",
    break_string("Es liegt dir zu Fuessen.", 78));
  AddDetail(({"fuss","fuesse"}),
    break_string("Deine Fuesse.", 78));
  AddDetail("phantasie",
    break_string("Sie schlummert in dir. Es ist an dir, ihr Gestalt "
                 "zu geben.", 78));

  AddExit("bsp","/doc/beispiele/misc/bspraum1");
  AddExit("ssp","/doc/beispiele/ssp/l1/m3x1");
  AddExit("zauberwald","/doc/beispiele/zauberwald/room/eingang");
  AddExit("gilde","/gilden/abenteurer");

#if defined(__TESTMUD__) || MUDHOST!=__HOST_NAME__
  ({"/doc/beispiele/misc/bspraum1",
    "/doc/beispiele/ssp/l1/m3x1",
    "/doc/beispiele/zauberwald/room/eingang"})->AddExit("konstrukt",
                                                  load_name(this_object()));
  call_out(#'add_testmud_gexits, 1);
#endif
}

protected string get_vcomp_possibility(mapping gdirectory) {
  foreach(string path, int tp: gdirectory) {
    if(tp>20) {
      string head = read_file(path);
      if(head && strstr(head, "compile_object")>=0) {
        return path;
      }
    }
  }
  return 0;
}

protected void get_dirs_and_files(mapping gdirs, mapping gfiles) {
  mapping gdirectory =
    mkmapping(get_dir("/gilden/", GETDIR_PATH|GETDIR_UNSORTED),
              get_dir("/gilden/", GETDIR_SIZES));
  m_delete(gdirectory, "/gilden/access_rights.c");

  gdirs = filter(gdirectory, function int(string path, int tp) {
                               if(tp == FSIZE_DIR)
                                 return 1;
                               return 0;
                             });
  gfiles = filter(gdirectory, function int(string path, int tp) {
                                if(tp > 0)
                                  return 1;
                                return 0;
                              });
}

protected void try_add_gexit(string realpath, string *added_exists) {
  object ob;
  if(member(added_exists, realpath)<0 &&
     !catch(ob=load_object(realpath); nolog) && ob &&
     member(inherit_list(ob), "/std/gilden_room.c")>=0) {
    AddExit(explode(realpath, "/")[<1][0..<3], realpath);
    ob->AddExit("konstrukt", load_name(this_object()));
  }
}

protected void add_testmud_gexits() {
  mapping gdirs, gfiles;
  get_dirs_and_files(&gdirs, &gfiles);

  string vcomp = get_vcomp_possibility(gfiles);
  m_delete(gfiles, vcomp);

  string *added_exists = allocate(0);
  foreach(string path, int tp: gfiles)
    try_add_gexit(path, &added_exists);

  if(vcomp)
    foreach(string path, int tp: gdirs)
      if(strstr(path, "files.")>0) {
        string realpath = implode(explode(path, "files."), "")+".c";
        try_add_gexit(realpath, &added_exists);
      }
}
