/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#include "tcpecho.h"
#include "tcpclient.h"
#include "aes_crc.h"

#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
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
//Cliente
/*-----------------------------------------------------------------------------------*/
static void
tcpclient_thread(void *arg)
{
  struct netconn *conn;
  err_t err;
  ip_addr_t server_addr;
  IP4_ADDR(&server_addr, 192, 168, 0, 100);
  LWIP_UNUSED_ARG(arg);

  /* Create a new connection identifier. */
  conn = netconn_new(NETCONN_TCP);

  LWIP_ERROR("tcpecho: invalid conn", (conn != NULL), return;);


  /*while (1)*/ {

	  /*connect to server*/
	 err = netconn_connect(conn, &server_addr, 10002);
	  if (err == ERR_OK) PRINTF("Connected to server!\n");

	  uint8_t max_iterations = 8;

	  while(max_iterations --)
	  {

		  /*read message from console*/
		  char message [128];
		  PRINTF("Enter message: \n");
		  SCANF("%s", message);

		  /*Enviar mensaje al servidor*/
		  void *data;
		  u16_t len;
		  data = (void*) message;
		  len = strlen(message);
		  err = aes_crc_write(conn, data, len);
		  if (err != ERR_OK) {
              PRINTF("tcpclient: netconn_write: error \"%s\"\n", lwip_strerr(err));
              break;
            }

		  /* Receive response from server */

		  err = aes_crc_read(conn);
		  if (err != ERR_OK) {
		      PRINTF("tcpclient: aes_crc_read: error \"%s\"\n", lwip_strerr(err));
		      break;
		      }
	  }

      /*printf("Got EOF, looping\n");*/
      /*Close connection and discard connection identifier.*/
      netconn_close(conn);
      netconn_delete(conn);
    }
  	vTaskDelete(NULL);
}


/*-----------------------------------------------------------------------------------*/
void
tcpclient_init(void)
{
  sys_thread_new("tcpclient_thread", tcpclient_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
/*-----------------------------------------------------------------------------------*/

#endif /* LWIP_NETCONN */
