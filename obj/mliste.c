inherit "/std/thing/properties";

#include <defines.h>
#include <properties.h>
#include <sys_debug.h>

#define EM_DATA   "/etc/Erzmagier"
#define MAGIER    "/etc/Magierliste"
#define MASTER    "/secure/master"
#define WIZ_HOME  "/players"

#define REM   0   // RegionsErzMagier
#define MAG   1   // Normale Magier

private mapping domains, build_domains;
private int Finished, today;
void MagierListe();

private mapping _query_Magierliste() { return domains; }

void create()
{
  ::create();
  seteuid(getuid());
  Set("Magierliste", #'_query_Magierliste/*'*/, F_QUERY_METHOD);
  call_out("reset", 0);
}

void reset()
{
  if (today >= time()/86400)
    return;

  today=time()/86400;
  MagierListe();
}

void ProcessList(mixed names)
{
  mixed Dname;
  int i;
  i = 5;

  while(i-- && sizeof(names))
  {
    if(!pointerp(Dname = MASTER->get_domain_homes(names[0])))
      Dname = ({Dname});
    if(!sizeof(Dname)) Dname = ({"Keine Region"});
    while(sizeof(Dname))
    {
      if(!build_domains[Dname[0]])
      {
        build_domains[Dname[0], REM] = ({});
        build_domains[Dname[0], MAG] = ({});
      }
      if(Dname[0]!="Keine Region" && MASTER->domain_master(names[0], Dname[0]))
        build_domains[Dname[0], REM] += ({names[0]});
      else
        build_domains[Dname[0], MAG] += ({names[0]});
      Dname = Dname[1..];
    }
    names = names[1..];
  }
  if(sizeof(names)) call_out("ProcessList", 0, names);
  else { Finished = 1; MagierListe(); }
}

void MagierListe()
{
  string output;
  mixed d;

  if(!Finished)
  {
    if(find_call_out("ProcessList") == -1)
    {
      Finished = 0;
      build_domains = m_allocate(0,2);
      call_out("ProcessList", 0, get_dir(WIZ_HOME+"/*") - ({".", "..", ".readme"}));
    }
    call_out("MagierListe", 20, 1);
    return;
  }
  while(remove_call_out("Magierliste") !=  -1);

  if (!build_domains)
    return;

  output = "Magierliste von MorgenGrauen vom "+dtime(time())+"\n";

  domains = deep_copy(build_domains);
  build_domains=0;
  for(d = m_indices(domains); sizeof(d); d = d[1..])
  {
    output += "REGION: "+capitalize(d[0])
             +" REM: "
             +implode(map(domains[d[0], REM], #'capitalize), ", ")
             +" ("+sizeof(domains[d[0], REM]+domains[d[0], MAG])
             +" Magier)\n"
             + break_string(implode(map(domains[d[0], MAG],#'capitalize),
                                    ", "), 62, 8)
             +"\n";
  }
  rm(MAGIER);
  write_file(MAGIER, output);
}
