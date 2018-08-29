inherit "std/thing";
inherit "std/more";
inherit "/mail/nedit";

#include <properties.h>
#include <wizlevels.h>
#include "../haus.h"

#define FAQ(f) (SPECIALPATH+"faq/"+(f))
#define FAQUSER ({ "ryne", "toss", "sting" })
#define IS_USER(pl) (member(FAQUSER, getuid(pl)) >= 0)

static string *seite;
static string f,*s;

mapping synonym = ([]);

void create()
{
  ::create();
  SetProp(P_NAME,"SeherFAQ");
  SetProp(P_GENDER,FEMALE); 
  SetProp(P_NOGET,1);
  AddId( ({"faq","seherfaq","FAQ","SeherFAQ" }) );
  AddCmd("lies","lesen");
  AddCmd(({"beschreib","beschreibe"}),"beschreiben");
  AddCmd("seiten","seiten");
  AddCmd(({"loesch","loesche"}),"loeschen");
  AddCmd(({"kopier","kopiere"}),"kopieren");
  restore_object(FAQ("faq"));
  seite = get_dir(FAQ("*"))-({".","..","faq.o"});
}

public varargs void init(object origin)
{
  thing::init();
  if (this_player() && IS_USER(this_player()))
    nedit::init_rescue();
}

string
_query_short()
{
    return 0;
}

string 
_query_long()
{
    return 0;
}

int
id(string str)
{
    return 0;
}

int
seiten(string str)
{
  int i,j;
  string *syn;
  closure filter_ldfied;

  if (!IS_USER(this_player()))
    return 0;

  if(!(i = sizeof(seite)))
    write("Keine Seiten beschrieben!\n");
  else {
    filter_ldfied = lambda(({'ind,'page,'map_ldfied/*'*/}),
		    ({#'==,'page,({#'[/*]'*/,'map_ldfied,'ind})}));
    for (str = "",i-=1;i>=0;i--) {
      syn = m_indices(filter_indices(synonym,filter_ldfied,seite[i],synonym));
      printf("Seite '%s' - ",seite[i]);
      if (j=sizeof(syn)) {
	for(j-=1;j>=0;j--)
	  printf("'%s' ",syn[j]);
	write("\n");
      }
      else
	write( "(keine Synonyme definiert)\n");
    }
  }
  return 1;
}

int
lesen(string str)
{
  if (!IS_USER(this_player()))
    return 0;

  if (!str || str == "") {
    notify_fail("Was willst Du denn lesen?\n");
    return 0;
  }
  str = lower_case(this_player()->_unparsed_args());
  if (member(seite,str)<0) {
    if (!member(synonym,str)) {
      notify_fail("Diese Seite gibt es nicht...\n");
      return 0;
    }
    else 
      str = synonym[str];
  }
  this_player()->More(FAQ(str),1);
  return 1;
}

int
beschreiben(string str)
{
  string faq,*desc;
  int i;

  if (!IS_USER(this_player()))
    return 0;

  if (!str || str == "") {
    notify_fail("Was willst Du denn beschreiben?\n");
    return 0;
  }
  
  str = lower_case(this_player()->_unparsed_args());

  if (sscanf(str,"faq %s",str)!=1) {
    notify_fail("Syntax: beschreibe faq <seite>\n");
    return 0;
  }
  str = implode(old_explode(str," ")-({""}), " ");
  desc = old_explode(implode(old_explode(str,", "),","),",");
  f = 0;
  s = 0;

  for (i=sizeof(desc)-1;i>=0;i--) {
    if (member(seite,desc[i])>=0) {
      f = desc[i];
      s = desc-({desc[i]});
      break;
    }
  }
  if (!f) {
    f = desc[0];
    s = desc[1..];
  }
  write("Bitte Beschreibung fuer die FAQ-Seite '"+f+"' angeben\n");
  nedit("faq");
  return 1;
}

static void
faq(string str)
{
  int i;
  string file;

  if (!str || str == 0) {
    write("** Abbruch! **\nNichts geaendert!\n");
    return;
  }
  
  if (file_size(file=FAQ(f))>=0)
    rm(file);
  write_file(file, str);
  if (member(seite,f)==-1)
    seite += ({f});

  for (i=sizeof(s)-1;i>=0;i--) 
    synonym += ([ s[i] : f ]);

  save_object(FAQ("faq"));
}

int
loeschen(string str)
{
  string *syn;
  int i;

  if(!IS_USER(this_player()))
    return 0;

  str = lower_case(this_player()->_unparsed_args());
  if (!str || str == "") {
    notify_fail("Was willst Du denn loeschen?\n");
    return 0;
  }
  if (member(seite,str)>=0) {
    syn = m_indices(filter_indices(synonym,
				 lambda(({'ind,'page,'map_ldfied/*'*/}),
					({#'==,'page,({#'[/*]'*/,'map_ldfied,'ind})}))
				 ,seite[i],synonym));
    rm(FAQ(str));
    seite -= ({str});
  }
  else {
    str = implode(old_explode(str," ")-({""}), " ");
    syn = old_explode(implode(old_explode(str,", "),","),",");
  }
  for (i=sizeof(syn)-1; i>=0; i--)
    synonym = filter_indices(synonym,
                             lambda(({'key,'cmp}),
				    ({#'!=,'key,'cmp/*'*/})),
			     syn[i]);

  save_object(FAQ("faq"));
  write("OK\n");
  return 1;
}

int
kopieren(string str)
{
  string *part;
  int i;

  if (!IS_USER(this_player()))
    return 0;

  str = lower_case(this_player()->_unparsed_args());
  part = old_explode(str," ")-({""});
  if(sizeof(part)<4 || part[0]!="faq" || part[2] != "nach" || 
     member(seite,part[1]) == -1) {
    notify_fail("kopiere faq <seite> nach <alias>\n");
    return 0;
  }
  str = part[1];
  part=old_explode(implode(old_explode(implode(part[3..]," "),", "),","),",");
  for(i=sizeof(part)-1;i>=0;i--)
    synonym += ([ part[i] : str ]);
  save_object(FAQ("faq"));
  write("OK.\n");
  return 1;
}
