// invmaster.c by Nachtwind@MG V1.1 (5.3.2001)
// A small master that provides a graphical display of the player´s
// equipment. 
#pragma strong_types
#pragma save_types,rtt_checks
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <input_to.h>
#include <properties.h>
#include <ansi.h>
#include <combat.h>
#include <language.h>
#include "invmaster.h"


mapping data;
closure abbreviate;


// i'm aware this can be determined with m_indices(VALID_ARMOUR_TYPE),
// but the position in the arrays is important for the drawing order.
// first item in the array is drawn last
static string *armour_types = 
({AT_BELT,
  AT_SHIELD,
  AT_HELMET,
  AT_BOOT,
  AT_TROUSERS,
  AT_AMULET,
  AT_RING,
  AT_GLOVE,
  AT_QUIVER,
  AT_CLOAK,
  AT_ARMOUR});

static mapping colors = 
([0:ANSI_BLACK,
  1:ANSI_RED, 
  2:ANSI_GREEN,
  3:ANSI_YELLOW,
  4:ANSI_BLUE,
  5:ANSI_PURPLE,
  6:ANSI_CYAN,
  7:ANSI_WHITE,
  8:""]);
  
static mapping bgcolors = 
([0:ANSI_BG_BLACK,
  1:ANSI_BG_RED, 
  2:ANSI_BG_GREEN,
  3:ANSI_BG_YELLOW,
  4:ANSI_BG_BLUE,
  5:ANSI_BG_PURPLE,
  6:ANSI_BG_CYAN,
  7:ANSI_BG_WHITE,
  8:""]);
  


static string Mapping2ColoredText(mapping pic, object player);
static string Mapping2PlainText(mapping pic);
static void AddDescription(mapping pic, string type, object item);
static string ComposeDesc(object item);
static void ConfigureColors(string text);

void ShowInv(object player, string arg);

// ok, let´s just read in the graphics...
// really takes some time (~250 eval ticks) but is only done
// once in an uptime
void create()
{
  mapping pic;
  string *files, *lines, text;
  int i,j,k, indentx,indenty, color;

  data=([]);
  
  DB("Trying to fire up master, path is '"+INVPATH+"'...");
  files=get_dir(INVPATH+"gfx/*")-({".", ".."});
  DB(sprintf("Files found in 'gfx/': \n%O", files));
  for (i=sizeof(files)-1;i>=0;i--)
  {
    DB("Reading '"+files[i]+"' ...");
    text=read_file(INVPATH+"gfx/"+files[i]);
    if (!stringp(text))
    {
      DB("Failed to read file.");
      continue;
    }
    lines=explode(text, "\n");
    if (sizeof(lines) < 4)
    {
      DB("File corrupt.");
      continue;
    }
    indentx=to_int(lines[1]);
    indenty=to_int(lines[2]);
    color=to_int(lines[0]);
    pic=([]);
    for (j=sizeof(lines)-1;j>2;j--)
    {
      for (k=sizeof(lines[j])-1;k>=0;k--)
      {
        if (lines[j][k..k]!="?")
          pic+=([(j-3+indenty)*80+k+indentx:lines[j][k..k];color]);
      }
    }
    data+=([files[i]:pic]);
    DB("File successfully read.");
  }
  DB(sprintf("Types covered:\n%O\n", m_indices(data)));

  // create closure only once to save time
  // needed by ComposeDesc()
  // the closure ist not as complicated as it seems ;)
  // it just checks every word of the name, if it does not begin
  // with a capital letter, it is abbreviated
  // this happens only if the name length exceeds 20 chars...
  abbreviate=lambda(({'x}), 
      ({#'?, ({#'>, ({#'member, quote(({"der", "des"})), 'x}), 0}),
        "d.",
        ({#'?, ({#'>, ({#'sizeof, 'x}), 3}), 
          ({#'?, ({#',, ({#'=, 'a, ({#'allocate, 1}) }),
                        ({#'=, ({#'[, 'a, 0}), 'x }), 
                        ({#'sizeof, ({#'regexp, 'a, "^[a-z].*"}) }) 
                 }), 
            ({#'+, ({#'[..], 'x, 0, 1}), "."}),
            'x
          }), 
          'x
        })
      }));
}

// function that tries to guess a good item name and use abbrevations
// where possible
static string ComposeDesc(object item)
{
  int i;
  string text, *buff;
  
  text=regreplace(item->QueryProp(P_SHORT)
                ||item->QueryProp(P_NAME)
                ||"<?>",
                "^(Ein Paar|Ein|Eine|Der|Die|Das) ","",0);
                
// try to shorten the name with the closure
  if (sizeof(text) > 20)
    return implode(map(explode(text, " "), abbreviate), " ");
  else      
    return text;
}

// converts a mapping with characters and color info into a
// text. data in the mapping is stored in a one-dimensional 
// order with the position as key (ypos is pos/80, xpos pos%80)
// this setup has a huge advantage: combining several
// graphics just takes a "+" operator, the rest is handled
// by the game driver. freakin' fast, much better than doing an
// iteration over one or more array in lpc.
static string Mapping2ColoredText(mapping pic, object player)
{
  string text;
  mapping configmap;
  int i,j,color;
  
  configmap=default_config+(player->QueryProp(P_INVMASTER_CONFIG)||([]));

  text="";
  color=0;
  for (i=0;i<22;i++)
  {
    text+=bgcolors[configmap[8]];
    for (j=0;j<78;j++)
    { 
      if (pic[i*80+j,1]!=color)
      {
        color=pic[i*80+j,1];
        text+=colors[configmap[color]];
      }
      text+=pic[i*80+j];
    }
    text+=ANSI_NORMAL+"\n";
    color=0;
  }
  return text;
}

static string Mapping2PlainText(mapping pic)
{
  string text;
  int i,j;
  
  text="";

  for (i=0;i<22;i++)
  {
    for (j=0;j<78;j++)
      text+=pic[i*80+j];
    text+="\n";
  }
  return text;
}
static void AddDescription(mapping pic, string type, object item)
{
  int indentx, indenty, i;
  string text;

  switch(type)
  {
   case AT_HELMET:
      indentx=47;
      indenty=0;
      text=sprintf("%-30s",ComposeDesc(item)[0..30]);break;
    case AT_QUIVER:
      indentx=49;
      indenty=2;
      text=sprintf("%-28s",ComposeDesc(item)[0..28]);break;     
    case AT_AMULET:
      indentx=49;
      indenty=4;     
      text=sprintf("%-27s",ComposeDesc(item)[0..28]);break;     
    case AT_ARMOUR:
      indentx=53;
      indenty=7;
      text=sprintf("%-24s",ComposeDesc(item)[0..25]);break;          
    case AT_SHIELD:
      indentx=54;
      indenty=10;   
      text=sprintf("%-20s",ComposeDesc(item)[0..24]);break;
    case AT_CLOAK:
      indentx=53;
      indenty=15;
      text=sprintf("%-20s",ComposeDesc(item)[0..25]);break;
    case AT_TROUSERS:
      indentx=49;
      indenty=17;
      text=sprintf("%-20s",ComposeDesc(item)[0..20]);break;     
    case AT_RING:
      indentx=0;
      indenty=9;
      text=sprintf("%14s",ComposeDesc(item)[0..17]);break;
    case AT_GLOVE:
      indentx=0;
      indenty=11;
      text=sprintf("%14s",ComposeDesc(item)[0..17]);break;
    case AT_BELT:
      indentx=1;
      indenty=13;
      text=sprintf("%14s",ComposeDesc(item)[0..18]);break;     
    case AT_BOOT:
      indentx=1;
      indenty=20;
      text=sprintf("%18s",ComposeDesc(item)[0..18]);break;
    case "Waffe":
      indentx=1;
      indenty=1;
      text=sprintf("%18s",ComposeDesc(item)[0..25]);
      if (item->QueryProp(P_NR_HANDS) > 1 &&
          this_player() &&
          !(this_player()->QueryArmorByType(AT_SHIELD)))
        AddDescription(pic, AT_SHIELD, item);break;
    default: return;
  }
  for (i=0;i<sizeof(text);i++)
    pic+=([(80*indenty+indentx+i):text[i..i];2]);
}

varargs static void ConfigureColors(string text)
{ 
  mapping config, display;
  string *strs;
  
  if (!objectp(this_player())) return;
  
  if (this_player()->InFight())
  {
    write(break_string(
      "Im Kampf? Na Du hast Nerven, das lassen wir doch mal lieber! "
      "Probier es danach nochmal...", 78));
    return;
  }
  
  if (stringp(text)) text=lower_case(text);
  
  if (text=="ok")
  {
    write("Farbkonfiguration beendet.\n");
    return;
  }

  //"ansi_config", def in invmaster.h
  config=this_player()->QueryProp(P_INVMASTER_CONFIG)||([]);
  display=default_config+config;
  
  if (!text || text=="")
  {
    write( 
    "*** Farbkonfiguration fuer den Ausruestungsbefehl ***\n\n"
    " Farbe:                     wird dargestellt mit:\n"
    "------------------          --------------------\n"
    " Hintergrund                 "+COLORNAMES[display[8]]+"\n"
    " Schwarz                     "+COLORNAMES[display[0]]+"\n"
    " Rot                         "+COLORNAMES[display[1]]+"\n"
    " Gruen                       "+COLORNAMES[display[2]]+"\n"
    " Gelb                        "+COLORNAMES[display[3]]+"\n"
    " Blau                        "+COLORNAMES[display[4]]+"\n"
    " Magenta                     "+COLORNAMES[display[5]]+"\n"
    " Tuerkis                     "+COLORNAMES[display[6]]+"\n"
    " Weiss                       "+COLORNAMES[display[7]]+"\n\n"
    "Farbe aendern mit '<farbe> <gewuenschte farbe>'.\n"
    "Beispiel: 'gelb rot'.\n"
    "Alles, was standardmaessig gelb waere, wuerde dann mit der ANSI-Farbe \n"
    "Rot dargestellt.\n"
    "Der Hintergrund kann zusaetzlich die Farbe 'keine' haben, bei der der \n"
    "Hintergrund eben ueberhaupt nicht gefaerbt wird.\n"
    "Beispiel: 'hintergrund keine'. Schaltet die Hintergrundfarbe aus.\n\n"
    "Beenden mit 'ok'. \n"
    "Wiederholung der Farbliste mit <Return>.\n"
    "Farbliste auf Standard zuruecksetzen mit 'reset'.\n");
  }
  else
  if (text=="reset")
  {
    this_player()->Set(P_INVMASTER_CONFIG, SAVE, F_MODE_AD);    
    this_player()->SetProp(P_INVMASTER_CONFIG, 0);
    write("Farben zurueckgesetzt!\n");    
  }
  else
  {
    if ( sizeof(strs=explode(text, " ")-({""})) !=2 
      || !member((COLORCODES-(["keine"])), strs[0])
      || !member((COLORCODES-(["hintergrund"])), strs[1])
      || ((strs[0]!="hintergrund") && (strs[1]=="keine")) ) 
    {
      write("Falsche Eingabe.\n"
            "Format: <farbe|hintergrund> <zugewiesene Farbe>\n"
            "Abbrechen mit 'ok'.\n");
    }
    else
    {
      if (COLORCODES[strs[1]]==default_config[COLORCODES[strs[0]]])
        config-=([COLORCODES[strs[0]]]);
      else
        config+=([COLORCODES[strs[0]]:COLORCODES[strs[1]]]);
      if (!sizeof(config))
      {
        this_player()->Set(P_INVMASTER_CONFIG, SAVE, F_MODE_AD);    
        this_player()->SetProp(P_INVMASTER_CONFIG, 0);
      }
      else
      {
        this_player()->SetProp(P_INVMASTER_CONFIG, deep_copy(config));
        this_player()->Set(P_INVMASTER_CONFIG, SAVE, F_MODE_AS);        
      }
      write("Ok, Farbe gewaehlt!\n");
    }
  }
  input_to("ConfigureColors", INPUT_PROMPT, "\nEingabe: ");
}


string* armour_order=({ 
    AT_HELMET, AT_AMULET, AT_QUIVER, AT_ARMOUR, AT_CLOAK,
  AT_GLOVE, AT_RING, AT_BELT,
  AT_TROUSERS, AT_BOOT, AT_SHIELD, AT_MISC}); 

mapping weapon_names=([
		       WT_SPEAR : "Speer",
		       WT_SWORD : "Schwert",
		       WT_STAFF : "Kampfstab",
		       WT_WHIP  : "Peitsche",
		       WT_CLUB  : "Keule",
		       WT_KNIFE : "Messer",
		       WT_MISC  : "Irgendwas",
		       WT_MAGIC : "Artefakt",
		       WT_AXE   : "Axt",
	       WT_RANGED_WEAPON : "Fernwaffe"
		       ]);

string SimpleInv(object player) {
  object* armours=player->QueryProp(P_ARMOURS);
  int count=sizeof(armour_order);
  string* list=allocate(count);
  string result="Ausruestung\n";
  int i;

  foreach(object ob: armours) {
    if (!objectp(ob)) continue;
      int idx = member(armour_order, ob->QueryProp(P_ARMOUR_TYPE));	      
      if (idx>=0)
        list[idx]=ob->QueryProp(P_SHORT);
  }

  // AT_MISC (letztes Element in list und armour_order) weglassen.
  for (i=0;i<count-1;i++) {
    result+=sprintf("%-20s %-57s\n",armour_order[i],list[i] || "");
  }

  object ob=ob=player->QueryProp(P_WEAPON);
  if (objectp(ob)) {
    result+=sprintf("%-20s %-57s\n",
		    (ob->QueryProp(P_NR_HANDS)==1 ? "Einhand-":"Zweihand-")
		    +weapon_names[ob->QueryProp(P_WEAPON_TYPE)],
		    ob->QueryProp(P_SHORT));
  } else result+="Keine Waffe\n";

  return result;
}
// the main function called by the player object.
// determines gender, then adds armor/weapon graphics
// dynamically. still very fast due to the use of the "+" operator,
// see above.
void ShowInv(object player, string arg)
{
  string gender, type;
  mapping pic;
  int i;
  object item;

  if (!objectp(player)||!interactive(player)) return;

  // split args.
  string *args;
  if (stringp(arg))
    args = explode(lower_case(arg), " ") - ({" "});
  else
    args = ({});

  if (member(args, "farben") > -1) {
      ConfigureColors();
      return;
  }

  if (member(args, "-k") > -1 || player->QueryProp(P_NO_ASCII_ART)) {
    tell_object(player, SimpleInv(player));
    return;
  }

  gender=player->QueryProp(P_GENDER)==FEMALE?"_female":"_male";
  pic=deep_copy(data["base"+gender]);
  pic+=data["Beschriftung"];
  for (i=sizeof(armour_types)-1;i>=0;i--)
    if (objectp(item=player->QueryArmourByType(armour_types[i])))
    {
      pic+=data[armour_types[i]+gender];
      AddDescription(pic, armour_types[i], item);
    }
  if (item=player->QueryProp(P_WEAPON))
  {
    pic+=data[(VALID_WEAPON_TYPE(type=item->QueryProp(P_WEAPON_TYPE)))?
      type:WT_MISC];
    AddDescription(pic, "Waffe", item);
  }      
  if (player->QueryProp(P_TTY)!="ansi")
    player->More(Mapping2PlainText(pic));
  else
    player->More(Mapping2ColoredText(pic, player));
  DB(geteuid(player)+" eval cost: "+(1000000-get_eval_cost())+" ticks.\n");
}

