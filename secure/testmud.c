// MorgenGrauen MUDlib
#pragma strong_types,save_types,rtt_checks
#pragma no_clone,no_inherit,no_shadow
#pragma pedantic, range_check

#include <files.h>
#include <config.h>
#include <new_skills.h>

// Variablen fuer das Savefile von Jof, was dieses Objekt schreibt.
string name = "jof";
string password = "";
int level = 100;
string shell = "/std/shells/magier";

private void generiere_gilden();

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
  generiere_gilden();
  master()->clear_cache();
  remove();
}

// Files zum Inherit aus Gilden- bzw. Spellbook-Repositories automatisch 
// erzeugen.
private void generiere_gilden()
{
  string* dir = get_dir("/gilden/",GETDIR_NAMES);
  string* search = ({
    "/gilden/files.%s/%s.c",
    "/gilden/files.%s/gilde.c",
    "/gilden/files.%s/room/%s.c",
    "/gilden/files.%s/room/gilde.c"});

  foreach(string entry : dir)
  {
    if(entry[0..4] != "files" || file_size("/gilden/"+entry[6..]+".c") > 0)
      continue;

    string file = entry[6..];
    // Zielpfad nullen, damit auffaellt, wenn kein Ziel gefunden wird.
    string path;
    foreach(string s : search)
    {
      if(file_size(sprintf(s, file, file)) > 0)
      {
        path = sprintf(s, file, file);
        break;
      }
    }

    if(sizeof(path))
    {
      write_file("/gilden/"+file+".c",
        "// Automatisch generierter Gilden-Inherit\n"
        "inherit \""+path+"\";\n\n"
        "protected void create()\n"
        "{\n"
        "  replace_program();\n"
        "}\n");
    }
  }

  // Bei Spellbooks sieht das leider etwas anders aus, daher ist hier eine
  // Codedoppelung der uebersichtlichere Weg.
  dir = get_dir("/spellbooks/", GETDIR_NAMES);
  search = ({
    "/spellbooks/%s/%s.c",
    "/spellbooks/%s/spellbook.c"});

  foreach(string entry : dir)
  {
    // Die Spellbooks heissen zwar alle unterschiedlich ... aber die Ordner
    // heissen wie die Gilde. Und die Gilde kennt ihr Spellbook.
    if(file_size("/spellbooks/"+entry) != FSIZE_DIR || 
      file_size("/gilden/"+entry+".c") == FSIZE_NOFILE)
    {
      continue;
    }

    string file;
    catch(file = ("/gilden/"+entry)->QueryProp(P_GUILD_DEFAULT_SPELLBOOK));
    if(!sizeof(file) || file_size("/spellbooks/"+file+".c") > 0) continue;

    string path;
    foreach(string s : search)
    {
      if(file_size(sprintf(s, entry, file)) > 0)
      {
        path = sprintf(s, entry, file);
        break;
      }
    }

    if(sizeof(path))
    {
      write_file("/spellbooks/"+file+".c",
        "// Automatisch generierter Spellbook-Inherit\n"
        "inherit \""+path+"\";\n\n"
        "protected void create()\n"
        "{\n"
        "  replace_program();\n"
        "}\n");
    }
  }
}
