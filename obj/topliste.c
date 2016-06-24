inherit "/std/thing";

#include <properties.h>
#include <language.h>

// ({Funktionsname, ({argumente}), tttel })
nosave <string|mixed*>* watdenn;

string _liesliste();

protected void create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT,"Die Top-Liste, Top-Spielerliste, Top-Seherliste und "
      "Top-Hardcoreliste");
  SetProp(P_WEIGHT,0);
  SetProp(P_GENDER,FEMALE);
  SetProp(P_WEIGHT,20);
  AddId(({"liste","topliste","top-liste",
          "topspielerliste","top-spielerliste","topspieler","spielerliste",
          "topseherliste","top-seherliste","topseher","seherliste",
          "tophardcoreliste","top-hardcoreliste","tophardcore","hardcoreliste"}));
  SetProp(P_NAME,"Liste");
  SetProp(P_NOGET,"Die ist an der Wand festgenagelt.\n");
  AddReadDetail(SENSE_DEFAULT, #'_liesliste);
}

#define NAME     0
#define LEP      1
#define QP       2
#define XP       3
#define LEVEL    4
#define AGE      5
#define RACE     6
#define GUILD    7
#define WIZLEVEL 8
#define HARDCORE 9
public string format_list(< <string|int>* >* list)
{
  int idx;
  //string res="      Name         Level  AP  Erfahrung    Alter     Gilde\n";
  string res = sprintf(
      "%c%:4s  %:11-s  %:5|s %:5|s %:4|s %:12|s %:12|s  %:14-s\n",
      ' ', "", "Name", "", "Level", "AP", "Erfahrung","Alter","Gilde");

  foreach(<string|int>* entry: list)
  {
    ++idx;
    entry[AGE] *= 2;
    int tage = entry[AGE]/86400;
    int stunden = (entry[AGE] % 86400) / 3600;
    int minuten = (entry[AGE] % 3600) / 60;
    res +=
    sprintf("%c%:4d. %:11-s  %:5-s %:5d %:4d %:12d %:4dd, %:2'0'd:%:2'0'd  %:14-s\n",
            (entry[WIZLEVEL]>0 ? '*' : ' '),
            idx, capitalize(entry[NAME]),
            (entry[HARDCORE]>1 ? "(RIP)" : ""),
            entry[LEVEL],
            entry[QP], entry[XP],
            tage, stunden, minuten,
            capitalize(entry[GUILD]));
  }
  return res;
}

private string get_list(<string|mixed*>* was)
{
  if (was == 0)
    was=({"Liste", ({}), " "*28 + "Liste der Topspieler !\n\n" });

  < <string|int>* >* list = "/secure/topliste"->(was[0])(was[1]...);
  
  return was[2] + format_list(list);
}

varargs int id(string str,int lvl)
{
  if (!::id(str,lvl))
    return 0;

  if (strstr(str,"seher")>=0)
    watdenn=({"SeherListe", ({ }), " "*29 + "Liste der Topseher !\n\n" });
  else if (strstr(str,"spieler")>=0)
    watdenn=({"SpielerListe", ({ }),
               " "*21 + "Liste der Topspieler (Keine Seher) !\n\n" });
  else if (strstr(str,"hardcore")>=0)
    watdenn=({"HardcoreListe", ({ }),
               " "*22 + "Liste der mutigsten aller Helden !\n\n" });
  else
    watdenn=({"Liste", ({ }), " "*28 + "Liste der Topspieler !\n\n" });
  
  return 1;
}

string _query_long()
{
  if (this_player()) this_player()->More(get_list(watdenn));
  watdenn=0;
  return "";
}

varargs string long(int mode)
{
  if (this_player()) this_player()->More(get_list(watdenn));
  watdenn=0;
  return "";
}

string _liesliste()
{
  if (this_player()) this_player()->More(get_list(watdenn));
  watdenn=0;
  return "";
}

