//----------------------------------------------------------------------------
// Name des Objects:    SCHRAENKER
// Letzte Aenderung:    06.04.2006 P_LOG_FILE (Seleven)
// Magier:              Swift
//----------------------------------------------------------------------------
#pragma strong_types,rtt_checks
inherit "/std/room";

#include <properties.h>
#include <language.h>
#include "schrankladen.h"


protected void create()
{
  ::create();
  SetProp(P_LOG_FILE,"seleven/schraenker"); // Seleven 06.04.2006
  SP(P_INT_SHORT, "Im SCHRAENKER");
  SP(P_INT_LONG, BS(
     "Du stehst in einem gemuetlichen kleinen Laden. Ein flauschiger "
    +"Teppichboden daempft Deine Schritte, und der Geruch verschiedener "
    +"Hoelzer kitzelt angenehm in der Nase. Irgendwoher erklingt eine "
    +"leise, beruhigende Melodie. An den Waenden haengen Bilder und "
    +"Zeichnungen verschiedenster Art, sowie eine grosse Preisliste. "
    +"Wenn Du nicht den Eindruck haettest, dass man hier an Deine Boerse "
    +"moechte, koenntest Du Dich hier richtig wohl fuehlen.",78));

  SP(P_LIGHT, 1);
  SP(P_INDOORS, 1);

  AD( ({"zeit"}), BS(
     "Die aktuelle Zeit: "+ctime(time()),78));
  AD( ({"ausgang"}), BS(
     "Nach Osten geht's raus.",78));
  AD( ({"ausgaenge"}), BS(
     "So sehr Du auch suchst, mehr als den einen Ausgang findest Du "
    +"einfach nicht.",78));
  AD( ({"geldboerse", "boerse"}), BS(
     "Man redet halt von Boerse, auch wenn Du keine dabeihast. Es geht "
    +"halt um Dein Geld!",78));
  AD( ({"geld"}), BS(
     "Irgendwo wirst Du schon Kohle haben! :)",78));
  AD( ({"kohle", "zaster", "moneten", "piepen", "maeuse"}), BS(
     "Kohle, Zaster, Moneten, Piepen, Maeuse... alles dasselbe! :)",78));
  AD( ({"boden"}), BS(
     "Ein flauschiger, aber auch sehr strapazierfaehiger Teppich bedeckt "
    +"den ganzen Boden des Ladens. Billig sieht er nicht aus.",78));
  AD( ({"teppich", "teppichboden"}), BS(
     "Es handelt sich um einen echten Vorberg-Teppich. Er wurde "
    +"von einem Fachmann verlegt und scheint aus einem einzigen grossen "
    +"Stueck zu sein.",78));
  AD( ({"stueck"}), BS(
     "Der Teppich scheint aus einem einzigen grossen Stueck zu sein. So gut "
    +"kann das eigentlich nur ein Fachmann.",78));
  AD( ({"fachmann"}), BS("Es ist keiner zu sehen, aber es ist anzunehmen, dass "
    +"es sich um einen professionellen Teppichleger handelte."));
  AD( ({"teppichleger"}), "Wie gesagt, keiner zu sehen. Der Teppich liegt ja "
    +"schon, und der wird auch noch eine Weile halten.\n");
  AD( ({"weile"}), "Schon noch ein paar Jaehrchen!\n");
  AD( ({"jaehrchen"}), "Teppiche der Firma Vorberg koennen lange halten.\n");
  AD( ({"vorberg", "vorbergteppich", "vorberg-teppich"}), BS(
     "Die Firma Vorberg stellt, im Gegensatz zur Firma Hinterberg, sehr gute "
    +"Teppiche her, wie jeder weiss.",78));
  AD( ({"gegensatz"}), "Das weiss wirklich Jeder!\nDu koenntest Ihn fragen, "
    +"waere er nicht gebanisht. :)\n");
  AD( ({"ihn"}), BS("Wen IHN ? Meinst Du Jeder? Der ist doch gebanisht. Aus gutem "
    +"Grund. Wenn irgendwo was wegkommt, dann koennte das Jeder gewesen sein. "
    +"Das reicht doch, um ihn zu banishen, oder? :)"));
  AD( ({"hinterberg", "hinterbergteppich", "hinterberg-teppich"}), BS(
     "Das hier ist ein Teppich der Firma Vorberg. Niemand mit Geschmack wuerde "
    +"den billigen Abklatsch der Firma Hinterberg kaufen.",78));
  AD( ({"jeder"}), BS(
     "Der ist nicht da. Siehst Du ihn etwa?",78));
  AD( ({"grund", "gruende"}), BS(
     "Gruende lassen sich immer finden, sei also vorsichtig! :)",78));
  AD( ({"geschmack"}), BS(
     "Ueber den laesst sich bekanntlich streiten.",78));
  AD( ({"teppiche"}), BS(
     "Du siehst hier einen. Der aber fuellt den ganzen Raum aus.",78));
  AD( ({"abklatsch"}), BS(
     "Vorberg-Teppiche sind halt das einzig Wahre!",78));
  AD( ({"laden"}), BS(
     "Der Laden ist bekannt unter dem Namen \"SCHRAENKER\". Aber das weisst "
    +"Du sicher.",78));
  AD( ({"namen"}), BS(
     "Der Namen des Ladens ist \"SCHRAENKER\".",78));
  AD( ({"schraenker"}), BS(
     "Ja, schau Dich nur um!",78));
  AD( ({"melodie"}), BS(
     "Lausche ihr besser.",78));
  AD( ({"geruch"}), BS(
     "Sehen kannst Du den nicht. Vielleicht solltest Du Deine Nase benutzen?",78));
  AD( ({"schritte"}), BS(
     "Du gehst testweise ein paar Schritte, der Teppich daempft sie tatsaechlich "
    +"so gut, dass Du nicht das geringste hoeren kannst.",78));
  AD( ({"schritt"}), BS(
     "Mach das doch irgendwo, wo es keiner sieht!",78));
  AD( ({"nase"}), BS(
     "Du fasst Dir an die Nase. Ein Glueck, sie ist noch da!",78));
  AD( ({"glueck"}), BS(
     "Ohne Nase koenntest Du hier gar nichts riechen, stell Dir das nur "
    +"mal vor!",78));
  AD( ({"raum", "ort"}), BS(
     "Du stehst in einem noblen Laden. Mach einfach \"schau\".",78));
  AD( ({"firma"}), BS(
     "Du stehst hier in einem Laden. Natuerlich ist das eine Firma, und der "
    +"Ladeninhaber ist ebenso natuerlich der Firmenbesitzer. Hattest Du denn "
    +"etwas anderes erwartet?",78));
  AD( ({"eindruck"}), BS(
     "Es ist ein Laden. Er sieht nobel aus, Preise wie bei ALDI wird's hier also "
    +"wohl nicht geben. Und ausserdem ist der erste Eindruck meist der richtige.",78));
  AD( ({"aldi"}), BS(
     "NEIN, Du bist hier NICHT bei ALDI !!!",78));
  AD( ({"preis", "preise", "liste", "preisliste"}), BS(
     "An der Wand haengt eine grosse Preisliste. Vielleicht solltest Du sie "
    +"lesen?",78));
  AD( ({"inhaber", "ladeninhaber", "besitzer", "firmenbesitzer", "verkaeufer"}), BS(
     "Ist der denn gerade nicht da? Das ist sehr verwunderlich, echt ehrlich.",78));
  AD( ({"elf", "elfen", "person"}), BS(
     "Schau Dir halt das Portrait an!",78));
  AD( ({"darstellung", "darstellungen"}), BS(
     "Die Bilder zeigen Moebelstuecke, hauptsaechlich Schraenke.",78));
  AD( ({"reihe"}), BS(
     "Bei den meisten Bildern handelt es sich um Darstellung von Moebelstuecken. "
    +"Nur ein einziges Portrait ist dabei, das eine Person zeigt.",78));
  AD( ({"art"}), BS(
     "Bei den meisten Bildern handelt es sich um Darstellungen von Moebelstuecken, "
    +"zumeist von Schraenken. Ein Portrait faellt Dir allerdings auf, das nicht in "
    +"die Reihe der anderen Bilder passt.",78));
  AD( ({"bilder"}), BS(
     "Es gibt mehrere Darstellungen von verschiedenen Moebelstuecken. Zumeist "
    +"handelt es sich dabei um Schraenke verschiedener Bauweise. Ausserdem ist "
    +"da noch ein Portrait.",78));
  AD( ({"bauart", "bauweise", "moebelstueck", "moebelstuecke", "arten"}), BS(
     "Es gibt Waffenschraenke, Kleiderschraenke, Kuehlschraenke (JA!), Kommoden, "
    +"sonstige Schraenke (Du kannst ihren Zweck nicht mal erahnen) und einige "
    +"Truhen, die zwar keine Schraenke im eigentlichen Sinne sind, aber Zeug "
    +"bringt man darin mit Sicherheit auch unter. Sogar eine Vitrine siehst "
    +"Du.",78));
  AD( ({"zweck"}), BS(
     "Stauraum fuer irgendwelches Zeugs. Das muss es sein. Was fuer einen "
    +"Zweck koennte es sonst noch geben?",78));
  AD( ({"vitrine"}), BS("Die Vitrine ist leer, aber das aendert sich sicher, "
    +"sobald Du eine gekauft hast.",78));
  AD( ({"kleiderschrank", "kleiderschraenke"}), BS(
     "Sicher eignet sich solch ein Schrank sehr gut, um darin Ruestungen "
    +"aufzubewahren.",78));
  AD( ({"waffenschrank", "waffenschraenke"}), BS(
     "In einem Waffenschrank kannst Du eine Menge verschiedener Waffen "
    +"unterbringen, und der Vorteil dabei ist, dass sie dann auch sauber "
    +"sortiert im Schrank haengen.",78));
  AD( ({"kommoden"}), BS(
     "Die kann man nicht nur in einen Flur stellen, sondern auch in jeden "
    +"anderen Raum!",78));
  AD( ({"flur"}), BS(
     "Die meisten Haeuser haben einen.",78));
  AD( ({"haeuser"}), BS(
     "HIER ???",78));
  AD( ({"menge"}), BS(
     "Mehr als Du brauchst, es sei denn Du bist Sammler.",78));
  AD( ({"zeugs", "dinge"}), BS(
     "Dinge halt.",78));
  AD( ({"waffen"}), BS(
     "Du kannst sie in einem Waffenschrank unterbringen, wenn Du einen "
    +"besitzt. Wenn nicht kannst Du ja einen kaufen.",78));
  AD( ({"waffe"}), BS(
     "Welche Waffe meinst Du?",78));
  AD( ({"ruestungen"}), BS(
     "Du kannst sie in einem Kleiderschrank unterbringen, wenn Du einen "
    +"besitzt. Wenn nicht koenntest Du Dir einen zulegen.",78));
  AD( ({"ruestung"}), BS(
     "Von welcher Ruestung redest Du?",78));
  AD( ({"vorteil"}), BS(
     "Jeder ist doch auf seinen persoenlichen Vorteil bedacht.",78));
  AD( ({"kuehlschrank", "kuehlschraenke"}), BS(
     "In so einem Kuehlschrank kann man eigentlich alles reinstecken, aber "
    +"natuerlich bietet er sich besonders fuer Getraenke und Speisen aller "
    +"Art an. Wie z.B. Schnellhaerter, Broetchen usw.",78));
  AD( ({"getraenke", "speisen"}), BS(
     "Das hier ist keine Kneipe. Sowas gibt's hier nicht!",78));
  AD( ({"kneipe"}), BS(
     "Das hier ist KEINE Kneipe !!!",78));
  AD( ({"truhe", "truhen"}), BS(
     "Es gibt grosse und kleine, aus verschiedenem Material.",78));
  AD( ({"material"}), BS(
     "Total verschieden, wirklich.",78));
  AD( ({"schnellhaerter", "broetchen"}), BS(
     "Besorg Dir welche, bevor Du sie untersuchen willst!",78));
  AD( ({"sammler"}), BS(
     "Bist Du einer?",78));
  AD( ({"sicherheit"}), BS(
     "Jo, in so eine Truhe passt viel Zeug rein.",78));
  AD( ({"zeug"}), BS(
     "Alles, was Du so mit Dir rumschleppst. Das meiste davon brauchst "
    +"Du sowieso nie.",78));
  AD( ({"bild"}), BS(
     "Es gibt mehrere Darstellungen von verschiedenen Moebelstuecken und ein "
    +"Portrait. Welches Bild meinst Du?",78));
  AD( ({"portrait"}), BS(
     "Es zeigt einen Elf mit strahlend blauen Augen und den typisch spitzen "
    +"Elfenohren, die frech aus einer beeindruckenden, silbergrauen Loewenmaehne "
    +"hervorschauen. Der Spitzhut, der ihm auf dem Kopf sitzt, laesst darauf "
    +"schliessen, dass es sich um einen Zauberer handelt. Unten in der rechten "
    +"Bildecke ist eine Widmung angebracht.",78));
  AD( ({"sinn", "sinne"}), BS(
     "Schraenke und Truhen sehen schon etwas veschieden aus.",78));
  AD( ({"swift"}), BS(
     "Siehst Du ihn hier etwa gerade?",78));
  AD( ({"bildecke"}), BS(
     "Unten in der rechten Bildecke ist eine Widmung angebracht.",78));
  AD( ({"augen"}), BS(
     "Der Elf auf dem Portrait hat strahlend blaue Augen.",78));
  AD( ({"widmung"}), BS(
     "Sieht aus als ob man die sogar lesen kann!",78));
  AD( ({"maehne", "loewenmaehne"}), BS(
     "Der Elf auf dem Bild hat eine silbergraue Loewenmaehne.",78));
  AD( ({"zauberer", "zauberhut", "zaubererhut", "spitzhut"}), BS(
     "Der Zauberhut macht den Elfen auf dem Portrait erst zu einem waschechten "
    +"Zauberer.",78));
  AD( ({"ohren", "elfenohren"}), BS(
     "Die Ohren des Elfen auf dem Portrait sind so spitz, wie man das von "
    +"einem Elfen erwartet.",78));
  ARD( ({"widmung"}),
     "Dank an Geordi fuer diese grossartige Geschaeftsidee.\n"
    +"Moegen Dir Deine Zauber gelingen und die Spellpunkte niemals ausgehen.\n"
    +"(Darunter hat jemand unterschrieben, aber das Gekrakel kannst Du nicht "
    +"identifizieren)\n");
  AD( ({"unterschrift"}), BS(
     "Meinst Du das Gekrakel?",78));
  AD( ({"rotfaerbung", "farbe"}), BS(
     "Das Holz hat eine leicht roetliche Farbe.",78));
  AD( ({"ruhe"}), BS(
     "Aaaahhhhh. Ruhe tut gut. Und dank der leisen Melodie entspannst Du "
    +"Dich allmaehlich.",78));
  AD( ({"chaosdaemon"}), BS(
     "Keiner da. Weder Swift noch ein anderer. Glueck fuer Dich!",78));
  AD( ({"sachen"}), BS(
     "Was auch immer.",78));
  AD( ({"klaue"}), BS(
     "Wahrscheinlich kann Swift damit andere Sachen besser erledigen als "
    +"schreiben. Schliesslich ist er ein Chaosdaemon.",78));
  AD( ({"gekrakel"}), BS(
     "Du nimmst Dir Zeit und schaust die Unterschrift doch nochmal in aller "
    +"Ruhe an. Endlich kommt Dir die Erleuchtung. Das soll \"Swift\" heissen. "
    +"Mann, hat der ne Klaue! (Naja, bei einem Chaosdaemonen wundert das "
    +"eigentlich nicht)",78));
  AD( ({"erleuchtung"}), BS(
     "Sie fand in Deinem Kopf statt. Den muesstet Du schon oeffnen, wenn Du "
    +"die Erleuchtung physisch untersuchen moechtest. Und das kann sich "
    +"negativ auf Deine Gesundheit auswirken. Lass es lieber!",78));
  AD( ({"gesundheit"}), BS(
     "Mit Kopf bist Du gesund. Das ist gewiss.",78));
  AD( ({"wand", "waende"}), BS(
     "Die Waende bestehen aus einem feinen, exotischen Holz mit leichter "
    +"Rotfaerbung. Sieht sehr edel aus. Mehrere Bilder wurden an den Waenden "
    +"aufgehaengt und verteilen sich gleichmaessig ueber den Raum.",78));
  AD( ({"kopf"}), BS(
     "Ein Glueck, er ist noch dran. Genauso wie bei dem Elfen auf dem "
    +"Portrait!",78));
  AD( ({"schrank", "schraenke"}), BS(
     "Es gibt verschiedene Arten von Schraenken.",78));
  AD( ({"zeichnung"}), BS(
     "Es gibt mehrere davon.",78));
  AD( ({"zeichnungen"}), BS(
     "Sie zeigen - teilweise skizzenhaft - den Aufbau verschiedener "
    +"Moebelstuecke - vorzugsweise Schraenke.",78));
  AD( ({"aufbau", "zusammensetzung"}), BS(
     "Mit dem Aufbau ist die Zusammensetzung gemeint - und umgekehrt.",78));
  AD( ({"holz", "hoelzer"}), BS(
     "Schon hier im Raum wurde mit verschiedensten Hoelzern gearbeitet, "
    +"ihr edler Duft vermischt sich und bietet einen erhebenden Geruch. "
    +"Leider sind die meisten Holzarten so exotisch, dass Du beim besten "
    +"Willen nicht sagen kannst, um was fuer einen Baum es sich da mal "
    +"gehandelt hat.",78));
  AD( ({"holzart", "holzarten"}), BS(
     "Sehr exotische Holzarten. Du bist kein Experte fuer sowas, und "
    +"vermutlich hast Du auch keine Lust, einer zu werden.",78));
  AD( ({"experte"}), BS(
     "Keiner da.",78));
  AD( ({"lust"}), BS(
     "Wenn jemand seine Lust untersucht/ueberprueft, bekundet er damit "
    +"eigentlich schon in Wirklichkeit seine Unlust.",78));
  AD( ({"unlust"}), BS(
     "Tja, Unlust kennzeichnet den Faulpelz.",78));
  AD( ({"faulpelz"}), BS(
     "Du bist gemeint!",78));
  AD( ({"wirklichkeit", "realitaet"}), BS(
     "Die Wirklichkeit ist die Realitaet - und andersrum. Meistens "
    +"jedenfalls.",78));
  AD( ({"meistens"}), BS(
     "Hier schon.",78));
  AD( ({"hier"}), BS(
     "Hier halt!",78));
  AD( ({"baum", "baeume"}), BS(
     "Hier gibt es keine Baeume. Die wurden bereits alle verarbeitet.",78));
  AD( ({"duft"}), BS(
     "Rieche halt mal!",78));
  AD( ({"wille", "willen"}), BS(
     "Dein Wille ist stark...",78));
  AD( ({"decke"}), BS(
     "Maechtige Balken bilden die Decke. Es koennte sich um Eiche handeln, aber "
    +"genausogut koennte es irgendeine aehnliche, exotische Holzart sein. "
    +"Ein herrlicher Kronleuchter haengt von der Mitte herab.",78));
  AD( ({"eiche", "eichenbalken"}), BS(
     "Es ist nicht sicher, dass es sich um Eichenbalken handelt, aber Balken "
    +"sind's auf jeden Fall.",78));
  AD( ({"fall"}), BS(
     "Echt ehrlich.",78));
  AD( ({"balken"}), BS(
     "Sie bilden die Decke. Ein herrlich anzusehender Kronleuchter haengt "
    +"daran herab.",78));
  AD( ({"kronleuchter"}), BS(
     "Er haengt von der Decke herab und muss ein Vermoegen wert sein. Reines "
    +"Bergkristall mit einer Unzahl von Kerzen. Was das fuer ein Aufwand sein "
    +"muss, die immer am Brennen zu halten.",78));
  AD( ({"vermoegen"}), BS(
     "Mehr als Du tragen kannst.",78));
  AD( ({"bergkristall"}), BS(
     "Nur Bergkristall hat diesen Glanz.",78));
  AD( ({"brennen"}), BS(
     "Die Kerzen flackern munter vor sich hin.",78));
  AD( ({"unzahl"}), BS(
     "1, 2, ...viele!",78));
  AD( ({"glanz"}), BS(
     "Herrlich!",78));
  AD( ({"mitte"}), BS(
     "Von der Mitte der Decke haengt ein Kronleuchter herab.",78));
  AD( ({"kerzen"}), BS(
     "Dir faellt auf, dass die Kerzen sich gar nicht verbrauchen. Es muss "
    +"sich um magische Kerzen handeln.",78));
  AD( ({"aufwand"}), BS(
     "Entsetzt denkst Du . o O (Das schlaegt sich bestimmt auf den Preis nieder.)",78));
  AD( ({"ecken"}), BS(
     "In den Ecken entdeckst Du kleine, edle Lautsprecher, aus denen die "
    +"Melodie zu kommen scheint.",78));
  AD( ({"lautsprecher"}), BS(
     "Du wusstest gar nicht, dass sowas im MorgenGrauen schon erfunden wurde.",78));
  AD( ({"morgengrauen"}), BS(
     "Tust Du das nicht schon die ganze Zeit?",78));

  AddSmells(SENSE_DEFAULT, "Es riecht hier angenehm nach verschiedenen "
                          +"Hoelzern.\n");
  AddSmells(({"hoelzer", "holz", "holzarten"}), BS("Du kannst nicht sagen, "
    +"um was fuer Holzarten es sich handelt, aber der Geruch laesst auf viele "
    +"verschiedene schliessen. Eine Trennung des Geruchs ist Dir nicht moeglich.") );
  AddSmells(({"geruch", "gerueche"}), BS("Es riecht nach verschiedenen "
    +"Holzarten.") );

  AddSounds(SENSE_DEFAULT, "Ganz leise hoerst Du eine angenehme, beruhigende "
                          +"Melodie.\n");
  AddSounds(({"melodie"}), "Sie scheint von allen Ecken des Ladens zu kommen.\n");

  AddItem( LADEN("verkaeufer"), REFRESH_DESTRUCT);

  AddCmd(({"oeffne"}), "kopf_oeffnen");
  AddCmd(({"lies", "les", "lese"}), "preisliste_lesen");
  AddCmd(({"idee"}), "idee_loggen");
  AddCmd(({"trenn", "trenne"}), "gerueche_trennen");
  AddCmd(({"werd", "werde"}), "experte_werden");
  AddCmd(({"zaehl", "zaehle"}), "kerzen_zaehlen");

  AddSpecialExit("osten", "rausgehen");
}

int kerzen_zaehlen(string str)
{
  notify_fail("Was moechtest Du zaehlen?\n");
  if(!str) return 0;
  if(str!="kerzen") return 0;
  write("1..2..viele.\n");
  return 1;
}

int rausgehen(string dir)
{
  return TP->move(RAUM_VOR_LADEN, M_GO, 0,
    "verlaesst den Laden", "kommt aus dem Laden");
}

varargs string GetExits( object viewer )
{
  return "Der Ausgang befindet sich im Osten.\n";
}

int kopf_oeffnen(string str)
{
  notify_fail("Was moechtest Du oeffnen?\n");
  if(!str) return 0;
  if(str!="kopf" && str!="meinen kopf" && str!="deinen kopf")
    return 0;
  write( BS("Neugierig, wie Du bist, greifst Du Dir an den Kopf. Da faellt Dir "
    +"ein, dass das oeffnen Deines Kopfes evtl. eine Copyright-Verletzung "
    +"darstellen koennte, Du heisst ja schliesslich nicht Hannibal. Also laesst "
    +"Du es dann doch bleiben."));
  return 1;
}

int preisliste_lesen(string str)
{
  notify_fail("Was moechtest Du lesen?\n");
  if(!str) return 0;
  if(str!="preisliste" && str!="liste")
    return 0;
  TP->More( read_file(LADEN("preisliste.txt")) );
  return 1;
}

int idee_loggen(string str)
{
  string txt;
  str=TP->_unparsed_args();
  txt=BS(str, 78, CAP(geteuid(TP))+"............."[0..12], BS_INDENT_ONCE);
  write_file( LADEN(IDEEN_LOG), txt+"\n");
  write("Vielen Dank fuer Deine Idee!\n");
  return 1;
}

int gerueche_trennen(string str)
{
  notify_fail("Was willst Du trennen?\n");
  if(!str) return 0;
  if(str!="geruch" && str!="gerueche")
    return 0;
  write("Du versuchst es, aber Du schaffst es einfach nicht.\n");
  return 1;
}

int experte_werden(string str)
{
  notify_fail("Was willst Du werden?\n");
  if(!str) return 0;
  if(str!="experte" && str!="fachmann")
    return 0;
  write("Tja, so einfach geht das nicht, aber das haettest Du Dir denken koennen.\n");
  return 1;
}
