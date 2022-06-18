#!/bin/bash
#set -x
set -e

if [ $# -ne 1 ]
then
  echo "Usage: `basename $0` appimage"
  exit $E_BADARGS
fi

if [ ! -d distrib ]
then
  echo "Error ! Must be run from base directory"
  exit $E_BADARGS
fi

base=$(pwd)
echo "base dir: $base"
version=`cat src/version.h | grep "VERSION" | sed 's/.*VERSION "//' | sed 's/[^0-9.a-zA-Z_].*//'`
echo "version : $version"
distrib/make_shortcut.sh

dir_name=AITD_PakEdit_${version}_linux

echo "appimage name: " $1
rm -f $base/${dir_name}.tgz
mkdir -p distrib/tmp/
cd distrib/tmp/
  rm -rf $dir_name
  mkdir $dir_name
cd -
  cp $base/$1 distrib/tmp/$dir_name
cd -
  cp $base/LICENSE.txt $base/README.md $dir_name
  cp $base/data/*.json $dir_name
  rm -Rf $base/io_alone/__pycache__
  cp -R $base/io_alone/ $dir_name
  tar zcvf $base/${dir_name}.tgz $dir_name
cd -

echo "Tgz created: " $base/${dir_name}.tgz
