/* MorgenGrauen MudLib : Pubtool */

#include <defines.h>
#include <properties.h>
#include <language.h>

inherit "/std/thing";

create()
{
    if (!clonep(ME))
        return;
    ::create();

    SetProp(P_SHORT,"Ein Pubtool");
    SetProp(P_LONG,
    "Das Pubtool berechnet die maximal zulaessige Heilung einer Kneipe.\n"+
  "\n"+
  "Syntax: pubtest <P_ALCOHOL> <P_DRINK> <P_FOOD> <P_VALUE> <Rate> <Delay>\n"+
  "\n"+
  "Wertebereich:      >=0         >=0       >=0      >=0    1..20   0..\n"+
  "\n");
    SetProp(P_NAME,"Pubtool");
    SetProp(P_GENDER,NEUTER);
    SetProp(P_WEIGHT,0);
    SetProp(P_VALUE,0); 
    SetProp(P_NODROP,"Das Pubtool behaelst Du lieber bei Dir.\n");
    SetProp(P_NEVERDROP,1);
    SetProp(P_AUTOLOADOBJ,1);

    AddId( "pubtool" );

    AddCmd( "pubtest", "pubtest" );
}

int pubtest(string arg)
{   int v,d,a,f,r,h,y;

    notify_fail(
  "Syntax: pubtest <P_ALCOHOL> <P_DRINK> <P_FOOD> <P_VALUE> <Rate> <Delay>\n");

    if (!arg || !stringp(arg) ||
        sscanf(arg,"%d %d %d %d %d %d",a,d,f,v,r,y)!=6)
        return 0;

    if (v<1 || d<0 || a<0 || f<0 || r<1 || r>20 || y<0)
        return 0;

    h=call_other("/secure/pubmaster","CalcMax",
                 ([P_VALUE:v,P_ALCOHOL:a,P_DRINK:d,P_FOOD:f,
                 "delay":y,"rate":r]),0);

    printf("Berechnete Maximalheilung fuer:\n"+
           "P_ALCOHOL = %3d    P_DRINK   = %3d    P_FOOD    = %3d \n"+
           "P_VALUE   = %3d    Rate      = %3d    Delay     = %3d \n"+
           "Ergebnis:\n"+
           "Gesamt: %3d (HP+SP), aufgeteilt z.B. %3d HP und %3d SP.\n",
           a,d,f,v,r,y,h,(h/2),(h%2?(h/2)+1:(h/2)));
    return 1;
}
