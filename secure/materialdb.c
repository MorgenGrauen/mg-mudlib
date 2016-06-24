// MorgenGrauen MUDlib
//
// - Prototypen und Properties in thing/material.h
// - Liste in materials.h
//
// TODO: properties.h um materials.h erweitern
//
// - implizite Gruppenzuordnung entfernen, da jetzt explizit in
//   Definitionsdateien vorhanden
// - Materialdoku ueberarbeiten, dabei Hinweis auf nicht mehr implizite
//   Gruppenzuordnung
// /p/daemon/materialdb.c -- Materialdatenbank
//
// $Id: materialdb.c 8755 2014-04-26 13:13:40Z Zesstra $

#pragma strong_types
#pragma no_clone
#pragma no_inherit
#pragma no_shadow
#pragma pedantic

// Die Propertydefinition reinholen
#include <language.h>
#include <thing/description.h>
// Materialliste nicht mit reinziehen
#define _SKIP_MATERIALS_
#include <thing/material.h>
#include <player/description.h>
#include <rtlimits.h>

// Basisverzeichnis der Materialien
#define MAT_DIR "/doc/materials"

// Ausgabeverzeichnis fuer Doku
#define DOC_DIR(x) ("/doc/materials/"+x)

// Dateiname des Headers mit Materialdefinitionen
#define HEADERFILE "/sys/materials.h"

// Savefile.
#define SAVEFILE DOC_DIR("materialdb")

// Rein intern verwendete Namen
#define P_RECOC "recognizability"
#define P_ID    "id"
#define P_DEFSTR "defstr"
#define P_MEMBERS "members"
#define P_MG_FRACTIONS "mg_fractions"

#define LOG_ERROR(x) if(find_player("raschaua")&&find_player("raschaua")->QueryProp("mdb-debug"))tell_object(find_player("raschaua"),"MDB-Error:"+x)
#define LOG_WARN(x) if(find_player("raschaua")&&find_player("raschaua")->QueryProp("mdb-debug"))tell_object(find_player("raschaua"),"MDB-Warn:"+x)

// Prototypes:
// (Liefert den Anteil der Materialgruppe grp an mats)
int MaterialGroup(mapping mats, string grp);
// Konvertiert eine Liste von Materialien zu ihren Namen, dabei wird die
// Erkennungsfaehigkeit beruecksichtigt und evtl. falsch erkannt
varargs string ConvMaterialList(mixed mats, int casus, mixed idinf);
varargs string MaterialName(string mat, int casus, mixed idinf);
// Gibt den Namen einer Materialgruppe zurueck
string GroupName(string grp);
// Gibt alle Materialien zurueck
string *AllMaterials();
// Gibt alle Gruppen zurueck
string *AllGroups();
// Gibt alle Gruppen zurueck, in denen mat enthalten ist
string *GetMatMembership(string mat);
// Gibt alle Materialien zurueck, die in grp enthalten sind
string *GetGroupMembers(string grp);
// Erneuert die Materialien durch Scannen des Materialverzeichnisses
void Update();
// generiert Headerfile aus den Daten
varargs void GenHeaderFile(string fn);

mapping materials;
mapping material_groups;
private status initialized;
mapping old_mat_keys;     // Alter Materialkey -> neuer Key (Kompatibilitaet)
nosave mapping new_materials;       // Materialien waehrend des Scannens
nosave mapping new_material_groups; // Materialgruppen waehrend des Scannens
private nosave status isScanning;
private nosave int updateTicks;

void create() {
  seteuid(getuid());
  // Savefile einlesen, falls moeglich, damit die DB direkt initialisert ist,
  // wenn auch ggf. mit alten Daten.
  restore_object(SAVEFILE);
  if (initialized) {
    // falls erfolgreich, direkt Header fuer die Mudlib schreiben
    GenHeaderFile();
  }
  // jetzt Update der Daten durchfuehren.
  Update();
}

//==================== Umwandeln der Strings aus der thing/material.h
private string getMatId(string key) {
  // Alte Bezeichner umwandeln
  if (!member(materials, key))
    key = old_mat_keys[key];
  return key;
}
private string getMatGroupId(string key) {
  // Alte Bezeichner umwandeln
  if (!member(material_groups, key))
    key = "MATGROUP_"+upperstring(key[3..]);
  return key;
}
private string matKey2Defstr(string key, mapping mats) {
  string id;
  if (member(mats[key], P_DEFSTR))
    id = mats[key][P_DEFSTR];
  else
    id = key;
  return id;
}
private string groupKey2Defstr(string key) {
  if (sizeof(key) > 9)
    key = "mg_"+lowerstring(key[9..]);
  else
    key = "";
  return key;
}

//==================== Schnittstellenfunktionen zur Verwendung der DB
varargs string MaterialName(string mat, int casus, mixed idinf) {
  if (initialized) {
    string *names;
    mapping props;
    mixed *dif;
    // Anpassen der Materialid
    mat = getMatId(mat);

    if (!mappingp(props=materials[mat]))
      props=([]);

    // Je nach Koennen des Spielers kann man das exakte Material
    // mehr oder weniger gut erkennen:
    if (pointerp(dif=props[P_RECOC])
&& (!intp(idinf)||idinf<100) ) { // 100=exakte Erkennung
      int i, n, recval;
      mixed *grps, tmp, x;

      recval=0;
      grps=props[P_MG_FRACTIONS];
      if (!pointerp(idinf))
        idinf=({idinf});

      // Zunaechst die Faehigkeit des Spielers (da koennen noch
      // Gildenfaehigkeiten hinzu kommen) ermitteln, dieses
      // Material zu erkennen:
      i=sizeof(idinf);
      while(i--) {
        tmp=idinf[i];
        if (objectp(tmp)) // Diese Property ist hauptsaechlich fuer Rassen:
          tmp=tmp->QueryProp(P_MATERIAL_KNOWLEDGE);
        if (intp(tmp)) {
          recval+=tmp; // Allgemeine Erkennungsfaehigkeit
          break;
        }
        if (closurep(tmp) && intp(x=funcall(tmp,mat,grps))) {
          recval+=x;
          break; // Closures koennen immer nuetzlich sein :)
        }
        if (mappingp(tmp)) {
          int j;
          if ((x=tmp[mat]) && intp(x)){
            // Erkennung von speziell diesem Material
            recval+=x;
            break;
          }
          // Erkennung von Gruppen
          j=sizeof(grps);
          while(j--)
            if((x=tmp[grps[j]]) && intp(x))
              recval+=x;
          if (pointerp(tmp=tmp[MATERIAL_SYMMETRIC_RECOGNIZABILITY])) {
            for (j=sizeof(tmp)-2;j>=0;j-=2) {
              if (!intp(x=tmp[j+1]))
                raise_error("materialdb: illegal sym.recoc. format\n");
              if (props[tmp[j]])
                recval+=x;
              else // bei passenden Gruppen +, bei anderen -
                recval-=x;
            }
          }
        }
      }

      // Jetzt wird ermittelt, ob vielleicht eine ungenauere
      // Beschreibung gegeben werden soll:
      x=dif[0];
      n = sizeof(dif)-1;
      for (i=2;i<=n;i+=2) {
        if (recval>=dif[i-1])
          x=dif[i];
      }
      // Wenn die Faehigkeiten des Spielers nicht fuer den echten Klarnamen
      // ausreichen, gib die Alternative zurueck:
      if (x!=mat)
        return MaterialName(x, casus, 100);
    }

    if (!pointerp(names=props[P_NAME]) || sizeof(names)<4)
      names=({"unbekanntes Material", "unbekannten Materials",
              "unbekanntem Material", "unbekannten Material"});
    if (casus<0 || casus>3)
      casus=0;
    return names[casus];
  }
}

varargs string ConvMaterialList(mixed mats, int casus, mixed idinf) {
  if (initialized) {
    string *ms,ml;
    int i;

    ml="";
    if (mappingp(mats))
      ms=m_indices(mats);
    else if (stringp(mats))
      ms=({mats});
    else if (pointerp(mats))
      ms=mats;
    else
      ms=({});
    i=sizeof(ms);
    while(i) {
      ml+=MaterialName(ms[--i],casus,idinf);
      if (i)
        ml+=((i>1)?", ":" und ");
    }
    return ml;
  }
}

int MaterialGroup(mapping mats, string grp) {
   if (initialized) {
     string *ms;
     int i,res;

     res=0;
     if (!mappingp(mats) || !stringp(grp))
       return res;
     ms=m_indices(mats);
     i=sizeof(ms);
     while(i--) {
       string mat;
       mapping props;
       mat=ms[i];
       if (mappingp(props=materials[getMatId(mat)]))
         res+=(mats[mat]*props[P_MG_FRACTIONS][getMatGroupId(grp)])/100;
     }
     if (res<-100) // Vielleicht noch Antimaterie zulassen
       res=-100;   // (noch nicht sicher ob das so bleiben wird oder 0 sein wird)
     if (res>100)
       res=100;
     return res;
   }
}

string *AllMaterials() {
  if (initialized) {
    // Aus Kompatibilitaetsgruenden die alten Schluessel (#define-String)
    // zurueckgeben
    return m_indices(old_mat_keys);
  }
  return 0;
}

string *AllGroups() {
  if (initialized) {
    // Aus Kompatibilitaetsgruenden die alten Schluessel (#define-String)
    // zurueckgeben
    return map(m_indices(material_groups), #'groupKey2Defstr);
  }
  return 0;
}

string *GetMatMembership(string mat) {
  if (initialized) {
    mapping props;
    // Anpassen der Materialid
    mat = getMatId(mat);

    if (!mappingp(props=materials[mat]))
      return ({});
    return map(m_indices(props[P_MG_FRACTIONS]), #'groupKey2Defstr);
  }
  return 0;
}

string *GetGroupMembers(string grp) {
  if (initialized) {
    string *mats;
    // Anpassen der Materialid
    grp = getMatGroupId(grp);
    if (!member(material_groups, grp) ||
	!pointerp(mats=material_groups[grp][P_MEMBERS]))
      return ({});
    return map(mats, #'matKey2Defstr, materials);
  }
  return 0;
}

string GroupName(string grp) {
  if (initialized) {
    if (member(material_groups, getMatGroupId(grp)))
      return material_groups[getMatGroupId(grp)][P_NAME];
    else
      return "Unbekanntes";
  }
}

string GroupDescription(string grp) {
  if (initialized) {
    if (member(material_groups, getMatGroupId(grp)))
      return material_groups[getMatGroupId(grp)][P_DESCRIPTION];
    else
      return "Gruppe unbekannt";
  }
}

//==================== Generieren von Headerfile und Manpages
private string *get_ordered_groups()
{
  return ({"MATGROUP_WOOD", "MATGROUP_JEWEL", "MATGROUP_STONE", "MATGROUP_MAGNETIC",
           "MATGROUP_METAL", "MATGROUP_DRUG", "MATGROUP_HERBAL", "MATGROUP_FLEXIBLE",
           "MATGROUP_BIO", "MATGROUP_ACIDIC", "MATGROUP_BASIC", "MATGROUP_POISONOUS",
           "MATGROUP_EXPLOSIVE", "MATGROUP_INFLAMMABLE",
           "MATGROUP_ELEMENTAL", "MATGROUP_ELECTRICAL", "MATGROUP_MAGIC",
           "MATGROUP_HOLY", "MATGROUP_UNHOLY", "MATGROUP_INVIS",
           "MATGROUP_SOLID", "MATGROUP_FLUID", "MATGROUP_GAS"});
}
private string gen_material_h_head()
{
  return
    "// MorgenGrauen MUDlib\n//\n"
    "// materials.h -- material definitions\n//\n"
    "// This file is generated by /secure/materialdb.c\n//\n"
    "// DO NOT EDIT!\n//\n"
    "// $Id: materialdb.c 8755 2014-04-26 13:13:40Z Zesstra $\n\n"
    "#ifndef __MATERIALS_H__\n"
    "#define __MATERIALS_H__\n\n";
}
private string gen_material_h_material(string mat, string last_grp)
{
  mat = old_mat_keys[mat];
  return sprintf("#define %-24s\"%-20s // %s\n", mat,
                (member(materials[mat], P_DEFSTR)?materials[mat][P_DEFSTR]:mat)+"\"",
                materials[mat][P_DESCRIPTION]||materials[mat][P_NAME][WER]);
}
private string gen_material_h_materials_grp(string grp, string *left)
{
  string txt, *mats;
  txt = sprintf("\n// Gruppe: %s\n", GroupName(grp));
  mats = GetGroupMembers(grp) - (GetGroupMembers(grp) - left);
  txt += sprintf("%@s", map(sort_array(mats, #'>), #'gen_material_h_material));
  left -= GetGroupMembers(grp);
  return txt;
}
private string gen_material_h_materials()
{
  string txt, last_grp;
  string *grps, *mats;
  txt = "// ****************************** Materialien ******************************\n";
  // Gruppenweise ordnen
  grps = get_ordered_groups();
  mats = AllMaterials();
  txt += sprintf("%@s", map(grps, #'gen_material_h_materials_grp,
                                  &mats));
  // Übriggebliene Materialien ausgeben
  txt += "// sonstige Materialien:\n";
  txt += sprintf("%@s", map(mats, #'gen_material_h_material));
  return txt;
}
private string gen_material_h_group(string grp)
{
  return sprintf("#define %-27s\"%-18s // %s\n",
                 grp, groupKey2Defstr(grp)+"\"", GroupName(grp));
}
private string gen_material_h_groups()
{
  string txt;
  txt = "\n// **************************** Materialgruppen ****************************\n\n"
    "#ifndef _IS_MATERIALDB_\n";
  txt += sprintf("%@s\n", map(sort_array(m_indices(material_groups), #'>),
                                    #'gen_material_h_group));
  txt += "\n#endif // _IS_MATERIALDB_\n";
  return txt;
}
private string gen_material_h_foot()
{
  return
    "#endif // __THING_MATERIAL_H__\n";
}
private int dump_material_h(string fn)
{
  return (write_file(fn, gen_material_h_head()) &&
          write_file(fn, gen_material_h_materials()) &&
          write_file(fn, gen_material_h_groups()) &&
          write_file(fn, gen_material_h_foot()));
}
private string gen_material_list_material(string mat)
{
  mat = old_mat_keys[mat];
  return sprintf("  %-28s%=-45s\n", mat,
                 materials[mat][P_DESCRIPTION]||materials[mat][P_NAME][WER]);
}
private string gen_material_list_materials_grp(string grp, string *left)
{
  string txt, *mats;
  txt = sprintf("%s:\n", capitalize(GroupName(grp)));
  mats = sort_array(GetGroupMembers(grp) - (GetGroupMembers(grp) - left), #'>);
  txt += sprintf("%@s\n", map(mats, #'gen_material_list_material));
  left -= GetGroupMembers(grp);
  return txt;
}
private void dump_material(string fn)
{
  string txt;
  string *grps, *mats;
  // Gruppenweise ordnen
  grps = get_ordered_groups();
  mats = AllMaterials();
  txt = sprintf("%@s", map(grps, #'gen_material_list_materials_grp,
                                 &mats));
  // Übriggebliene Materialien ausgeben
  txt += "sonstige Materialien:\n";
  txt += sprintf("%@s", map(mats, #'gen_material_list_material));
  write_file(fn, txt) ||
    raise_error(sprintf("Konnte Liste nicht weiter in Datei %s schreiben,"
                        " Abbruch\n", fn));
}
private void dump_group(string grp, string fn)
{
  // upperstring langsame simul_efun, warum?
  write_file(fn, sprintf("  %-28s%=-48s\n", (grp),
                         GroupName(grp))) ||
    raise_error(sprintf("Konnte Liste nicht weiter in Datei %s schreiben,"
                        " Abbruch\n", fn));
}
private string gen_doc_foot(string other)
{
  return sprintf("\nSIEHE AUCH:\n"
                 "     Konzepte:    material, materialerkennung\n"
                 "     Grundlegend: P_MATERIAL, /sys/materials.h, /sys/thing/material.h\n"
                 "     Methoden:    QueryMaterial(), QueryMaterialGroup(), MaterialList(),\n"
                 "     Listen:      AllMaterials(), AllGroups()\n"
                 "                  %s\n"
                 "     Master:      ConvMaterialList(), MaterialGroup(),\n"
                 "                  GroupName(), MaterialName(),\n"
                 "                  GetGroupMembers(), GetMatMembership()\n"
                 "     Sonstiges:   P_MATERIAL_KNOWLEDGE\n\n"
                 "%s generiert aus /secure/materialdb\n", other, dtime(time()));
}

/* GenMatList
 *
 * Generiert Datei mit registrierten Materialien fuer die Dokumentation,
 */
varargs void GenMatList(string fn)
{
  if (initialized) {
    string txt;
    if (!stringp(fn) || !sizeof(fn))
      fn = DOC_DIR("materialliste");
    if (file_size(fn) >= 0) {
      printf("Datei %s existiert bereits, loesche sie\n", fn);
      rm(fn);
    }
    if (write_file(fn, "Material Liste\n==============\n\n")) {
      dump_material(fn);
      write_file(fn, gen_doc_foot("materialgruppen"));
      printf("Materialliste erfolgreich in Datei %s geschrieben\n", fn);
    } else
      printf("Konnte Liste nicht in Datei %s schreiben, Abbruch\n", fn);
  }
}

/* GenMatGroupList
 *
 * Generiert Datei mit registrierten Materialgruppen fuer die Dokumentation,
 */
varargs void GenMatGroupList(string fn)
{
  if (initialized) {
    string txt;
    if (!stringp(fn) || !sizeof(fn))
      fn = DOC_DIR("materialgruppen");
    if (file_size(fn) >= 0) {
      printf("Datei %s existiert bereits, loesche sie\n", fn);
      rm(fn);
    }
    if (write_file(fn, "Materialgruppen\n===============\n")) {
      map(sort_array(m_indices(material_groups), #'>), #'dump_group, fn);
      write_file(fn, gen_doc_foot("materialliste"));
      printf("Materialliste erfolgreich in Datei %s geschrieben\n", fn);
    } else
      printf("Konnte Liste nicht in Datei %s schreiben, Abbruch\n", fn);
  }
}

/* GenHeaderFile
 *
 * Generiert Headerfile mit Definitionen der moeglichen Materialien und
 * Gruppen
 */
varargs void GenHeaderFile(string fn)
{
  if (initialized) {
    string txt;
    if (!stringp(fn) || !sizeof(fn))
      fn = HEADERFILE;
    if (file_size(fn) >= 0) {
      printf("Datei %s existiert bereits, loesche sie\n", fn);
      rm(fn);
    }
    if (dump_material_h(fn))
      printf("Headerdatei erfolgreich in %s geschrieben\n", fn);
    else
      printf("Konnte Headerdatei nicht in Datei %s schreiben, Abbruch\n", fn);
  }
}

//==================== Pruef- und Hilfsfunktionen fuer Materialien
private void updateGroupMembers(mapping groups, string mat_id, mapping mat) {
  mixed *addgrps; // Array zum Ableiten von Gruppenzugehoerigkeiten
  string *h;
  int i, val;
  mapping fractions; // Mapping mit Anteilen an Gruppen
  fractions = mat[P_MG_FRACTIONS];
  if (!mappingp(fractions))
    fractions = ([]);
  addgrps=({ // Reihenfolge wird rueckwaerts abgearbeitet
    // Ableitungen sind z.T. abenteuerlich gewesen, mal ordentlich
    // ausmisten. Die Zugehoerigkeit gehoert explizit in die
    // Materialdefinition
    // Gase sieht man normalerweise nicht:
    ({"MATGROUP_INVIS", "MATGROUP_GAS"}),
    // Mineralien sind auch Steine
    ({"MATGROUP_STONE","MATGROUP_MINERAL"}),
    // Edelmetalle sind Metalle:
    ({"MATGROUP_METAL","MATGROUP_PRECIOUS_METAL"}),
    // Lebewesen und deren Ueberreste, Paiere und Stoffe sind biologisch
    ({"MATGROUP_BIO","MATGROUP_LIVING","MATGROUP_DEAD",
      "MATGROUP_PAPER"}),
    // Holz ist pflanzlich:
    ({"MATGROUP_HERBAL", "MATGROUP_WOOD"}),
    // Holz ist meistens tot:
    ({"MATGROUP_DEAD","MATGROUP_WOOD"}),
    // Holz, Papier und Stoffe brennen:
    ({"MATGROUP_INFLAMMABLE","MATGROUP_WOOD","MATGROUP_PAPER"}),
    // Laubhoelzer, Nadelhoelzer und Tropenhoelzer sind Holz
    ({"MATGROUP_WOOD","MATGROUP_TROPICAL_WOOD","MATGROUP_DECIDUOUS_WOOD",
      "MATGROUP_CONIFER_WOOD"}),
    // Explosive Dinge sind immer entzuendlich:
    ({"MATGROUP_INFLAMMABLE","MATGROUP_EXPLOSIVE"})
  });
  i=sizeof(addgrps);
  while(i--) {
    int j;
    h=addgrps[i];
    if (member(fractions,h[0])) // Existiert schon eigener Eintrag?
      continue; // Automatische Eintragung unnoetig
    val=0;
    for (j=sizeof(h)-1;j>=1;j--)
      val+=fractions[h[j]];
    if (!val)
      continue;
    if (val>100)
      val=100;
    else if (val<-100)
      val=-100;
    fractions[h[0]]=val;
  }
  if (fractions["MATGROUP_LIVING"]) // Im Falle von lebendem Holz, tot loeschen
    m_delete(fractions,"MATGROUP_DEAD");
  // Alles, was nicht als gasfoerming, fluessig oder fest eingeordnet ist, ist
  // sonstwas:
  if (!member(fractions, "MATGROUP_FLUID")
      && !member(fractions, "MATGROUP_GAS")
      && !member(fractions, "MATGROUP_SOLID"))
    fractions["MATGROUP_MISC"]=100;
  // Materialien als Mitglieder in die Gruppen eintragen
  addgrps=m_indices(fractions);
  i=sizeof(addgrps);
  while(i--) {
    mixed ind;
    ind=addgrps[i];
    if (!fractions[ind] || !member(groups, ind)) {
      // Unbekannte Gruppe und Gruppe ohne Anteil aus Mapping loeschen
      m_delete(fractions,ind);
      continue;
    }
    if (!pointerp(h=groups[ind][P_MEMBERS]))
      h=({});
    h+=({mat_id});
    groups[ind][P_MEMBERS]=h;
  }
  mat[P_MG_FRACTIONS] = fractions;
}

//==================== Einlesen der Mappings aus Dateien

#define MDESC_ERROR(x, y) LOG_ERROR(sprintf("Materialbeschreibung '%s': %s\n", x, y))
#define MDESC_WARN(x, y) //LOG_WARN(sprintf("Materialbeschreibung '%s': %s\n", x, y))

private mapping getDescParts(string s) {
  string* lines;
  string key, val;
  int i, n;
  mapping m;
  m = ([]);
  val = "";
  lines = explode(s, "\n");
  n = sizeof(lines);
  if (n > 0) {
    while (i < n) {
      if (sscanf(lines[i], "%s:", key)) {
        status multiline;
        multiline = 0;
        // Schluessel gefunden, Wert auslesen
        while ( (++i < n) && sizeof(lines[i])) {
          // Mehrzeilige Werte mit newline verketten
          if (multiline) {
            val += "\n";
          }
          val += lines[i];
          multiline = 1;
        }
        m += ([key:val]);
        val = "";
      }
      i++;
    }
  }
  return m;
}
private varargs int isFile(string fn, string path) {
  if (stringp(path) && sizeof(path))
    fn = path+"/"+fn;
  return (file_size(fn) >= 0);
}

private varargs mixed readGroupDesc(string id) {
  mixed m;
  string fn;
  fn = MAT_DIR+"/groups/"+id;
  if (file_size(fn) > 0) {
    mapping parts;
    string desc;
    parts = getDescParts(read_file(fn));
    m = ([P_NAME:parts["Name"],
          P_MEMBERS:({})]);
    if (member(parts,"Beschreibung"))
      m += ([P_DESCRIPTION:parts["Beschreibung"]]);
    if (parts["Gruppenid"] != id)
      LOG_WARN(sprintf("Unstimmigkeit Gruppenid bei '%s'\n", id));
  } else {
    LOG_ERROR(sprintf("Kann Gruppenbeschreibung %s nicht laden\n", fn));
  }
  return m;
}

private mapping convMatId(string s) {
  mapping m;
  string* parts;
  parts = explode(s, "\"");
  if (sizeof(parts)) {
    int ende;
    ende = strstr(parts[0]," ")-1;
    if (ende < 0)
      ende = sizeof(parts[0]);
    m = ([P_ID:parts[0][0..ende]]);
    if (sizeof(parts) > 1)
      m += ([P_DEFSTR:parts[1]]);
  }
  return m;
}
private string* convMatNames(string s) {
  string* names;
  names = filter(explode(s, "\""),
                       lambda( ({'x}), ({#'>, ({#'sizeof, 'x}), 1}) ));
  if (sizeof(names)<1)
    names=0;
  else {
    if (sizeof(names)<2)
      names+=({names[0]+"s"});
    if (sizeof(names)<3)
      names+=({names[0]});
    if (sizeof(names)<4)
      names+=({names[0]});
  }
  return names;
}
private int convMatGender(string s) {
  int gender;
  s = lowerstring(s);
  // Ein Buchstabe reicht zur Bestimmung. Wenn nur weiblich|female
  // bzw. maennlich|male verwendet wird. Dabei ist dann allerdings die
  // Reihenfolge der Auswertung wichtig, damit das m bei MALE nicht mehr bei
  // female passt.
  if (sizeof(regexp( ({s}), "f|w"))) {
    gender = FEMALE;
  } else if (sizeof(regexp( ({s}), "m"))) {
    gender = MALE;
  } else {
    gender = NEUTER;
  }
  return gender;
}
private string convMatDesc(string s) {
  if (sizeof(regexp( ({s}), "- nicht vorhanden -"))) {
    s = 0;
  } else {
    // Mehrzeilige Beschreibungen zu einer Zeile zusammenfassen
    s = implode(explode(s, "\n"), " ");
  }
  return s;
}
private void addRecocLine(string s, mixed* r) {
  // Die weitere Bewertung der Schwierigkeit kann erst vorgenommen werden,
  // wenn alle Materialien bekannt sind und passiert spaeter. Zuerst werden
  // nur die Elemente des Arrays konvertiert und eingetragen
  string mat;
  int val;
  if (sscanf(s, "%s:%d", mat, val)) {
    r += ({mat,val});
  } else if (sscanf(s, "%d", val)) {
    r += ({val});
  } else {
    r += ({s});
  }
}
private mixed convMatRec(string s) {
  mixed difficulties;
  if (sizeof(regexp( ({s}), "- keine Einschraenkung -"))) {
    difficulties = 0;
  } else {
    difficulties = ({});
    // Jede Zeile enthaelt eine Bedingung
    map(explode(s, "\n"), #'addRecocLine, &difficulties);
  }
  return difficulties;
}
private void addGroupLine(string s, mapping g) {
  // Die weitere Bewertung der Zugehoerigkeit passiert spaeter.
  string grp;
  int val;
  if (sscanf(s, "%s:%d", grp, val)) {
    g += ([grp:val]);
  } else {
    g += ([grp:100]);
  }
}
private mapping convMatGroups(string s) {
  mapping groups;
  if (!sizeof(regexp( ({s}), "- keine -"))) {
    groups = ([]);
    // Jede Zeile enthaelt eine Bedingung
    map(explode(s, "\n"), #'addGroupLine, groups);
  }
  return groups;
}
private mapping convMaterialDesc(string id, mapping desc) {
  /* Struktur Materialmapping:
     P_GENDER,
     P_NAME:({name_nom, name_gen, name_dativ, name_akkusativ}),
     (P_RECOC:({mat1,faehigkeit1,mat2,faehigkeit2,...}),)
     (P_DEFSTR: bei bedarf),
     P_DESCRIPTION,
     (grupp1:anteil1,
     gruppe2:anteil2,
     ...)
  */
  mapping m;
  mixed val, val2;
  m = ([]);
  // Der string fuer das #define zuerst:
  val = convMatId(desc["Materialid"]);
  if (mappingp(val)) {
    if (val[P_ID] != id)
      LOG_WARN(sprintf("Unstimmigkeit Materialid bei '%s':%O\n", id, val[P_ID]));
    if (member(val, P_DEFSTR)) {
      m += ([P_DEFSTR:val[P_DEFSTR]]);
    } else {
      // Wenn kein String fuers #define angegeben wurde, dann direkt ID verwenden
      //m += ([P_DEFSTR:lowerstring(id)[4..]]);
    }
  }
  // Die Namen
  if (val = convMatNames(desc["Name"])) {
    m += ([P_NAME:val]);
  } else {
    MDESC_WARN(id, "keine Namen");
    m += ([P_NAME:({"", "", "", ""})]);
  }
  // Das Geschlecht, standard ist NEUTER
  m += ([P_GENDER:convMatGender(desc["Geschlecht"]) ]);
  // Die Beschreibung
  val = convMatDesc(desc["Beschreibung"]);
  if (sizeof(val)) {
    m += ([P_DESCRIPTION:val]);
  } else {
    MDESC_WARN(id, "keine Beschreibung");
  }
  // Die Erkennbarkeit
  val = convMatRec(desc["Erkennbarkeit"]);
  if (sizeof(val)) {
    m += ([P_RECOC:val]);
  }
  // und zum Schluss die Gruppenzugehoerigkeit
  val = convMatGroups(desc["Gruppenzugehoerigkeit"]);
  if (mappingp(val) && sizeof(val)) {
    m += ([P_MG_FRACTIONS:val]);
  }
  return m;
}
private varargs mixed readMaterialDesc(string id) {
  mixed m;
  string fn;
  fn = MAT_DIR+"/materials/"+id;
  if (file_size(fn) > 0) {
    mapping parts;
    string desc;
    parts = getDescParts(read_file(fn));
    m = convMaterialDesc(id, parts);
  } else {
    LOG_ERROR(sprintf("MDB:Kann Materialbeschreibung %s nicht laden\n", fn));
  }
  return m;
}

public int GetUpdateTicks() {
  return updateTicks;
}

private void scanFinished() {
  isScanning = 0;
  initialized = 1;
  // Mappings umkopieren
  materials = new_materials;
  material_groups = new_material_groups;
  // Letzter Schritt: Mapping mit alten Schluesseln anlegen
  old_mat_keys = mkmapping(map(m_indices(materials), #'matKey2Defstr, materials),
                           m_indices(materials));
  // Generieren der Doku und des Materialheaders
  GenHeaderFile();
  GenMatList();
  GenMatGroupList();
  // Savefile schreiben
  save_object(SAVEFILE);
}

public int IsScanning() {
  return isScanning;
}

private varargs void doScanMaterials(string* mats, int i, int step) {
  int ticks, start;
  string matid;
  start = get_eval_cost();
  if (step < 2) {
    while ( (i < sizeof(mats)) &&
            ((start - get_eval_cost()) < query_limits()[LIMIT_EVAL]/3 ) ) {
      matid = mats[i];
      switch (step) {
      case 0:
        // Erster Schritt: Einlesen der Dateien
        new_materials[matid] = readMaterialDesc(matid);
        break;
      case 1:
        // Zweiter Schritt: Bearbeiten der Erkennung und Gruppenzugehoerigkeit
        updateGroupMembers(new_material_groups, matid, new_materials[matid]);
        break;
      default:
        break;
      }
      i++;
    }
  }
  if (i < sizeof(mats)) {
    catch(raise_error(sprintf("MaterialDB: Initialisierung noch nicht beendet,"
                              " fehlende Materialbeschreibungen moeglich"
                              " (Phase %d:%d/%d)\n",
                              step, i, sizeof(mats)));publish);
    call_out(#'doScanMaterials, 2, mats, i, step);
  } else {
    // Zweite Stufe ausloesen oder beenden
    if (step < 1) {
      if ((start - get_eval_cost()) < query_limits()[LIMIT_EVAL]/2 )
          doScanMaterials(mats, 0, step+1);
      else
          call_out(#'doScanMaterials, 2, mats, 0, step+1);
    }
    else
      scanFinished();
  }
  updateTicks += start - get_eval_cost();
}

private mapping ScanGroups() {
  mapping groups;
  string* grpfiles;
  groups = ([]);
  grpfiles = filter(get_dir(MAT_DIR+"/groups/MATGROUP_*"),
                          #'isFile, MAT_DIR+"/groups");
  groups = mkmapping(grpfiles, map(grpfiles, #'readGroupDesc, 1));
  return groups;
}

private void ScanMaterials() {
  string *matfiles;
  matfiles = filter(get_dir(MAT_DIR+"/materials/MAT_*"),
                          #'isFile, MAT_DIR+"/materials");
  doScanMaterials(matfiles);
}

void Update() {
  int start;
  updateTicks = 0;
  start = get_eval_cost();
  if (!isScanning) {
    if (sizeof(get_dir(MAT_DIR))) {
      isScanning = 1;
      new_material_groups = ScanGroups();
      new_materials = ([]);
      updateTicks = start - get_eval_cost();
      ScanMaterials();
    } else {
      LOG_ERROR("Kann Materialverzeichnis nicht finden, keine Materialien angelegt!\n");
    }
  }
}
