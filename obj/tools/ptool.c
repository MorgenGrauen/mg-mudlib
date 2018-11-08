/*
   Objekt zum Eintragen von Raeumen die Zaubertraenke enthalten beim
   potionmaster. 
*/

#include <wizlevels.h>
#include <properties.h>
#include <language.h>

inherit "/std/secure_thing";

int dump();

int secure()
{
  if (!previous_object()) return 0;
  if (geteuid(previous_object())==ROOTID) return 1;
  if (geteuid(previous_object()) != geteuid(this_interactive())) return 0;
  if (this_interactive() != this_player()) return 0;
  if (!ARCH_SECURITY) return 0;
  return 1;
}

void create()
{
  if (!clonep(this_object())) return;
  ::create();
  SetProp(P_SHORT, "Potion-Tool");
  SetProp(P_LONG, 
     "Ein Tool zum Eintragen von Raumen in den Potionmaster.\n"+
     "Folgende Befehle gibt es:\n"+
     "ptadd <liste>     Addiert den ZT im Raum in Liste <liste>\n"+
     "                  und aktiviert ihn gleichzeitig.\n"+
     "ptset <liste>     Aktiviert den ZT im Raum und setzt ihn in\n"+
     "                  die Liste <liste>.\n"+
     "ptchange <nummer> Ersetzt den ZT <nummer> durch den ZT im Raum.\n"+
     "ptact             Aktiviert einen deaktivierten ZT in seiner\n"+
     "                  bisherigen Liste.\n"+
     "ptdeact           Deaktiviert den ZT im Raum aus der ZTListe\n"+
     "                  und verschiebt ihn nach 'deaktivierte ZTs'.\n"+
     "ptinfo [para]     kein para:   Zeigt Infos ueber den ZT im aktu-\n"+
     "                               ellen Raum.\n"+
     "                  int para:    Zeigt Infos zum ZT para an.\n"+
     "                  string para: Zeigt ZT-Infos des Spielers para.\n"+
     "ptdump            Erzeugt ein Dump der aktuellen ZTListe und\n"+
     "                  speichert diesen ab.\n\n");
  SetProp(P_NAME, "PTool");
  SetProp(P_GENDER, NEUTER);
  AddId(({"tool", "ptool"}));
  SetProp(P_NODROP, 1);
  SetProp(P_AUTOLOADOBJ, 1);
  AddCmd("ptadd", "add");
  AddCmd("ptset", "setlist");
  AddCmd("ptchange", "changepath");
  AddCmd("ptact", "activate");
  AddCmd("ptdeact", "deactivate");
  AddCmd("ptinfo", "info");
  AddCmd("ptdump", "dump");
}

int add(string number)
{
  int nr, next, active;
  string room;
  if (!secure())
  { printf("Fehler: -1\n"); return -1; }
  if (!query_once_interactive(environment()))
  { printf("Fehler: -2\n"); return -2; }
  if (!number)
  { printf("Fehler: -3\n"); return -3; }
  sscanf(number, "%d", nr);
  if (!intp(nr))
  { printf("Fehler: -4\n"); return -4; }
  room = object_name(environment(environment(this_object())));
  next = "/secure/potionmaster"->AddPotionRoom(room,nr);
  if (next>0)
    printf("Raum in Gesamtliste eingetragen, Nummer: %d\n", next-1);
  else
  {
    printf("Raum konnte nicht eingetragen werden, Ergebnis: %d\n", next);
    return 1;
  }
  // Nicht mehr noetig, Arathorn, 2013-Mai-30
  //active = "/secure/potionmaster"->SetListNr(room, nr);
  if (active>=0)
  {
    printf("Raum aktiviert in Liste %d\n", nr);
    dump();
  }
  else
    printf("Fehler beim Aktivieren, Ergebnis: %d\n", active);
  return 1;
}

int changepath(string number)
{
  int nr,neu;
  string room;
  mixed roomold;
  if (!secure())
  { printf("Fehler: -1\n"); return -1; }
  if (!query_once_interactive(environment()))
  { printf("Fehler: -2\n"); return -2; }
  if (!number)
  { printf("Fehler: -3\n"); return -3; }
  sscanf(number, "%d", nr);
  if (!intp(nr))
  { printf("Fehler: -4\n"); return -4; }
  room = object_name(environment(environment(this_object())));
  roomold = "/secure/potionmaster"->GetFilenameByNumber(nr);
  if (roomold==-1)
  { printf("Fehler: -5\n"); return -5; }
  neu = "/secure/potionmaster"->ChangeRoomPath(roomold,room);
  if (neu>0)
    {
      printf("ZT %d durch aktuellen Raum ersetzt.\n", neu);
      dump();
    }
  else
    printf("ZT %d konnte nicht ersetzt werden, Ergebnis: %d\n", nr,neu);
  return 1;
}

int setlist(string number)
{
  int nr, next, active;
  string room;
  if (!secure())
  { printf("Fehler: -1\n"); return -1; }
  if (!query_once_interactive(environment()))
  { printf("Fehler: -2\n"); return -2; }
  if (!number)
  { printf("Fehler: -3\n"); return -3; }
  sscanf(number, "%d", nr);
  if (!intp(nr) || nr<0 || nr>7)
  { printf("Fehler: -4\n"); return -4; }
  room = object_name(environment(environment(this_object())));
  active = "/secure/potionmaster"->SetListNr(room, nr);
  if (active>=0)
  {
    printf("Raum aktiviert in Liste %d\n", nr);
    dump();
  }
  else
    printf("Fehler beim Aktivieren, Ergebnis: %d\n", active);
  return 1;
}

int activate()
{
  int nr, active;
  string room;
  
  if (!secure())
  { printf("Fehler: -1\n"); return -1; }
  if (!query_once_interactive(environment()))
  { printf("Fehler: -2\n"); return -2; }
  room = object_name(environment(environment(this_object())));
  active = "/secure/potionmaster"->ActivateRoom(room);
  if (active>=0)
    {
      printf("Raum aktiviert in Liste %d\n", active);
      dump();
    }
  else
    printf("Fehler beim Aktivieren, Ergebnis: %d\n", active);
  return 1;
}

int deactivate()
{
  int deactive;
  string room;
  
  if (!secure())
  { printf("Fehler: -1\n"); return -1; }
  if (!query_once_interactive(environment()))
  { printf("Fehler: -2\n"); return -2; }
  room = object_name(environment(environment(this_object())));
  deactive = "/secure/potionmaster"->DeactivateRoom(room);
  if (deactive>=0)
    {
      printf("Raum deaktiviert in Liste %d\n",deactive);
      dump();
    }
  else
    printf("Fehler beim Deaktivieren, Ergebnis: %d\n", deactive);
  return 1;
}

int info(string para)
{
  int info,nr;
  string s;
  object room,o;
  mixed m,*potions;
  if (!secure())
  { printf("Fehler: -1\n"); return -1; }
  if (!query_once_interactive(environment()))
  { printf("Fehler: -2\n"); return -2; }
  if (!para || para=="")
    {
      room = environment(environment(this_object()));
      info = "/secure/potionmaster"->HasPotion(room);
      if (info>=0)
        {
          printf("Raum hat ZT mit Nr:  %d\n\n",info);
          nr = "/secure/potionmaster"->GetListByNumber(info);
          if (nr>=0)
            printf("ZT aktiv in Liste:   %d\n\n",nr);
          else
            {
              nr = "/secure/potionmaster"->GetInactListByNumber(info);
              if (nr>=0)
                printf("ZT INaktiv in Liste: %d\n\n",nr);
              else
                printf("ZT INaktiv\n\n");
            }
          if (info>=0 && s=read_file("/secure/ARCH/ZT/"+info+".zt"))
            {
              write("Tip:\n"+s);
            }
        }
      else
        printf("Raum hat keinen ZT eingetragen.\n");
    }
  else
    if (sscanf(para,"%d",info)==1)
      {
        printf("ZT mit Nummer: %d\n\n",info);
        m = "/secure/potionmaster"->GetFilenameByNumber(info);
        if (m!=-1)
          {
            write("Filename: "+m+"\n\n");
            nr = "/secure/potionmaster"->GetListByNumber(info);
            if (nr>=0)
              printf("ZT aktiv in Liste: %d\n\n",nr);
            else
              {
                nr = "/secure/potionmaster"->GetInactListByNumber(info);
                if (nr>=0)
                  printf("ZT INaktiv in Liste: %d\n\n",nr);
                else
                  printf("ZT INaktiv\n\n");
              }
            if (info>=0 && s=read_file("/secure/ARCH/ZT/"+info+".zt"))
              {
                write("Tip:\n"+s);
              }
          }
        else
          write("Kein ZT mit dieser Nummer bekannt.\n");
      }
    else
      {
        write("Spieler "+capitalize(para)+"\n\n");
        if (o=find_player(para))
          {
            potions=sort_array(o->QueryProp(P_POTIONROOMS),#'>);
            if (sizeof(potions))
              {
                s="";
                for (nr=0;nr<sizeof(potions);nr++)
                  s+=potions[nr]+", ";
                write("Potionrooms:\n"+break_string(s[0..<3],78)+"\n");
              }
            else write("Spieler hat keine weiteren Potionrooms.\n");
            potions=sort_array(o->QueryProp(P_KNOWN_POTIONROOMS),#'>);
            if (sizeof(potions))
              {
                s="";
                for (nr=0;nr<sizeof(potions);nr++)
                  s+=potions[nr]+", ";
                write("Bekannte Potionrooms:\n"+
                      break_string(s[0..<3],78));
              }
            else write("Spieler hat keine bekannten Potionrooms.\n");
          }
        else
          write("Kein Spieler mit diesem Namen anwesend.\n");
      }
  return 1;
}

int dump()
{
  "/secure/potionmaster"->DumpList();
  printf("Liste wurde erzeugt.\n");
  return 1;
}

