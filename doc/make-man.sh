#/bin/bash

export MUDHOME=/home/mud
export MUDLIB=$MUDHOME/mudlib
export MDOCDIR=$MUDLIB/doc
export MRSTDIR=$MDOCDIR/sphinx
export MHTMLDIR=$MDOCDIR/sphinx/_build/html
export MTEXTDIR=$MDOCDIR/sphinx/_build/text
export MMANDIR=$MDOCDIR/sphinx/man/
export MANWIDTH=78

#rst2man < asin.rst | man -7 -P cat -l -

cd $MRSTDIR

make html
make text

cd $MTEXTDIR
SOURCES=`find . -type f -name \*.txt`
for FILE in $SOURCES
do
  # skip leading ./
#  DIR=${DIR:2}
#  echo "Processing $FILE..."
  DIR=`dirname ${FILE}`
  BASE=`basename ${FILE} .txt`
#  mkdir -p ${MHTMLDIR}/${DIR}
#SIG=`git log -n 1 --date=iso-local --pretty="%cd von %an" -- $FILE`
#  sed -e "s/\\\$LastChange\\\$/$SIG/g" $FILE | rst2html > ${MHTMLDIR}/${DIR}/${BASE}.html
#  rst2html $FILE ${MHTMLDIR}/${DIR}/${BASE}.html
#  if [ $? -eq 0 ] ; then
    mkdir -p ${MMANDIR}/${DIR}
    cp $FILE ${MMANDIR}/${DIR}/${BASE}
#  fi
done

