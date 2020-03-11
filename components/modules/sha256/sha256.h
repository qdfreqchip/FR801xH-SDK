/*********************************************************************
* Filename:   sha256.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding SHA1 implementation.
*********************************************************************/

#ifndef SHA256_H
#define SHA256_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

typedef struct
{
    BYTE data[64];
    WORD datalen;
    unsigned long long bitlen;
    WORD state[8];
} SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
/*********************************************************************
 * @fn      sha256_init
 *
 * @brief   Initialize the sh256 algorithm.
 *
 * @param   ctx  - Initialized structure.
 *                              
 * @return  none.
 */

void sha256_init(SHA256_CTX *ctx);

/*********************************************************************
 * @fn      sha256_update
 *
 * @brief   Update the encrypted data of sha256.
 *
 * @param   ctx  - Initialized structure.
 *          data - Data to be encrypted.
 *          len  - Data length
 *                              
 * @return  none.
 */

void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len);
/*********************************************************************
 * @fn      sha256_final
 *
 * @brief   Get the encrypted result of sha256.
 *
 * @param   ctx  - Initialized structure.
 *          hash - Encrypted result data.
 *                              
 * @return  none.
 */

void sha256_final(SHA256_CTX *ctx, BYTE hash[]);
/*********************************************************************
 * @fn      sha256_gen_auth_value
 *
 * @brief   Encrypt data and get results.
 *
 * @param   pid    - pid array.
 *          bdaddr - bdaddr array.
 *          secret - secret array
 *          result - result
 *
 * @return  none.
 */

void sha256_gen_auth_value(BYTE *pid, BYTE *bdaddr, BYTE *secret, BYTE *result);
/*********************************************************************
 * @fn      sha256_gatt_gen_auth_value
 *
 * @brief   Encrypt data and get results.
 *
 * @param   random - random data
 *          pid    - pid array.
 *          bdaddr - bdaddr array.
 *          secret - secret array
 *          result - result
 *
 * @return  none.
 */



void sha256_gatt_gen_auth_value(BYTE*random,BYTE *pid, BYTE *bdaddr, BYTE *secret, BYTE *result);

#endif   // SHA256_H
