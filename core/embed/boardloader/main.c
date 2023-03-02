/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "compiler_traits.h"
#include "display.h"
#include "emmc.h"
#include "ff.h"
#include "flash.h"
#include "image.h"
#include "mini_printf.h"
#include "mipi_lcd.h"
#include "qspi_flash.h"
#include "rng.h"
#include "sdcard.h"
#include "sdram.h"
#include "touch.h"
#include "usb.h"

#include "lowlevel.h"
#include "version.h"

#include "memzero.h"

#include "atca_hal.h"

#include "atca_command.h"
#include "ble.h"
#include "sys.h"
#include "usart.h"

#define BOARD_MODE 1
#define BOOT_MODE 2

#if PRODUCTION
const uint8_t BOARDLOADER_KEY_M = 4;
const uint8_t BOARDLOADER_KEY_N = 7;
#else
const uint8_t BOARDLOADER_KEY_M = 2;
const uint8_t BOARDLOADER_KEY_N = 3;
#endif

static const uint8_t * const BOARDLOADER_KEYS[] = {
#if PRODUCTION
    (const uint8_t
    *)"\x15\x4b\x8a\xb2\x61\xcc\x88\x79\x48\x3f\x68\x9a\x2d\x41\x24\x3a\xe7\xdb\xc4\x02\x16\x72\xbb\xd2\x5c\x33\x8a\xe8\x4d\x93\x11\x54",
    (const uint8_t
    *)"\xa9\xe6\x5e\x07\xfe\x6d\x39\xa8\xa8\x4e\x11\xa9\x96\xa0\x28\x3f\x88\x1e\x17\x5c\xba\x60\x2e\xb5\xac\x44\x2f\xb7\x5b\x39\xe8\xe0",
    (const uint8_t
    *)"\x6c\x88\x05\xab\xb2\xdf\x9d\x36\x79\xf1\xd2\x8a\x40\xcd\x99\x03\x99\xb9\x9f\xc3\xee\x4e\x06\x57\xd8\x1d\x38\x1e\xa1\x48\x8a\x12",
    (const uint8_t
    *)"\x3e\xd7\x97\x79\x06\x4d\x56\x57\x1b\x29\xbc\xaa\x73\x4c\xbb\x6d\xb6\x1d\x2e\x62\x65\x66\x62\x8e\xcf\x4c\x89\xe1\xdb\x45\xea\xec",
    (const uint8_t
    *)"\x54\xa4\x06\x33\xbf\xd9\xe6\x0b\x8a\x39\x12\x65\xb2\xe0\x06\x37\x4a\xbe\x63\x1d\x1e\x11\x07\x33\x2b\xca\x56\xbf\x9f\x8c\x5c\x99",
    (const uint8_t
    *)"\x4b\x71\x13\x4f\x18\xe0\x07\x87\xc5\x83\xd4\x07\x42\xcc\x18\x8e\x17\xfc\x85\xad\xe4\xcb\x47\x2d\xae\x5e\xf8\xe0\x69\xf0\xfe\xc5",
    (const uint8_t
    *)"\x2e\xcf\x80\xc8\x2b\x44\x98\x48\xc0\x00\x33\x50\x92\x13\x95\x51\xbf\xe4\x7b\x3c\x73\x17\xb4\x99\x50\xf6\x5e\x1d\x82\x43\x20\x24",
#else
// TREZOR dev_key
// (const uint8_t
// *)"\xdb\x99\x5f\xe2\x51\x69\xd1\x41\xca\xb9\xbb\xba\x92\xba\xa0\x1f\x9f\x2e\x1e\xce\x7d\xf4\xcb\x2a\xc0\x51\x90\xf3\x7f\xcc\x1f\x9d",
// (const uint8_t
// *)"\x21\x52\xf8\xd1\x9b\x79\x1d\x24\x45\x32\x42\xe1\x5f\x2e\xab\x6c\xb7\xcf\xfa\x7b\x6a\x5e\xd3\x00\x97\x96\x0e\x06\x98\x81\xdb\x12",
// (const uint8_t
// *)"\x22\xfc\x29\x77\x92\xf0\xb6\xff\xc0\xbf\xcf\xdb\x7e\xdb\x0c\x0a\xa1\x4e\x02\x5a\x36\x5e\xc0\xe3\x42\xe8\x6e\x38\x29\xcb\x74\xb6",

// ONEKEY dev_key
(const uint8_t
*)"\xEC\x3C\x75\x23\xE9\x1D\x55\x7D\xD2\xA5\x83\x05\xD6\xF9\x77\x64\xB2\xA2\x54\xC6\x19\x97\x7B\x25\x10\xD4\xE7\xE1\x8A\x83\x21\x14",
(const uint8_t
*)"\xF4\x79\xA8\x44\x45\x22\xB3\xF5\x81\x49\xB3\x31\x85\xA5\x07\x68\xCD\xFF\xC0\x28\x5D\x54\x69\xF4\x0D\xB6\x55\x45\x8E\x86\xED\x60",
(const uint8_t
*)"\x6D\xDA\xA3\x3C\x09\x1F\x0C\xB0\x20\x43\xF6\x9E\x2D\x2A\xF7\x93\x29\x6F\x65\x91\x3C\x2F\xBC\x65\xCD\xC5\x64\x67\xB1\x80\x30\xBA",
#endif
};

// clang-format off
static const uint8_t toi_icon_onekey[] = {
    // magic
    'T', 'O', 'I', 'f',
    // width (16-bit), height (16-bit)
    0x80, 0x00, 0x80, 0x00,
    // compressed data length (32-bit)
    0x4c, 0x05, 0x00, 0x00,
    // compressed data
    0xed, 0x92, 0x2b, 0x8e, 0xeb, 0x30, 0x14, 0x86, 0x0d, 0x0c, 0x0c, 0x02, 0x0c, 0xba, 0x01, 0x2f, 0x21, 0x5b, 0x30, 0x1c, 0x58, 0x38, 0x34, 0x68, 0x34, 0x1a, 0x70, 0x15, 0x05, 0x44, 0x95, 0x49, 0x14, 0x05, 0x54, 0x57, 0x01, 0x55, 0x35, 0x28, 0xb4, 0x30, 0x70, 0xa0, 0xb7, 0x90, 0x25, 0x64, 0x03, 0x03, 0x0c, 0x02, 0x0c, 0x02, 0x7c, 0x55, 0x5d, 0x55, 0xd3, 0x36, 0x7d, 0x24, 0xed, 0xc9, 0x73, 0xf2, 0x85, 0xb5, 0xb6, 0xff, 0xf3, 0x9d, 0x73, 0x10, 0x9a, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x81, 0x85, 0xe1, 0xe5, 0xda, 0xb1, 0x42, 0x9a, 0x14, 0x49, 0x21, 0x75, 0x5e, 0xee, 0x3f, 0x73, 0xc6, 0xfe, 0xb7, 0xcc, 0x97, 0x3a, 0xf5, 0x92, 0x22, 0xa4, 0x8e, 0xc5, 0x89, 0x1d, 0x8d, 0xd9, 0x99, 0x06, 0x9c, 0xbc, 0x7f, 0x24, 0x45, 0xd5, 0xb4, 0x09, 0x99, 0x9f, 0x7a, 0x21, 0xe5, 0x84, 0x06, 0x63, 0xf2, 0xde, 0x6e, 0x32, 0xdf, 0x00, 0x93, 0xf9, 0xdb, 0xcd, 0x72, 0x3d, 0xe4, 0x3e, 0xd0, 0xc0, 0xb1, 0xa4, 0x56, 0xc2, 0xb4, 0x4a, 0xea, 0xbd, 0x7e, 0x0f, 0xaf, 0x0b, 0xfb, 0x99, 0xb7, 0x6d, 0xfe, 0x83, 0x12, 0x49, 0xc1, 0xc9, 0x70, 0xdc, 0xa5, 0x36, 0x3d, 0x20, 0xf5, 0xeb, 0xf7, 0x6f, 0x75, 0x3f, 0x90, 0x97, 0xfd, 0xf5, 0x80, 0xe1, 0x7e, 0xdd, 0x0f, 0xa4, 0x1e, 0xc3, 0xdd, 0xdb, 0xbf, 0x7f, 0x28, 0x61, 0x06, 0xc3, 0x4a, 0xfd, 0xc6, 0xc9, 0x1f, 0x93, 0xf9, 0x5d, 0x6d, 0x01, 0x27, 0x43, 0x9a, 0xfc, 0x0f, 0x79, 0x69, 0x47, 0x5d, 0xec, 0xbd, 0x19, 0x30, 0x6e, 0xdc, 0xae, 0xfd, 0x4a, 0x99, 0x81, 0xb3, 0x52, 0xbf, 0xd9, 0xbe, 0xcd, 0x0e, 0x0c, 0x7b, 0xf3, 0x8f, 0x71, 0x63, 0x78, 0x7b, 0x3b, 0x32, 0x23, 0x82, 0x13, 0x58, 0x7b, 0x86, 0xf3, 0x72, 0x4c, 0xfe, 0x4a, 0x30, 0x0c, 0xe9, 0x9f, 0xf9, 0x66, 0x64, 0x48, 0x0d, 0x67, 0xbf, 0x52, 0x66, 0x84, 0xb8, 0x31, 0x8c, 0x3d, 0xc3, 0x66, 0x94, 0x28, 0xc1, 0x30, 0x84, 0x7f, 0xea, 0x99, 0x91, 0x22, 0xf5, 0xf3, 0xf6, 0x8e, 0x65, 0x46, 0x0c, 0x27, 0xcf, 0xfa, 0xe7, 0x25, 0x5c, 0x35, 0x79, 0x79, 0xeb, 0x1b, 0xe2, 0x06, 0x40, 0x4e, 0x3f, 0x29, 0x6e, 0x25, 0x31, 0x3c, 0xc4, 0x0d, 0x90, 0x1a, 0xae, 0x92, 0x97, 0xb7, 0x3e, 0xfc, 0x9f, 0xd9, 0x00, 0x4e, 0xe0, 0xea, 0x50, 0xe2, 0x76, 0x56, 0x5b, 0xfe, 0xcf, 0x6c, 0xc0, 0x6e, 0x01, 0x57, 0x45, 0x52, 0xf4, 0xe5, 0xbf, 0xdd, 0x3c, 0x66, 0x4f, 0x03, 0x25, 0xe0, 0xaa, 0x78, 0x79, 0xeb, 0xcb, 0x5f, 0x09, 0x1a, 0x3c, 0xe2, 0xef, 0x58, 0x90, 0x55, 0xdc, 0xab, 0xa1, 0x3d, 0x7f, 0x63, 0x1c, 0xeb, 0x11, 0xff, 0xd4, 0x83, 0xab, 0xe0, 0xeb, 0xf3, 0x5e, 0x5a, 0x9b, 0xfe, 0x52, 0x37, 0xb7, 0xa7, 0x01, 0x64, 0x05, 0x21, 0x65, 0xf8, 0xf8, 0xab, 0x6e, 0x43, 0x9b, 0xfe, 0x4a, 0xdc, 0xdb, 0xbe, 0x2a, 0xcb, 0xb5, 0x69, 0x91, 0xa4, 0xe8, 0xd2, 0xdf, 0x18, 0x4e, 0x9a, 0xfa, 0x6f, 0x37, 0x53, 0xf2, 0xdf, 0x6e, 0x9a, 0xfa, 0x67, 0xfe, 0x94, 0xfc, 0x33, 0xbf, 0x99, 0x3d, 0x0d, 0x8c, 0x99, 0x92, 0xbf, 0x31, 0x34, 0x68, 0xe2, 0xcf, 0xc9, 0xd4, 0xfc, 0x39, 0x69, 0xe2, 0x1f, 0xd2, 0xa9, 0xf9, 0xbb, 0x71, 0x13, 0xff, 0xd4, 0x9b, 0x9a, 0x7f, 0x35, 0xf1, 0x16, 0x99, 0x3f, 0x35, 0xff, 0xbc, 0x6c, 0xe2, 0x6f, 0xcc, 0xd4, 0xfc, 0x8d, 0xa9, 0x6f, 0x6f, 0x47, 0x53, 0xf4, 0x67, 0xb8, 0xae, 0x3f, 0x27, 0x53, 0xf4, 0x7f, 0x79, 0xab, 0xeb, 0xef, 0x58, 0x53, 0xf4, 0x77, 0xac, 0xba, 0xfe, 0x21, 0x9d, 0xa2, 0x7f, 0x48, 0xeb, 0xfa, 0xef, 0x16, 0x53, 0xf4, 0xaf, 0x66, 0x5e, 0x23, 0xf5, 0x7e, 0xb7, 0xbf, 0xd4, 0x53, 0xf4, 0x97, 0xba, 0xae, 0x7f, 0xe6, 0xb7, 0x5f, 0x8b, 0x63, 0x9d, 0x7e, 0x6e, 0xdc, 0x76, 0x66, 0x5e, 0xd6, 0xf5, 0xcf, 0x4b, 0x33, 0x41, 0x66, 0xff, 0xba, 0xfe, 0xdd, 0x55, 0xf4, 0xff, 0xeb, 0x2a, 0x6f, 0x28, 0xfe, 0x52, 0xbf, 0x7f, 0xd8, 0x11, 0x0d, 0x7e, 0x12, 0xed, 0xe8, 0xfd, 0x43, 0xea, 0xe9, 0xfb, 0x2b, 0x11, 0xd2, 0x63, 0xef, 0x53, 0x18, 0xde, 0x2d, 0xa6, 0xec, 0x2f, 0x35, 0xc3, 0xf7, 0xb2, 0xdb, 0xec, 0x41, 0xbf, 0xfe, 0x6e, 0x5c, 0x37, 0x3f, 0xa4, 0x53, 0xf3, 0x57, 0x62, 0xb9, 0x46, 0x0d, 0x58, 0xae, 0x95, 0x98, 0x92, 0x7f, 0x33, 0xfb, 0xff, 0x1d, 0x98, 0x8e, 0x7f, 0x48, 0xd1, 0x03, 0xb8, 0x71, 0x5f, 0xfe, 0x79, 0x09, 0x99, 0x9a, 0x14, 0x97, 0x53, 0x68, 0xc0, 0x89, 0x63, 0x39, 0x16, 0x27, 0x34, 0xb8, 0x7c, 0x42, 0x6a, 0xc8, 0x3a, 0xf2, 0xb2, 0x1f, 0x7f, 0x86, 0xab, 0x09, 0x9c, 0x9c, 0xba, 0xa5, 0x9e, 0x1d, 0x55, 0x4f, 0x31, 0xac, 0xc4, 0xd8, 0xfd, 0x2f, 0x4d, 0xff, 0xef, 0x9f, 0x4b, 0x27, 0xff, 0xfe, 0xa9, 0x9e, 0x0c, 0x29, 0x5c, 0x25, 0x99, 0x5f, 0xd7, 0x1f, 0x72, 0xef, 0xaa, 0xd3, 0xbf, 0xfe, 0xba, 0xd4, 0xd5, 0x0d, 0x80, 0xab, 0xa4, 0xfa, 0xfa, 0x35, 0x76, 0x8b, 0xf6, 0x32, 0x2f, 0xcf, 0xfe, 0xfa, 0x0e, 0xc0, 0xcd, 0xe2, 0xeb, 0xb3, 0x7b, 0x7f, 0x37, 0x6e, 0x3a, 0x4f, 0x4e, 0x4e, 0x6f, 0xb8, 0x31, 0x54, 0x2d, 0x49, 0x51, 0xd7, 0x3f, 0xa4, 0x50, 0x99, 0xe7, 0x36, 0xf7, 0x3b, 0x7b, 0xbe, 0x31, 0x9c, 0x40, 0xd5, 0x12, 0xd2, 0xba, 0xfe, 0x8e, 0x05, 0x95, 0x49, 0x83, 0xe3, 0x77, 0x69, 0x70, 0xff, 0x86, 0x12, 0xcd, 0xef, 0xd4, 0xc3, 0xb1, 0xea, 0xfa, 0x2f, 0xd7, 0x30, 0x89, 0x4a, 0x3c, 0x32, 0x4b, 0x3b, 0x3a, 0xbd, 0xa5, 0x44, 0x3b, 0xbb, 0x78, 0x1d, 0x86, 0x61, 0x12, 0xf3, 0xf2, 0x91, 0xbe, 0x9e, 0xcf, 0x29, 0x2f, 0x61, 0xaa, 0x39, 0xef, 0xeb, 0x2d, 0xda, 0xf1, 0x77, 0xac, 0x3a, 0xb7, 0x5e, 0xde, 0xda, 0xf1, 0x47, 0x0d, 0x80, 0xc9, 0x54, 0xe2, 0xf4, 0x55, 0x3b, 0x7a, 0x64, 0x4f, 0x95, 0x80, 0xa8, 0x25, 0xf3, 0x9b, 0xf8, 0xef, 0x16, 0x30, 0x3d, 0xa7, 0xc1, 0xf1, 0xab, 0x34, 0xa8, 0xe3, 0x72, 0x5a, 0x09, 0x0d, 0x60, 0x2a, 0xf9, 0xfa, 0x6c, 0xe2, 0xef, 0xc6, 0x30, 0xa9, 0xe7, 0xb3, 0x94, 0xfa, 0xde, 0x8d, 0xa4, 0x38, 0xbd, 0xc1, 0x09, 0x4c, 0x25, 0x21, 0x6d, 0xe2, 0x0f, 0x95, 0xea, 0xc6, 0x4d, 0xdf, 0x65, 0xb8, 0x9b, 0x49, 0xdc, 0x06, 0x6a, 0xeb, 0xa4, 0x3e, 0x7f, 0x79, 0xbb, 0x69, 0x36, 0xa5, 0xfb, 0x1b, 0x53, 0x0f, 0x1a, 0xa0, 0x46, 0x64, 0x3e, 0x4c, 0xae, 0x1d, 0x9d, 0xbf, 0xbc, 0x5b, 0x5c, 0x3b, 0xbb, 0x5b, 0x9c, 0x9f, 0x65, 0x18, 0xa6, 0x8a, 0xcc, 0x47, 0x0d, 0xb9, 0x3d, 0xa7, 0xfa, 0x24, 0x45, 0xf5, 0xed, 0x90, 0x5e, 0x3a, 0xb9, 0xdd, 0x54, 0x4f, 0x5e, 0xef, 0x55, 0x33, 0x2e, 0xbd, 0x7d, 0x1b, 0x4e, 0x0c, 0x10, 0x0c, 0x57, 0x5f, 0x67, 0x78, 0xb7, 0x50, 0xe2, 0x70, 0x42, 0x89, 0xa4, 0xe0, 0xe4, 0xd2, 0x29, 0xa8, 0x1a, 0x5e, 0xde, 0x9a, 0xfa, 0xd3, 0xe0, 0xa7, 0xbe, 0xe7, 0x90, 0xfa, 0x7a, 0x8f, 0x39, 0x59, 0xae, 0xed, 0x88, 0x06, 0x97, 0xff, 0x97, 0x1a, 0xca, 0xff, 0x5a, 0xc2, 0x2d, 0xe0, 0xd2, 0x57, 0x0a, 0x3d, 0xc0, 0x4a, 0x41, 0xe5, 0x7f, 0x7d, 0x3e, 0x92, 0xef, 0x58, 0x06, 0x8c, 0xd7, 0xef, 0xa6, 0xe9, 0xaf, 0xdf, 0x7d, 0xa6, 0xef, 0xa1, 0x81, 0x12, 0x7d, 0xd5, 0x00, 0x69, 0xaf, 0x04, 0x0d, 0xd0, 0x43, 0x6c, 0x37, 0x06, 0x90, 0x95, 0xaa, 0x9b, 0x0b, 0xb7, 0xf9, 0x7b, 0x92, 0x02, 0x3d, 0x08, 0x27, 0x06, 0x94, 0xbc, 0x64, 0xf8, 0x5e, 0xa6, 0x1d, 0x49, 0x0d, 0x9b, 0xca, 0x09, 0x7a, 0x18, 0xe8, 0x5a, 0xf6, 0xd3, 0xb8, 0xde, 0x03, 0x4e, 0x76, 0x0b, 0xe8, 0x3c, 0xa9, 0xd1, 0x13, 0x40, 0x6f, 0xc0, 0x7f, 0x32, 0x3f, 0xa4, 0x9c, 0xd0, 0xe0, 0x90, 0x42, 0x03, 0x4e, 0x56, 0x0a, 0xbe, 0xd7, 0x7b, 0x5e, 0xbf, 0xd1, 0x53, 0xb4, 0x53, 0xd5, 0x81, 0xbc, 0xcc, 0xcb, 0x76, 0xdf, 0x47, 0x4f, 0xd2, 0xce, 0x06, 0x74, 0xc5, 0xb3, 0xd3, 0x6f, 0x7f, 0x03, 0xda, 0x24, 0x29, 0x10, 0x00, 0x0c, 0x2b, 0x31, 0x4e, 0x7f, 0x86, 0x11, 0x08, 0x6e, 0x3c, 0x46, 0xfb, 0x90, 0x22, 0x30, 0xa4, 0x1e, 0x9b, 0x7d, 0xe6, 0x23, 0x40, 0x18, 0x56, 0x62, 0x4c, 0xf6, 0x79, 0xc9, 0x30, 0x02, 0x85, 0x93, 0x31, 0xf9, 0xdb, 0x11, 0x02, 0xc7, 0x8d, 0xc7, 0x62, 0xef, 0xc6, 0xa8, 0x15, 0x42, 0x3a, 0x06, 0xfb, 0x90, 0xa2, 0xd6, 0x18, 0x7e, 0x07, 0xda, 0xb4, 0xdf, 0xe3, 0xc6, 0x43, 0xb6, 0x6f, 0x6b, 0xf3, 0x8f, 0xb1, 0xa3, 0xbc, 0x1c, 0xa2, 0xbb, 0x12, 0x9c, 0xa0, 0x4e, 0x60, 0x38, 0xf3, 0x87, 0x66, 0x2f, 0x35, 0xc3, 0xa8, 0x43, 0x42, 0x3a, 0xa4, 0xc9, 0x77, 0xb1, 0xf7, 0xd5, 0x2d, 0x48, 0xbd, 0xdf, 0x38, 0xf9, 0x63, 0x1c, 0x2b, 0x2f, 0xfb, 0x75, 0xe7, 0x04, 0xf5, 0x8c, 0x63, 0x49, 0xfd, 0x5b, 0xdd, 0x0f, 0x70, 0xb2, 0x5b, 0x28, 0xd1, 0x95, 0xb9, 0x12, 0xdb, 0xcd, 0x70, 0xdc, 0x0f, 0xd0, 0xc0, 0xb1, 0x52, 0xaf, 0x6d, 0x73, 0xa9, 0x1d, 0x8b, 0x06, 0x68, 0xb0, 0xd0, 0x60, 0xb9, 0xde, 0x6e, 0x32, 0x1f, 0xda, 0x3c, 0xf3, 0xf7, 0x33, 0x1f, 0xb2, 0xf9, 0x79, 0x1f, 0x38, 0x09, 0x69, 0xea, 0x3d, 0xd7, 0x89, 0xbc, 0x4c, 0x8a, 0xf7, 0x8f, 0x31, 0x79, 0x5f, 0xc2, 0x8e, 0x38, 0x71, 0xac, 0x90, 0x26, 0x45, 0xea, 0x49, 0x9d, 0xf9, 0x79, 0x99, 0x97, 0x55, 0xd3, 0xfd, 0x27, 0x75, 0x52, 0x24, 0x45, 0x48, 0x1d, 0x6b, 0xb9, 0x66, 0x18, 0xcd, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x00, 0xf3, 0x0f,
};
// clang-format on

#if defined(STM32H747xx)

#include "stm32h7xx_hal.h"

static FATFS fs_instance;
PARTITION VolToPart[FF_VOLUMES] = {
    {0, 1},
    {0, 2},
};
uint32_t *sdcard_buf = (uint32_t *)0x24000000;

void fatfs_init(void) {
  FRESULT res;
  BYTE work[FF_MAX_SS];
  MKFS_PARM mk_para = {
      .fmt = FM_FAT32,
  };

  LBA_t plist[] = {
      BOOT_EMMC_BLOCKS,
      100};  // 1G sectors for 1st partition and left all for 2nd partition

  res = f_mount(&fs_instance, "", 1);
  if (res != FR_OK) {
    if (res == FR_NO_FILESYSTEM) {
      res = f_fdisk(0, plist, work); /* Divide physical drive 0 */
      if (res) {
        display_printf("f_fdisk error%d\n", res);
      }
      res = f_mkfs("0:", &mk_para, work, sizeof(work));
      if (res) {
        display_printf("f_mkfs 0 error%d\n", res);
      }
      res = f_mkfs("1:", &mk_para, work, sizeof(work));
      if (res) {
        display_printf("fatfs Format error");
      }
      f_setlabel("Onekey Data");
    } else {
      display_printf("mount err %d\n", res);
    }
  }
}

int fatfs_check_res(void) {
  FRESULT res;
  FIL fsrc;
  res = f_mount(&fs_instance, "", 1);
  if (res != FR_OK) {
    display_printf("fatfs mount error");
    return 0;
  }
  res = f_open(&fsrc, "/res/.ONEKEY_RESOURCE", FA_READ);
  if (res != FR_OK) {
    f_unmount("");
  }
  return res;
}

static uint32_t check_sdcard(void) {
  FRESULT res;

  res = f_mount(&fs_instance, "", 1);
  if (res != FR_OK) {
    return 0;
  }
  uint64_t cap = emmc_get_capacity_in_bytes();
  if (cap < 1024 * 1024) {
    return 0;
  }

  memzero(sdcard_buf, IMAGE_HEADER_SIZE);

  FIL fsrc;
  UINT num_of_read = 0;

  res = f_open(&fsrc, "/boot/bootloader.bin", FA_READ);
  if (res != FR_OK) {
    return 0;
  }
  res = f_read(&fsrc, sdcard_buf, IMAGE_HEADER_SIZE, &num_of_read);
  if ((num_of_read != IMAGE_HEADER_SIZE) || (res != FR_OK)) {
    f_close(&fsrc);
    return 0;
  }
  f_close(&fsrc);

  image_header hdr_old;
  image_header hdr_new;
  secbool new_present = secfalse, old_present = secfalse;

  old_present = load_image_header(
      (const uint8_t *)BOOTLOADER_START, BOOTLOADER_IMAGE_MAGIC,
      BOOTLOADER_IMAGE_MAXSIZE, BOARDLOADER_KEY_M, BOARDLOADER_KEY_N,
      BOARDLOADER_KEYS, &hdr_old);

  new_present =
      load_image_header((const uint8_t *)sdcard_buf, BOOTLOADER_IMAGE_MAGIC,
                        BOOTLOADER_IMAGE_MAXSIZE, BOARDLOADER_KEY_M,
                        BOARDLOADER_KEY_N, BOARDLOADER_KEYS, &hdr_new);
  if (sectrue == new_present && secfalse == old_present) {
    return hdr_new.codelen;
  } else if (sectrue == new_present && sectrue == old_present) {
    if (memcmp(&hdr_new.version, &hdr_old.version, 4) > 0) {
      return hdr_new.codelen;
    }
  }
  return 0;
}

static void progress_callback(int pos, int len) { display_printf("."); }

static secbool copy_sdcard(uint32_t code_len) {
  display_backlight(255);

  display_printf("OneKey Boardloader\n");
  display_printf("==================\n\n");

  display_printf("new version bootloader found\n\n");
  display_printf("applying bootloader in 5 seconds\n\n");
  display_printf("touch screen if you want to abort\n\n");

  uint32_t touched = 0;
  for (int i = 5; i >= 0; i--) {
    display_printf("%d ", i);
    hal_delay(1000);

    touched = touch_is_detected() | touch_read();
    if (touched) {
      display_printf("\n\ncanceled, aborting\n");
      display_printf("Device will be restart in 3 seconds\n");

      for (int i = 3; i >= 0; i--) {
        display_printf("%d ", i);
        hal_delay(1000);
      }
      HAL_NVIC_SystemReset();
      return secfalse;
    }
  }

  display_printf("\n\nerasing flash:\n\n");

  // erase all flash (except boardloader)
  static const uint8_t sectors[] = {
      FLASH_SECTOR_BOOTLOADER_1,
      FLASH_SECTOR_BOOTLOADER_2,
  };

  if (sectrue !=
      flash_erase_sectors(sectors, sizeof(sectors), progress_callback)) {
    display_printf(" failed\n");
    return secfalse;
  }
  display_printf(" done\n\n");

  ensure(flash_unlock_write(), NULL);

  // copy bootloader from SD card to Flash
  display_printf("copying new bootloader from SD card\n\n");

  memzero(sdcard_buf, EMMC_BLOCK_SIZE);

  FIL fsrc;
  FRESULT res;
  UINT num_of_read;
  res = f_open(&fsrc, "/boot/bootloader.bin", FA_READ);
  if (res != FR_OK) {
    return secfalse;
  }
  int blocks = (IMAGE_HEADER_SIZE + code_len) / EMMC_BLOCK_SIZE;
  int percent = 0, percent_bak = 0;
  for (int i = 0; i < blocks; i++) {
    percent = (i * 100) / blocks;
    if (percent != percent_bak) {
      percent_bak = percent;
      display_printf("%d ", percent);
    }

    f_lseek(&fsrc, i * EMMC_BLOCK_SIZE);
    res = f_read(&fsrc, sdcard_buf, EMMC_BLOCK_SIZE, &num_of_read);
    if ((num_of_read != EMMC_BLOCK_SIZE) || (res != FR_OK)) {
      f_close(&fsrc);
      return secfalse;
    }
    if (i * EMMC_BLOCK_SIZE < FLASH_FIRMWARE_SECTOR_SIZE) {
      for (int j = 0; j < EMMC_BLOCK_SIZE / (sizeof(uint32_t) * 8); j++) {
        ensure(
            flash_write_words(FLASH_SECTOR_BOOTLOADER_1,
                              i * EMMC_BLOCK_SIZE + j * (sizeof(uint32_t) * 8),
                              (uint32_t *)&sdcard_buf[8 * j]),
            NULL);
      }
    } else {
      for (int j = 0; j < EMMC_BLOCK_SIZE / (sizeof(uint32_t) * 8); j++) {
        ensure(flash_write_words(
                   FLASH_SECTOR_BOOTLOADER_2,
                   (i - FLASH_FIRMWARE_SECTOR_SIZE / EMMC_BLOCK_SIZE) *
                           EMMC_BLOCK_SIZE +
                       j * (sizeof(uint32_t) * 8),
                   (uint32_t *)&sdcard_buf[8 * j]),
               NULL);
      }
    }
  }
  f_close(&fsrc);
  ensure(flash_lock_write(), NULL);

  display_printf("\ndone\n\n");
  display_printf("Device will be restart in 3 seconds\n");

  for (int i = 3; i >= 0; i--) {
    display_printf("%d ", i);
    hal_delay(1000);
  }
  HAL_NVIC_SystemReset();
  return sectrue;
}

#define COLOR_BL_BG COLOR_BLACK                   // background
#define COLOR_BL_FG COLOR_WHITE                   // foreground
#define COLOR_BL_FAIL RGB16(0xFF, 0x00, 0x00)     // red
#define COLOR_BL_DONE RGB16(0x00, 0xAE, 0x0B)     // green
#define COLOR_BL_PROCESS RGB16(0x4A, 0x90, 0xE2)  // blue
#define COLOR_BL_GRAY RGB16(0x99, 0x99, 0x99)     // gray

typedef enum {
  SCREEN_TEST = 0,
  TOUCH_TEST,
  SE_TEST,
  SPI_FLASH_TEST,
  EMMC_TEST,
  SDRAM_TEST,
  BEEP_TEST,
  MOTOR_TEST,
  BLE_TEST,
  TEST_NUMS
} TEST_ITEM;

static uint16_t screen_bg[TEST_NUMS + 1];

static void ui_generic_confirm_simple(const char *msg) {
  if (msg == NULL) return;
  display_clear();
  display_text_center(DISPLAY_RESX / 2, DISPLAY_RESY / 2, msg, -1, FONT_NORMAL,
                      COLOR_WHITE, COLOR_BLACK);

  display_bar_radius(32, DISPLAY_RESY - 160, 128, 64, COLOR_RED, COLOR_BLACK,
                     16);
  display_bar_radius(DISPLAY_RESX - 32 - 128, DISPLAY_RESY - 160, 128, 64,
                     COLOR_GREEN, COLOR_BLACK, 16);
  display_text(80, DISPLAY_RESY - 120, "No", -1, FONT_NORMAL, COLOR_WHITE,
               COLOR_RED);
  display_text(DISPLAY_RESX - 118, DISPLAY_RESY - 120, "Yes", -1, FONT_NORMAL,
               COLOR_WHITE, COLOR_GREEN);
}

static bool ui_response(void) {
  for (;;) {
    uint32_t evt = touch_click();
    uint16_t x = touch_unpack_x(evt);
    uint16_t y = touch_unpack_y(evt);

    if (!evt) {
      continue;
    }
    // clicked on Cancel button
    if (x >= 32 && x < 32 + 128 && y > DISPLAY_RESY - 160 &&
        y < DISPLAY_RESY - 160 + 64) {
      return false;
    }
    // clicked on Confirm button
    if (x >= DISPLAY_RESX - 32 - 128 && x < DISPLAY_RESX - 32 &&
        y > DISPLAY_RESY - 160 && y < DISPLAY_RESY - 160 + 64) {
      return true;
    }
  }
}

void screen_test(void) {
  display_bar(0, 0, MAX_DISPLAY_RESX, MAX_DISPLAY_RESY, COLOR_RED);
  display_text_center(MAX_DISPLAY_RESX / 2, MAX_DISPLAY_RESY / 2,
                      "TOUCH SCREEN", -1, FONT_NORMAL, COLOR_BL_FG, COLOR_RED);
  while (!touch_click()) {
  }

  display_bar(0, 0, MAX_DISPLAY_RESX, MAX_DISPLAY_RESY, COLOR_GREEN);
  display_text_center(MAX_DISPLAY_RESX / 2, MAX_DISPLAY_RESY / 2,
                      "TOUCH SCREEN", -1, FONT_NORMAL, COLOR_BL_FG,
                      COLOR_GREEN);
  while (!touch_click()) {
  }
  display_bar(0, 0, MAX_DISPLAY_RESX, MAX_DISPLAY_RESY, COLOR_BLUE);
  display_text_center(MAX_DISPLAY_RESX / 2, MAX_DISPLAY_RESY / 2,
                      "TOUCH SCREEN", -1, FONT_NORMAL, COLOR_BL_FG, COLOR_BLUE);
  while (!touch_click()) {
  }
  display_bar(0, 0, MAX_DISPLAY_RESX, MAX_DISPLAY_RESY, COLOR_BLACK);
  display_text_center(MAX_DISPLAY_RESX / 2, MAX_DISPLAY_RESY / 2,
                      "TOUCH SCREEN", -1, FONT_NORMAL, COLOR_BL_FG,
                      COLOR_BLACK);
  while (!touch_click()) {
  }
  display_bar(0, 0, MAX_DISPLAY_RESX, MAX_DISPLAY_RESY, COLOR_WHITE);
  display_text_center(MAX_DISPLAY_RESX / 2, MAX_DISPLAY_RESY / 2,
                      "TOUCH SCREEN", -1, FONT_NORMAL, COLOR_BLACK,
                      COLOR_WHITE);
  while (!touch_click()) {
  }

  ui_generic_confirm_simple("SCREEN PASS?");
  if (ui_response()) {
    screen_bg[SCREEN_TEST] = COLOR_GREEN;
  } else {
    screen_bg[SCREEN_TEST] = COLOR_RED;
  }
}

void touch_input_test(void) {
  display_clear();
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 6; j++) {
      display_bar_radius(j * 80, (j % 2) * 80 + i * 160, 80, 80, COLOR_RED,
                         COLOR_WHITE, 16);
    }
  }
  uint32_t pos = 0;
  for (;;) {
    uint32_t evt = touch_read();
    uint16_t x = touch_unpack_x(evt);
    uint16_t y = touch_unpack_y(evt);

    if (!evt) {
      continue;
    }

    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 6; j++) {
        if (x > (j * 80) && x < (j * 80 + 80) && y > ((j % 2) * 80 + i * 160) &&
            y < ((j % 2) * 80 + i * 160 + 80)) {
          display_bar_radius(j * 80, (j % 2) * 80 + i * 160, 80, 80,
                             COLOR_GREEN, COLOR_WHITE, 16);
          pos |= 1 << (6 * i + j);
        }
        if (pos == 0x3FFFFFFF) {
          screen_bg[TOUCH_TEST] = COLOR_GREEN;
          return;
        }
      }
    }
  }
}

void beep_test(void) {
  display_clear();
  display_text_center(DISPLAY_RESX / 2, DISPLAY_RESY / 2, "BEEP TEST", -1,
                      FONT_NORMAL, COLOR_WHITE, COLOR_BLACK);

  buzzer_ctrl(1);
  hal_delay(1000);
  buzzer_ctrl(0);
  hal_delay(1000);
  buzzer_ctrl(1);
  hal_delay(1000);
  buzzer_ctrl(0);

  display_bar_radius(32, DISPLAY_RESY - 160, 128, 64, COLOR_RED, COLOR_BLACK,
                     16);
  display_bar_radius(DISPLAY_RESX - 32 - 128, DISPLAY_RESY - 160, 128, 64,
                     COLOR_GREEN, COLOR_BLACK, 16);
  display_text(80, DISPLAY_RESY - 120, "No", -1, FONT_NORMAL, COLOR_WHITE,
               COLOR_RED);
  display_text(DISPLAY_RESX - 118, DISPLAY_RESY - 120, "Yes", -1, FONT_NORMAL,
               COLOR_WHITE, COLOR_GREEN);

  if (ui_response()) {
    screen_bg[BEEP_TEST] = COLOR_GREEN;
  } else {
    screen_bg[BEEP_TEST] = COLOR_RED;
  }
}

void motor_test(void) {
  display_clear();

  display_text_center(DISPLAY_RESX / 2, DISPLAY_RESY / 2, "MOTOR TEST", -1,
                      FONT_NORMAL, COLOR_WHITE, COLOR_BLACK);

  motor_ctrl(MOTOR_REVERSE);
  hal_delay(1000);
  motor_ctrl(MOTOR_BRAKE);

  display_bar_radius(32, DISPLAY_RESY - 160, 128, 64, COLOR_RED, COLOR_BLACK,
                     16);
  display_bar_radius(DISPLAY_RESX - 32 - 128, DISPLAY_RESY - 160, 128, 64,
                     COLOR_GREEN, COLOR_BLACK, 16);
  display_text(80, DISPLAY_RESY - 120, "No", -1, FONT_NORMAL, COLOR_WHITE,
               COLOR_RED);
  display_text(DISPLAY_RESX - 118, DISPLAY_RESY - 120, "Yes", -1, FONT_NORMAL,
               COLOR_WHITE, COLOR_GREEN);

  if (ui_response()) {
    screen_bg[MOTOR_TEST] = COLOR_GREEN;
  } else {
    screen_bg[MOTOR_TEST] = COLOR_RED;
  }
}

void se_test(void) {
  uint8_t rand_buffer[32];

  if (atca_random(rand_buffer)) {
    screen_bg[SE_TEST] = COLOR_RED;
  } else {
    screen_bg[SE_TEST] = COLOR_GREEN;
  }
}

int spi_flash_test(void) {
  // if (qspi_flash_read_id() == 0) {
  //   screen_bg[SPI_FLASH_TEST] = COLOR_RED;
  // } else {
  //   screen_bg[SPI_FLASH_TEST] = COLOR_GREEN;
  // }
  char show_tip[64] = {0};
  volatile uint32_t write_start_time, write_end_time;
  write_start_time = HAL_GetTick();

  uint8_t flash_data[2048] = {0};
  uint8_t test_data[2048] = {0};
  for (uint32_t i = 0; i < sizeof(test_data); i++) {
    test_data[i] = i;
  }

  for (uint32_t address = 0; address < (1 * 1024 * 1024);
       address += QSPI_SECTOR_SIZE) {
    qspi_flash_erase_block_64k(address);

    for (uint32_t offset = 0; offset < QSPI_SECTOR_SIZE;
         offset += sizeof(flash_data)) {
      qspi_flash_read_buffer(flash_data, address + offset, sizeof(flash_data));
      for (uint32_t i = 0; i < sizeof(flash_data); i++) {
        if (flash_data[i] != 0xFF) {
          screen_bg[SPI_FLASH_TEST] = COLOR_RED;
          return 0;
        }
      }
    }

    for (uint32_t offset = 0; offset < QSPI_SECTOR_SIZE;
         offset += sizeof(test_data)) {
      qspi_flash_write_buffer_unsafe(test_data, address + offset,
                                     sizeof(test_data));
      memset(flash_data, 0x00, sizeof(flash_data));
      qspi_flash_read_buffer(flash_data, address + offset, sizeof(flash_data));
      for (uint32_t i = 0; i < sizeof(flash_data); i++) {
        if (flash_data[i] != i % 256) {
          screen_bg[SPI_FLASH_TEST] = COLOR_RED;
          return 0;
        }
      }
    }

    qspi_flash_erase_block_64k(address);

    display_bar(0, 130, 480, 30, COLOR_BL_BG);
    mini_snprintf(show_tip, sizeof(show_tip), "SPI TEST... %d%%",
                  (unsigned int)(address * 100) / (1024 * 1024));
    display_text_center(DISPLAY_RESX / 2, 160, show_tip, -1, FONT_NORMAL,
                        COLOR_BL_FG, COLOR_BL_BG);
  }

  display_bar(0, 130, 480, 30, COLOR_BL_BG);
  display_text_center(DISPLAY_RESX / 2, 160, "SPI TEST... 100%", -1,
                      FONT_NORMAL, COLOR_BL_FG, COLOR_BL_BG);

  write_end_time = HAL_GetTick();
  screen_bg[SPI_FLASH_TEST] = COLOR_GREEN;
  return write_end_time - write_start_time;
}

void emmc_test(void) {
  if (emmc_get_capacity_in_bytes() == 0) {
    screen_bg[EMMC_TEST] = COLOR_RED;
  } else {
    screen_bg[EMMC_TEST] = COLOR_GREEN;
  }
}

void sdram_test(void) {
  uint32_t i, j;
  uint32_t write_start_time, write_end_time, read_start_time, read_end_time;
  uint32_t *buffer;

  // char write_info[128] = {0};
  // char read_info[128] = {0};

  write_start_time = HAL_GetTick();
  j = 0;
  buffer = (uint32_t *)(FMC_SDRAM_ADDRESS + 1024 * 1024);

  for (i = ((1024 * 1024 / 4) - (1024 * 1024 / 128)); i > 0; i--) {
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;

    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;

    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;

    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
    *buffer++ = j++;
  }
  write_end_time = HAL_GetTick();

  j = 0;
  buffer = (uint32_t *)(FMC_SDRAM_ADDRESS + 1024 * 1024);
  for (i = 0; i < ((1024 * 1024 * 8) - (1024 * 1024 / 4)); i++) {
    if (*buffer++ != j++) {
      screen_bg[SDRAM_TEST] = COLOR_RED;
      return;
    }
  }

  volatile uint32_t data;
  (void)data;
  buffer = (uint32_t *)FMC_SDRAM_ADDRESS;

  read_start_time = HAL_GetTick();
  for (i = 1024 * 1024 / 4; i > 0; i--) {
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;

    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;

    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;

    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
    data = *buffer++;
  }
  read_end_time = HAL_GetTick();
  // display_clear();

  // snprintf(write_info, sizeof(write_info), "write:time = %dms,speed= %dMB/s",
  //          (unsigned)(write_end_time - write_start_time),
  //          (unsigned)((32 * 1000) / (write_end_time - write_start_time)));
  // snprintf(read_info, sizeof(read_info), "read:time = %dms,speed= %dMB/s",
  //          (unsigned)(read_end_time - read_start_time),
  //          (unsigned)((32 * 1000) / (read_end_time - read_start_time)));

  // display_text(0, 100, write_info, -1, FONT_NORMAL, COLOR_WHITE,
  // COLOR_BLACK); display_text(0, 150, read_info, -1, FONT_NORMAL, COLOR_WHITE,
  // COLOR_BLACK); display_bar_radius(32, DISPLAY_RESY - 160, 128, 64,
  // COLOR_RED, COLOR_BLACK,
  //                    16);
  // display_bar_radius(DISPLAY_RESX - 32 - 128, DISPLAY_RESY - 160, 128, 64,
  //                    COLOR_GREEN, COLOR_BLACK, 16);
  // display_text(80, DISPLAY_RESY - 120, "No", -1, FONT_NORMAL, COLOR_WHITE,
  //              COLOR_RED);
  // display_text(DISPLAY_RESX - 118, DISPLAY_RESY - 120, "Yes", -1,
  // FONT_NORMAL,
  //              COLOR_WHITE, COLOR_GREEN);

  // if (ui_response()) {
  //   screen_bg[SDRAM_TEST] = COLOR_GREEN;
  // } else {
  //   screen_bg[SDRAM_TEST] = COLOR_RED;
  // }

  if ((31 * 1000) / (write_end_time - write_start_time) > 400 &&
      (32 * 1000) / (read_end_time - read_start_time) > 200) {
    screen_bg[SDRAM_TEST] = COLOR_GREEN;
  } else {
    screen_bg[SDRAM_TEST] = COLOR_RED;
  }
}

void ble_test(void) {
  if (!ble_name_state()) {
    ble_cmd_req(BLE_VER, BLE_VER_ADV);
    hal_delay(5);
  }

  if (!ble_battery_state()) {
    ble_cmd_req(BLE_PWR, BLE_PWR_EQ);
    hal_delay(5);
  }
}

uint16_t *p_test_result = (uint16_t *)0x08020000;
uint16_t test_result[16];

void test_menu(void) {
  uint8_t secotrs[1];
  secotrs[0] = 1;

  memcpy(test_result, screen_bg, sizeof(screen_bg));

  ensure(flash_erase_sectors(secotrs, 1, NULL), "erase data sector 1");
  ensure(flash_unlock_write(), NULL);
  ensure(flash_write_words(1, 0, (uint32_t *)test_result), "write test result");
  ensure(flash_lock_write(), NULL);

  display_clear();
  display_text_center(DISPLAY_RESX / 2, 100, "DEVICE TEST", -1, FONT_NORMAL,
                      COLOR_BL_FG, COLOR_BL_BG);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 5; j++) {
      display_bar_radius((i + 1) * 60 + i * 150, 150 + j * 100, 150, 80,
                         screen_bg[j * 2 + i], COLOR_BLACK, 16);
    }
  }
  display_text_center(135, 200, "SCREEN", -1, FONT_NORMAL, COLOR_BL_FG,
                      screen_bg[SCREEN_TEST]);
  display_text_center(345, 200, "TOUCH", -1, FONT_NORMAL, COLOR_BL_FG,
                      screen_bg[TOUCH_TEST]);
  display_text_center(135, 300, "SE", -1, FONT_NORMAL, COLOR_BL_FG,
                      screen_bg[SE_TEST]);
  display_text_center(345, 300, "SPI-FLASH", -1, FONT_NORMAL, COLOR_BL_FG,
                      screen_bg[SPI_FLASH_TEST]);
  display_text_center(135, 400, "EMMC", -1, FONT_NORMAL, COLOR_BL_FG,
                      screen_bg[EMMC_TEST]);
  display_text_center(345, 400, "SDRAM", -1, FONT_NORMAL, COLOR_BL_FG,
                      screen_bg[SDRAM_TEST]);
  display_text_center(135, 500, "BEEP", -1, FONT_NORMAL, COLOR_BL_FG,
                      screen_bg[BEEP_TEST]);
  display_text_center(345, 500, "MOTOR", -1, FONT_NORMAL, COLOR_BL_FG,
                      screen_bg[MOTOR_TEST]);
  display_text_center(135, 600, "BLE", -1, FONT_NORMAL, COLOR_BL_FG,
                      screen_bg[BLE_TEST]);
  // display_text_center(345, 600, "NULL", -1, FONT_NORMAL, COLOR_BL_FG,
  //                     screen_bg[SCREEN_TEST]);
}
uint16_t pos_x, pos_y;
uint32_t test_ui_response(void) {
  uint32_t evt = touch_click();
  pos_x = touch_unpack_x(evt);
  pos_y = touch_unpack_y(evt);

  if (!evt) {
    return 0xFF;
  }

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 5; j++) {
      if (pos_x > ((i + 1) * 60 + i * 150) &&
          pos_x < ((i + 1) * 60 + (i + 1) * 150) && pos_y > (150 + j * 100) &&
          pos_y < (150 + j * 100 + 80)) {
        return j * 2 + i;
      }
    }
  }
  return 0xFF;
}

void ble_response(void) {
  static bool flag = true;
  char battery_str[32] = {0};
  if (ble_name_state()) {
    display_text(0, 700, ble_get_name(), -1, FONT_NORMAL, COLOR_BL_FG,
                 COLOR_BL_BG);
    screen_bg[BLE_TEST] = COLOR_GREEN;
    if (flag) {
      flag = false;
      test_menu();
    }
  }
  if (ble_battery_state()) {
    mini_snprintf(battery_str, sizeof(battery_str), "battery %d%%",
                  battery_cap);
    display_text(0, 730, battery_str, -1, FONT_NORMAL, COLOR_BL_FG,
                 COLOR_BL_BG);
  }
}

int main(void) {
  volatile uint32_t startup_mode_flag = *STAY_IN_FLAG_ADDR;
  pcb_version = PCB_VERSION_2_1_0;

  reset_flags_reset();

  periph_init();

  /* Enable the CPU Cache */
  cpu_cache_enable();

  system_clock_config();

  rng_init();

  flash_option_bytes_init();

  clear_otg_hs_memory();

  flash_otp_init();

  gpio_init();

  sdram_init();

  qspi_flash_init();
  qspi_flash_config();
  // qspi_flash_memory_mapped();

  mpu_config();

  lcd_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
  display_clear();
  lcd_pwm_init();

  display_image((DISPLAY_RESX - 128) / 2, (DISPLAY_RESY - 128) / 2, 128, 128,
                toi_icon_onekey + 12, sizeof(toi_icon_onekey) - 12);

#if !PRODUCTION
  display_text_center(DISPLAY_RESX / 2, DISPLAY_RESX / 2, "TEST VERSION", -1,
                      FONT_NORMAL, COLOR_RED, COLOR_BLACK);
#endif

  touch_init();

  emmc_init();
  atca_init();
  buzzer_init();
  motor_init();
  ble_usart_init();

  // fatfs_init();
  memcpy(test_result, p_test_result, 32);
  for (int i = 0; i < TEST_NUMS; i++) {
    if (test_result[i] == (COLOR_RED)) {
      screen_bg[i] = COLOR_RED;
    } else if (test_result[i] == (COLOR_GREEN)) {
      screen_bg[i] = COLOR_GREEN;
    } else {
      screen_bg[i] = COLOR_BL_GRAY;
    }
  }

  screen_bg[TEST_NUMS] = COLOR_BLACK;

  display_clear();
  display_text_center(DISPLAY_RESX / 2, 100, "AUTO TEST", -1, FONT_NORMAL,
                      COLOR_BL_FG, COLOR_BL_BG);

  display_text_center(DISPLAY_RESX / 2, 130, "SE TEST...", -1, FONT_NORMAL,
                      COLOR_BL_FG, COLOR_BL_BG);

  se_test();

  display_text_center(DISPLAY_RESX / 2, 160, "SPI TEST...", -1, FONT_NORMAL,
                      COLOR_BL_FG, COLOR_BL_BG);

  spi_flash_test();

  display_text_center(DISPLAY_RESX / 2, 190, "EMMC TEST...", -1, FONT_NORMAL,
                      COLOR_BL_FG, COLOR_BL_BG);

  emmc_test();

  display_text_center(DISPLAY_RESX / 2, 220, "SDRAM TEST...", -1, FONT_NORMAL,
                      COLOR_BL_FG, COLOR_BL_BG);

  sdram_test();

  display_text_center(DISPLAY_RESX / 2, 250, "BLE TEST...", -1, FONT_NORMAL,
                      COLOR_BL_FG, COLOR_BL_BG);

  ble_test();

  uint32_t button = 0;

  test_menu();
  while (1) {
    ble_uart_poll();
    ble_response();
    button = test_ui_response();
    if (button != 0xFF) {
      switch (button) {
        case SCREEN_TEST:
          screen_test();
          break;
        case TOUCH_TEST:
          touch_input_test();
          break;
        case SE_TEST:
          se_test();
          break;
        case SPI_FLASH_TEST:
          spi_flash_test();
          break;
        case EMMC_TEST:
          emmc_test();
          break;
        case SDRAM_TEST:
          sdram_test();
          break;
        case BEEP_TEST:
          beep_test();
          break;
        case MOTOR_TEST:
          motor_test();
          break;
        case BLE_TEST:
          ble_test();
          break;
        default:
          break;
      }
      test_menu();
    }
  }

  uint32_t mode = 0;

  if (startup_mode_flag == STAY_IN_BOARDLOADER_FLAG) {
    mode = BOARD_MODE;
    *STAY_IN_FLAG_ADDR = 0;
  } else if (fatfs_check_res() != 0) {
    mode = BOARD_MODE;
  }
  if (startup_mode_flag == STAY_IN_BOOTLOADER_FLAG) {
    mode = BOOT_MODE;
  }

  if (!mode) {
    ble_usart_init();
    bool touched = false;
    uint32_t touch_data, x_start, y_start, x_mov, y_mov;
    touch_data = x_start = y_start = x_mov = y_mov = 0;

    for (int timer = 0; timer < 1250; timer++) {
      ble_uart_poll();

      if (ble_power_button_state() == 2) {
        if (touched) {
          mode = BOARD_MODE;
        } else {
          mode = BOOT_MODE;
        }
        break;
      }
      touch_data = touch_read();
      if (touch_data != 0) {
        if (touch_data & TOUCH_START) {
          x_start = x_mov = (touch_data >> 12) & 0xFFF;
          y_start = y_mov = touch_data & 0xFFF;
        }

        if (touch_data & TOUCH_MOVE) {
          x_mov = (touch_data >> 12) & 0xFFF;
          y_mov = touch_data & 0xFFF;
        }

        if ((abs(x_start - x_mov) > 100) || (abs(y_start - y_mov) > 100)) {
          touched = true;
        }
      }

      hal_delay(1);
    }
    ble_usart_irq_disable();
  }

  if (mode == BOARD_MODE) {
    display_printf("OneKey Boardloader\n");
    display_printf("USB Mass Storage Mode\n");
    display_printf("=====================\n\n");
    usb_msc_init();
    while (1)
      ;
  }

  if (mode == BOOT_MODE) {
    *STAY_IN_FLAG_ADDR = STAY_IN_BOOTLOADER_FLAG;
    SCB_CleanDCache();
  }

  uint32_t code_len = 0;
  code_len = check_sdcard();
  if (code_len) {
    return copy_sdcard(code_len) == sectrue ? 0 : 3;
  }

  image_header hdr;

  ensure(load_image_header((const uint8_t *)BOOTLOADER_START,
                           BOOTLOADER_IMAGE_MAGIC, BOOTLOADER_IMAGE_MAXSIZE,
                           BOARDLOADER_KEY_M, BOARDLOADER_KEY_N,
                           BOARDLOADER_KEYS, &hdr),
         "invalid bootloader header");

  const uint8_t sectors[] = {
      FLASH_SECTOR_BOOTLOADER_1,
      FLASH_SECTOR_BOOTLOADER_2,
  };
  ensure(
      check_image_contents(&hdr, IMAGE_HEADER_SIZE, sectors, sizeof(sectors)),
      "invalid bootloader hash");

  jump_to(BOOTLOADER_START + IMAGE_HEADER_SIZE);

  return 0;
}

#else

// we use SRAM as SD card read buffer (because DMA can't access the CCMRAM)
extern uint32_t sram_start[];
#define sdcard_buf sram_start

static uint32_t check_sdcard(void) {
  if (sectrue != sdcard_power_on()) {
    return 0;
  }

  uint64_t cap = sdcard_get_capacity_in_bytes();
  if (cap < 1024 * 1024) {
    sdcard_power_off();
    return 0;
  }

  memzero(sdcard_buf, IMAGE_HEADER_SIZE);

  const secbool read_status =
      sdcard_read_blocks(sdcard_buf, 0, IMAGE_HEADER_SIZE / SDCARD_BLOCK_SIZE);

  sdcard_power_off();

  image_header hdr;

  if ((sectrue == read_status) &&
      (sectrue ==
       load_image_header((const uint8_t *)sdcard_buf, BOOTLOADER_IMAGE_MAGIC,
                         BOOTLOADER_IMAGE_MAXSIZE, BOARDLOADER_KEY_M,
                         BOARDLOADER_KEY_N, BOARDLOADER_KEYS, &hdr))) {
    return hdr.codelen;
  } else {
    return 0;
  }
}

static void progress_callback(int pos, int len) { display_printf("."); }

static secbool copy_sdcard(void) {
  display_backlight(255);

  display_printf("Trezor Boardloader\n");
  display_printf("==================\n\n");

  display_printf("bootloader found on the SD card\n\n");
  display_printf("applying bootloader in 10 seconds\n\n");
  display_printf("unplug now if you want to abort\n\n");

  uint32_t codelen;

  for (int i = 10; i >= 0; i--) {
    display_printf("%d ", i);
    hal_delay(1000);
    codelen = check_sdcard();
    if (0 == codelen) {
      display_printf("\n\nno SD card, aborting\n");
      return secfalse;
    }
  }

  display_printf("\n\nerasing flash:\n\n");

  // erase all flash (except boardloader)
  static const uint8_t sectors[] = {
      FLASH_SECTOR_STORAGE_1,
      FLASH_SECTOR_STORAGE_2,
      3,
      FLASH_SECTOR_BOOTLOADER,
      FLASH_SECTOR_FIRMWARE_START,
      7,
      8,
      9,
      10,
      FLASH_SECTOR_FIRMWARE_END,
      FLASH_SECTOR_UNUSED_START,
      13,
      14,
      FLASH_SECTOR_UNUSED_END,
      FLASH_SECTOR_FIRMWARE_EXTRA_START,
      18,
      19,
      20,
      21,
      22,
      FLASH_SECTOR_FIRMWARE_EXTRA_END,
  };
  if (sectrue !=
      flash_erase_sectors(sectors, sizeof(sectors), progress_callback)) {
    display_printf(" failed\n");
    return secfalse;
  }
  display_printf(" done\n\n");

  ensure(flash_unlock_write(), NULL);

  // copy bootloader from SD card to Flash
  display_printf("copying new bootloader from SD card\n\n");

  ensure(sdcard_power_on(), NULL);

  memzero(sdcard_buf, SDCARD_BLOCK_SIZE);

  for (int i = 0; i < (IMAGE_HEADER_SIZE + codelen) / SDCARD_BLOCK_SIZE; i++) {
    ensure(sdcard_read_blocks(sdcard_buf, i, 1), NULL);
    for (int j = 0; j < SDCARD_BLOCK_SIZE / sizeof(uint32_t); j++) {
      ensure(flash_write_word(FLASH_SECTOR_BOOTLOADER,
                              i * SDCARD_BLOCK_SIZE + j * sizeof(uint32_t),
                              sdcard_buf[j]),
             NULL);
    }
  }

  sdcard_power_off();
  ensure(flash_lock_write(), NULL);

  display_printf("\ndone\n\n");
  display_printf("Unplug the device and remove the SD card\n");

  return sectrue;
}

int main(void) {
  reset_flags_reset();

  // need the systick timer running before many HAL operations.
  // want the PVD enabled before flash operations too.
  periph_init();

  if (sectrue != flash_configure_option_bytes()) {
    // display is not initialized so don't call ensure
    const secbool r =
        flash_erase_sectors(STORAGE_SECTORS, STORAGE_SECTORS_COUNT, NULL);
    (void)r;
    return 2;
  }

  clear_otg_hs_memory();

  display_init();
  sdcard_init();

  if (check_sdcard()) {
    return copy_sdcard() == sectrue ? 0 : 3;
  }

  image_header hdr;

  ensure(load_image_header((const uint8_t *)BOOTLOADER_START,
                           BOOTLOADER_IMAGE_MAGIC, BOOTLOADER_IMAGE_MAXSIZE,
                           BOARDLOADER_KEY_M, BOARDLOADER_KEY_N,
                           BOARDLOADER_KEYS, &hdr),
         "invalid bootloader header");

  const uint8_t sectors[] = {
      FLASH_SECTOR_BOOTLOADER,
  };
  ensure(check_image_contents(&hdr, IMAGE_HEADER_SIZE, sectors, 1),
         "invalid bootloader hash");

  jump_to(BOOTLOADER_START + IMAGE_HEADER_SIZE);

  return 0;
}

#endif
