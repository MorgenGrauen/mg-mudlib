// Wenn ein Verzeichnis fuer ein Objekt nicht schreibbar ist, kann der
// Besitzer des Verzeichnisses eine Datei access_rights anlegen, die
// dieses Recht vergibt.
// Da DOC hier eigentlich nicht schreiben darf, war das hier noetig.
// Ein Magier wird das fuer die eigenen Verz. idR nicht brauchen.

int access_rights( string user, string pfad ) {
  if( user=="DOC" && pfad=="opferstocklog" ) {
  	// DOC darf opferstocklog schreiben
	return 1;
  }
  return 0;
}
