/* -*- lpc -*- */
//--------------------------------------------------------------------------
//
//   morph.c
//
//   (c) Troy (troy@mg.mud.de)
//   Kopieren, Veraendern oder Weitergabe: na klar, immer zu, je schlimmer 
//   um so besser
//
//   Objekt erstellt: 14.08.01, Troy
//
//   Dieser shadow implementiert generische Verwandlungen. Im Gegensatz oder
//   in Ergänzung zum Tarnhelm sind diese nicht auf die Beschreibung
//   beschränkt, sondern schlagen sich auch in anderen Properties nieder.
//
//--------------------------------------------------------------------------

#include <moving.h>
#include <properties.h>
#include <wizlevels.h>

//--------------------------------------------------------------------------

#pragma strong_types,save_types

//--------------------------------------------------------------------------

varargs int remove( int silent );

//--------------------------------------------------------------------------
//
//   Property-Einstellungen je nach Rasse
//
//--------------------------------------------------------------------------

private mapping morph_properties;

private object pl; // der schattierte Spieler

//--------------------------------------------------------------------------
//
//   start_shadow( Spieler, Properties )
//
//   Startet das Shadowing von Spieler. Properties ist ein Mapping mit
//   den zu ändernden Properties. Dort nicht vorhandene Properties werden
//   zum Spieler durchgereicht. Es werden dort entweder einzelne Werte
//   erwartet (Beispiel: ([ P_GENDER: MALE ])), die dann für alle Rassen
//   gelten, oder closures, die dann ausgeführt werden unter Übergabe der
//   Spielerrasse als Parameter oder aber Mappings mit den Rassennamen
//   (Beispiel: ([ P_GENDER: ([ "Mensch": NEUTER, "Elf": FEMALE,
//   "Zwerg": MALE, ... ]) ])). Ist eine Rasse in dem Rassenmapping nicht
//   vorhanden, so wird das Property zum Spieler durchgereicht. Speziell
//   behandelt werden P_IDS (siehe _query_ids()) und P_NAME (siehe
//   _query_name()). 
//
//--------------------------------------------------------------------------
int start_shadow( object _pl, mapping preset )
{
  if ( !clonep( this_object() ) )
    return 1;
  if ( !_pl || !query_once_interactive( _pl ) )
    return remove();
  pl = _pl;
  morph_properties = deep_copy( preset );
  shadow( pl, 1 );
  return 1;
}

//--------------------------------------------------------------------------
//
//   stop_shadow()
//
//   Beendet das Shadowing und zerstört dieses Objekt
//
//--------------------------------------------------------------------------
int stop_shadow( /* void */ )
{
  if ( !clonep( this_object() ) )
    return 0;
  unshadow();
  return remove();
}

//--------------------------------------------------------------------------
//
//   _query_property( Property )
//
//   Generische Property-Maskierung. liefert aus morph_properties den zur
//   Rasse des Trägers passenden Eintrag.
//
//--------------------------------------------------------------------------
nomask static mixed _query_property( string prop )
{
  string race;

  // Rasse holen.
  if ( IS_LEARNER(pl) )
    race = "Magier";
  else // _query_race() ist notwendig, um closures zu umgehen.
    race = pl->_query_race();

  if ( member( morph_properties, prop ) == 0 )
    return pl->Query( prop );
  if ( closurep( morph_properties[ prop ] ) )
    return funcall( morph_properties[ prop ], race );
  if ( mappingp( morph_properties[ prop ] ) )
  {
    if ( member( morph_properties[ prop ], race ) == 0 )
      return pl->Query( prop );
    if ( closurep( morph_properties[ prop ][ race ] ) )
      return funcall( morph_properties[ prop ][ race ] );

    return morph_properties[ prop ][ race ];
  }
  return morph_properties[ prop ];
}

//--------------------------------------------------------------------------
//
//   _query_article()
//
//   Property-Maskierung für P_ARTICLE
//
//--------------------------------------------------------------------------
int _query_article( /* void */ )
{
  return (int)_query_property( P_ARTICLE );
}

//--------------------------------------------------------------------------
//
//   _query_average_size()
//
//   Property-Maskierung für P_AVERAGE_SIZE
//
//--------------------------------------------------------------------------
int _query_average_size( /* void */ )
{
  return (int)_query_property( P_AVERAGE_SIZE );
}

//--------------------------------------------------------------------------
//
//   _query_average_weight()
//
//   Property-Maskierung für P_AVERAGE_WEIGHT
//
//--------------------------------------------------------------------------
int _query_average_weight( /* void */ )
{
  return (int)_query_property( P_AVERAGE_WEIGHT );
}

//--------------------------------------------------------------------------
//
//   _query_body()
//
//   Property-Maskierung für P_BODY
//
//--------------------------------------------------------------------------
int _query_body( /* void */ )
{
  return (int)_query_property( P_BODY );
}

//--------------------------------------------------------------------------
//
//   _query_gender()
//
//   Property-Maskierung für P_GENDER
//
//--------------------------------------------------------------------------
int _query_gender( /* void */ )
{
  return (int)_query_property( P_GENDER );
}

//--------------------------------------------------------------------------
//
//   _query_hands()
//
//   Property-Maskierung für P_HANDS
//
//--------------------------------------------------------------------------
mixed _query_hands( /* void */ )
{
  return _query_property( P_HANDS );
}

//--------------------------------------------------------------------------
//
//   _query_ids()
//
//   Property-Maskierung für P_IDS - Nicht-Standard, da je nach Ursprungs-
//   geschlecht des Spielers zusätzliche ids fällig werden. Ablauf: Es gibt
//   5 Schritte, bei deren jeweiligem Versagen die ids des Spielers durch-
//   gereicht werden:
//   1. P_IDS ist im property-mapping vorhanden
//   2. a) es ist ein string oder ein array: es wird an die ids des Spielers
//         angehängt und zurückgegeben.
//      b) es ist eine closure. Diese wird ausgeführt unter Übergabe der
//         Spieler-ids, der Rasse des Spielers und der Geschlechter
//         (dieses Objekts und des Spielers). Der Ergebniswert der closure
//         wird direkt zurückgegeben.
//      c) es ist ein mapping. Hier nehmen wir nun das übliche Rassennamen-
//         mapping an -> 3.)
//   3. Für die Rasse des Spielers wird ein Eintrag gesucht
//      a) er ist ein string oder ein array: er wird an die ids des Spielers
//         angehängt und zurückgegeben.
//      b) er ist eine closure. Diese wird ausgeführt unter Übergabe der
//         Spieler-ids, der Rasse des Spielers und der Geschlechter
//         (dieses Objekts und des Spielers). Der Ergebniswert der closure
//         wird direkt zurückgegeben.
//      c) er ist ein mapping. Es wird angenommen, dass je Geschlecht DIESES
//         Objekts ein Eintrag vorhanden ist. -> 4.)
//   4. Für das Geschlecht dieses Objekts wird ein Eintrag gesucht
//      a) er ist ein string oder ein array: er wird an die ids des Spielers
//         angehängt und zurückgegeben.
//      b) er ist eine closure. Diese wird ausgeführt unter Übergabe der
//         Spieler-ids, der Rasse des Spielers und der Geschlechter
//         (dieses Objekts und des Spielers). Der Ergebniswert der closure
//         wird direkt zurückgegeben.
//      c) er ist ein mapping. Es wird angenommen, dass je Geschlecht DES
//         Spielers ein Eintrag vorhanden ist. -> 5.)
//   5. Für das Geschlecht des Spielers wird ein Eintrag gesucht
//      a) er ist ein string oder ein array: er wird an die ids des Spielers
//         angehängt und zurückgegeben.
//      b) er ist eine closure. Diese wird ausgeführt unter Übergabe der
//         Spieler-ids, der Rasse des Spielers und der Geschlechter
//         (dieses Objekts und des Spielers). Der Ergebniswert der closure
//         wird direkt zurückgegeben.
//
//--------------------------------------------------------------------------
mixed _query_ids( /* void */ )
{
  string race;
  mixed ids;
  int gender, sgender;

  // Test 1.
  ids = pl->Query( P_IDS );
  if ( member( morph_properties, P_IDS ) == 0 )
    return ids;

  // Rasse holen.
  if ( IS_LEARNER(pl) )
    race = "Magier";
  else // _query_race() ist notwendig, um closures zu umgehen.
    race = pl->_query_race();

  // Geschlechter holen
  gender = _query_gender();
  sgender = pl->Query( P_GENDER );

  // Test 2.
  // string? Dann einfach den normalen ids dazu, genauso mit array
  if ( stringp( morph_properties[ P_IDS ] ) )
    return ids + ({ morph_properties[ P_IDS ] });
  if ( pointerp( morph_properties[ P_IDS ] ) )
    return ids + morph_properties[ P_IDS ];
  if ( closurep( morph_properties[ P_IDS ] ) )
    return funcall( morph_properties[ P_IDS ], ids, race, gender, sgender );
  // falls kein mapping, dann raus
  if ( !mappingp( morph_properties[ P_IDS ] ) )
    return ids;

  // Test 3.
  if ( member( morph_properties[ P_IDS ], race ) == 0 )
    return ids;
  if ( stringp( morph_properties[ P_IDS ][ race ] ) )
    return ids + ({ morph_properties[ P_IDS ][ race ] });
  if ( pointerp( morph_properties[ P_IDS ][ race ] ) )
    return ids + morph_properties[ P_IDS ][ race ];
  if ( closurep( morph_properties[ P_IDS ][ race ] ) )
    return funcall( morph_properties[ P_IDS ][ race ], ids, race, gender, sgender );
  // falls kein mapping, dann raus
  if ( !mappingp( morph_properties[ P_IDS ][ race ] ) )
    return ids;

  // Test 4.
  if ( member( morph_properties[ P_IDS ][ race ], gender ) == 0 )
    return ids;
  if ( stringp( morph_properties[ P_IDS ][ race ][ gender ] ) )
    return ids + ({ morph_properties[ P_IDS ][ race ][ gender ] });
  if ( pointerp( morph_properties[ P_IDS ][ race ][ gender ] ) )
    return ids + morph_properties[ P_IDS ][ race ][ gender ];
  if ( closurep( morph_properties[ P_IDS ][ race ][ gender ] ) )
    return funcall( morph_properties[ P_IDS ][ race ][ gender ],
		    ids, race, gender, sgender );
  // falls kein mapping, dann raus
  if ( !mappingp( morph_properties[ P_IDS ][ race ][ gender ] ) )
    return ids;

  // Test 5.
  if ( member( morph_properties[ P_IDS ][ race ][ gender ], sgender ) == 0 )
    return ids;
  if ( stringp( morph_properties[ P_IDS ][ race ][ gender ][ sgender ] ) )
    return ids + ({ morph_properties[ P_IDS ][ race ][ gender ][ sgender ] });
  if ( pointerp( morph_properties[ P_IDS ][ race ][ gender ][ sgender ] ) )
    return ids + morph_properties[ P_IDS ][ race ][ gender ][ sgender ];
  if ( closurep( morph_properties[ P_IDS ][ race ][ gender ][ sgender ] ) )
    return funcall( morph_properties[ P_IDS ][ race ][ gender ][ sgender ],
		    ids, race, gender, sgender );

  return ids;
}

//--------------------------------------------------------------------------
//
//   _query_is_morphed()
//
//   Property-Methode für "is_morphed"
//
//--------------------------------------------------------------------------
int _query_is_morphed( /* void */ )
{
  return 1;
}

//--------------------------------------------------------------------------
//
//   _query_max_hands()
//
//   Property-Maskierung für P_MAX_HANDS
//
//--------------------------------------------------------------------------
int _query_max_hands( /* void */ )
{
  return (int)_query_property( P_MAX_HANDS );
}

//--------------------------------------------------------------------------
//
//   _query_mmsgin()
//
//   Property-Maskierung für P_MMSGIN
//
//--------------------------------------------------------------------------
string _query_mmsgin( /* void */ )
{
  return (string)(_query_property( P_MMSGIN ) || "");
}

//--------------------------------------------------------------------------
//
//   _query_mmsgout()
//
//   Property-Maskierung für P_MMSGOUT
//
//--------------------------------------------------------------------------
string _query_mmsgout( /* void */ )
{
  return (string)(_query_property( P_MMSGOUT ) || "");
}

//--------------------------------------------------------------------------
//
//   _query_msgin()
//
//   Property-Maskierung für P_MSGIN
//
//--------------------------------------------------------------------------
string _query_msgin( /* void */ )
{
  return (string)(_query_property( P_MSGIN ) || "");
}

//--------------------------------------------------------------------------
//
//   _query_msgout()
//
//   Property-Maskierung für P_MSGOUT
//
//--------------------------------------------------------------------------
string _query_msgout( /* void */ )
{
  return (string)(_query_property( P_MSGOUT ) || "");
}

//--------------------------------------------------------------------------
//
//   _query_name()
//
//   Property-Methode für P_NAME. Leider ist die player-shell so grottig,
//   dass überall angenommen wird, QueryProp(P_NAME) liefere einen String :-|
//   Vollständiges Property daher unter _query_name_full().
//
//--------------------------------------------------------------------------
string _query_name( /* void */ )
{
  mixed prop;
  prop = _query_property( P_NAME );
  if ( stringp( prop ) )
    return sprintf( prop, pl->Query( P_NAME ) );
  if ( pointerp( prop ) )
    return map( prop,
		      lambda( ({ 'el, 's }),
			      ({#'sprintf, 'el, 's}) ),
		      pl->Query( P_NAME ) )[ WER ];
  return pl->Query( P_NAME );
}

//--------------------------------------------------------------------------
//
//   _query_name_full()
//
//   Property-Methode für "name_full".
//
//--------------------------------------------------------------------------
mixed _query_name_full( /* void */ )
{
  mixed prop;
  prop = _query_property( P_NAME );
  if ( stringp( prop ) )
    return sprintf( prop, pl->Query( P_NAME ) );
  if ( pointerp( prop ) )
    return map( prop,
		      lambda( ({ 'el, 's }),
			      ({#'sprintf, 'el, 's}) ),
		      pl->Query( P_NAME ) );
  return pl->Query( P_NAME );
}

//--------------------------------------------------------------------------
//
//   _query_presay()
//
//   Property-Maskierung für P_PRESAY
//
//--------------------------------------------------------------------------
string _query_presay( /* void */ )
{
  return (string)(_query_property( P_PRESAY ) || "");
}

//--------------------------------------------------------------------------
//
//   _query_race()
//
//   Property-Maskierung für P_RACE
//
//--------------------------------------------------------------------------
string _query_race( /* void */ )
{
  return (string)(_query_property( P_RACE ) || "");
}

//--------------------------------------------------------------------------
//
//   _query_racestring()
//
//   Property-Maskierung für P_RACESTRING
//
//--------------------------------------------------------------------------
string* _query_racestring( /* void */ )
{
  return (string*)_query_property( P_RACESTRING );
}

//--------------------------------------------------------------------------
//
//   _query_size()
//
//   Property-Maskierung für P_SIZE
//
//--------------------------------------------------------------------------
int _query_size( /* void */ )
{
  return (int)_query_property( P_SIZE );
}

//--------------------------------------------------------------------------
//
//   _query_title()
//
//   Property-Maskierung für P_TITLE
//
//--------------------------------------------------------------------------
string _query_title( /* void */ )
{
  return (string)(_query_property( P_TITLE ) || "");
}

//--------------------------------------------------------------------------
//
//   _query_weight()
//
//   Property-Maskierung für P_WEIGHT
//
//--------------------------------------------------------------------------
int _query_weight( /* void */ )
{
  return (int)(_query_property( P_WEIGHT ) || "");
}

//--------------------------------------------------------------------------
//
//   id( Text, Level)
//
//   Die Identifizierung spinnt mit P_NAME-Arrays
//
//--------------------------------------------------------------------------
varargs int id( string str, int lvl )
{
  string plname;

  if ( pl->QueryProp( P_GHOST ) )
    if ( str == "geist" )
      return 1;
    else if ( ( sscanf( str, "geist von %s", plname ) == 1 ) &&
	      pl->id( plname ) )
      return 1;
    else if ( ( sscanf( str, "geist %s", plname ) == 1 ) &&
	      pl->id( plname ) )
      return 1;

  return pl->id( str, lvl );
}

//--------------------------------------------------------------------------
//
//   long()
//
//   Die Langbeschreibung im Spieler hadert mit dem Geschlecht NEUTER...
//
//--------------------------------------------------------------------------
varargs string long( /* void */ )
{
  string slong;
  slong = pl->long();

  if ( _query_gender() == NEUTER )
  {
    string *along;
    int i;

    // alle Er-s und er-s suchen...
    along = regexplode( slong, "\\<[Ee]r\\>" );
    // ... und das r durch ein s ersetzen.
    for ( i = 1 ; i < sizeof( along ) ; i += 2 )
      along[ i ][ 1 ] = 's';
    slong = implode( along, "" );
  }

  return slong;
}

//--------------------------------------------------------------------------
//
//   short()
//
//   Die Kurzbeschreibung im Spieler hat ein Problem mit P_NAME-Arrays
//
//--------------------------------------------------------------------------
string short( /* void */ )
{
  mixed names;
  string answer;
  string title;

  if ( pl->QueryProp( P_INVIS ) )
    if ( interactive( previous_object() ) &&
	 IS_LEARNING( previous_object() ) )
      return "(" + pl->Query( P_NAME ) + ") \n";
    else
      return (string)0;

  names = _query_name_full();
  if ( stringp( names ) )
    names = ({ names, names, names, names });

  if ( pl->QueryProp( P_GHOST ) )
    answer = "Der Geist " + pl->QueryArticle( WESSEN, 0 ) + names[ WESSEN ];
  else
    answer = pl->QueryArticle( WER, 0 ) + names[ WER ];
  if ( ( title = pl->QueryProp( P_TITLE ) ) &&
       ( title != "" ) )
     answer += " " + title;
  if ( !interactive( pl ) )
     answer += " (netztot)";
  return capitalize( answer ) + ".\n";
}

//--------------------------------------------------------------------------
//
//   remove( Schnauze )
//
//   aufräumen
//
//--------------------------------------------------------------------------
varargs int remove( int silent )
{
  destruct( this_object() );
  return 1;
}

//--------------------------------------------------------------------------
//   -- ENDE --
