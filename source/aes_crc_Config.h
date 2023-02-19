/*
 * aes_crc_Config.h
 *
 *  Created on: 14 feb 2023
 *      Author: jcro_
 */

#ifndef AES_CRC_CONFIG_H_
#define AES_CRC_CONFIG_H_

/* AES data */
#define KEY  (uint8_t []){0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 }
#define IV   (uint8_t []){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

#define SEED 0xFFFFFFFFU //valor de semilla de 32bits utilizado en el calculo de CRC


#endif /* AES_CRC_CONFIG_H_ */