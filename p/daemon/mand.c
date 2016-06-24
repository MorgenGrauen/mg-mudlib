//
// mand.c - Manpage Daemon
//
// Erweitert und umgearbeitet fuer die Anbindung durch
// die Magiershell
//
// von Mandragon@MG
//
// Basiert auf der Arbeit von Hadra,Mupfel und Tyron
// (alle @Anderland)
//

#define DEBUGGER "mandragon"

#define DEBUG(x) if (find_player(DEBUGGER)) \
                         tell_object(find_player(DEBUGGER),x+"\n")


#pragma strict_types,save_types
#pragma no_clone,no_shadow

#include <daemon/mand.h>

private nosave mapping pages;
private nosave string *keys;
private nosave mapping pages_case;
private nosave string *keys_case;

static string glob2regexp(string str)
{
  str=regreplace(str,"([\\.\\^\\$\\[\\]\\(\\)])","\\\\\\1",1);
  str=regreplace(str,"\\*",".*",1);
  return regreplace("^"+str+"$","?",".",1);
}

void cache_entry(string entry, string dir, string file)
{
  string *data;
  int j;

  if (m_contains(&data,pages,lower_case(entry)))
  {
    j=sizeof(data);
    while(j--) data[--j]=data[j];
    pages[lower_case(entry)]=data+({ dir+"/"+file, dir+"/"+file});
  }
  else
  {
    pages[lower_case(entry)]=({entry,dir+"/"+file});
    keys+=({lower_case(entry)});
  }
  if (m_contains(&data,pages_case,entry))
  {
    j=sizeof(data);
    while(j--) data[--j]=data[j];
    pages_case[entry]=data+({ dir+"/"+file, dir+"/"+file});
  }
  else
  {
    pages_case[entry]=({entry,dir+"/"+file});
    keys_case+=({entry});
  }
  return;
}

static void load_synonyms(string dir)
{
  string filedata,*syn;

  if (file_size(MAND_DOCDIR+dir+"/.synonym")<=0) return;
  if (!(filedata=read_file(MAND_DOCDIR+dir+"/.synonym"))) return;
  syn=regexplode(filedata, "([ \t][ \t]*|\n)")-({""});
  syn-=regexp(syn,"^([ \t][ \t]*|\n)$");
  while(sizeof(syn))
  {
    cache_entry(syn[0],dir,syn[1]);
    syn=syn[2..];
  }
  return;
}

static void cache_directory(string dir)
{
  string file;
  int i,j;
  mixed *files;

  files = get_dir(MAND_DOCDIR+dir+"/*",3)||({});
  i=sizeof(files);
  for (j=0;j<i;)
  {
    file=files[j++];
    if (file[0]=='.')
    {
      j++;
      continue;
    }
    if (files[j++]==-2)
    {
      cache_directory(dir+"/"+file);
      continue;
    }
    cache_entry(file,dir,file);
  }
  load_synonyms(dir);
}

void update_cache()
{
  int i;
  string *docdirs;
  mixed *tmp;

  tmp=get_dir(MAND_DOCDIR "*",3);
  docdirs=({});
  pages=([]);
  pages_case=([]);
  keys_case=({});
  keys=({});
  i=sizeof(tmp);
  while(i--)
    if (tmp[i]==-2)
      docdirs+=({ tmp[(i--)-1] });
  docdirs-=({".",".."});
  docdirs-=MAND_EXCLUDE;
  i=sizeof(docdirs);
  while(i--) cache_directory(docdirs[i]);
  return;
}

void create()
{
  seteuid(getuid());
  update_cache();
  set_next_reset(10080); //Einmal pro Woche langt
  return;
}

void reset()
{
  update_cache();
  set_next_reset(10080);
  return;
}

//
// locate(): Manpage im Cache suchen:
// match = 1 -> glob-matching, match=2 -> regexp-matching
// Pruefung ob str gueltige regexp erfolgt in der Shell
//

string *locate(string str,int match)
{
  string *matches,*ret,str2;
  int i;

  if (!match&&member(keys_case,str)!=-1) return pages_case[str];
  str=lower_case(str);
  if (match==1) str=glob2regexp(str);
  if (match) matches=regexp(keys,str);
  else if (member(keys,str)!=-1) matches=({ str });
  else return ({});

  ret=({});
  i=sizeof(matches);
  while(i--)
    ret+=pages[matches[i]];
  return ret;
}
