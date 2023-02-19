/*
 * aes_crc.h
 *
 *  Created on: 14 feb 2023
 *      Author: jcro_
 */

#ifndef AES_CRC_H_
#define AES_CRC_H_

#include <aes_crc_Config.h>
#include <aes_crc.h>

#include "lwip/opt.h"

#include <aes.h>
#include <fsl_crc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lwip/api.h"
#include "lwip/memp.h"
#include "lwip/ip.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/priv/api_msg.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/priv/tcpip_priv.h"

err_t aes_crc_write(struct netconn *conn, void *dataptr, uint16_t size);

/*err_t aes_crc_read(struct netconn *conn);*/


static void InitCrc32(CRC_Type *base, uint32_t seed);

#endif /* AES_CRC_H_ */
