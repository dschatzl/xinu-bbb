/* xinube_macs.c */

#include <xinu.h>

byte	xinube_macs[96][6] = {
0x98, 0x4f, 0xee, 0x00, 0x0c, 0x3a,
0x98, 0x4f, 0xee, 0x00, 0x0a, 0xcf,
0x98, 0x4f, 0xee, 0x00, 0x0a, 0xa2,
0x98, 0x4f, 0xee, 0x00, 0x0c, 0x22,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0xa2,
0x98, 0x4f, 0xee, 0x00, 0x20, 0x32,
0x98, 0x4f, 0xee, 0x00, 0x1d, 0xb6,
0x98, 0x4f, 0xee, 0x00, 0x22, 0x68,
0x98, 0x4f, 0xee, 0x00, 0x05, 0xa3,
0x98, 0x4f, 0xee, 0x00, 0x1d, 0x34,
0x98, 0x4f, 0xee, 0x00, 0x4f, 0xe7,
0x98, 0x4f, 0xee, 0x00, 0x3e, 0x95,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x98, 0x4f, 0xee, 0x00, 0x09, 0x3d,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0x1e,
0x98, 0x4f, 0xee, 0x00, 0x09, 0xb9,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0x9b,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0x90,
0x98, 0x4f, 0xee, 0x00, 0x05, 0xd2,
0x98, 0x4f, 0xee, 0x00, 0x0a, 0xc6,
0x98, 0x4f, 0xee, 0x00, 0x05, 0xc5,
0x98, 0x4f, 0xee, 0x00, 0x50, 0x84,
0x98, 0x4f, 0xee, 0x00, 0x3d, 0x7d,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0x52,
0x98, 0x4f, 0xee, 0x00, 0x08, 0x80,
0x98, 0x4f, 0xee, 0x00, 0x0c, 0x1c,
0x98, 0x4f, 0xee, 0x00, 0x07, 0xf7,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0xee,
0x98, 0x4f, 0xee, 0x00, 0x4c, 0x75,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0x9e,
0x98, 0x4f, 0xee, 0x00, 0x08, 0x0e,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0x38,
0x98, 0x4f, 0xee, 0x00, 0x09, 0xe9,
0x98, 0x4f, 0xee, 0x00, 0x0a, 0xf3,
0x98, 0x4f, 0xee, 0x00, 0x1f, 0x72,
0x98, 0x4f, 0xee, 0x00, 0x0c, 0x0b,
0x98, 0x4f, 0xee, 0x00, 0x0a, 0xfc,
0x98, 0x4f, 0xee, 0x00, 0x1f, 0xf4,
0x98, 0x4f, 0xee, 0x00, 0x21, 0x9f,
0x98, 0x4f, 0xee, 0x00, 0x21, 0x3c,
0x98, 0x4f, 0xee, 0x00, 0x1d, 0x88,
0x98, 0x4f, 0xee, 0x00, 0x1f, 0x38,
0x98, 0x4f, 0xee, 0x00, 0x20, 0xd0,
0x98, 0x4f, 0xee, 0x00, 0x20, 0xee,
0x98, 0x4f, 0xee, 0x00, 0x1f, 0x85,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0xe0,
0x98, 0x4f, 0xee, 0x00, 0x50, 0x98,
0x98, 0x4f, 0xee, 0x00, 0x50, 0x4f,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x98, 0x4f, 0xee, 0x00, 0x50, 0x5e,
0x98, 0x4f, 0xee, 0x00, 0x09, 0xeb,
0x98, 0x4f, 0xee, 0x00, 0x09, 0xe3,
0x98, 0x4f, 0xee, 0x00, 0x0a, 0xf8,
0x98, 0x4f, 0xee, 0x00, 0x09, 0x86,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x98, 0x4f, 0xee, 0x00, 0x0a, 0x94,
0x98, 0x4f, 0xee, 0x00, 0x0c, 0x04,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x98, 0x4f, 0xee, 0x00, 0x51, 0x1b,
0x98, 0x4f, 0xee, 0x00, 0x4f, 0xea,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0xf9,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0xf1,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0xe3,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0x19,
0x98, 0x4f, 0xee, 0x00, 0x1d, 0xd8,
0x98, 0x4f, 0xee, 0x00, 0x4a, 0x15,
0x98, 0x4f, 0xee, 0x00, 0x4a, 0xb9,
0x98, 0x4f, 0xee, 0x00, 0x4a, 0xd5,
0x98, 0x4f, 0xee, 0x00, 0x4c, 0x09,
0x98, 0x4f, 0xee, 0x00, 0x4c, 0x60,
0x98, 0x4f, 0xee, 0x00, 0x4b, 0xd9,
0x98, 0x4f, 0xee, 0x00, 0x4a, 0x5d,
0x98, 0x4f, 0xee, 0x00, 0x4b, 0xe0,
0x98, 0x4f, 0xee, 0x00, 0x4b, 0xf1,
0x98, 0x4f, 0xee, 0x00, 0x4b, 0xd7,
0x98, 0x4f, 0xee, 0x00, 0x4c, 0x34,
0x98, 0x4f, 0xee, 0x00, 0x4c, 0x55,
0x98, 0x4f, 0xee, 0x00, 0x4b, 0xcf,
0x98, 0x4f, 0xee, 0x00, 0x4c, 0x08,
0x98, 0x4f, 0xee, 0x00, 0x4c, 0x5d,
0x98, 0x4f, 0xee, 0x00, 0x4b, 0xa5,
0x98, 0x4f, 0xee, 0x00, 0x08, 0x43,
0x98, 0x4f, 0xee, 0x00, 0x08, 0x5e,
0x98, 0x4f, 0xee, 0x00, 0x09, 0xa5,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0x46,
0x98, 0x4f, 0xee, 0x00, 0x08, 0x1d,
0x98, 0x4f, 0xee, 0x00, 0x0b, 0x45,
0x98, 0x4f, 0xee, 0x00, 0x06, 0x9f,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x98, 0x4f, 0xee, 0x00, 0x1f, 0x33,
0x98, 0x4f, 0xee, 0x00, 0x1d, 0x93,
0x98, 0x4f, 0xee, 0x00, 0x15, 0x07,
0x98, 0x4f, 0xee, 0x00, 0x1e, 0x45,
0x98, 0x4f, 0xee, 0x00, 0x1e, 0x45,
};