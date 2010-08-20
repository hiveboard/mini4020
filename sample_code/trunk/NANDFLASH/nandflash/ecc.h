#ifndef _ECC_H
#define _ECC_H

#include "sep4020.h"
#include <stdio.h>
#include <string.h>

#define	XMODE	8
					  

typedef enum {
	ECC_NO_ERROR		= 0,		/* no error */
	ECC_CORRECTABLE_ERROR	= 1,		/* one bit data error */
	ECC_ECC_ERROR		= 2,		/* one bit ECC error */
	ECC_UNCORRECTABLE_ERROR	= 3		/* uncorrectable error */
} eccdiff_t;

/*****************************************************************************/
/* Address Types                                                             */
/*****************************************************************************/

typedef unsigned char *		address_t;		/* address (pointer) */
typedef unsigned long		address_value_t;	/* address (for calculation) */

/*****************************************************************************/
/* Integer Types                                                             */
/*****************************************************************************/

typedef unsigned long		uint32_t;			/* unsigned 4 byte integer */
typedef signed long		int32_t;			/* signed 4 byte integer */
typedef unsigned short		uint16_t;			/* unsigned 2 byte integer */
typedef signed short		int16_t;			/* signed 2 byte integer */
typedef unsigned char		uint8_t;			/* unsigned 1 byte integer */
typedef signed char		int8_t;				/* signed 1 byte integer */

#ifdef XMODE
void MakeEcc512(U8 * ecc_buf, U8 * data_buf);
eccdiff_t CompareEcc512(U8 *iEccdata1, U8 *iEccdata2, 
          U8 *pPagedata, S32 pOffset, U8 pCorrected);
#else
void MakeEcc512(U16 * ecc_buf, U16 * data_buf);
eccdiff_t CompareEcc512(U16 *iEccdata1, U16 *iEccdata2, 
          U16 *pPagedata, S32 pOffset, U16 pCorrected);
#endif


#endif
