#!/bin/bash

uname

if [ "`uname`" == Darwin ]; then
  BOOST_INC=/opt/local/include
  BOOST_LIB=/opt/local/lib
  BOOST_SUF=-mt
fi
echo "BOOST_INC = $BOOST_INC"
echo "BOOST_LIB = $BOOST_LIB"

# configure.ac
if [ ! -e configure.ac ]; then
  echo 'AC_INIT([ckon], [0.0])' >> configure.ac
  echo 'm4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])' >> configure.ac
  echo 'AC_CONFIG_AUX_DIR(config)' >> configure.ac
  echo 'm4_pattern_allow([AM_PROG_AR])' >> configure.ac
  echo 'm4_ifdef([AM_PROG_AR], [AM_PROG_AR])' >> configure.ac
  echo 'AM_INIT_AUTOMAKE([-Wall no-define])' >> configure.ac
  echo 'AC_PROG_CXX' >> configure.ac
  echo 'AC_CONFIG_FILES([Makefile])' >> configure.ac
  echo 'AC_OUTPUT' >> configure.ac
fi

if [ ! -e Makefile.am ]; then
  echo 'AUTOMAKE_OPTIONS = foreign subdir-objects -Wall' >> Makefile.am
  echo 'ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}' >> Makefile.am
  echo 'AM_CPPFLAGS = -I$(BOOST_INC)' >> Makefile.am
  echo 'bin_PROGRAMS = ckon' >> Makefile.am
  echo 'ckon_LDFLAGS = -L$(BOOST_LIB) -lstdc++' >> Makefile.am
  echo 'ckon_LDFLAGS += -lboost_filesystem$(BOOST_SUF)' >> Makefile.am
  echo 'ckon_LDFLAGS += -lboost_system$(BOOST_SUF)' >> Makefile.am
  echo 'ckon_LDFLAGS += -lboost_regex$(BOOST_SUF)' >> Makefile.am
  echo 'ckon_SOURCES =' >> Makefile.am
  for file in `find src -type f`; do
    echo "ckon_SOURCES += $file" >> Makefile.am
  done
fi

autoreconf --force --install

echo "****************************************************"
echo "run ./configure now. Use --prefix for local install."
echo "then, GNU-style make (-j4) && make install."
echo "****************************************************"
