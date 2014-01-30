#include "usb_scsi.h"
#include "memory.h"

u8 Page00_Inquiry_Data[] =
{
	0x00, /* PERIPHERAL QUALIFIER & PERIPHERAL DEVICE TYPE*/
	0x00,
	0x00,
	0x00,
	0x00 /* Supported Pages 00*/
};

u8 Standard_Inquiry_Data[] =
{
	0x00,		/* Direct Access Device */
	0x80,		/* RMB = 1: Removable Medium */
	0x02,		/* Version: No conformance claim to standard */
	0x02,

	36 - 4,		/* Additional Length */
	0x00,		/* SCCS = 1: Storage Controller Component */
	0x00,
	0x00,
	/* Vendor Identification */
	'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ',
	/* Product Identification */
	'S', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',
	'D', 'i', 's', 'k', ' ', ' ', ' ',
	/* Product Revision Level */
	'1', '.', '0', ' '
};

u8 Standard_Inquiry_Data2[] =
{
	0x00,		/* Direct Access Device */
	0x80,		/* RMB = 1: Removable Medium */
	0x02,		/* Version: No conformance claim to standard */
	0x02,

	36 - 4,		/* Additional Length */
	0x00,		/* SCCS = 1: Storage Controller Component */
	0x00,
	0x00,
	/* Vendor Identification */
	'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ',
	/* Product Identification */
	'N', 'A', 'N', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',
	'D', 'i', 's', 'k', ' ',
	/* Product Revision Level */
	'1', '.', '0', ' '
};

u8 Mode_Sense6_data[] =
{
	0x03,
	0x00,
	0x00,
	0x00,
};

u8 Mode_Sense10_data[] =
{
	0x00,
	0x06,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00
};

u8 Scsi_Sense_Data[] =
{
	0x70,		/* RespCode */
	0x00,		/* SegmentNumber */
	NO_SENSE,	/* Sens_Key */
	0x00,
	0x00,
	0x00,
	0x00,		/* Information */
	0x0A,		/* AdditionalSenseLength */
	0x00,
	0x00,
	0x00,
	0x00,		/* CmdInformation */
	NO_SENSE,	/* Asc */
	0x00,		/* ASCQ */
	0x00,		/* FRUC */
	0x00,		/* TBD */
	0x00,
	0x00		/* SenseKeySpecific */
};

u8 ReadCapacity10_Data[] =
{
	/* Last Logical Block */
	0,
	0,
	0,
	0,

	/* Block Length */
	0,
	0,
	0,
	0
};

u8 ReadFormatCapacity_Data[] =
{
	0x00,
	0x00,
	0x00,
	0x08, /* Capacity List Length */

	/* Block Count */
	0,
	0,
	0,
	0,

	/* Block Length */
	0x02,/* Descriptor Code: Formatted Media */
	0,
	0,
	0
};
