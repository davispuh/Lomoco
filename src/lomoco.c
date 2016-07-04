/* 
   Formerly lmctl.c - Control Logitech USB Mice
   Copyright (C) 2004 Alexios Chouchoulas

   lomoco.c - Logitech Mouse Control for USB Mice

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

char *progname;

/*
 * product id:    cat /proc/bus/input/devices
 * name:    cat /proc/bus/input/devices
 * model:   you can find on the hardware, look for M/N: ...
 * csr:           mouse with receiver (wireless)
 * res:           mouse has resolution control
 * sms:           mouse has smart control
 * dual:    dual receiver (wireless mouse+wireless keyboard)
 *
 *   product id,  name,                                        model,    csr, res, ssr, sms, dual */
mouse_t mice [] = {
      {0xc00e, "Wheel Mouse Optical",                        "M-BJ58",      0, 1, 0, 0, 0},
      {0xc00f, "MouseMan Traveler",                          "M-BJ79",      0, 1, 0, 0, 0},
      {0xc012, "MouseMan Dual Optical",                      "M-BL63B",     0, 1, 0, 0, 0},
      {0xc01d, "MX510 Optical Mouse",                        "M-BS81A",     0, 1, 1, 1, 0},
      {0xc01e, "MX518 Optical Mouse",                        "M-BS81A",     0, 1, 1, 0, 0},
      {0xc024, "MX300 Optical Mouse",                        "M-BP82",      0, 1, 0, 0, 0},
      {0xc01b, "MX310 Optical Mouse",                        "M-BP86",      0, 1, 0, 0, 0},
      {0xc025, "MX500 Optical Mouse",                        "M-BP81A",     0, 1, 1, 1, 0},
      {0xc031, "iFeel Mouse (silver)",                       "M-UT58A",     0, 1, 0, 0, 0},
      {0xc041, "G5 Laser Gaming Mouse",                      "M-UAC113",    0, 1, 0, 1, 0},
      {0xc051, "MX518 Optical Mouse",                        "M-BS81A",     0, 1, 1, 0, 0},
      {0xc501, "Mouse Receiver",                             "C-BA4-MSE",   1, 0, 0, 0, 0},
      {0xc502, "Dual Receiver",                              "C-UA3-DUAL",  1, 0, 0, 0, 1},
      {0xc503, "Receiver for MX900 Receiver",                "C-UJ16A",     1, 0, 0, 1, 0},
      {0xc504, "Receiver for Cordless Freedom Optical",      "C-BD9-DUAL",  1, 0, 0, 0, 1},
      {0xc505, "Receiver for Cordless Elite Duo",            "C-BG17-DUAL", 1, 0, 0, 0, 1},
      {0xc506, "Receiver for MX700 Optical Mouse",           "C-BF16-MSE",  1, 0, 0, 1, 0},
      {0xc508, "Receiver for Cordless Optical TrackMan",     "C-BA4-MSE",   1, 0, 0, 1, 0},
      {0xc525, "Receiver for MX Air",                        "C-UB34",      1, 0, 1, 1, 0},
      {0xc702, "Receiver for Cordless Presenter",            "C-UF15",      1, 0, 0, 0, 0},
      {0xc704, "Receiver for diNovo Media Desktop",          "C-BQ16A",     1, 0, 1, 1, 0},
      /* From Markus Wiesner <m_wiesner@gmx.net> */
      {0xc50e, "Receiver for MX1000 Laser",                  "C-BN34",      1, 0, 1, 1, 0},
      {0xc512, "Receiver for Cordless Desktop MX3100 Laser", "C-BO34",      1, 0, 0, 1, 1},
      {0x0000, NULL, NULL, 0, 0, 0, 0}
};


/*
 * Description:   Query the mouse and report all cordless mouse specific infos
 *          e.g. receiver type, mouse type, battery status
 *
 * Parameters:    mouse_t *m
 *                mouse struct with the mouse specs
 *          struct usb_dev_handle *handle
 *                usb device handle of the mouse
 *          unsigned int addr
 *                address for dual receivers
 *
 * Return:  void
 */
static void query_csr(mouse_t *m, struct usb_dev_handle *handle,
                  unsigned int addr) {
      unsigned char buf[12] = {0};
      
      if (usb_control_msg (   handle,
                        USB_TYPE_VENDOR | USB_ENDPOINT_IN,
                        0x09,
                        (0x0003 | addr),
                        (0x0000 | addr),
                        (char *) buf,
                        8,
                        TIMEOUT           ) != 8) {

            perror("Writing to USB device: CSR");
            exit(EXIT_FAILURE);
      }

      /* We have not obtained blocks P6 P0 P4 P5 P8 P9 PB0 PB1 */

      /* Is a C504 receiver busy? */
      if ((P0 == 0x3b) && (P4 == 0)) {
            printf("\t*** C504 receiver busy, try again later.\n");
      }

      /* Decode P0 */
      printf("\tReceiver type: ");
      switch (P0) {
            case 0x0f: printf ("C50E\n"); break;
            case 0x38: printf ("C501\n"); break;
            case 0x39: printf ("C502\n"); break;
            case 0x3a: printf ("C503\n"); break;
            case 0x3b: printf ("C504\n"); break;
            case 0x3c: printf ("C508\n"); break;
            case 0x3d: printf ("C506\n"); break;
            case 0x3e: printf ("C505\n"); break;
            case 0x42: printf ("C512\n"); break;
      default: printf ("Unknown (type %x)\n", P0);
      }

            /* Decode P1 */
      printf("\tMouse type: ");
      switch (P4) {
            case 0x00: printf ("None\n"); break;
            case 0x04: printf ("Cordless Mouse\n"); break;
            case 0x05: printf ("Cordless Wheel Mouse\n"); break;
            case 0x06: printf ("Cordless MouseMan Wheel\n"); break;
            case 0x07: printf ("Cordless Wheel Mouse\n"); break;
            case 0x08: printf ("Cordless Wheel Mouse\n"); break;
            case 0x09: printf ("Cordless TrackMan Wheel\n"); break;
            case 0x0A: printf ("Trackman Live\n"); break;
            case 0x0C: printf ("Cordless TrackMan FX\n"); break;
            case 0x0D: printf ("Cordless MouseMan Optical\n"); break;
            case 0x0E: printf ("Cordless Optical Mouse\n"); break;
            case 0x0F: printf ("Cordless Mouse\n"); break;
            case 0x12: printf ("Cordless MouseMan Optical (2ch)\n"); break;
            case 0x13: printf ("Cordless Optical Mouse (2ch)\n"); break;
            case 0x14: printf ("Cordless Mouse (2ch)\n"); break;
            case 0x15: printf ("MX1000 Laser Cordless Mouse\n"); break;
            case 0x1a: printf ("MX900 Optical Cordless Mouse\n"); break;
            case 0x82: printf ("Cordless Optical TrackMan\n"); break;
            case 0x8A: printf ("MX700 Cordless Optical Mouse\n"); break;
            case 0x8B: printf ("MX700 Cordless Optical Mouse (2ch)\n"); break;
      default: printf ("Unknown (type %x)\n", P4);
      }

      /* Decode P5 */
      printf ("\tConnect button on mouse pressed: %s\n",
            (P5 & 0x40 ? "yes" : "no"));
      printf ("\tActive RF Channel: %d\n",
            (P5 & 0x08 ? 2 : 1));
      printf ("\tBattery status: %d ", P5 & 0x07);
      switch (P5 & 0x07) {
            case 0: printf ("(unavailable)\n"); break;
            case 1:
            case 2: printf ("(critical)\n"); break;
            case 3:
            case 4: printf ("(low)\n"); break;
            case 5:
            case 6: printf ("(good)\n"); break;
            case 7: printf ("(full)\n"); break;
      }

      /* Decode P8 and P9 */
      printf ("\tCordless security ID: 0x%04x\n", (P8 | (P9 << 8)));

      /* Decode P6 */
      if (P6 & 0x20) {
            printf ("\tReceiver talking: yes\n");
            printf ("\tLock Request (CONNECT button pressed): %s\n",
                  P6 & 0x80? "yes": "no");
            printf ("\tMouse Powerup (First time out of the box auto-locking): %s\n",
                  P6 & 0x40? "yes": "no");
            printf ("\tReceiver Unlocked (Mouse disconnected): %s\n",
                  P6 & 0x10? "yes": "no");
            printf ("\tWait Lock (Searching for mouse): %s\n",
                  P6 & 0x08? "yes": "no");
      } else {
            printf ("\tReceiver talking: no\n");
      }
            
      /* Decode PB0 */
      if (PB0 < 0x80) 
            printf ("\tDevice physical shape type: %d\n", PB0);

      /* Decode PB1 */
      if (PB1 & 0x80) {
            printf ("\tDevice has two channels: %s\n",
                  PB1 & 0x40? "yes": "no");
            printf ("\tDevice supports 800 cpi: %s\n",
                  PB1 & 0x20? "yes": "no");
            printf ("\tDevice has horizontal roller: %s\n",
                  PB1 & 0x10? "yes": "no");
            printf ("\tDevice has vertical roller: %s\n",
                  PB1 & 0x08? "yes": "no");
            if ((PB1 & 7) < 7) {
                  printf ("\tNumber of buttons on device: %d\n",
                        (PB1 & 7) + 2);
            } else {
                  printf ("\tNumber of buttons on device: 9 or more\n");
            }
      }
}


/*
 * Description:   Query the mouse and report the current resolution
 *
 * Parameters:    mouse_t *m
 *                struct with the mouse specs
 *          struct usb_dev_handle *handle
 *                device handle of the mouse
 *          unsigned int addr
 *                address for dual receivers
 *
 * Return:  void
 */
static void query_res(mouse_t *m, struct usb_dev_handle *handle,
                  unsigned int addr) {
      unsigned char buf[4] = {0};

      if (usb_control_msg (   handle,
                        USB_TYPE_VENDOR | USB_ENDPOINT_IN,
                        0x01,
                        (0x000e | addr),
                        (0x0000 | addr),
                        (char *) buf,
                        1,
                        TIMEOUT           ) != 1) {

            perror("Writing to USB device: RES");
            exit(EXIT_FAILURE);
      }
            
      assert ((buf [0] == 3) || (buf [0] == 4) || (buf [0] == 5) || (buf [0] == 6)
            || (buf [0] == 7));

      printf ("\tResolution (RES): %d cpi\n", (buf [0] - 2) * 400);
}


/*
 * Description:   Query the mouse and report smart control if it is availiable
 *
 * Parameters:    mouse_t *m
 *                struct with the mouse specs
 *          struct usb_dev_handle *handle
 *                device handle of the mouse
 *          unsigned int addr
 *                address for dual receivers
 *
 * Return:  void
 */
static void query_sms(mouse_t *m, struct usb_dev_handle *handle,
                  unsigned int addr) {
      unsigned char buf[4] = {0};

      if (m->has_ssr) {
            if (usb_control_msg (   handle,
                              USB_TYPE_VENDOR | USB_ENDPOINT_IN,
                              0x01,
                              (0x0017 | addr),
                              (0x0000 | addr),
                              (char*) buf,
                              1,
                              TIMEOUT           ) != 1) {
                  perror("Writing to USB device: SMS");
                  exit(EXIT_FAILURE);
            }

            printf ("\tSmartScroll (SMS): %s\n", (buf [0] ? "on" : "off"));
      } else {
            printf ("\tSmartScroll (SMS): reporting not supported\n");
      }
}


/*
 * Description: print known usb information
 *
 * Parameters:    mouse_t *m
 *                struct with the mouse specs
 *          usb_device *device
 *                the device which should be queried
 *
 * Return:  void
 */
static void query_usb(mouse_t *m, struct usb_device *device) {
      static const char *usbhid_mousepoll = "/sys/module/usbhid/parameters/mousepoll";
      unsigned int mousepoll = 0;     /* polling interval value */
      unsigned int interval = 0;    /* default polling interval */
      char buf[4] = {0};
      int i, j, k, l, fd;

      if ((fd = open(usbhid_mousepoll, O_RDONLY)) != -1) {
            if ((read (fd, buf, (sizeof(buf) - 1))) < (sizeof(buf) - 1))
                  mousepoll = atoi (buf);
      }
      close (fd);

      for (i = 0; i < device->descriptor.bNumConfigurations; i++) {
            /* Loop through all of the interfaces */
            for (j = 0; j < device->config[i].bNumInterfaces; j++) {
                  /* Loop through all of the alternate mouse settings */
                  for (k = 0; k < device->config[i].interface[j].num_altsetting; k++) {
                        /* Check if this interface is a mouse */
                        if ( device->config[i].interface[j].altsetting[k].bInterfaceProtocol == 2 ) {
                              /* Loop trough all of the endpoints */
                              for (l = 0; l < device->config[i].interface[j].altsetting[k].bNumEndpoints; l++) {
                                    /* Get the values and print output */
                                    interval = device->config[i].interface[j].altsetting[k].endpoint[l].bInterval;
                                    
                                    if (mousepoll)
                                          printf ("\tUSB Mouse Polling Interval: %3dms (default: %3dms)\n", mousepoll, interval);
                                    else
                                          printf ("\tUSB Mouse Polling Interval: %3dms\n", interval);
                              }
                        }
                  }
            }
      }
}

/*
 * Description:   print mouse specific things like current resolution, 
 *          state of sms, and for wireless mouse additional: 
 *          receiver type, battery strenght and so on
 *
 * Parameters:    mouse_t *m
 *                struct with the mouse specs
 *              usb_device *device
 *                the device which should be queried
 *
 * Retrun:  void
 */
static void query (mouse_t *m, struct usb_device *device) {
      usb_dev_handle *handle;
      unsigned int  addr;

      /*
       * For dual receivers C502, C504 and C505, the mouse is the
       * second device and uses an addr of 1 in the value and index
       * fields' high byte. 1 << 8
       */
      addr = m->is_dual ? 1 << 8 : 0;

      /* Try to open the device */
      handle = usb_open (device);

      if (handle == NULL) {
            perror ("Error opening USB device");
            exit (EXIT_FAILURE);
      }

      /* Parse CSR Status (this one's huge) 
      *  for wireless mouse
      */
      if (m->has_csr) {
            query_csr(m, handle, addr);
      }


      /* The RES extension */
      if (m->has_res) {
            query_res(m, handle, addr);
      }
      

      /* The SMS extension */
      if (m->has_sms) {
            query_sms(m, handle, addr);
      }

      query_usb(m, device);

      printf ("\n");

      (void) usb_close (handle);
}


/*
 * Description:   change the wireless channel, start/stop the receiver to search for mouse
 *          and make the receiver to forget the mouse connected to it.
 *
 * Parameters:    mouse_t *m
 *                struct with the mouse specs
 *          usb_device *device
 *                the device which should be queried
 *
 * Retrun:      void
 */
static void configure_csr (/*@unused@*/mouse_t *m, struct usb_dev_handle *handle,
                    unsigned int addr) {
      /* Set the channel? */
      if (set_channel) {
            assert ((set_channel == 1) || (set_channel == 2));
            if (usb_control_msg (   handle,
                              USB_TYPE_VENDOR,
                              0x02,
                              (0x0008 | addr),
                              ((set_channel - 1) | addr),
                              NULL,
                              0,
                              TIMEOUT           ) != 0) {
                              
                  perror("Writing to USB device: CSR");
                  exit(EXIT_FAILURE);
            }
            
            printf ("\tRF Channel set to %d\n", set_channel);
      }
      
      /* Unlock? */
      if (set_unlock) {
            if (usb_control_msg (   handle,
                              USB_TYPE_VENDOR,
                              0x02,
                              (0x06 | addr),
                              (0x1 | addr),
                              NULL,
                              0,
                              TIMEOUT           ) != 0) {
                              
                  perror("Writing to USB device: CSR");
                  exit(EXIT_FAILURE);
            }
            
            printf ("\tReceiver unlocked, searching for a mouse.\n");
      }
      
      /* Lock? */
      if (set_lock) {
            if (usb_control_msg (   handle,
                              USB_TYPE_VENDOR,
                              0x02,
                              (0x0006 | addr),
                              (0x0000 | addr),
                              NULL,
                              0,
                              TIMEOUT           ) != 0) {
                              
                  perror("Writing to USB device: CSR");
                  exit(EXIT_FAILURE);
            }
            
            printf ("\tReceiver locked, no longer searching for a mouse.\n");
      }
      
      /* Clear? */
      if (set_clear) {
            if (usb_control_msg (   handle,
                              USB_TYPE_VENDOR,
                              0x09,
                              (0x0004 | addr),
                              (0x0000 | addr),
                              NULL,
                              0,
                              TIMEOUT           ) != 0) {
                              
                  perror("Writing to USB device: CSR");
                  exit(EXIT_FAILURE);
            }
            
            printf ("\tReceiver cleared, no longer associated with a mouse.\n");
      }
}


/*
 * Description:   change the resolution
 *
 * Parameters:  mouse_t *m
 *                struct with the mouse specs
 *          usb_device *device
 *                the device which should be queried
 *
 * Retrun:  void
 */
static void configure_res (/*@unused@*/mouse_t *m,
                        struct usb_dev_handle *handle) {
      if (set_res == 0) return;

      assert ((set_res == 400) || (set_res == 800) || (set_res == 1200) 
            || (set_res == 1600) || (set_res == 2000));
            
      if (usb_control_msg (   handle,
                        USB_TYPE_VENDOR,
                        0x0002,
                        0x000e,
                        (set_res / 400) + 2,
                        NULL,
                        0,
                        TIMEOUT           ) != 0) {
                        
            perror("Writing to USB device: RES");
            exit(EXIT_FAILURE);
            }
      
      printf ("\tResolution set to %d cpi\n", set_res);
}


/*
 * Description:   change state of sms
 *
 * Parameters:    mouse_t *m
 *                struct with the mouse specs
 *          usb_device *device
 *                the device which should be queried
 *
 * Retrun:      void
 */
static void configure_sms (/*@unused@*/mouse_t *m,
                        struct usb_dev_handle *handle) {
      if (set_sms == 0) return;
      assert ((set_sms == 1) || (set_sms == -1));
      if (usb_control_msg (   handle,
                        USB_TYPE_VENDOR,
                        0x02,
                        0x0017,
                        (set_sms == 1 ? 0x0001 : 0x0000),
                        NULL,
                        0,
                        TIMEOUT           ) != 0) {
                        
            perror("Writing to USB device: ");
            exit(EXIT_FAILURE);
      }
      
      if (set_sms == 1) printf ("\tSmartScroll enabled\n");
      else printf ("\tSmartScroll disabled\n");
}


/*
 * Description:   start the configuration functions of all features
 *          which are supported by the mouse.
 *
 * Parameters:    mouse_t *m
 *                struct with the mouse specs
 *          usb_device *device
 *                the device which should be queried
 *
 * Retrun:  void
 */
static void configure (mouse_t *m, struct usb_device *device) {
      usb_dev_handle * handle;
      unsigned int  addr;

      /* 
       * For dual receivers C502, C504 and C505, the mouse is the
       * second device and uses an addr of 1 in the value and index
       * fields' high byte. 1 << 8
       */
      addr = m->is_dual ? 1 << 8 : 0;

      /* Try to open the device */
      handle = usb_open (device);

      if (handle == NULL) {
            perror ("Error opening USB device");
            exit (EXIT_FAILURE);
      }

      if (m->has_csr) configure_csr (m, handle, addr);
      if (m->has_res) configure_res (m, handle);
      if (m->has_sms) configure_sms (m, handle);

      printf ("\n");

      (void) usb_close (handle);
}


/*
 * Description:   Search for the usb-mouse in our mice[] vector
 *
 * Parameters:    struct usb_device *device
 *                the usb device to be searched for
 *
 * Retrun:  the data entry of the mouse (or the NULL entry if the mouse
 *          wasn't found)
 */
static /*@null@*/mouse_t *find_mouse (struct usb_device *device) {
      int i;

      for (i = 0; mice [i].pid; i++) {
            if (device->descriptor.idProduct == mice [i].pid) {
                  return &mice [i];
            }
      }

      return NULL;
}


/*
 * Description: search for the correct mouse in mice[] and get needed data.
 *              then run the command, the user specified.             
 * 
 * Parameters:  struct usb_bus *bus
 *                usb bus which should be searched for logitech mice.
 *
 * Retrun:      return 1 (always) <-- no errorhandling yet !TODO!
 */
int scan_bus (struct usb_bus *bus) {
      struct usb_device* roottree = bus->devices;
      struct usb_device *device;
      usb_dev_handle *handle;
      mouse_t *m;

      int ret = 0;
      char product[128];
      
      for (device = roottree; device; device = device->next) {

            /* continue with next iteration of the "for" loop if:
             * - ID of the Product is different from the one specified match_pid by cmdline.c
             * - Device name different from the one specified by match_dev from cmdline.c
             * - Bus is different from the one specified by match_bus from cmdline.c
             */
            if (match_pid && (device->descriptor.idProduct != match_pid))
                  continue;
            if ((match_dev != NULL) &&
                (strcmp (device->filename, match_dev)))
                  continue;
            if ((match_bus != NULL) &&
                (strcmp (device->bus->dirname, match_bus)))
                  continue;

            /* continue with next iteration of the for loop if it is no Logitech mouse */
            if (device->descriptor.idVendor != VENDOR_LOGITECH) {
                  if (command == cmd_scan) {
                        printf ("%s.%s: %04x:%04x Not a Logitech device\n",
                              device->bus->dirname,
                              device->filename,
                              device->descriptor.idVendor,
                              device->descriptor.idProduct);
                  }
                  continue;
            }

            /* Do we support this device? If so, list it. */
            if ((m = find_mouse (device)) != NULL) {
                  
                  printf ("%s.%s: %04x:%04x %s (%s) Caps: %s%s%s\n",
                        device->bus->dirname,
                        device->filename,
                        device->descriptor.idVendor,
                        device->descriptor.idProduct,
                        m->name,
                        m->model,
                        m->has_csr? "CSR ": "",
                        m->has_res? "RES ": "",
                        m->has_sms? "SMS ": ""
                        );
            }
            else {
                  
                  handle = usb_open (device);   
                  ret = usb_get_string_simple(  handle,
                                          device->descriptor.iProduct,
                                          product,
                                          sizeof(product));
                  (void) usb_close (handle);
                  
                  printf ("%s.%s: %04x:%04x Unsupported Logitech device: %s\n",
                        device->bus->dirname,
                        device->filename,
                        device->descriptor.idVendor,
                        device->descriptor.idProduct,
                        ret > 0 ? product : "Unknown"
                        );
                  continue;
            }
            
            /* Run commands */
            if (command == cmd_inquire) query (m, device);
            if (command == cmd_set) configure (m, device);

      }
      
      return 1;
}


/*
 * Description:   initialize usblib find all busses and devices
 *          afterwards check every device with function "scan_bus()"
 *               
 * Parameters:    no parameter
 *
 * Retrun:  void
 */
void run () {
      struct usb_bus* bus;
      
      usb_init();
      (void) usb_find_busses();
      (void) usb_find_devices();
      
      for (bus = usb_busses; bus != NULL; bus = bus->next) {
            if (!scan_bus(bus)) {
                  printf ("Exiting happily.\n");
                  exit (EXIT_SUCCESS);
            }
      }
}


/*
 * Description: Main function; calls function "cmdline_pars"
 *              of cmdline.c with arguments given to the program
 *              and finally it calls "run"
 *
 * Parameters:  standard program arguments
 *
 * Retrun:      0 on success
 */
int main (int argc, char **argv) {
      progname = argv[0];
      

      /* cmdline_parse puts all necessary things in following 
       * variables:
   *
       * extern command_t   command;                                                                   
       * extern int         set_res;
       * extern int         set_sms;
       * extern char       *match_bus;
       * extern char       *match_dev;
       * extern int         match_pid;
       * extern int         set_channel;
       * extern int         set_lock;
       * extern int         set_unlock;
       * extern int         set_clear;     
       */
      cmdline_parse (argc, argv);
        

      run();
      return EXIT_SUCCESS;
}