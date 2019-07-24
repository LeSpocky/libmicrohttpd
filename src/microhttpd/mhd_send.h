/*
  This file is part of libmicrohttpd
  Copyright (C) 2019 ng0

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

/**
 * @file mhd_send.h
 * @brief Implementation of send() wrappers.
 * @author ng0
 */

#ifndef MHD_SEND_H
#define MHD_SEND_H

#include "platform.h"
#include "internal.h"
#if defined(HAVE_STDBOOL_H)
#include <stdbool.h>
#endif /* HAVE_STDBOOL_H */
#include <errno.h>
#include "mhd_sockets.h"
#include "connection.h"
#include "connection_https.h"

/**
 * The enumeration of send socket options.
 */
enum MHD_SendSocketOptions
{
 /**
  * definitely no corking (use NODELAY, or explicitly disable cork)
  */
  MHD_SSO_NO_CORK = 0,
  /**
   * should enable corking (use MSG_MORE, or explicitly enable cork)
   */
  MHD_SSO_MAY_CORK = 1,
  /**
   * consider tcpi_snd_mss and consider not corking for the header
   * part if the size of the header is close to the MSS.
   * Only used if we are NOT doing 100 Continue and are still
   * sending the header (provided in full as the buffer to
   * MHD_send_on_connection_ or as the header to
   * MHD_send_on_connection2_).
   */
  MHD_SSO_HDR_CORK = 2
};


ssize_t
MHD_send_on_connection_ (struct MHD_Connection *connection,
                         const char *buffer,
                         size_t buffer_size,
                         enum MHD_SendSocketOptions options);

ssize_t
MHD_send_on_connection2_ (struct MHD_Connection *connection,
                          const char *header,
                          size_t header_size,
                          const char *buffer,
                          size_t buffer_size);

ssize_t
MHD_sendfile_on_connection_ (struct MHD_Connection *connection);

void
MHD_send_socket_state_nopush_ (struct MHD_Connection *connection,
                               bool value,
                               bool state_store);

void
MHD_send_socket_state_nodelay_ (struct MHD_Connection *connection,
                                bool value);
#endif /* MHD_SEND_H */
