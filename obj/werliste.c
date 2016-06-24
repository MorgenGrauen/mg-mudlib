#pragma strong_types

inherit "/std/user_filter";

#include <properties.h>
#include <wizlevels.h>
#include <ansi.h>
#include <werliste.h>

mixed *erwarte;

#define MIN(a,b)    (((a) > (b)) ? (b) : (a))
#define ABS(a)      (((a) < 0) ? -(a) : (a))


// als Funktion statt als define, da als lfun-Closure genutzt
private int is_alph_greater( object a, object b )
{
    return geteuid(a) > geteuid(b);
}


// keine Ahnung, wer sich _die_ Berechnungsgrundlage ausgedacht hat.
// Aber da sie schon uralt ist, bleibt sie auch drin ;-)
private int is_greater( object a, object b )
{
    int a1, b1;
    
    a1 = 26000 * query_wiz_level(a) +
        26 * MIN( 2121, ABS(a->QueryProp(P_LEVEL)) ) + getuid(a)[0]-'a';
    
    b1 = 26000 * query_wiz_level(b) +
        26 * MIN( 2121, ABS(b->QueryProp(P_LEVEL)) ) + getuid(b)[0]-'a';
    
    return a1 > b1;
}


private string stat( object ob, int ist_magier )
{
    int l;
  
    l = query_wiz_level(ob);
    
    if ( ob->Query(P_TESTPLAYER) ){
        if ( l < SEER_LVL )
            return "t";
        else if ( l < LEARNER_LVL )
            return "T";
    }
    
    if ( ob->Query(P_SECOND)) {
      if (ob->Query(P_SECOND_MARK) > -1 ){
        if ( l < SEER_LVL )
            return "z";
        else if ( l < LEARNER_LVL )
            return "Z";
      } else
      if (ist_magier) {
        if ( l < SEER_LVL )
            return "n";
        else if ( l < LEARNER_LVL )
            return "N";
      }
    }

    switch(l){
    case GOD_LVL .. 666:
        return "G";
    case ARCH_LVL .. GOD_LVL-1:
        return "E";
    case ELDER_LVL .. ARCH_LVL-1:
        return "W";
    case LORD_LVL .. ELDER_LVL-1:
        return "R";
    case SPECIAL_LVL .. LORD_LVL-1:
        return "H";
    case DOMAINMEMBER_LVL .. SPECIAL_LVL-1:
        return "M";
    case WIZARD_LVL+1 .. DOMAINMEMBER_LVL-1:
        return "m";
    case LEARNER_LVL .. WIZARD_LVL:
        return "L";
    case SEER_LVL .. LEARNER_LVL-1:
        return "S";
    default:
        return "s";
    }
    return "s";
}


private string idlestat( object ob )
{
    switch( query_idle(ob) ){
    case 1800 .. 7199:
        return "j";
    case 600 .. 1799:
        return "I";
    case 120 .. 599:
        return "i";
    case 0 .. 119:
        return ".";
    default:
        return "J";
    }
    return ".";
}


private string wegstat( object ob )
{
    if ( ob->QueryProp(P_AWAY) )
        return "w";
    
    return ".";
}


private string hcstat( object ob )
{
    int hcstat;
    
    hcstat=ob->query_hc_play();
    
    if(!hcstat)
    {
      return ".";
    }
    
    if(hcstat>1)
        return "+";
    
    return "c";

}


private string get_location( object ob )
{
    string res;

    if ( stringp(res = ob->Query(P_LOCATION)) )
        res = capitalize(res) + " (ueber " + country(ob) + ")";
    else
        res = country(ob);

    return res;
}


varargs private mixed
QueryView( object ob, int ist_magier, int sh,
           int obs, string tty, string opt )
{
    string race, ret, tmp, prefix, a_prefix, normal;

    if ( !ist_magier && ob->QueryProp(P_INVIS) )
        return "";
    
    prefix = "";
    a_prefix = "";
        
    ret = capitalize(getuid(ob));
    
    switch(tty){
    case "ansi":
        if ( IS_LEARNER(ob) )
            a_prefix = ANSI_UNDERL;
        
        if ( ob->QueryProp(P_FROG) )
            a_prefix += ANSI_GREEN;

        if ( pointerp(erwarte) && member( erwarte, ret ) != -1 )
            a_prefix += ANSI_RED;
        
        normal = ANSI_NORMAL;
        break;
        
    default:
        if ( pointerp(erwarte) && member( erwarte, ret ) != -1 )
            prefix = "*";
        else
            prefix = "";

        normal = "";
    }

    if ( ret == ROOTID )
        ret = "Logon";

    if ( sh ){ // kwer
        if ( ob->QueryProp(P_INVIS) )
            ret = sprintf( "%s(%-11'.'s).%s%s", a_prefix, prefix + ret,
                         hcstat(ob) + idlestat(ob) + wegstat(ob) + stat(ob,ist_magier), normal );
        else
            ret = sprintf( "%s%-13'.'s.%s%s", a_prefix, prefix + ret,
                         hcstat(ob) + idlestat(ob) + wegstat(ob) + stat(ob,ist_magier), normal );
    } 
    else if ( opt == "k" ){ // wer -k
        ret = sprintf( "[%s] %-13s %15s / %-20s %22s", stat(ob,ist_magier),
                       ob->QueryProp(P_INVIS) ? "(" + ret + ")" : ret,
                       stringp(race = ob->QueryProp(P_RACE)) ?
                       capitalize(race)[0..14] : "<keine Rasse>",
                       stringp(race = ob->QueryProp(P_GUILD)) ?
                       capitalize(race)[0..19] : "<keine Gilde>",
                       "[" + country(ob)[0..19] + "]" );
    } 
    else { // wer, wer -o, wer -s
        if ( tmp = ob->short() ){
            ret = tmp[0..<3];

            if ( ob->QueryProp(P_TEAM) )
                ret = ob->TeamPrefix() + ret;
        }
        else
            ret = "(" + ret + ")";

        if ( !IS_LEARNER(ob) && ob->Query(P_TESTPLAYER) )
            tmp = " <TestSpieler>";
        else
            tmp = "";

        if ( !IS_LEARNER(ob) && ob->QueryProp(P_SECOND) &&
             ob->QueryProp(P_SECOND_MARK) > -1 )
            tmp += " <Zweitie>";
        
        ret = sprintf( "%s%s - %s%s%s",
                       (opt == "o") ? "" : "[" + stat(ob,ist_magier) + "] ", ret,
                       stringp(race = ob->QueryProp(P_RACE)) ?
                       capitalize(race) : "<keine Rasse>",
                       (opt == "s") ? " - [" + get_location(ob) + "]" :
                       (stringp(race = ob->QueryProp(P_GUILD)) ?
                        "/" + capitalize(race) : "/<keine Gilde>"),
                       (opt == "o") ? tmp : "" );
    }
            
    if ( obs )
        return ({ ob, ret });
    else
        return ret;
}


private object *make_unique( object* arr )
{
    int i;

    // muss ueber eine Schleife gemacht werden, um die Reihenfolge zu erhalten!
    for ( i = sizeof(arr); i-- > 1; )
        if( arr[i] == arr[i-1] )
            arr[i] = 0;

    return arr - ({ 0 });
}


#define OFFSET(sp)  ((maxi/4) * sp + ((sp <= maxi%4) ? sp : maxi%4))

// diese Funktion wird von aussen aufgerufen fuer die Befehle "wer" & Co
public varargs string
*QueryWhoListe( int ist_magier, int short, int obs, string arg, int old )
{
    object *pl;
    string *pls, *s, opt;
    int i, j, k, l, *t, maxi;

    erwarte = 0;
    
    if ( short & WHO_SHORT ){
        if ( previous_object() )
            erwarte = previous_object()->QueryProp(P_WAITFOR);
        
        if ( !pointerp(erwarte) )
            erwarte = ({});
    }

    // Wer Spielersicht moechte, soll Spielersicht bekommen
    if ( short & WHO_PLAYER_VIEW )
    {	    
	    ist_magier = 0;
    }

    
    if ( old ){
        opt = "o";
        arg = 0;
    }
    else if ( arg == "k" || arg == "s" ){
        opt = arg;
        arg = 0;
    }
    else if ( sizeof(arg) == 1 ) // (noch) unbekannte Option
        arg = 0;
  
    pl = filter_users(arg);
    
    if ( short & WHO_ALPHA )
        pl = sort_array( pl, #'is_alph_greater/*'*/ );
    else
        pl = sort_array( pl, #'is_greater/*'*/ );
    
    pl = make_unique( pl );
    
    pls = map( pl, #'QueryView/*'*/, ist_magier, short, obs,
                     previous_object()->QueryProp(P_TTY), opt ) - ({""});
    
    if ( !(short & WHO_SHORT) )
        return pls;
    
    maxi = sizeof(pls);
    s = allocate( maxi / 4 + (maxi % 4 ? 1 : 0) );
    
    if ( short & WHO_VERTICAL ) { // senkrecht sortieren !
        // wird so nur einmal berechnet
        t = ({ 0, OFFSET(1), OFFSET(2), OFFSET(3), maxi });
        
        for ( i = 0, l = sizeof(s); i < l; i++ ) {
            s[i] = "";
            
            for ( j = 0; j < 4; j++ ) {
                if ( (k = i+t[j]) < t[j+1] && pls[k] != "" )
                    s[i] += pls[k];
                
                if( j < 3 )
                    s[i] += " ";
            }
        }
    }
    else {
        for ( i = 0; i < maxi; i++ ) {
            if ( !s[i/4] )
                s[i/4] = "";
            
            if ( pls[i] != "" )
                s[i/4] += pls[i];
            
            if ( (i%4 != 3) && i < maxi-1 )
                s[i/4] += " ";
        }
    }

    return ({ sprintf( "%|78s", "Liste der Mitspieler vom " + dtime(time()) +
                     sprintf( " (%d Teilnehmer)", maxi )), "" }) + s;
}


public varargs int query_prevent_shadow( object ob )
{
    return 1;
}
