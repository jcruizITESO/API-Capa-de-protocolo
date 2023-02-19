/*
 * aes_crc.c
 *
 *  Created on: 14 feb 2023
 *      Author: jcro_
 */

/*
 *  aes_crc.c
 *
 *  Author: JC Ruiz
 */

#include <aes_crc.h>
#include <aes_crc_config.h>
#include <stddef.h>

#include "aes.h"
#include "fsl_crc.h"

/*----------------------------------------------------------------------------------*/
#if SDK_DEBUGCONSOLE    /* Select printf, scanf, putchar, getchar of SDK version. */
#define PRINTF                 DbgConsole_Printf
#define SCANF                  DbgConsole_Scanf
#define PUTCHAR                DbgConsole_Putchar
#define GETCHAR                DbgConsole_Getchar
#else                   /* Select printf, scanf, putchar, getchar of toolchain. */
#define PRINTF                 printf
#define SCANF                  scanf
#define PUTCHAR                putchar
#define GETCHAR                getchar
#endif /* SDK_DEBUGCONSOLE */
/*-----------------------------------------------------------------------------------*/
/* CRC data */
CRC_Type *base = CRC0;

static void InitCrc32(CRC_Type *base, uint32_t seed)
{
    crc_config_t config;

    config.polynomial         = 0x04C11DB7U;
    config.seed               = seed;
    config.reflectIn          = true;
    config.reflectOut         = true;
    config.complementChecksum = true;
    config.crcBits            = kCrcBits32;
    config.crcResult          = kCrcFinalChecksum;

    CRC_Init(base, &config);
}

err_t
aes_crc_write(struct netconn *conn, void *dataptr, uint16_t size)
{
	/*uint8_t test_string[] = {"01234567890123456789"};*/
	char message [128]; //declaracion de cadena para almacenar mensaje
	memcpy(message, dataptr, size);
	message[size] = '\0';


	/* AES data */
	uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
	uint8_t iv[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	struct AES_ctx ctx;
	size_t message_len, padded_len;
	uint8_t padded_msg[512] = {0};
	/*CRC data */
	CRC_Type *base = CRC0;
	uint32_t checksum32;


	PRINTF("\nAES and CRC test task\r\n");

	PRINTF("\nTesting AES128\r\n\n");

	/* Init the AES context structure */
	AES_init_ctx_iv(&ctx, key, iv);

	/* To encrypt an array its lenght must be a multiple of 16 so we add zeros */
	message_len = strlen(message);
	padded_len = message_len + (16 - (message_len%16) );
	memcpy(padded_msg, message, message_len);

	AES_CBC_encrypt_buffer(&ctx, padded_msg, padded_len);

	PRINTF("Encrypted Message: ");
	for(int i=0; i<padded_len; i++) {
		PRINTF("0x%02x,", padded_msg[i]);
	}
	PRINTF("\r\n");


	PRINTF("\nTesting CRC32\r\n\n");

    InitCrc32(base, 0xFFFFFFFFU);
    CRC_WriteData(base, (uint8_t *)&padded_msg[0], padded_len);
    checksum32 = CRC_Get32bitResult(base);

    PRINTF("CRC-32: 0x%08x\r\n", checksum32);

    /* Send encrypted message and CRC-32 to the server */
      uint8_t *send_buf = (uint8_t *)memp_malloc(MEMP_TCP_PCB);
      if (send_buf == NULL) {
        return ERR_MEM;
      }

      uint16_t send_buf_len = sizeof(uint32_t) + padded_len;
      memcpy(send_buf, padded_msg, padded_len);
      memcpy(send_buf + padded_len, &checksum32, sizeof(uint32_t));

      err_t err = netconn_write(conn, send_buf, send_buf_len, NETCONN_COPY);

      memp_free(MEMP_TCP_PCB, send_buf);

    return ERR_OK;

}

err_t
aes_crc_read(struct netconn *conn)
{
	uint8_t buffer[1024];
	uint32_t checksum32;

	/* Read data from server */
	struct netbuf *buf;
	err_t err = netconn_recv(conn, &buf);
	if (err != ERR_OK) {
	   return err;
	}

	/* Extract the encrypted message and the CRC-32 */
	uint16_t buf_len;
	netbuf_data(buf, (void **)&buffer, &buf_len);
	uint16_t msg_len = buf_len - sizeof(uint32_t);
	memcpy(&checksum32, buffer + msg_len, sizeof(uint32_t));

	/* Verify the CRC-32 */
	InitCrc32(base, 0xFFFFFFFFU);
	CRC_WriteData(base, buffer, msg_len);
	if (checksum32 != CRC_Get32bitResult(base)) {
	  PRINTF("CRC-32 check failed!\r\n");
	  netbuf_delete(buf);
	  return ERR_VAL;
	 }

	/* AES data */
	uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
	uint8_t iv[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	struct AES_ctx ctx;

	/* Init the AES context structure */
	AES_init_ctx_iv(&ctx, key, iv);

	/* Decrypt the message */
	AES_CBC_decrypt_buffer(&ctx, buffer, msg_len);

	/* Print the decrypted message */
	PRINTF("Decrypted Message: ");
	for (int i = 0; i < msg_len; i++) {
	   PRINTF("%c", buffer[i]);
	  }

	PRINTF("\r\n");

	netbuf_delete(buf);

	return ERR_OK;
}



