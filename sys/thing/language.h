// MorgenGrauen MUDlib
//
// thing/language.h -- language handling
//
// $Id: language.h 6330 2007-05-26 17:53:21Z Zesstra $
 
#ifndef __THING_LANGUAGE_H__
#define __THING_LANGUAGE_H__

// properties
#define P_ARTICLE            "article"
#define P_GENDER             "gender"
#define P_PLURAL             "plural"

// special defines

#define WER 	0
#define WESSEN 	1
#define WEM 	2
#define WEN 	3
#define RAW     99   // name without article

#define NEUTER 		0
#define MALE 		1
#define FEMALE 		2

#define SINGULAR 	0
#define PLURAL 		1

#endif // __THING_LANGUAGE_H__

#ifdef NEED_PROTOTYPES

#ifndef __THING_LANGUAGE_H_PROTO__
#define __THING_LANGUAGE_H_PROTO__

// prototypes

// Artikel im passenden Fall sowie demonst. bzw undemonst. zurueck-
// geben. force ueberschreibt das SetArticle-Flag.
public varargs string QueryArticle(int casus, int dem, int force);

// Empfehle einen Artikel fuer das Objekt, getestet wird, ob ausser
// diesem Objekt sich ein anderes Objekt mit der id im selben
// Raum befindet.
public varargs int SuggestArticle(string id);

// Gib ein Pronomen zurueck, welches ausdrueckt, das ein Objekt diesem
// Objekt gehoert. Dabei ist what das Geschlecht des Objektes.
public varargs string QueryPossPronoun(mixed what, int casus, int number);

// Er/Sie/Es u.ae. zurueckgeben.
public varargs string QueryPronoun(int casus);

// Du im passenden Fall zurueckgeben.
public varargs string QueryDu(int casus,int gender, int zahl);

// Geschlecht in Worten zurueckgeben (maennlich, weiblich, saechlich)
public string QueryGenderString();

// Deklination eines Adjektivs
public varargs string DeclAdj(mixed adj, int casus, int demon);

// Query- und Set-Methoden
static int _set_article(int fl);
static int _query_article();
static int _set_gender(int i);

// Interna
static int QueryAFlag();

#endif // __THING_LANGUAGE_H_PROTO__

#endif // NEED_PROTOYPES





