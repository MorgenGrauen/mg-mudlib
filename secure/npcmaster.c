// MorgenGrauen MUDlib
//
// npcmaster.c - Verwaltung der eindeutigen Nummernvergabe fuer NPCs und
//               der Stufenpunkte, die sie geben
//
// $Id: npcmaster.c 9142 2015-02-04 22:17:29Z Zesstra $
//
/*
 * $Log: npcmaster.c,v $
 * Revision 1.6  1996/09/18 08:59:49  Wargon
 * Globale Variablen private gemacht...
 *
 * Revision 1.5  1996/04/10 17:48:37  Jof
 * Dump-Funktion eingebaut, damit man auch was zu lesen hat :)
 * ,
 *
 * Revision 1.4  1996/04/10 14:32:24  Wargon
 * QueryNPC(): int * -> mixed
 *
 * Revision 1.3  1996/04/04 12:33:06  Wargon
 * QueryNPCbyNumber() gibt jetzt wirklich Nummer und Score zurueck...
 *
 * Revision 1.2  1996/03/06 14:34:51  Jof
 * QueryNPCbyNumber eingebaut, Typos gefixed
 *
 * Revision 1.1  1996/03/06 13:45:36  Jof
 * Initial revision
 *
 */

#include "/secure/npcmaster.h"
#include "/secure/wizlevels.h"

private int lastNum;
private mapping npcs;
private nosave mapping by_num;

void make_num(string key) {
  by_num += ([ npcs[key,NPC_NUMBER] : key; npcs[key,NPC_SCORE] ]);
}

void create()
{
  seteuid(getuid());
  if (!restore_object(NPCSAVEFILE))
  {
    lastNum=0;
    npcs=m_allocate(0,2);
  }
  by_num = m_allocate(0,2);
  filter_indices(npcs, #'make_num/*'*/);
}

static int DumpNPCs();

varargs mixed QueryNPC(int score)
{
  string key, val;
  
  if (!previous_object())
    return NPC_INVALID_ARG;
  key=old_explode(object_name(previous_object()),"#")[0];
  if (val=npcs[key,NPC_NUMBER])
    return ({val,npcs[key,NPC_SCORE]});
  if (score<=0 || member(inherit_list(previous_object()),"/std/living/life.c") < 0)
    return NPC_INVALID_ARG;
  npcs[key,NPC_SCORE]=score;
  npcs[key,NPC_NUMBER]=++lastNum;
  by_num += ([lastNum:key;score]);
   save_object(NPCSAVEFILE);
  call_out("DumpNPCs",0);
  return ({lastNum,score});
}

int SetScore(string key,int score)
{
  if (!IS_ARCH(this_interactive())||!IS_ARCH(geteuid(previous_object())))
    return NPC_NO_PERMISSION;
  if (!npcs[key,NPC_NUMBER])
    return NPC_INVALID_ARG;
  npcs[key,NPC_SCORE]=score;
  by_num += ([npcs[key,NPC_NUMBER]:key;score]);
  save_object(NPCSAVEFILE);
  DumpNPCs();
  return 1;
}

static int DumpNPCs()
{
  mixed *keys;
  int i, max;

  keys=m_indices(npcs);
  rm(NPCDUMPFILE);
  for (i=sizeof(keys)-1, max=0;i>=0;i--) {
    write_file(NPCDUMPFILE,sprintf("%5d %4d %s\n",npcs[keys[i],NPC_NUMBER],
	   npcs[keys[i],NPC_SCORE],keys[i]));
    max += npcs[keys[i],NPC_SCORE];
  } 
  write_file(NPCDUMPFILE,sprintf("====================\nGesamt: %d Punkte\n",max)); 
  return 1;
}

mixed *QueryNPCbyNumber(int num)
{
  if (by_num[num])
    return ({num,by_num[num,NPC_SCORE],by_num[num,NPC_NUMBER]});

  return 0;
}

int Recalculate(string s)
{
  int i, j;

  if (!s || !sizeof(s))
    return 0;

  for (j=0, i=6*sizeof(s)-1; i>0; i--) {
    if (test_bit(s,i))
      j+=by_num[i,NPC_SCORE];
  }
  return j;
}
