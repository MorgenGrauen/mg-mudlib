// MorgenGrauen MUDlib
//
// secure/master/autoinclude.c -- module of the master object: Autoincludes
//
// $Id: master.c 7041 2008-10-13 18:18:27Z Zesstra $

#define PRAGMA(x) "#pragma "x"\n"

#define RTTCHECKS PRAGMA("rtt_checks")
#define DEFAULTS PRAGMA("save_types,warn_unused_variables,\
    warn_applied_functions,warn_dead_code")

// geschachteltes Mapping in toplevel.region.magier Hierarchie.
// Wichtig: jede Hierarchiebene _muss_ ein Mapping sein, welches einen Eintrag
//          0 als Default enthaelt, welcher einen Strings als Wert hat.
//          Ausnahme: die letzte Ebene (Magierebene), die muss ein String ein.
private nosave mapping autoincludes = ([
    "d":      ([
                 "inseln": ([
                             0: "",
                             "zesstra": PRAGMA("strong_types") RTTCHECKS,
                            ]),
                 0: "",
               ]),
    "std":    ([
                 0: PRAGMA("strong_types") RTTCHECKS,
               ]),
    "items":    ([
                 0: PRAGMA("strong_types") RTTCHECKS,
               ]),
    "secure": ([
                 0: PRAGMA("strong_types,range_check") RTTCHECKS,
               ]),
    "p":      ([
                 0: "",
                 "daemon": ([
                             0: PRAGMA("strong_types") RTTCHECKS
                            ]),
                 "service": ([
                              0: ""
                            ]),
               ]),
    0: DEFAULTS,
]);

string autoincludehook(string base_file, string current_file, int sys_include)
{
  mapping topleveldir, region; // mappings for spezialisiertere Pfade
  string ainc_string;

  // Wenn current_file != 0 ist, wird gerade vom kompilierten Objekt
  // <base_file> etwas (indirekt) inkludiert. Dort duerfen die Pragmas
  // keinesfalls reingeschrieben werden.
  if (current_file)
    return 0;

  string res=autoincludes[0]; // global default.

  string *p_arr = explode(base_file,"/")-({""});
  //DEBUG(sprintf("AINC: File: %O, Pfad: %O\n",base_file, p_arr));

  if (sizeof(p_arr) && m_contains(&topleveldir, autoincludes, p_arr[0])) {
    // p_arr[0]: d, p, std, etc.
    // erst wird der Defaulteintrag 0 genommen
    res += topleveldir[0];
    if (sizeof(p_arr) > 1 && m_contains(&region, topleveldir, p_arr[1])) {
      // p_arr[1] ebene, polar, unterwelt, service, ...
      // erst den Defaulteintrag der Region nehmen
      res += region[0];
      if (sizeof(p_arr) > 2 && m_contains(&ainc_string, region, p_arr[2])) {
        // p_arr[2]: magiername. Fuer den Magier was hinterlegt.
        res += ainc_string;
      }
    }
    // Fuer einige Top-Level-Verzeichnisse sollen explizit eingeschaltete
    // Pragmas auch fuer alte Files nicht ausgeschaltet werden. In dem Fall
    // direkt beenden.
    switch(p_arr[0])
    {
      case "std":
      case "secure":
      case "room":
      case "obj":
      case "items":
        return res;
    }
  }
  // Fuer aeltere Files schalten wir einige Warnungen explizit aus. :-(
  // (1407179680 == "Mon,  4. Aug 2014, 21:14:40")
  // (1609455600 == "Fre,  1. Jan 2021, 00:00:00")
  // Auf anderen Rechnern als dem Mudrechner werden die Warnungen nur
  // eingeschaltet, wenn die Files seit dem Mudstart geaendert wurden.
  // Hintergrund: bei kopierten Mudlibs werden oft die mtimes geaendert und
  // dann scrollt auf einmal alles. Aber wenn man nach dem Mudstart was
  // aendert, ist es vermutlich ne einzelne, gezielte Aenderung an einem File.
#if MUDHOST == __HOST_NAME__
  int ftime = call_sefun("file_time", base_file);
  if (ftime < 1407179680)
  {
      res += PRAGMA("no_warn_missing_return, no_warn_unused_variables\
                     no_warn_dead_code");
  }
  else if (ftime < 1609455600)
  {
      res += PRAGMA("no_warn_unused_variables,no_warn_dead_code");
  }
#else
  if (call_sefun("file_time", base_file) < __BOOT_TIME__)
  {
      res += PRAGMA("no_warn_missing_return,no_warn_unused_variables\
                     no_warn_dead_code");
  }
#endif
  //DEBUG(res);
  return res;
}
#undef RTTCHECKS
#undef DEFAULTS
#undef PRAGMA


