// MorgenGrauen MUDlib
//
// master/domain.c -- Regionen und Regionsmagier
//
// $Id: domain.c 7162 2009-02-26 21:14:43Z Zesstra $

#pragma strict_types

#include "/secure/master.h"
#include "/sys/files.h"

int domain_master(string user, string domain)
{
  string *domains;
  int i;
  
  if (!find_userinfo(user)||
      !pointerp(domains=get_userinfo(user)[USER_DOMAIN+1]))
    return 0;
  return (member(domains,domain) != -1);
}

int domain_member(string user, string domain)
{
  if (domain=="erzmagier") return 0;
  return (IS_DOMAINMEMBER(user)&&file_size("/d/"+domain+"/"+user)==-2);
}

int add_domain_master(string user,string dom)
{
  string *domains;
  
  if ((call_other(SIMUL_EFUN_FILE, "process_call") ||
        call_other(SIMUL_EFUN_FILE, "secure_level") < GOD_LVL) ||
        !find_userinfo(user))
    return 0;
  domains=get_userinfo(user)[USER_DOMAIN+1];
  if (!domains) 
    set_domains(user,({dom}));
  else
  {
    // Doppelte Eintraege vermeiden
    domains=domains-({dom})+({dom});
    set_domains(user, domains);
  }
  if (get_wiz_level(user) < LORD_LVL+5) {
    update_wiz_level(user,LORD_LVL+5);
    SendWizardHelpMail(user, LORD_LVL+5);
  }
  return 1;
}

int remove_domain_master(string user,string dom)
{
  string *domains;

  if (!IS_GOD(geteuid(previous_object()))
      ||!find_userinfo(user)
      ||!(domains=get_userinfo(user)[USER_DOMAIN+1])
      || member(domains,dom)==-1)
    return 0;
  domains-=({dom});
  set_domains(user,domains);
  return 1;
}

string *get_domains() {
  string *regions=({});
  // alle Verzeichnisse in /d/ ermitteln
  foreach(string region: (get_dir("/"DOMAINDIR"/*") 
        || ({}))-({".","..","erzmagier"})) {
    if (region[0]!='.' && file_size("/d/"+region) == FSIZE_DIR)
        //Verzeichnis, kein File und faengt nicht mit "." an: aufnehmen.
        regions+=({region});
  }
  return regions;
}

// in welchen Regionen hat 'wiz' ein Verzeichnis?
string *get_domain_homes(string wiz)
{
  string *homes=({});
  string tmp;
  
  if (query_wiz_level(wiz)<=WIZARD_LVL) return ({});

  tmp = "/d/%s/"+wiz;
  foreach(string dir: get_domains()) {
      if (dir[0]!='.' && file_size(sprintf(tmp,dir)) == FSIZE_DIR)
          //Magierverzeichnis da und faengt nicht mit "." an: aufnehmen.
          homes+=({dir});
  }
  return homes;
}

