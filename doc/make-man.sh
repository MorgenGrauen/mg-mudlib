#/bin/bash

export MUDHOME=/home/mud
export MUDLIB=$MUDHOME/mudlib
export MDOCDIR=$MUDLIB/doc
export MRSTDIR=$MDOCDIR/sphinx
export MHTMLDIR=$MDOCDIR/sphinx/_build/html
export MTEXTDIR=$MDOCDIR/sphinx/_build/text
export MMANDIR=$MDOCDIR//
#export MANWIDTH=78

cd $MRSTDIR

make text

cd $MTEXTDIR
SOURCES=`find . -type f -name \*.txt`
umask o+rx
for FILE in $SOURCES
do
#  echo "Processing $FILE..."
  DIR=`dirname ${FILE}`
  BASE=`basename ${FILE} .txt`
  mkdir -p ${MMANDIR}/${DIR}
#chmod o+rx ${MMANDIR}/${DIR}
  cp -a $FILE ${MMANDIR}/${DIR}/${BASE}
#  chmod o+r ${MMANDIR}/${DIR}/${BASE}
done

