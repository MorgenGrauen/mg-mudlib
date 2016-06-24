#include <defines.h>
#include <properties.h>
#include <moving.h>
//
// By Rumata@MorgenGrauen 3/99
//
// Beispieldatei fuer die Benutzung von Mastern und Klienten.
//
// Ich gehe hier nicht auf die "normalen" Funktionen ein.

inherit "/std/thing";

#define OS_MASTER "/doc/beispiele/master/opferstockmaster"

void create()
{
	if(IS_BLUE(ME)) return;
	::create();
	SetProp( P_NAME, "Opferstock" );
	SetProp( P_GENDER, MALE );
	SetProp( P_VALUE, 1000 + random(2000) );
	AddId( ({"stock","inschrift","opferstock"}) );
	SetProp( P_SHORT, "In einer Ecke steht ein Opferstock" );
	SetProp( P_LONG,
		"Der Opferstock besteht aus solidem Holz. Vorne auf dem Kasten ist eine\n"
	+	"Inschrift zu sehen, die Du lesen kannst.\n"
	+	"@@contents@@"
	);
	SetProp( P_READ_MSG,
		">>>>>>>>>>>> Fuer den Aufbau eines Orkwaisenhauses <<<<<<<<<<<<\n"
	+	"In den letzten Jahren wurden immer wieder unschuldige Orkkinder\n"
	+	"durch brutale Abenteurer ihrer Eltern beraubt. Bitte unter-\n"
	+	"stuetzen Sie mit einer kleinen Spende den Aufbau eines Waisen-\n"
	+	"hauses fuer diese bemitleidenswerten Kreaturen.\n"
	);
	AddDetail( "holz", "Solide und so gut wie unzerbrechlich.\n" );
	SetProp( P_NOGET,
		"Der Opferstock ist nicht ohne Grund am Boden festgenagelt.\n" );
	AddCmd( "spende","spende" );
	AddCmd( ({"stecke","steck"}), "stecken" );
	AddCmd( "oeffne","oeffne" );
	AddCmd( ({"brich","breche"}),"breche" );
}

contents()
{
	switch(QueryProp(P_VALUE))
	{
	case 0:
		return "Er ist leer.\n";
	case 1:
		return "Er enthaelt:\nEine Muenze.\n";
	default:
		return "Er enthaelt:\n"+QueryProp(P_VALUE)+" Muenzen.\n";
	}
}

stecken( str )
{
	string was, worein;
	if( !str || sscanf(str,"%s in %s",was,worein)!=2 || !id(worein) ) return 0;
	return spende( str );
}

spende( str )
{
	int anz, newAl;
	string arg;
	
	notify_fail( "Wieviele Muenzen willst Du denn spenden?\n" );
	if( !str || str=="" )
		return 0;
	if( sscanf(str,"%d %s",anz,arg)== 2 )
		str = arg;
	else
	{
		if( str=="eine muenze" || str=="ein goldstueck" )
		{
			anz = 1;
			str = "muenze";
		}
		else
			return 0;
	}
	if( anz<=0 || 
		member(({"muenze","goldstueck","muenzen","goldstuecke"}), str)==-1
	)
		return 0;
	if( anz>PL->QueryMoney() )
	{
		write( "So viel Geld hast Du nicht.\n" );
		return 1;
	}
	PL->AddMoney(-anz);
	SetProp(P_VALUE,QueryProp(P_VALUE)+anz);

	// Hier wird der Master aufgerufen, der das Alignment der Spieler
	// dann aendert.
	if( OS_MASTER->addAlignment( PL, anz/3 ) > 0 ) {
		// Eigentlich könnte man hier auch die Meldung ausgeben, aber
		// der Spieler soll den Unterschied zwischen Alignment geaendert
		// unt Alignment nicht geaendert sehen koennen.
	}
	write( "Du hast wahrhaft das Gefuehl, etwas Gutes getan zu haben.\n" );

	say( capitalize(PL->name(WER))+" spendet "+anz
		+ ((anz==1)?" Muenze.\n":" Muenzen.\n") );
	return 1;
}

oeffne( str )
{
	int newAl;
	
	notify_fail( "WAS willst Du oeffnen?\n" );
	if( !id(str) )
		return 0;
	OS_MASTER->addAlignment( PL, -30 );
	write( "Allein schon der Gedanke....\n" );
	return 1;
}

breche( str )
{
	string arg;
	notify_fail( "WAS willst Du aufbrechen?\n" );
	if( !str )
		return 0;
	if( sscanf(str,"%s auf",arg)==1 )
		str = arg;
	if( !id(str) )
		return 0;
	write( "Dein lautes Getoese ruft einen Teufel herbei, der Dich gleich\n"
	+	"mit in die Hoelle nimmt.\n"
	);
	say( capitalize(PL->name(WER))+" versucht, den Opferstock aufzubrechen.\n"
	+ "Gleich erscheint ein Teufel, um "+PL->QueryPronoun(WEN)
	+	" in die Hoelle zu reissen.\n" );
	PL->move("/d/unterwelt/raeume/qualenraum",M_GO,"zur Hoelle","faehrt");
	OS_MASTER->addAlignment( PL, -200 );
	return 1;
}
