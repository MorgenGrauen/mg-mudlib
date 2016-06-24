// MorgenGrauen MUDlib
//
// thing/language.c -- language handling object
//
// $Id: language.c 8359 2013-02-09 11:43:39Z Zesstra $

#pragma strict_types
#pragma save_types
#pragma range_check
#pragma no_clone
#pragma pedantic

#include <thing/language.h>
#include <thing/description.h>

#define NEED_PROTOTYPES
#include <thing/properties.h>

// Kein create()
void create()
{
  return;
}

//Inversion, damit Defaultwert von 0 einen Artikel ausgibt
static int _query_article() { return !Query(P_ARTICLE); }

static int _set_article(int art) { return !Set(P_ARTICLE,!art); }

// Bestimmten Artikel bestimmen
private string query_c_article(int casus)
{
  if (QueryProp(P_PLURAL))
    return ({ "die ", "der ", "den ", "die "})[casus];

  return ({ ({ "das ", "des ", "dem ", "das " }),
            ({ "der ", "des ", "dem ", "den " }),
            ({ "die ", "der ", "der ", "die " }) })
    [(int)QueryProp(P_GENDER)][casus];
}

// Ende fuer Artikel und Adjektive
private varargs string query_g_suffix(int gen, int casus, int anzahl)
{
  return ({ ({ ({ "","e"}), ({"es","er"}), ({"em","en"}), ({  "","e"}) }),
	    ({ ({ "","e"}), ({"es","er"}), ({"em","en"}), ({"en","e"}) }),
	    ({ ({"e","e"}), ({"er","er"}), ({"er","en"}), ({ "e","e"}) }) })
    [gen][casus][anzahl];
}

// Artikel vorschlagen: gibt es noch mehr Objekte im inv?
varargs int SuggestArticle(string id)
{
  object ob,*obs;

  // Raum oder Master: Bestimmt.
  if (!environment()) return 1;

  // Keine id? Dann raus
  if (!id) return 1;

  // Objekt mit gleichem Namen im env? Dann unbestimmt
  for ( ob=first_inventory(environment()) ; ob ; ob=next_inventory(ob) )
    if ( ob!=this_object()&& id==(string)ob->QueryProp(P_NAME) )
      return 0;

  // sonst bestimmt
  return 1;
}

// Artikel bestimmen
varargs string QueryArticle(int casus, int dem, int force)
{
  // Kein Artikel
  if (!force &&!(QueryProp(P_ARTICLE))) return "";

  // Artikelart aussuchen
  if ( dem==2 ) dem = SuggestArticle(QueryProp(P_NAME));

  // Bestimmter Artikel
  if (dem) return query_c_article(casus);

  // Unbestimmter Artikel
  if (QueryProp(P_PLURAL)) return "";
  
  return sprintf("ein%s ",query_g_suffix((int)QueryProp(P_GENDER),casus));
}

// Besitzanzeiger fuer Objekt bestimmen
varargs string QueryOwn(int casus)
{
  return sprintf("Dein%s",query_g_suffix(QueryProp(P_GENDER),casus));
}

// Possessivpronomen bestimmen
varargs string QueryPossPronoun(mixed what, int casus, int number)
{
  int gen2;

  // Geschlecht ermitteln
  gen2 = (objectp(what)?(int)what->QueryProp(P_GENDER):(int)what);

  // Plural ist schoen einfach
  if (QueryProp(P_PLURAL)) return "ihr"+query_g_suffix(gen2, casus, number);

  return (((((int)QueryProp( P_GENDER ))==FEMALE )? "ihr":"sein")+
          query_g_suffix(gen2%3, casus, number));
}

// Pronomen bestimmen nach KNG
varargs string QueryPronoun(int casus)
{
  int gender;

  // Plural ist immer einfach ...
  if (QueryProp(P_PLURAL)) 
  {
     if (casus==WEM) return "ihnen";
     return "sie";
  }

  switch(QueryProp(P_GENDER))
  {
    case FEMALE:
      if (casus==WESSEN) return "ihrer";
      if (casus==WEM) return "ihr";
      return "sie";

    case MALE:
      if (casus==WER) return "er";
      if (casus==WESSEN) return "seiner";
      if (casus==WEM) return "ihm";
      return "ihn";
  }    
  if (casus==WESSEN) return "seiner";
  if (casus==WEM) return "ihm";
  return "es"; //fall-through
}

// Anrede fuer Spieler bestimmen
varargs string QueryDu(int casus,int gender,int zahl)
{
  return
    ({ ({({    "du",   "ihr"}),({    "du",   "ihr"}),({    "du",   "ihr"})}),
       ({({"deiner",  "euer"}),({"deiner",  "euer"}),({"deiner",  "euer"})}),
       ({({   "dir",  "euch"}),({   "dir",  "euch"}),({   "dir",  "euch"})}),
       ({({  "dich",  "euch"}),({  "dich",  "euch"}),({  "dich",  "euch"})})
     })[casus][gender][zahl];
}

// Welches Geschlecht hat das Objekt als String?
string QueryGenderString()
{
  switch( (int)QueryProp( P_GENDER ))
  {
    case MALE:   return "maennlich";
    case FEMALE: return "weiblich";
  }
  return("saechlich"); //fall-through
}

// Artikel durchdeklinieren nach Kasus, Numerus, Genus und Art
// des Artikels (demon==bestimmt)
varargs string DeclAdj(mixed adj, int casus, int demon)
{
	// Unregelmaessige Adjektive
	if( pointerp(adj) ) return adj[casus]+" ";

  // Falscher Typ? Und Tschuess ...
  if (!stringp(adj)) return "";

  // Plural ist einfach
  if (QueryProp(P_PLURAL))
  {
    // Bestimmt
    if (demon) return adj+"en ";

    // Unbestimmt
    return adj + ({ "e ", "er ", "en ", "e " })[casus];
  }
  
  if ( demon )
    return adj + ({ ({ "e " , "en ", "en ", "e "  }),
		    ({ "e " , "en ", "en ", "en " }),
		    ({ "e " , "en ", "en ", "e "  }) })
      [(int)QueryProp( P_GENDER )][casus];
  else
    return adj + ({ ({ "es ", "en ", "en ", "es " }),
		    ({ "er ", "en ", "en ", "en " }),
		    ({ "e " , "en ", "en ", "e "  }) })
      [(int)QueryProp( P_GENDER )][casus];
}

// P_GENDER setzen. Es sind nur 0,1 und 2 (MALE,FEMALE,NEUTER) erlaubt
static int _set_gender(int i)
{
  if (i>-1&&i<3) return Set(P_GENDER,i);
  return Query(P_GENDER);
}
