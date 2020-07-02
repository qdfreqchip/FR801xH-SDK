/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

#ifndef _AES_H
#define _AES_H


/*
 * INCLUDES
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * TYPEDEFS
 */

typedef void (*aes_func_result_cb) (uint8_t status, const uint8_t* aes_res, uint32_t src_info);

/*
 * FUNCTIONS
 */
 
/*********************************************************************
 * @fn      aes_encrypt
 *
 * @brief   Perform an AES encryption - result within callback
 *          Sample:
 *              const uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00};
 *              const uint8_t value[] = {0x11, 0x22, 0x33, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0xee, 0x0f, 0xaa};
 *              aes_encrypt(key, value, true, aes_enc_result_cb, 0);
 *
 * @param   key         - Key used for the encryption
 *          val         - Value to encrypt using AES
 *          copy        - Copy parameters because source is destroyed
 *          res_cb      - Function that will handle the AES based result (16 bytes)
 *          src_info    - Information used retrieve requester
 *
 * @return  None.
 */
void aes_encrypt(const uint8_t* key, const uint8_t *val, bool copy, aes_func_result_cb res_cb, uint32_t src_info);

/*********************************************************************
 * @fn      aes_decrypt
 *
 * @brief   Perform an AES decryption - result within callback
 *
 * @param   key         - Key used for the decryption
 *          val         - Value to decrypt using AES
 *          copy        - Copy parameters because source is destroyed
 *          res_cb      - Function that will handle the AES based result (16 bytes)
 *          src_info    - Information used retrieve requester
 *
 * @return  None.
 */
void aes_decrypt(const uint8_t* key, const uint8_t *val, bool copy, aes_func_result_cb res_cb, uint32_t src_info);

#endif  // _AES_H
