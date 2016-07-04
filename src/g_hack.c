/*
 * gcc -o g_hack g_hack.c
 * ./g_hack /dev/hidraw0
 *
 * g_hack - proof of conecpt code
 *
 *
 * Copyright (c) 2006-2009 Andreas Schneider <mail@cynapses.org>
 * Copyright (c) 2006-2007 Peter Feuerer <piie@piie.net>
 *
 * License: GPLv2 or later
 *
 * Additional Info:
 *
 * the G-series has two modes:
 *
 * 1. mode: no driver
 * in this mode there is a list of resolutions implemented in the mouse
 * and you can switch between them using the buttons
 *
 * 2. mode is: windows driver
 * in this mode the buttons talk to the driver
 
 * they have absolute no effect on the hardware in first place
 * the driver gets the button event looks in its table, which resolution to use
 * sends it to the mouse sets the leds and the resolution
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/hiddev.h>
 
#define VERSION "0.0.7"
 
#define VENDOR 0x046d
#define MOUSE_G5 0xc041
#define MOUSE_G7 0xc51a
 
// Icehawk78 - first insertion for new mouse product code
#define MOUSE_MX1100 0xc526
 
/* thanks to Michael "sanni" Dewein */
#define MOUSE_G3 0xc042
 
/* thanks to Niels "aboe" Abspoel */
#define MOUSE_G9 0xc048
 
 
static int send_report(int fd, const unsigned char *buf, size_t size) {
  struct hiddev_report_info rinfo;
  struct hiddev_usage_ref uref;
  int i, err;
 
  for (i = 0; i < size; i++) {
    memset(&uref, 0, sizeof(uref));
    uref.report_type = HID_REPORT_TYPE_OUTPUT;
    uref.report_id   = 0x10;
    uref.field_index = 0;
    uref.usage_index = i;
    uref.usage_code  = 0xff000001;
    uref.value       = buf[i];
 
    err = ioctl(fd, HIDIOCSUSAGE, &uref);
 
    if (err < 0)
      return err;
  }
 
  memset(&rinfo, 0, sizeof(rinfo));
  rinfo.report_type = HID_REPORT_TYPE_OUTPUT;
  rinfo.report_id   = 0x10;
  rinfo.num_fields  = 1;
  err = ioctl(fd, HIDIOCSREPORT, &rinfo);
 
  return err;
}
 
void send_msg(int fd,char c0,char c1,char c2, char c3, char c4, char c5)
{
  unsigned char b[6];
  b[0]=c0;
  b[1]=c1;
  b[2]=c2;
  b[3]=c3;
  b[4]=c4;
  b[5]=c5;
 
  if(send_report(fd,b,6)<0)
  {
    perror("error sending to device");
    close(fd);
    exit(1);
  }
}
int main (int argc, char **argv) {
 
  int fd = -1;
  int version;
  int resolution=0;
  struct hiddev_devinfo device_info;
 
  printf("g5hack version: %s\n\n", VERSION);
 
  /* ioctl() requires a file descriptor, so we check we got one, and
     then open it */
  if (argc != 3) {
    fprintf(stderr, "Usage: %s hiddevice resolution\n - hiddevice probably /dev/usb/hiddev0 or /dev/hidraw0\n - resolution: 0=400 1=800 2=1600 3=2000\n"
        "only g9: 4-7 (7=3200)\n\n", argv[0]);
 
    exit(1);
  }
  if ((fd = open(argv[1], O_RDONLY)) < 0) {
    perror("hiddev open");
    exit(1);
  }
  resolution=atoi(argv[2]);
 
  /* ioctl() accesses the underlying driver */
  ioctl(fd, HIDIOCGVERSION, &version);
 
  /* the HIDIOCGVERSION ioctl() returns an int
   * so we unpack it and display it
   * we create a patch
   */
  printf("hiddev driver version is %d.%d.%d\n", version >> 16, (version >> 8) & 0xff, version & 0xff);
 
  /* suck out some device information */
  ioctl(fd, HIDIOCGDEVINFO, &device_info);
 
  /* the HIDIOCGDEVINFO ioctl() returns hiddev_devinfo
 
   * structure - see <linux/hiddev.h>
   * So we work through the various elements, displaying
   * each of them
   */
  printf("vendor 0x%04hx product 0x%04hx version 0x%04hx ",
      device_info.vendor, device_info.product,
      device_info.version);
  printf("has %i application%s ", device_info.num_applications,
      (device_info.num_applications==1?"":"s"));
  printf("and is on bus: %d   devnum: %d   ifnum: %d\n",
      device_info.busnum, device_info.devnum,
      device_info.ifnum);
 
// Icehawk78 - Second section - need to add in the check so that the product continues with your mouse code. The printf isn't necessary, and was mostly added for consistency sake.
  /* We have a G5? */
  if((device_info.vendor == (short)VENDOR) &&
      ((device_info.product == (short)MOUSE_MX1100) ||
       (device_info.product == (short)MOUSE_G3) ||
       (device_info.product == (short)MOUSE_G9) ||
       (device_info.product == (short)MOUSE_G5) ||
       (device_info.product == (short)MOUSE_G7))) {
 
    if(device_info.product == (short)MOUSE_MX1100)
      printf(">> MX1100 Mouse detected!\n");
 
    if(device_info.product == (short)MOUSE_G3)
      printf(">>  G3 Gaming Mouse detected!\n");
 
    if(device_info.product == (short)MOUSE_G9)
      printf(">>  G9 Gaming Mouse detected!\n");
 
    if(device_info.product == (short)MOUSE_G5)
      printf(">>  G5 Gaming Mouse detected!\n");
 
    if(device_info.product == (short)MOUSE_G7)
      printf(">>  G7 cordless Gaming Mouse detected!\n");
    /*
     * Initialise the internal report structures
     *
     * The G5 has an additional HID Interface
     * with a report descriptor.
     */
    if (ioctl(fd, HIDIOCINITREPORT, 0) < 0) {
      perror("hid report init failed");
      exit(1);
 
    }
 
/* Icehawk78 - Now we're getting into the meat of it.
   This is the first place that you'll have to change.
   On my mouse, when changing between any two non-DPI-related modes,
   I captured a total of three URB_CONTROL out signals. The first and the third matched
   the first four digits [10 01 80 65] while the second did not [10 01 80 63].
   The second signal appears to be the first required signal, which I added here as
   a super-hacky conditional. Note that you drop the first 0x10 because send_msg()
   is already including that, for some reason. */
 
    /* disable speed buttons */
    if (device_info.product != (short)MOUSE_MX1100) {
      send_msg(fd,0x00, 0x80, 0x01, 0x00, 0x00, 0x00);
    } else {
      send_msg(fd,0x01,0x80,0x63,0x82,0x00,0x00);
    }
 
/* Icehawk78 - This is the second location to change.
   The first/third capture codes I put into the hacky if block inside
   case 0 and case 1; case 0 enables the buttons, case 1 sets it back to
   DPI mode. */
 
    switch(resolution){
      case 0:
        if (device_info.product == (short)MOUSE_MX1100) {
          send_msg(fd,0x01,0x80,0x65,0x82,0x85,0xFF);
          break;
        } else {
          /* set current speed to 400dpi */
          send_msg(fd,0x00,0x80,0x63,0x80,0x00,0x00);
 
          /* turn off all speed leds */
          send_msg(fd,0x00,0x80,0x51,0x11,0x01,0x00);
          break;
        }
 
      case 1:
        if (device_info.product != (short)MOUSE_MX1100) {
          /* set current speed to 800dpi */
          send_msg(fd,0x00,0x80,0x63,0x81,0x00,0x00);
          /* turn on first led */
          send_msg(fd,0x00,0x80,0x51,0x12,0x01,0x00);
          break;
        } else {
          send_msg(fd,0x00,0x80,0x65,0x82,0x85,0x9A);
          break;
        }
 
      case 2:
        /* set current speed to 1600dpi */
        send_msg(fd,0x00, 0x80, 0x63, 0x82, 0x00, 0x00);
        /* turn on second led */
        send_msg(fd,0x00, 0x80, 0x51, 0x21, 0x01, 0x00);
 
        break;
 
      case 3:
        /* set current speed to 2000dpi */
        send_msg(fd,0x00, 0x80, 0x63, 0x83, 0x00, 0x00);
        /* turn on third led */
        send_msg(fd,0x00, 0x80, 0x51, 0x11, 0x02, 0x00);
        break;
 
      case 4:
        /* set current speed to whatever dpi
         * - only available for g9 */
        if(device_info.product == (short)MOUSE_G9){
          send_msg(fd,0x00, 0x80, 0x63, 0x84, 0x00, 0x00);
          /* turn on third led */
          send_msg(fd,0x00, 0x80, 0x51, 0x11, 0x02, 0x00);
        }
        else{
          printf("speed not supported for your device\n");
        }
        break;
 
      case 5:
        /* set current speed to whatever dpi
         * - only available for g9 */
        if(device_info.product == (short)MOUSE_G9){
          send_msg(fd,0x00, 0x80, 0x63, 0x85, 0x00, 0x00);
          /* turn on third led */
          send_msg(fd,0x00, 0x80, 0x51, 0x11, 0x02, 0x00);
        }
        else{
          printf("speed not supported for your device\n");
        }
        break;
 
      case 6:
        /* set current speed to whatever dpi (most probably 3200)
         * - only available for g9 */
        if(device_info.product == (short)MOUSE_G9){
          send_msg(fd,0x00, 0x80, 0x63, 0x86, 0x00, 0x00);
          /* turn on third led */
          send_msg(fd,0x00, 0x80, 0x51, 0x11, 0x02, 0x00);
        }
        else{
 
          printf("speed not supported for your device\n");
        }
        break;
 
      case 7:
        /* set current speed to whatever dpi (most probably 3200)
         * - only available for g9 */
        if(device_info.product == (short)MOUSE_G9){
          send_msg(fd,0x00, 0x80, 0x63, 0x87, 0x00, 0x00);
          /* turn on third led */
          send_msg(fd,0x00, 0x80, 0x51, 0x11, 0x02, 0x00);
        }
        else{
          printf("speed not supported for your device\n");
        }
        break;
 
      default:
        printf("speed not supported for your device\n");
        break;
    }
 
    /* foobar: */
    /* at lowest speed ("-" button does not function anymore) */
    /* send_msg(fd,0x00, 0x81, 0x51, 0x00, 0x00, 0x00); */
 
    /* turn off all speed leds */
    /* send_msg(fd,0x00, 0x80, 0x51, 0x11, 0x01, 0x00); */
 
    /* turn on all speed leds */
    /* send_msg(fd,0x00, 0x80, 0x51, 0x22, 0x02, 0x00); */
 
  } /* G5 */
 
  close(fd);
 
  exit(0);
}