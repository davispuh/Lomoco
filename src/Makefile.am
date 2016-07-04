#
# Automake description for lomoco
#

XMLTO = xmlto

AM_CFLAGS=-Wall
bin_PROGRAMS=lomoco
lomoco_SOURCES= \
	lomoco.c \
	lomoco.h \
	cmdline.c \
	cmdline.h

#
# Create Manpages
#
man_MANS= \
	lomoco.1

lomoco.1: lomoco.xml
	-$(XMLTO) man lomoco.xml

EXTRA_DIST = \
	lomoco.xml \
	$(man_MANS)

# Report splint warnings
splint:
	@echo "Running splint on lomoco..."
	-splint -preproc $(lomoco_SOURCES)
