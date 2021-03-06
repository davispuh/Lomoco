#!/bin/sh
#
# lomoco autogen script
#

# Automakeversion
AM_1=1
AM_2=7
AM_3=6

# Autoconfversion
AC_1=2
AC_2=59

# Check automake version
AM_VERSION=`automake --version | sed -n -e 's#[^0-9]* \([0-9]*\)\.\([0-9]*\)\.*\([0-9]*\).*$#\1 \2 \3#p'`
AM_V1=`echo $AM_VERSION | awk '{print $1}'`
AM_V2=`echo $AM_VERSION | awk '{print $2}'`
AM_V3=`echo $AM_VERSION | awk '{print $3}'`

if [ "$AM_1" -gt "$AM_V1" ]; then
	AM_ERROR=1 
else
	if [ "$AM_1" -eq "$AM_V1" ]; then
		if [ "$AM_2" -gt "$AM_V2" ]; then
			AM_ERROR=1 
		else
			if [ "$AM_2" -eq "$AM_V2" ]; then
				if [ -n "$AM_V3" -o "$AM_3" -gt "$AM_V3" ]; then
					AM_ERROR=1 
				fi
			fi
		fi
	fi
fi

if [ "$AM_ERROR" = "1" ]; then
	echo -e  '\E[31;m'
	echo -n "Your automake version `automake --version | sed -n -e 's#[^0-9]* \([0-9]*\.[0-9]*\.[0-9]*\).*#\1#p'`"
	echo " is older than the suggested one, $AM_1.$AM_2.$AM_3"
	echo "Go on at your own risk. :-)"
	echo
	tput sgr0
fi

# Check autoconf version
AC_VERSION=`autoconf --version | sed -n -e 's#[^0-9]* \([0-9]*\)\.\([0-9]*\).*$#\1 \2#p'`
AC_V1=`echo $AC_VERSION | awk '{print $1}'`
AC_V2=`echo $AC_VERSION | awk '{print $2}'`

if [ "$AC_1" -gt "$AC_V1" ]; then
	AC_ERROR=1 
else
	if [ "$AC_1" -eq "$AC_V1" ]; then
		if [ "$AC_2" -gt "$AC_V2" ]; then
			AC_ERROR=1 
		fi
	fi
fi

if [ "$AC_ERROR" = "1" ]; then
	echo -e  '\E[31;m'
	echo -n "Your autoconf version `autoconf --version | sed -n -e 's#[^0-9]* \([0-9]*\.[0-9]*\).*#\1#p'`"
	echo " is older than the suggested one, $AC_1.$AC_2"
	echo "Go on at your own risk. :-)"
	echo
	tput sgr0
fi

if [ "$LT_ERROR" = "1" ]; then
	echo -e  '\E[31;m'
	echo -n "Your libtool version `libtool --version | sed -n -e 's#[^0-9]* \([0-9]*\.[0-9]*\).*#\1#p'`"
	echo " is older than the suggested one, $LT_1.$LT_2"
	echo "Go on at your own risk. :-)"
	echo
	tput sgr0
fi

echo Configuring build environment for lomoco
echo ...
aclocal \
  && autoheader --force \
  && automake --add-missing --foreign --copy \
  && autoconf --force \
  && echo "Now running configure to configure lomoco" \
  && echo "./configure $@" \
  && ./configure $@
