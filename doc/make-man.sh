#/bin/bash

export MUDHOME=/home/mud
export MUDLIB=$MUDHOME/mudlib
export MDOCDIR=$MUDLIB/doc
export MRSTDIR=$MDOCDIR/rst
export MHTMLDIR=$MDOCDIR/html
export MANWIDTH=78

#rst2man < asin.rst | man -7 -P cat -l -

cd $MRSTDIR

SOURCES=`find . -type f -name asin.rst`

for FILE in $SOURCES
do
  # skip leading ./
#DIR=${DIR:2}
  echo "Processing $FILE..."
  DIR=`dirname ${FILE}`
  BASE=`basename ${FILE} .rst`
  mkdir -p ${MHTMLDIR}/${DIR}
  rst2html $FILE ${MHTMLDIR}/${DIR}/${BASE}.html
  if [ $? -eq 0 ] ; then
    mkdir -p ${MDOCDIR}/${DIR}
#    rst2man < $FILE | man -7 -P cat -l - > ${MDOCDIR}/${DIR}/${BASE}
  fi
done
