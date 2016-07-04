#!/bin/awk -f
BEGIN {
	FS = ","
	print "ACTION != \"add\", GOTO=\"lomoco_end\""
	print "SUBSYSTEM != \"usb\", GOTO=\"lomoco_end\""
	print "SYSFS{idVendor} != \"046d\", GOTO=\"lomoco_end\""
	print ""
}

$1 ~ /0xc[a-f0-9][a-f0-9][a-f0-9]/ {
    print "# " substr($3, index($3, "\"")) ", " $2
    print "SYSFS{idProduct}==\"" substr($1, index($1, "x")+1) \
	  "\", RUN=\"lomoco\""
}

END {
	print ""
	print "LABEL=\"lomoco_end\""
}

