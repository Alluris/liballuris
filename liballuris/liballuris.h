/*

Copyright (C) 2015 Alluris GmbH & Co. KG <weber@alluris.de>

This file is part of liballuris.

Liballuris is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Liballuris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with liballuris. See ../COPYING.LESSER
If not, see <http://www.gnu.org/licenses/>.

*/

/*!
 * \file liballuris.h
 * \brief Header for generic Alluris device driver
*/

/*! \mainpage liballuris API Reference
 *
 * Generic driver for Alluris devices with USB measurement interface.
 *
 * - \ref liballuris.h
 * - \ref liballuris.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <libusb-1.0/libusb.h>

#ifndef liballuris_h
#define liballuris_h

//#define PRINT_DEBUG_MSG

//! Number of device which can be enumerated and simultaneously opened
#define MAX_NUM_DEVICES 4

//! Default timeout in milliseconds while writing to the device
#define DEFAULT_SEND_TIMEOUT 50

//! Default timeout in milliseconds while reading from the device
#define DEFAULT_RECEIVE_TIMEOUT 100

//! Default buffer size. Should be multiple of wMaxPacketSize
#define DEFAULT_SEND_BUF_LEN 64
#define DEFAULT_RECV_BUF_LEN 64
static unsigned char out_buf[DEFAULT_SEND_BUF_LEN];
static unsigned char in_buf[DEFAULT_RECV_BUF_LEN];

//! liballuris specific errors
enum liballuris_error
{
  LIBALLURIS_SUCCESS         = 0, //!< No error
  LIBALLURIS_MALFORMED_REPLY = 1, //!< The received reply contains a malformed header. This should never happen, check EMI and physical connection
  LIBALLURIS_DEVICE_BUSY     = 2, //!< Device is in a state where it cannot process the request
  LIBALLURIS_OUT_OF_RANGE    = 3  //!< Parameter out of valid range
};

//! measurement mode
enum liballuris_measurement_mode
{
  LIBALLURIS_MODE_STANDARD = 0, //!< 10Hz sampling rate
  LIBALLURIS_MODE_PEAK     = 1, //!< 900Hz sampling rate
  LIBALLURIS_MODE_PEAK_MAX = 2, //!< 900Hz sampling rate, maximum detection
  LIBALLURIS_MODE_PEAK_MIN = 3  //!< 900Hz sampling rate, minimum detection
};

//FIXME: add documentation, update liballuris_print_state
struct liballuris_state
{
  unsigned char dummy0 : 1;
  unsigned char pos_limit_exceeded    : 1; //!< Force > positive limit
  unsigned char neg_limit_underrun    : 1; //!< Force < negative limit
  unsigned char some_peak_mode_active : 1;
  unsigned char peak_plus_active : 1;
  unsigned char peak_minus_active : 1;
  unsigned char mem_active : 1;
  unsigned char dummy7 : 1;
  unsigned char dummy8 : 1;
  unsigned char dummy9 : 1;
  unsigned char overload : 1;
  unsigned char fracture : 1;
  unsigned char dummy12 : 1;
  unsigned char mem : 1;
  unsigned char mem_conti: 1;
  unsigned char dummy15 : 1;
  unsigned char grenz_option : 1;
  unsigned char dummy17 : 1;
  unsigned char dummy18 : 1;
  unsigned char dummy19 : 1;
  unsigned char dummy20 : 1;
  unsigned char dummy21 : 1;
  unsigned char dummy22 : 1;
  unsigned char measuring : 1;
};

union __liballuris_state__
{
  struct liballuris_state bits;
  unsigned int _int;
};

/*!
 * \brief composition of libusdb device and alluris device informations
 *
 * product and serial_number should help to identify a specific alluris device if more than one
 * device is connected via USB.
 * \sa get_alluris_device_list, open_alluris_device, free_alluris_device_list
 */
struct alluris_device_description
{
  libusb_device* dev;     //!< pointer to a structure representing a USB device detected on the system
  char product[30];       //!< product identification, for exmample "FMI-S Force-Gauge"
  char serial_number[30]; //!< serial number of device, for example "P.25412"
};

const char * liballuris_error_name (int error_code);

int liballuris_get_device_list (libusb_context* ctx, struct alluris_device_description* alluris_devs, size_t length, char read_serial);
int liballuris_open_device (libusb_context* ctx, const char* serial_number, libusb_device_handle** h);
int liballuris_open_device_with_id (libusb_context* ctx, int bus, int device, libusb_device_handle** h);
void liballuris_free_device_list (struct alluris_device_description* alluris_devs, size_t length);

void liballuris_clear_RX (libusb_device_handle* dev_handle, unsigned int timeout);

int liballuris_serial_number (libusb_device_handle *dev_handle, char* buf, size_t length);
int liballuris_digits (libusb_device_handle *dev_handle, int* v);
int liballuris_raw_value (libusb_device_handle *dev_handle, int* value);
int liballuris_raw_pos_peak (libusb_device_handle *dev_handle, int* peak);
int liballuris_raw_neg_peak (libusb_device_handle *dev_handle, int* peak);

/* read and print state */
int liballuris_read_state (libusb_device_handle *dev_handle, struct liballuris_state* state, unsigned int timeout);
void liballuris_print_state (libusb_device_handle *dev_handle, struct liballuris_state state);

int liballuris_cyclic_measurement (libusb_device_handle *dev_handle, char enable, size_t length);
int liballuris_poll_measurement (libusb_device_handle *dev_handle, int* buf, size_t length);

int liballuris_tare (libusb_device_handle *dev_handle);
int liballuris_clear_pos_peak (libusb_device_handle *dev_handle);
int liballuris_clear_neg_peak (libusb_device_handle *dev_handle);

int liballuris_start_measurement (libusb_device_handle *dev_handle);
int liballuris_stop_measurement (libusb_device_handle *dev_handle);

int liballuris_set_pos_limit (libusb_device_handle *dev_handle, int limit);
int liballuris_set_neg_limit (libusb_device_handle *dev_handle, int limit);

int liballuris_get_pos_limit (libusb_device_handle *dev_handle, int* limit);
int liballuris_get_neg_limit (libusb_device_handle *dev_handle, int* limit);

int liballuris_set_mode (libusb_device_handle *dev_handle, enum liballuris_measurement_mode mode);
int liballuris_get_mode (libusb_device_handle *dev_handle, enum liballuris_measurement_mode *mode);


#endif
