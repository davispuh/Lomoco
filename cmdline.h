/* 
   cmdline.h - Command-line parser interface.

   Copyright (C) 2002 Alexios Chouchoulas
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


#ifndef __CMDLINE_H
#define __CMDLINE_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <argp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lomoco.h"

#define _(x) x

/* argp option keys */

typedef enum {
      cmd_none = 0,
      cmd_scan,
      cmd_set,
      cmd_inquire,
} command_t;


enum {
      DUMMY_KEY=129,
      KEY_SMS,
      KEY_NO_SMS
};

/* Option flags and variables.  These are initialized in parse_opt.  */

extern command_t   command;
extern int         set_res;
extern int         set_sms;
extern char       *match_bus;
extern char       *match_dev;
extern int         match_pid;
extern int         set_channel;
extern int         set_lock;
extern int         set_unlock;
extern int         set_clear;

void cmdline_parse (int argc, char **argv);

#endif /* __CMDLINE_H */