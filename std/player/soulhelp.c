// MorgenGrauen MUDlib
//
// player/soulhelp.c -- Hilfe zu den Soulkommandos
//
// $Id: soulhelp.c 7423 2010-02-07 22:56:38Z Zesstra $

#pragma strong_types,save_types

#include <wizlevels.h>
#ifdef WECKE
#undef WECKE
#endif
#define WECKE

static string *wizcmds, *plrcmds, *ghostcmds;
static mapping help;

private string HelpVerb(string v);
private string* SortIt(string *arr);


/**
  Initialisierung
*/
void create()  {
	plrcmds=({
		"zuck", "schmieg", "antworte", "applaudiere",
		"betaste", "cls", "drehe (daeumchen)", "danke",
		"druecke", "erroete", "flippe", "frage",
		"furze", "gaehne", "gluckse", "grinse",
		"guck", "haetschel", "hickse", "huepfe",
		"huste", "keuche", "kichere", "klatsche",
		"knabbere", "knickse", "knirsche", "knurre",
		"knutsche", "kotze", "kuesse", "lache",
		"laechle", "liebe", "nicke", "niese",
		"gib", "pfeife", "ruelpse", "runzle",
		"schmolle", "schmunzle", "schnippe", "schnarche",
		"schnurre", "schreie", "schuettle", "seufze",
		"singe", "sniefe/schniefe", "spucke", "starre",
		"staune", "stiere", "stoehne", "stosse",
		"streichle", "tanze", "tritt", "troeste",
		"umarme", "verneige", "wackle", "waelze",
		"weine", "winke", "zwinkere", "verben",
		"aechze", "erbleiche", "fluche", "verfluche",
		"kitzle", "nein", "deute", // "zeige",
		"denke [text]", "knuddle", "taetschel", "wuschel",
		"strecke ... [die] zunge raus", "kratz",
		"grummel", "jubel / juble ... [zu]", "wuerg",
		"gratuliere / beglueckwuensche", "raeusper",
		"argl", "rotfl", "grunz", "kuschel", "atme ... auf",
		"freue", "sniff", "grueble", "bohre ... [in der nase]",
		"putze [nase]", "bibbere", "quietsche/quieke", "schluchze",
		"schnaufe", "schnaube", "philosophiere", "sabbere",
		"stimme [...] zu", "krieche", "mustere", "schaeme",
		"schnalze ... [zunge]", "stampfe ... [auf]", "zitiere", "lobe",
		"quake", "reibe ... [die] Augen|Haende", "stutze", "schweige",
		"klopfe", "wundere", "stupse", "brummel", "entschuldige",
		"mopper", "zeige", "traeume", "begruesse","jammer",
	});
#ifdef WECKE
	plrcmds+=({"wecke"});
#endif
	wizcmds=({
		"kick", "splash", "flame",
	});
	ghostcmds=({
		"rassel/rassle", "heule", "erschrecke",
	});
	// Aufbau des help-mappings:
	// key    : Verb, wie es in soul.c in CHECK_HELP geschrieben ist
	// entry 1: &n = [<Name>]   &a = [<Adverb>]    &t=[<Text>}
	//          ! danach heisst noetige Angabe, also ohne []
	// entry 2: &g = Verhaelt sich bei Geistern anders
	//          &a = Man kann "alle" als Ziel angeben
	//          &d = Defaultadverb; muss am Ende angegeben werden
	help=([
		"zuck" : "&a [[mit [den]] schultern]"; "Gibt man nicht mindestens "
			+"\"schultern\" an, so zuckt man zusammen. Das Defaultadverb gilt "
			+"nur fuer das Schulterzucken.&dratlos",
		"schmieg" : "&n &a"; "",
		"antwort" : "&n &t!"; "",
		"applaudier" : "&n &a"; "&g&a",
		"begruess" : "&n! &a"; "",
		"betast" : "&n! &a"; "",
		"bibber" : "&a"; "",
		"bohr" : "&a [[in [der]] nase]"; "",
		"brummel" : "&a | &t"; "",
		"cls" : ""; "Loescht den Bildschirm auf vt100-kompatiblen Terminals.",
		"dreh" : "&a [daeumchen | daumen]"; "",
		"dank" : "&n! &a"; "",
		"drueck" : "&n! &a"; "&g&a&dzaertlich",
		"entschuldig" : "&n &a"; "",
		"erroet" : "&a"; "&g",
		"erschreck" : "&n! &a"; "&g&dfuerchterlich",
		"flipp" : "&a"; "&dtotal",
		"frag" : "&n![|]&t!"; "",
		"freu" : "&n &a"; "",
		"furz" : "&a"; "&gHinterlaesst eine duftige Erinnerung im Raum.",
		"gaehn" : "&a"; "&g",
		"glucks" : "&a"; "&dwie ein Huhn",
		"gratulier" : "&n! &a"; "",
		"grins" : "&n &a"; "&g&a",
		"gruebel" : "&a"; "",
		"grummel" : "&a"; "",
		"guck" : "&a"; "",
		"haetschel" : "&n! &a"; "&g&a",
		"hicks" : "&a"; "&g",
		"huepf" : "&n &a"; "&g",
		"hust" : "&n &a"; "&g",
		"jubel" : "&n &a"; "",
		"jammer" : "&a"; "",
		"keuch" : "&a"; "&g&dvor Anstrengung",
		"kicher" : "&n &a"; "",
		"klatsch" : "&a"; "&g",
		"klopf" : "&n! &a"; "",
		"knabber" : "&n! &a"; "&g",
		"knicks" : "&n &a"; "&g&a",
		"knirsch" : "&a"; "&g",
		"knurr" : "&n &a"; "&a",
		"knutsch" : "&n! &a"; "&g",
		"kotz" : "&n &a"; "&g",
		"kriech" : "&n &a"; "",
		"kuess" : "&n! &a"; "&gKann ausserdem bei bestimmten Gelegenheiten "
			+"gewisse Nebeneffekte haben.",
		"lach" : "&n &a"; "&g&a",
		"laechel" : "&n &a"; "&g&a",
		"lieb" : "&n! &a"; "",
		"lob" : "{&n! &a} | &t!"; "Kann einen Nebeneffekt haben.",
		"mopper" : "&a"; "",
		"muster" : "&n! &a"; "",
		"nick" : "&n &a"; "&a&dzustimmend",
		"nies" : "&a"; "&g",
		"gib" : "&n! &a [[eine] ohrfeige]"; "&g&a",
		"pfeif" : "&n &a [nach]"; "&g&danerkennend",
		"philosophier" : "&a | &t"; "",
		"putz" : "&a [[die] nase]"; "",
		"quak" : "&a"; "Geht als Frosch wesentlich besser.",
		"quiek" : "&a"; "&dvergnuegt",
		"raeusper" : "&a"; "",
		"reib" : "&a [[die] Augen] | [[die] Haende]"; "Ohne weitere Angabe reibt "
			+"man sich die Augen;&dmuede bzw. vergnuegt",
		"ruelps" : "&a"; "&g",
		"runzel" : "&a"; "&g",
		"sabber" : "&n &a"; "",
		"schaem" : "&a"; "",
		"schluchz" : "&a"; "&dherzzerreissend",
		"schmoll" : "&a"; "&g",
		"schmunzel" : "&a"; "",
		"schnalz" : "&a [[mit [der]] zunge]"; "",
		"schnaub" : "&a"; "&dentruestet",
		"schnauf" : "&a"; "&dvor Anstrengung",
		"schnipp" : "&a"; "&g",
		"schnarch" : "&a"; "&g&dlaut",
		"schnief" : "&a"; "&g",
		"schnurr" : "&a"; "&dzufrieden",
		"schrei" : "&n &a"; "&g",
		"schuettel" : "&n &a"; "&g&aHat einen Unterschied, je nachdem, ob ein Name"
			+" angegeben wurde oder nicht.",
		"schweig" : "&a"; "",
		"seufz" : "&a"; "&g",
		"sing" : "&a | &t"; "",
		"sniff" : "&n &a"; "&dtraurig",
		"spuck" : "&n &a"; "&g",
		"stampf" : "&a"; "",
		"starr" : "&n &a"; "Standardadverb gilt hier nur bei Angabe eines Namens."
			+"&dvertraeumt",
		"staun" : "&n &a"; "",
		"stier" : "&n &a"; "&g",
		"stimm" : "&n &a"; "",
		"stoehn" : "&a"; "&g",
		"stoss" : "&n! &a"; "&g",
		"streichel" : "&n! &a"; "&g&a",
		"stups" : "&n! &a"; "",
		"stutz" : "&a"; "",
		"tanz" : "&n &a"; "&g",
		"traeum" : "{&n &a} | &t"; "Es koennen Probleme auftreten, wenn ein Adverb"
			+" in freiem Text erkannt wird. Text wird genau so an \"traeum(s)t\" "
			+"angehaengt, wie er angegeben wurde.",
		"tritt" : "&n &a"; "&g&a",
		"troest" : "&n! &a"; "",
		"umarm" : "&n! &a"; "&g&a",
		"verneig" : "&n &a"; "&g&a",
		"wackel" : "&a [mit &t!]"; "&gMan kann mit allem moeglichem wackeln.",
		"waelz" : "&a"; "&dvor Lachen",
		"wart" : "&n &a"; "Man kann auf alles moegliche warten (warte alles "
			+"moegliche)",
		"weck" : "&n {&a | &t}"; "Piepst <Name> an und sendet ihm ggf. den Text.",
		"wein" : "&n &a"; "&g",
		"wink" : "&n &a"; "&a",
		"wuerg" : "&n &a"; "",
		"wunder" : "&n &a"; "",
		"zeig" : "<Objekt-ID>"; "&aZeigt das Objekt (Langbeschreibung).",
		"zitier" : "&n &a"; "Hier uebernimmt das Adverb ggf. die Stelle des "
			+"zitierten, wenn dieser nicht anwesend ist (zitiere Ja /Jof)",
		"zitter" : "&a"; "",
		"zwinker" : "&n &a"; "&g",
		"aechz" : "&a"; "",
		"argl" : "&a"; "",
		"atm" : "&a"; "&derleichtert",
		"bewunder" : "&n! &a"; "",
		"erbleich" : "&a"; "&g",
		"fluch" : "&a"; "&g",
		"grunz" : "&a"; "",
		"kuschel" : "&n! &a"; "&g",
		"rotfl" : "&a"; "",
		"verfluch" : "{&n &a} | &t!"; "",
		"kitzel" : "&n! &a"; "&g",
		"nein" : "&a"; "&g",
		"deut" : "&n! &a"; "&a",
		"denk" : "&a | &t"; "&g",
		"knuddel" : "&n! &a"; "&g&a",
		"kratz" : "&a"; "",
		"streck" : "&n &a [[die] zunge raus"; "&g",
		"taetschel" : "&n! &a"; "&g",
		"wuschel" : "&n! &a"; "",
		"rassel" : "&a"; "&g&dfuerchterlich",
		"heul" : "&a"; "&g&dschauerlich",
		"kick" : "&n &a"; "&g&a",
		"splash" : "&n! &a"; "&a",
		"flame" : "&n! &a"; "&g&a",
		"rknuddel" : "&n! &a"; "Nur fuer Ferngebrauch gedacht.",
		"rwink" : "&n! &a"; "Nur fuer Ferngebrauch gedacht.",
		"verb" : ""; "Listet alle derzeit moeglichen Verben auf.",
		"adverb" : "[# | $ | {? <Abkuerzung>} | <Abkuerzung> [<Adverb>]]";
			"Genauere Hilfe mit \"adverb ?\"",
	]);
}

/**
  Gibt die Hilfe zu einem Verb oder die Verbenuebersicht zurueck
  \param verb   "string"  Ein Verb dessen Hilfe angezeigt werden soll.
                0 oder "" Ueberblicksseite
  \return 	String der an den Spieler ausgegeben werden kann.
*/
string Help(string verb)  {
	string out;
	if (verb && verb!="")
		return HelpVerb(verb);
	out="";
	out+=break_string("Standardverben:\n"+implode(SortIt(plrcmds), ", ")
		+".", 78, 0, 1);
	if (IS_WIZARD(this_player()))
		out+="\n"+break_string("Magierverben:\n"+implode(SortIt(wizcmds), ", ")
			+".", 78, 0, 1);
	if (this_player()->ghost())
		out+="\n"+break_string("Geisterverben:\n"+implode(SortIt(ghostcmds), ", ")
			+".", 78, 0, 1);
	out+=break_string("\nAdverbien koennen entweder in der Abkuerzung (bei "
		+"bereits definierten Adverbien) oder mit \"/<Text>\" angegeben werden. "
		+"Bei der zweiten Methode wird <Text> so, wie er angegeben wurde, an "
		+"Stelle des Adverbs eingesetzt.", 78, 0, 1);
	out+="\n"+break_string("Eine kurze Hilfe zu einem Verb bekommst du mit einem der "
                +"Befehle \"<Verb> -?\", \"<Verb> /?\", \"<Verb> -h\", \"<Verb> /h\" oder "
                +"\"<Verb> hilfe\". (Da war jemand wirklich fleissig).", 78);
	return out+"\n"+break_string("Einige Befehle reagieren auch auf leicht "
		+"unterschiedliche Schreibweise. Diese Befehlsliste kann sich mit Deinem "
		+"Zustand aendern.", 78);
}

/**
  Sortiert ein array of string alphabetisch. 
  \param arr Das zu sortierende Array
  \return    Sortierte Kopie des Arrays
*/
private string*
SortIt(string *arr)  {
  return sort_array(arr, 
           function int (string x, string y){ 
             return lower_case(x) > lower_case(y) ;
           } 
         );
}

#define NO_MORE_E ({"cls","flipp","gruebel","grummel","guck","haetschel",\
  "jubel","laechel","gib","runzel","schmunzel","schuettel","sniff",\
  "streichel","tritt", "wackel","argl","rotfl","kuschel","kitzel","knuddel",\
  "taetschel","wuschel","rassel","kick","splash","flame","verb","adverb",\
  "nein","brummel","mopper",})

/**
  Gibt die Hilfe zu einem Verb 
  \param v     Ein Verb dessen Hilfe angezeigt werden soll.
  \return 	Hilfe zum Verb in Form eines Strings
*/
private string
HelpVerb(string v)  {
	string h,t;
	if (member(m_indices(help),v)<0)
		return "Dazu ist keine Hilfe vorhanden.\n";
	h="Syntax:\n* "+v;
	if (member(NO_MORE_E, v)<0)
		h+="e";
	h+=" ";
	t=help[v,0];
	t=implode(explode(t,"&t!"), "<Text>");
	t=implode(explode(t,"&n!"), "<Name>");
	t=implode(explode(t,"&t"), "[<Text>]");
	t=implode(explode(t,"&n"), "[<Name>]");
	t=implode(explode(t,"&a"), "[<Adverb>]");
	h+=t;
	t=help[v,1];
	if (t!="")  {
		h+="\nBemerkungen:";
		if ((explode(t,"&"))[0]!="")
			t="\n"+break_string((explode(t,"&"))[0], 76, "* ", 1)[0..<2]
				+"&"+implode((explode(t,"&"))[1..<1],"&");
		t=implode(explode(t,"&g"), "\n* Verhaelt sich bei Geistern anders "
			+"als sonst.");
		t=implode(explode(t,"&a"), "\n* Bei diesem Verb geht alle(n) als "
			+"Zielangabe.");
		if (strstr(t,"&d")>=0)  {
			t=implode(explode(t,"&d"), "\n* Standardadverb ist ");
			t+=".";
		}
	}
	return break_string(h+t, 78, 2, 1);
}
