// MorgenGrauen MUDlib
//
// www.news.c -- WWW frontend for reading mud news
//
// $Id: www.news.c 8755 2014-04-26 13:13:40Z Zesstra $

#pragma strong_types
#pragma combine_strings

#include <www.h>

#define DBG(x)  tell_object(find_player("hate"), sprintf("DBG: %O\n", x))

#define NEWSD "/secure/news"

#define N_GROUP 0
#define N_AUTHOR 1
#define N_TID 2
#define N_DATE 3
#define N_TITLE 4
#define N_ARTICLE 5

varargs private string url(string group, string article)
{
  return "\""+MUDWWW+"?"+REQ+"="+R_NEWS
    +(group?"&"+GROUP+"="+group:"")
    +(article?"&"+ARTICLE+"="+article:"")+"\"";
}

varargs private string make_link(string text, string group, string article)
{
  if(!text || !sizeof(text)) text = "-Unbenannt-";
  return "<A HREF="+url(group, article)+">"+text+"</A>";
}

string GroupList()
{
  string *list, ret;
  int i, t;

  list = NEWSD->GetGroups();
  for (i = 0, ret = ""; i < sizeof(list); i++)
    ret += sprintf("[%3d Artikel, %-6s] %s\n",
                   sizeof(NEWSD->GetNotes(list[i])),
                   dtime(t = NEWSD->GetNewsTime(list[i]))[5..11],
                   make_link(list[i],list[i]+":"+t));
  return "<H2>&Uuml;bersicht</H2>"
        +"<H3>["+sizeof(list)+" Gruppen, "
        +"letzte &Auml;nderung "+dtime(NEWSD->GetNewsTime())+"]</H3>"
        +"<PRE>" + ret +"</PRE>";
}

#define THREAD(a) ("~#! rn="+(a[N_AUTHOR])+" rt="+(a[N_DATE])+ \
                   " rg="+(a[N_GROUP]))
#define TEXPR "rn=[^\n ]*|rt=[^\n ]*|rg=[^\n ]*"

private mixed tinfo(mixed article)
{
  mixed tmp, info;
  string rn, rt, rg, tid;
  int j, k;

  tmp = regexp(old_explode(article[N_ARTICLE], "\n"), "^~#!");
  for(j = sizeof(tmp); j--;) {
    mixed line;
    line = old_explode(tmp[j], " ");
    for(k = sizeof(line); k--;) {
      if(line[k][0..1]=="rn") rn = line[k][3..];
      if(line[k][0..1]=="rt") rt = line[k][3..];
      if(line[k][0..1]=="rg") rg = line[k][3..];
      if(line[k][0..2]=="tid") tid = line[k][4..];
    }
  }
  if(!tid) tid = ""+article[N_DATE];
  return ({ rn, rt, rg, tid });
}

#define RN  0
#define RT  1
#define RG  2
#define TID 3

private int thread(mixed article, int i, mapping t)
{
  mixed info;
  info = tinfo(article);

  if(info[TID]) {
    t[info[TID]]++;
    t[info[TID], 1] = sprintf("%3.3d [%-12s %-6s]%-3s %s\n",
            i+1,
            article[N_AUTHOR]+":",
            dtime(article[N_DATE])[5..11],
            (t[info[TID]] - 1) ? "+"+(t[info[TID]]-1) : " - ",
            make_link(article[N_TITLE],
          article[N_GROUP], to_string(i)))
      + (t[info[TID], 1] ? t[info[TID], 1] : "");
    t[info[TID], 2] = info;
    if(article[N_DATE] > to_int(t[info[TID], 3]))
      t[info[TID], 3] = ""+article[N_DATE];
    return 1;
  }
}

private int expired(mixed *list, int i)
{
  mixed info;
  info = tinfo(list[i]);
  for(i--; i >= 0; i--)
    if(list[i][N_AUTHOR] == info[RN] &&
       list[i][N_DATE] == to_int(info[RT]))
      return 0;
  return 1;
}

string ArticleList(string group)
{
  string *list, ret;
  mapping t;
  int i;

  list = NEWSD->GetNotes(group = old_explode(group, ":")[0]);
  if (!pointerp(list)) {
  return "<H2>Gruppe: "+group+"</H2>"
    "<H3>existiert nicht.</H3>"
    "["+make_link("Gruppen&uuml;bersicht")+"]";
  }
  t = m_allocate(0,4);
  for (i = sizeof(list)-1, ret = ""; i >= 0; i--)
    if(!thread(list[i], i, t) || expired(list, i))
    {
      int ttmp;
      string tid;
      mixed tt; tt = tinfo(list[i]);
      ttmp = t[tid = tt[TID]] - 1;
      ret = sprintf("%3.3d [%-12s %-6s]%-3s %s\n",
		    i+1,
                    list[i][N_AUTHOR]+":",
                    dtime(list[i][N_DATE])[5..11],
                    ttmp > 0 ? "+"+ttmp : " - ",
                    make_link((string)list[i][N_TITLE]
            +(ttmp > 0 ?
        " ("+dtime(to_int(t[tid, 3]))[5..11]
        +dtime(to_int(t[tid, 3]))[17..]+")" : ""),
            group, to_string(i)+":"+t[tid, 3])) + ret;
    }

  return "<H2>Gruppe: "+group+"</H2>"
    +"<H3>["+sizeof(list)+" Artikel, "
    +"letzte &Auml;nderung "+dtime(NEWSD->GetNewsTime(group))+"]</H3>"
    +"<PRE>" + ret + "</PRE>"
    +"["+make_link("Gruppen&uuml;bersicht")+"]";
}

private varargs string Message(string group, mixed article)
{
  mixed text, tmp, ttmp, next, prev, info;
  string art;
  mapping t;
  int i;

  if (!article) article = 0;
  else article = to_int(old_explode(article, ":")[0]);

  tmp = NEWSD->GetNotes(group = old_explode(group, ":")[0]);
  if (pointerp(tmp) && (article >= sizeof(tmp)))
      return("Artikel nicht gefunden, soviele Artikel hat diese Rubrik "
	  "nicht!\n");

  text = tmp[article];

  t = m_allocate(0,4);
  for(i = sizeof(tmp)-1; i > article; i--)
    thread(tmp[i], i, t);
  next = "N&auml;chster Artikel";
  prev = "Voriger Artikel";

  art = implode((ttmp = old_explode(text[N_ARTICLE], "\n"))
                - regexp(ttmp, "^~#!"), "\n");

  art = regreplace(art, "<", "\\&lt;", 1);
  art = regreplace(art, ">", "\\&gt;", 1);
  art = regreplace(art, "([a-zA-Z][a-zA-Z]*://[^ \n\t][^ \n\t]*)", "<a href=\"\\1\">\\1</a>", 1);

  info = tinfo(text);

  return "<H1>" + text[N_TITLE] + "</H1><HR>\n"
       + "<H3>" + group + ": " + text[N_AUTHOR]
       + " (Artikel " + (article + 1) + ", " + dtime(text[N_DATE]) + ")\n</H3>"
       + (info[RN] ? ("<H4>Antwort auf "+info[RN]+
          (expired(tmp, article) ? " (verfallen)" : "")+"</H4>")
    : "")
       + "<PRE>" + art + "</PRE>\n"
       + (t[info[TID]] ?
          "<HR>Weitere Artikel zu diesem Thema:"
  + "<PRE>" + t[info[TID], 1] + "</PRE><HR>" : "")
       + " ["+(article < sizeof(tmp)-1 ? make_link(next, group,to_string(article+1)) :
              next)+"]"
       + " ["+(article ? make_link(prev, group, to_string(article-1)) : prev)+"]"
       + " ["+make_link("Artikel&uuml;bersicht", group)+"]"
       + " ["+make_link("Gruppen&uuml;bersicht")+"]";
}

string Request(mapping cmds)
{
  string text;
  if(!cmds[GROUP]) text = GroupList();
  else
    if(!cmds[ARTICLE]) text = ArticleList(cmds[GROUP]);
    else text = Message(cmds[GROUP], cmds[ARTICLE]);

  return "<H2>"+MUDNAME+" Zeitung</H2><HR>"
        +text;
}
