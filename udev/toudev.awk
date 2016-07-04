#!/bin/awk -f
BEGIN {
	FS = ","
	print "# udev.rules file for Logitech mouse control using lomoco"
	print "#"
	print ""
	print "ACTION != \"add\", GOTO=\"lomoco_end\""
	print "SUBSYSTEM == \"usb\", ENV{DEVTYPE}==\"usb_device\", GOTO=\"lomoco_start\""
	print "SUBSYSTEM != \"usb_device\", GOTO=\"lomoco_end\""
	print ""
	print "LABEL=\"lomoco_start\""
	print ""
}

$1 ~ /0xc[a-f0-9][a-f0-9][a-f0-9]/ {
    print "# " substr($3, index($3, "\"")) ", " $2
    print "ATTRS{idVendor}==\"046d\", ATTRS{idProduct}==\"" substr($1, index($1, "x")+1) \
	  "\", RUN+=\"udev.lomoco\""
    print ""
}

END {
	print ""
	print "LABEL=\"lomoco_end\""
}


