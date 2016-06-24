// MorgenGrauen MUDlib
//
// master.c -- master object
//
// $Id: master.c 7041 2008-10-13 18:18:27Z Zesstra $
#pragma strict_types
#pragma no_clone
#pragma no_shadow
#pragma verbose_errors
#pragma combine_strings
#pragma pedantic
#pragma range_check
#pragma warn_deprecated

#define WHITELIST "/secure/ARCH/players_deny_whitelist.o"
#define TARGETLIST "/secure/ARCH/players_deny_targets.o"
#define WHITELISTDUMP "/secure/ARCH/players_deny_whitelist.dump"
#define TARGETLISTDUMP "/secure/ARCH/players_deny_targets.dump"

//#define PLDENY_LEARNMODE

#include "/secure/wizlevels.h"
#include "/secure/master.h"

/* Diese Objekte duerfen aus /players/ includieren/erben */
nosave private mapping whitelist;
/* Diese Dinge werden von Objektn in whitelist aus /players/ inkludiert/geerbt. */
nosave private mapping targets;

private mapping ParseList(string list) {
  mixed data;

  if (!stringp(list)) return ([:0 ]);
  if (!stringp(data = read_file(list))) return ([:0 ]);
  data = explode(data, "\n") || ({});

  return mkmapping(data);
}

private void ParseWhiteList() {
  DEBUG("Parsing Whitelist\n");
  whitelist=ParseList(WHITELISTDUMP);
}
private void ParseTargetList() {
  DEBUG("Parsing Targetlist\n");
  targets=ParseList(TARGETLISTDUMP);
}

private void DumpList(mapping list, string file) {
  if (!stringp(file) || !mappingp(list)
      || !sizeof(file) || !sizeof(list))
    return;
  
  write_file(file, implode(m_indices(list),"\n"), 1);
}

private void DumpWhiteList() {
  DEBUG("Dumping Whitelist\n");
  DumpList(whitelist, WHITELISTDUMP);
}
private void DumpTargetList() {
  DEBUG("Dumping Targetlist\n");
  DumpList(targets, TARGETLISTDUMP);
}

public void DumpPLDenyLists() {
  if ( extern_call() && call_sefun("secure_level") < ARCH_LVL )
    return;
  DEBUG("Dumping PLDenylists\n");
  limited(#'DumpWhiteList);
  limited(#'DumpTargetList);
}

public void SavePLDenyLists() {
  if ( extern_call() && call_sefun("secure_level") < ARCH_LVL )
    return;
  DEBUG("Saving PLDenyLists\n");
  write_file(WHITELIST,
      save_value(whitelist), 1);
  write_file(TARGETLIST,
      save_value(targets), 1);
}

public void LoadPLDenylists() {
  mixed tmp;

  if ( extern_call() && call_sefun("secure_level") < ARCH_LVL )
    return;

  write("Loading PLDenylists\n");

  if (stringp(tmp=read_file(WHITELIST))) {
    // savefile exists
    whitelist = restore_value(tmp);
  }
  else {
      limited(#'ParseWhiteList);
  }
  if (stringp(tmp=read_file(TARGETLIST))) {
    targets = restore_value(tmp);
  }
  else {
      limited(#'ParseTargetList);
  }
}

mixed include_file(string file, string compiled_file, int sys_include) {
  
  if (sys_include) return 0;
  // Loggen, wenn Files ausserhalb /players/ Kram aus /players/
  // inkludieren/erben.
  if (strstr(file, "/players/") == 0
      && (strstr(compiled_file,"/players/") == -1
      && !member(whitelist, compiled_file) )) {
#ifdef PLDENY_LEARNMODE
    DEBUG("include_file(): Whitelisting: "+compiled_file+"\n");
    m_add(whitelist, compiled_file);
    m_add(targets, file);
    call_sefun("log_file", "PLAYERSWHITELIST",
        sprintf("%s (inkludiert %s)\n",compiled_file, file),
        1000000);
#else
    // verweigern.
    return -1;
#endif
  }
  return 0;
}

mixed inherit_file(string file, string compiled_file) {

  // Loggen, wenn Files ausserhalb /players/ Kram aus /players/
  // inkludieren/erben.
  if (strstr(file, "/players/") == 0
      && (strstr(compiled_file,"/players/") == -1
      && !member(whitelist, compiled_file) )) {
#ifdef PLDENY_LEARNMODE
    DEBUG("include_file(): Whitelisting: "+compiled_file+"\n");
    m_add(whitelist, compiled_file);
    m_add(targets, file);
    call_sefun("log_file", "PLAYERSWHITELIST", 
        sprintf("%s (erbt %s)\n",compiled_file, file),
        1000000);
#else
    // verweigern.
    return -1;
#endif
  }
  return 0;
}

