/* --------------------------------------------------------------------- *
    $Appl:     Cryptographic library: DES, DCB, Luhn, CRC32, conversions.
    $Version:  1.05
    $Module:   32-bit CRC prototypes & definitions.
    $Design:   Gerasimov R.V., SCT department.
    $Date:     Tue Nov 10 17:25:42 1998
 * --------------------------------------------------------------------- */
#ifndef _CRC32_H
#define _CRC32_H

typedef unsigned long CRC32;	       /* the type of the 32-bit CRC */

void  Init_CRC32(void);		 // initializes CRC32 value
CRC32  UpdateCRC32(unsigned char b);  // updates current CRC32

#endif
