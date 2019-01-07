#!/bin/bash
#
# 2019 Rumata
#
# Baut die .idx Datei auf, wie sie von der documentation.shtml Webseite gebraucht
# wird.
# 
# Listet alle Dateien unter doc, die nicht zu sphinx gehoeren und nach einer Manpage aussehen
# in der Form basename<TAB>pathname auf.
#

find . -type f -name '[a-zA-Z0-9]*' |\
	grep -v sphinx |\
	while read LINE
	do
	 egrep -q '^(DESCRIPTION|----|====)' "$LINE" &&\
		printf "%s\t%s\n" $(basename "$LINE") $(echo "$LINE" | sed 's/^.\///')
done >.idx
