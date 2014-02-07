/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		aice.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AICE_H
#define TB_ASIO_AICE_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "aico.h"
#include "../network/ipv4.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the aice code enum
typedef enum __tb_aice_code_e
{
 	TB_AICE_CODE_NONE 			= 0

, 	TB_AICE_CODE_ACPT 			= 1 	//!< for sock, accept it
, 	TB_AICE_CODE_CONN 			= 2 	//!< for sock, connect to the host address
,	TB_AICE_CODE_RECV 			= 3		//!< for sock, recv data for tcp
,	TB_AICE_CODE_SEND 			= 4		//!< for sock, send data for tcp
,	TB_AICE_CODE_URECV 			= 5		//!< for sock, recv data for udp
,	TB_AICE_CODE_USEND 			= 6		//!< for sock, send data for udp
,	TB_AICE_CODE_RECVV 			= 7		//!< for sock, recv iovec data for tcp
,	TB_AICE_CODE_SENDV 			= 8		//!< for sock, send iovec data for tcp
,	TB_AICE_CODE_URECVV 		= 9		//!< for sock, recv iovec data for udp
,	TB_AICE_CODE_USENDV 		= 10	//!< for sock, send iovec data for udp
,	TB_AICE_CODE_SENDFILE 		= 11	//!< for sock, maybe return TB_AICE_STATE_NOTSUPPORTED

,	TB_AICE_CODE_READ 			= 12	//!< for file, read data
,	TB_AICE_CODE_WRIT 			= 13	//!< for file, writ data
,	TB_AICE_CODE_READV 			= 14	//!< for file, read iovec data
,	TB_AICE_CODE_WRITV 			= 15	//!< for file, writ iovec data
,	TB_AICE_CODE_FSYNC 			= 16	//!< for file, flush data to file

,	TB_AICE_CODE_RUNTASK 		= 17	//!< for task or sock or file, run task with the given delay

, 	TB_AICE_CODE_MAXN 			= 18

}tb_aice_code_e;

/// the aice state code enum
typedef enum __tb_aice_state_e
{
 	TB_AICE_STATE_OK 			= 0
, 	TB_AICE_STATE_FAILED 		= 1
, 	TB_AICE_STATE_KILLED 		= 2
,	TB_AICE_STATE_CLOSED 		= 3
, 	TB_AICE_STATE_PENDING 		= 4
,	TB_AICE_STATE_TIMEOUT 		= 5
,	TB_AICE_STATE_NOTSUPPORTED 	= 6

}tb_aice_state_e;

/// the addr aice type
typedef struct __tb_aice_addr_t
{
	/// the host
	tb_char_t const* 			host;

	/// the addr
	tb_ipv4_t 					addr;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[4];

}tb_aice_addr_t;

/// the acpt aice type
typedef struct __tb_aice_acpt_t
{
	/// the client socket 
	tb_handle_t 				sock;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[5];

}tb_aice_acpt_t;

/// the conn aice type
typedef struct __tb_aice_conn_t
{
	/// the port
	tb_size_t 					port;

	/// the addr
	tb_ipv4_t 					addr;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[4];

}tb_aice_conn_t;

#ifdef TB_CONFIG_OS_WINDOWS
/// the recv aice type, base: tb_iovec_t
typedef struct __tb_aice_recv_t
{
	/// the data size for (tb_iovec_t*)->size
	tb_size_t 					size;

	/// the recv data for (tb_iovec_t*)->data
	tb_byte_t* 					data;

	/// the data real
	tb_size_t 					real;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[3];

}tb_aice_recv_t;

/// the send aice type, base: tb_iovec_t
typedef struct __tb_aice_send_t
{
	/// the data size for (tb_iovec_t*)->size
	tb_size_t 					size;

	/// the send data for (tb_iovec_t*)->data
	tb_byte_t const* 			data;

	/// the data real
	tb_size_t 					real;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[3];

}tb_aice_send_t;

/// the urecv aice type, base: tb_iovec_t
typedef struct __tb_aice_urecv_t
{
	/// the data size for (tb_iovec_t*)->size
	tb_size_t 					size;

	/// the recv data for (tb_iovec_t*)->data
	tb_byte_t* 					data;

	/// the data real
	tb_size_t 					real;

	/// the addr
	tb_ipv4_t 					addr;

	/// the port
	tb_size_t 					port;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_urecv_t;

/// the usend aice type, base: tb_iovec_t
typedef struct __tb_aice_usend_t
{
	/// the data size for (tb_iovec_t*)->size
	tb_size_t 					size;

	/// the send data for (tb_iovec_t*)->data
	tb_byte_t const* 			data;

	/// the data real
	tb_size_t 					real;

	/// the addr
	tb_ipv4_t 					addr;

	/// the port
	tb_size_t 					port;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_usend_t;

/// the read aice type, base: tb_iovec_t
typedef struct __tb_aice_read_t
{
	/// the data size for (tb_iovec_t*)->size
	tb_size_t 					size;

	/// the read data for (tb_iovec_t*)->data
	tb_byte_t* 					data;

	/// the data real
	tb_size_t 					real;

	/// the file seek
	tb_hize_t 					seek;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_read_t;

/// the writ aice type, base: tb_iovec_t
typedef struct __tb_aice_writ_t
{
	/// the data size for (tb_iovec_t*)->size
	tb_size_t 					size;

	/// the writ data for (tb_iovec_t*)->data
	tb_byte_t const* 			data;

	/// the data real
	tb_size_t 					real;

	/// the file seek
	tb_hize_t 					seek;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_writ_t;
#else
/// the recv aice type, base: tb_iovec_t
typedef struct __tb_aice_recv_t
{
	/// the recv data for (tb_iovec_t*)->data
	tb_byte_t* 					data;

	/// the data size for (tb_iovec_t*)->size
	tb_size_t 					size;

	/// the data real
	tb_size_t 					real;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[3];

}tb_aice_recv_t;

/// the send aice type, base: tb_iovec_t
typedef struct __tb_aice_send_t
{
	/// the send data for (tb_iovec_t*)->data
	tb_byte_t const* 			data;

	/// the data size for (tb_iovec_t*)->size
	tb_size_t 					size;

	/// the data real
	tb_size_t 					real;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[3];

}tb_aice_send_t;

/// the urecv aice type, base: tb_iovec_t
typedef struct __tb_aice_urecv_t
{
	/// the recv data for (tb_iovec_t*)->data
	tb_byte_t* 					data;

	/// the data size for (tb_iovec_t*)->size
	tb_size_t 					size;

	/// the data real
	tb_size_t 					real;

	/// the addr
	tb_ipv4_t 					addr;

	/// the port
	tb_size_t 					port;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_urecv_t;

/// the usend aice type, base: tb_iovec_t
typedef struct __tb_aice_usend_t
{
	/// the send data for (tb_iovec_t*)->data
	tb_byte_t const* 			data;

	/// the data size for (tb_iovec_t*)->size
	tb_size_t 					size;

	/// the data real
	tb_size_t 					real;

	/// the addr
	tb_ipv4_t 					addr;

	/// the port
	tb_size_t 					port;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_usend_t;

/// the read aice type, base: tb_iovec_t
typedef struct __tb_aice_read_t
{
	/// the read data for (tb_iovec_t*)->data
	tb_byte_t* 					data;

	/// the data size for (tb_iovec_t*)->size
	tb_size_t 					size;

	/// the data real
	tb_size_t 					real;

	/// the file seek
	tb_hize_t 					seek;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_read_t;

/// the writ aice type, base: tb_iovec_t
typedef struct __tb_aice_writ_t
{
	/// the writ data for (tb_iovec_t*)->data
	tb_byte_t const* 			data;

	/// the data size for (tb_iovec_t*)->size
	tb_size_t 					size;

	/// the data real
	tb_size_t 					real;

	/// the file seek
	tb_hize_t 					seek;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_writ_t;
#endif

/// the recvv aice type
typedef struct __tb_aice_recvv_t
{
	/// the recv list
	tb_iovec_t const* 			list;

	/// the list size
	tb_size_t 					size;

	/// the data real
	tb_size_t 					real;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[3];

}tb_aice_recvv_t;

/// the sendv aice type
typedef struct __tb_aice_sendv_t
{
	/// the send list
	tb_iovec_t const* 			list;

	/// the list size
	tb_size_t 					size;

	/// the data real
	tb_size_t 					real;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[3];

}tb_aice_sendv_t;

/// the urecvv aice type
typedef struct __tb_aice_urecvv_t
{
	/// the recv list
	tb_iovec_t const* 			list;

	/// the list size
	tb_size_t 					size;

	/// the data real
	tb_size_t 					real;

	/// the addr
	tb_ipv4_t 					addr;

	/// the port
	tb_size_t 					port;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_urecvv_t;

/// the usendv aice type
typedef struct __tb_aice_usendv_t
{
	/// the send list
	tb_iovec_t const* 			list;

	/// the list size
	tb_size_t 					size;

	/// the data real
	tb_size_t 					real;

	/// the addr
	tb_ipv4_t 					addr;

	/// the port
	tb_size_t 					port;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_usendv_t;

/// the sendfile aice type
typedef struct __tb_aice_sendfile_t
{
	/// the file
	tb_handle_t 				file;

	/// the size
	tb_hize_t 					size;

	/// the real
	tb_hize_t 					real;

	/// the seek
	tb_hize_t 					seek;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_sendfile_t;

/// the readv aice type
typedef struct __tb_aice_readv_t
{
	/// the read list
	tb_iovec_t const* 			list;

	/// the list size
	tb_size_t 					size;

	/// the data real
	tb_size_t 					real;

	/// the file seek
	tb_hize_t 					seek;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_readv_t;

/// the writv aice type
typedef struct __tb_aice_writv_t
{
	/// the writ list
	tb_iovec_t const* 			list;

	/// the list size
	tb_size_t 					size;

	/// the data real
	tb_size_t 					real;

	/// the file seek
	tb_hize_t 					seek;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[1];

}tb_aice_writv_t;

/// the runtask aice type
typedef struct __tb_aice_runtask_t
{
	/// the when
	tb_hize_t 					when;

	/// the private data for using the left space of the union
	tb_handle_t 				priv[4];

}tb_aice_runtask_t;

/// the aice type
typedef struct __tb_aice_t
{
	/// the aice code
	tb_uint32_t 				code 	: 8;

	/// the state
	tb_uint32_t 				state 	: 8;

	/// the aicb
	tb_aicb_t 					aicb;

	/// the data
	tb_pointer_t 				data;

	/// the aico
	tb_aico_t* 					aico;

	/// the uion
	union
	{
		// for sock
		tb_aice_addr_t 			addr;
		tb_aice_acpt_t 			acpt;
		tb_aice_conn_t 			conn;
		tb_aice_recv_t 			recv;
		tb_aice_send_t 			send;
		tb_aice_urecv_t 		urecv;
		tb_aice_usend_t 		usend;
		tb_aice_recvv_t 		recvv;
		tb_aice_sendv_t 		sendv;
		tb_aice_urecvv_t 		urecvv;
		tb_aice_usendv_t 		usendv;
		tb_aice_sendfile_t 		sendfile;

		// for file
		tb_aice_read_t 			read;
		tb_aice_writ_t 			writ;
		tb_aice_readv_t 		readv;
		tb_aice_writv_t 		writv;

		// for task
		tb_aice_runtask_t 		runtask;

	} u;

}tb_aice_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the aice state c-string
 *
 * @param aice 		the aice
 *
 * @return 			the aice state c-string
 */
tb_char_t const* 	tb_aice_state_cstr(tb_aice_t const* aice);



#endif
