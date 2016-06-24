#ifndef _STD_HEADERS_
#define _STD_HEADERS_

//LANGUAGE
void SetArticle( int fl );
static int QueryAFlag();
public varargs string QueryArticle( int casus, int dem, int force );
public varargs int SuggestArticle( string myname );
public varargs string QueryPossPronoun( int what, int casus, int number );
public string QueryPronoun( int casus );
public varargs string QueryDu(int casus,int gender,int zahl);
public string QueryGenderString();
public varargs string DeclAdj( string adj, int casus, int demon );

#endif
