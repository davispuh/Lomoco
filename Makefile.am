#
# Automake description for lomoco
#

SUBDIRS=src

EXTRA_DIST = \
	AUTHORS \
	autogen.sh \
	ChangeLog \
	COPYING \
	INSTALL \
	NEWS \
	README	

# Report splint warnings
splint:
	$(MAKE) -C src/ splint

udev-rules:
	$(AWK) -f udev/toudev.awk < src/lomoco.c > udev/lomoco.rules
