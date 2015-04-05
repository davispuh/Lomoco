/* 
   Formerly lmctl.h - Main header file.
   Copyright (C) 2004 Alexios Chouchoulas

   lomoco.h - Main header file

   Copyright (C) 2005 Andreas Schneider <andreas.schneider@linux-gamers.net>
                      Peter Feuerer <peter.feuerer@linux-gamers.net>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/


#ifndef __LOMOCO_H
#define __LOMOCO_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stropts.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pty.h>
#include <iconv.h>
#include <fcntl.h>
#include <unistd.h>


#include <usb.h>

#include "cmdline.h"



#define __KERNEL__
        
#define VENDOR_LOGITECH 0x046d
#define TIMEOUT         0x1000      
                        
                        
#define P6  buf [0]
#define P0  buf [1]
#define P4  buf [2]
#define P5  buf [3]
#define P8  buf [4]
#define P9  buf [5]
#define PB0 buf [6]     
#define PB1 buf [7]

                
#define max(a,b) ((a) > (b) ? (a) : (b))

/* for debugging perror */
#define perror printf("%s:%d\n",__FILE__,__LINE__);perror

/* program name (lomoco) */
extern char *progname;

/*
 * pid       Product id
 * name      Mouse name
 * model     Mouse Model
 *
 * has_csr   Mouse with a receiver
 * has_res   Resolution control
 * has_ssr   Smart Control Reporting
 * has_sms   Smart Control
 *
 * is_dual   Dual receiver, mouse and keyboard
 */
typedef struct {
            ushort      pid;
/*@null@*/  char  *name;
/*@null@*/  char  *model;

            int   has_csr;
            int   has_res;
            int   has_ssr;
            int   has_sms;

            int   is_dual;
} mouse_t;

#endif /* __LOMOCO_H */