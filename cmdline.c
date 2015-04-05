/* 
   cmdline.c - Command-line parser.

   Copyright (C) 2004 Alexios Chouchoulas
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

#include "cmdline.h"

/* Option flags and variables. These are initialized in parse_opt. */

command_t command     = cmd_none;
int       set_res     = 0;
int       set_sms     = 0;
int       set_channel = 0;
int       set_lock    = 0;
int       set_unlock  = 0;
int       set_clear   = 0;

/*@null@*/char    *match_bus = NULL;
/*@null@*/char    *match_dev = NULL;
/*@null@*/int     match_pid = 0x0000;

/*@null@*/char    *local=NULL;            /* --local=LOCAL-ENCODING */
/*@null@*/char    *remote=NULL;           /* --remote=REMOTE-ENCODING */
/*@null@*/char    **cmdline=NULL;         /* SHELL-COMMAND */


/* prototypes */
static error_t parse_opt (int key, char *arg, struct argp_state *state);
static void show_version (FILE *stream, struct argp_state *state);


/* The argp functions examine these global variables.  */
const char *argp_program_bug_address = "<lomoco-dev@lists.linux-gamers.net>";
void (*argp_program_version_hook) (FILE *, struct argp_state *) = show_version;

static char args_doc[] = "";

/* This is the description table for all options argp needs to handle for lomoco */
struct /*@null@*/argp_option options[] =
{
      { NULL, 0, NULL, 0, _("Device Matching"), 0 },

      { "bus",         'b',     "USB-BUS-NUMBER",             0,
        "Only apply the command to the specified bus, specified in "
        "full (e.g. 001 for bus 1)", 0},
      { "dev",         'd',     "USB-DEV-NUMBER",             0,
        "Only apply the command to the specified device number, specified "
        "in full (e.g. 034 for device 34)", 0},
      { "pid",         'p',     "PRODUCT-ID",             0,
        "Only apply the command to the specified product ID, specified "
        "in hexadecimal and in full (e.g. C025 for an MX-500 optical mouse)", 0},

      { NULL, 0, NULL, 0, _("Actions"), 0 },

      { "scan",        's',     NULL,             0,
        "Scan the USB bus and report any matching, supported Logitech mice", 0},
      { "inquire",     'i',     NULL,             0,
        "Obtain and display the status of all supported capabilities "
        "for all matching devices", 0},

      { NULL, 0, NULL, 0, _("Wireless Status Reporting (CSR command set)"), 0 },

      { "ch1",         '1',     NULL,             0,
        "Set matching devices to channel 1", 0},
      { "ch2",         '2',     NULL,             0,
        "Set matching devices to channel 2", 0},
      { "unlock",      'u',     NULL,             0,
        "Unlock the receiver and search for a wireless mouse in range.", 0},
      { "lock",        'l',     NULL,             0,
        "Stop the 30 second search for a wireless mouse.", 0},
      { "clear",       'c',     NULL,             0,
        "Make the receiver forget about the mouse connected to it.", 0},

      { NULL, 0, NULL, 0, _("Resolution Changing (RES command set)"), 0 },

      { "400",         '4',     NULL,             0,
        "Set matching devices to a resolution of 400cpi", 0},
      { "800",         '8',     NULL,             0,
        "Set matching devices to a resolution of 800cpi", 0},
      { "1200",        'm',     NULL,             0,
        "Set matching devices to a medium resolution of 1200cpi", 0},
      { "1600",        'h',     NULL,             0,
        "Set matching devices to a high resolution of 1600cpi", 0},

      { "2000",        'g',     NULL,             0,
        "Set matching devices to a high resolution of 2000cpi", 0},

      { NULL, 0, NULL, 0, _("SmartScroll/Cruise Control (SMS command set)"), 0 },

      { "sms",         KEY_SMS, NULL,             0,
        "Enable SmartScroll/Cruise Control. 'Up' and 'down' buttons also issue"
        "wheel up/down events", 0},
      { "no-sms",      KEY_NO_SMS,     NULL,             0,
        "Disable SmartScroll/Cruise Control. 'Up' and 'down' buttons do "
        "not also issue wheel events", 0},
      
      { NULL, 0, NULL, 0, NULL, 0 }
};

struct /*@null@*/argp argp =
{
      options, parse_opt, args_doc,
      "Manage vendor-specific features of Logitech USB mice.",
      NULL, NULL, NULL
};





/*
 * Descripton: Show the version number and copyright information.
 *
 * Parameters:
 *
 * Return:
 */
static void show_version (FILE *stream, struct argp_state *state)
{
      /* 
       * Print in small parts whose localizations can hopefully be copied
       * from other programs.
       */
      fputs(PACKAGE" "VERSION"\n", stream);
      fprintf(stream, 
            "fork of lmctl written by Alexios Chouchoulas <alexios@bedroomlan.org>.\n"
            "Copyright (C) %s %s\n"
            "Copyright (C) %s %s\n"
            "This program is free software; you may redistribute it under the terms of\n"
            "the GNU General Public License.  This program has absolutely no warranty.\n"
            "\n",
            "2003", "Alexios Chouchoulas",
            "2005", "Andreas Schneider, Peter Feuerer");
}


/*
 * Description: Parse a single option. Is called by argp_parse and performes steps for
 * every single option.
 *
 * Parameters: key (which option was entered) 
 *             *arg (string after option, e.g. -b 001 <-- *arg would contain 001)
 *             *state (TODO) 
 *
 * Return: 
 */
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
      int dummy;

      switch (key) {
      case 'b':
            match_bus = strdup (arg);
            if ((sscanf (arg, "%03d", &dummy) != 1) ||
                (strlen (arg) != 3) ||
                (strspn (arg, "0123456789") != 3)) {
                  fprintf (stderr,
                         "%s: the bus ID must be a three-digit decimal\n"
                         "number (e.g. 005). Use lsusb(1) to see the proper"
                         "format. you must specify the\nUse %s --help for"
                         "usage information.\n",
                         progname, progname);
                  exit (1);
            }
            break;
            
      case 'd':
            match_dev = strdup (arg);
            if ((sscanf (arg, "%03d", &dummy) != 1) ||
                (strlen (arg) != 3) ||
                (strspn (arg, "0123456789") != 3)) {
                  fprintf (stderr,
                         "%s: the device ID must be a three-digit decimal\n"
                         "number (e.g. 005). Use lsusb(1) to see the proper"
                         "format. you must specify the\nUse %s --help for"
                         "usage information.\n",
                         progname, progname);
                  exit (1);
            }
            break;
            
      case 'p':
            if ((sscanf (arg, "%x", &match_pid) != 1) ||
                (match_pid > 0xffff) || (match_pid < 0)) {
                  fprintf (stderr,
                         "%s: the product ID must be a hexadecimal "
                         "number (e.g. c025),\nup to four digits long. "
                         "Use lsusb(1) to see the proper format. "
                         "you must\nspecify the\nUse %s --help for "
                         "usage information.\n",
                         progname, progname);
                  exit (1);
            }
            break;
            
      case 's':               /* --scan */
            command = cmd_scan;
            break;

      case 'i':               /* --inquiry */
            command = cmd_inquire;
            break;

      case '4':         /* set the resolution */
      case '8':
            command = cmd_set;
            set_res = (int) (key - '0') * 100;
            break;
      
      case 'm':
            command = cmd_set;
            set_res = (int) (12) * 100;
            break;
      case 'h':
            command = cmd_set;
            set_res = (int) (16) *100;
            break;
      case 'g':
            command = cmd_set;
            set_res = (int) (20) *100;
            break;


      case '1':         /* set the channel */
      case '2':
            command = cmd_set;
            set_channel = (int) (key - '0');
            break;
      
      case 'u':
            command = cmd_set;
            set_unlock = 1;
            break;

      case 'l':
            command = cmd_set;
            set_lock = 1;
            break;

      case 'c':
            command = cmd_set;
            set_clear = 1;
            break;

      case KEY_SMS:
      case KEY_NO_SMS:  /* enable/disable SMS */
            command = cmd_set;
            set_sms = key == KEY_SMS? 1: -1;
            break;

      case ARGP_KEY_ARG:
            /* End processing here. */
            cmdline = &state->argv [state->next - 1];
            state->next = state->argc;
            break;

      default:
            return ARGP_ERR_UNKNOWN;
      }
      
      return 0;
}

/*
 * Description: gets called by main function of lomoco.c
 *              and starts the argument parsing function 
 * 
 * Parameters:  argc (sum of args)
 *              **argv (array of args)
 *
 * Return:      nothing, exits program with (EXIT_FAILURE) if something failes
 */

void cmdline_parse (int argc, char **argv)
{
      /* Parse the arguments */
      
      argp_parse (&argp, argc, argv, ARGP_IN_ORDER, NULL, NULL);

      /* Validate the arguments */

      if (command == cmd_none) {
            fprintf (stderr,
                   "%s: you must specify a command.\n"
                   "\nUse %s --help for usage information.\n",
                   progname, progname);
            exit (EXIT_FAILURE);
            
      }
      
      /* Set up the default command, if one is needed. */
}

