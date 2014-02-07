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
 * @file		socket.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_SOCKET_H
#define TB_PLATFORM_SOCKET_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the socket type enum
typedef enum __tb_socket_type_e
{
 	TB_SOCKET_TYPE_NUL 	= 0
,	TB_SOCKET_TYPE_TCP 	= 1
, 	TB_SOCKET_TYPE_UDP 	= 2

}tb_socket_type_e;

/// the socket kill enum
typedef enum __tb_socket_kill_e
{
 	TB_SOCKET_KILL_RO 	= 0
,	TB_SOCKET_KILL_WO 	= 1
, 	TB_SOCKET_KILL_RW 	= 2

}tb_socket_kill_e;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init socket
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_socket_init(tb_noarg_t);

/// exit socket
tb_void_t 			tb_socket_exit(tb_noarg_t);

/*! open socket
 *
 * @param type 		the socket type
 *
 * @return 			the socket handle
 */
tb_handle_t 		tb_socket_open(tb_size_t type);

/*! connect socket
 *
 * @param handle 	the socket handle
 * @param ip 		the ip address
 * @param port 		the port
 *
 * @return 			ok: 1, continue: 0; failed: -1
 */
tb_long_t 			tb_socket_connect(tb_handle_t handle, tb_char_t const* ip, tb_size_t port);

/*! bind socket
 *
 * @param handle 	the socket handle
 * @param ip 		the ip address
 * @param port 		the bind port
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_socket_bind(tb_handle_t handle, tb_char_t const* ip, tb_size_t port);

/*! listen socket
 *
 * @param handle 	the socket handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_socket_listen(tb_handle_t handle);

/*! accept socket
 *
 * @param handle 	the socket handle
 *
 * @return 			the client socket handle
 */
tb_handle_t 		tb_socket_accept(tb_handle_t handle);

/*! kill socket
 *
 * @param handle 	the socket handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_socket_kill(tb_handle_t handle, tb_size_t mode);

/*! close socket
 *
 * @param handle 	the socket handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_socket_close(tb_handle_t handle);

/*! recv the socket data
 *
 * @param handle 	the socket handle
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size or -1
 */
tb_long_t 			tb_socket_recv(tb_handle_t handle, tb_byte_t* data, tb_size_t size);

/*! send the socket data
 *
 * @param handle 	the socket handle
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size or -1
 */
tb_long_t 			tb_socket_send(tb_handle_t handle, tb_byte_t* data, tb_size_t size);

/*! recv the socket data for udp
 *
 * @param handle 	the socket handle
 * @param host 		the host
 * @param port 		the port
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size or -1
 */
tb_long_t 			tb_socket_urecv(tb_handle_t handle, tb_char_t const* host, tb_size_t port, tb_byte_t* data, tb_size_t size);

/*! send the socket data for udp
 *
 * @param handle 	the socket handle
 * @param host 		the host
 * @param port 		the port
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size or -1
 */
tb_long_t 			tb_socket_usend(tb_handle_t handle, tb_char_t const* host, tb_size_t port, tb_byte_t* data, tb_size_t size);
	
#endif
