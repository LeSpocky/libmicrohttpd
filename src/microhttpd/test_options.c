/*
 This file is part of libmicrohttpd
 Copyright (C) 2007 Christian Grothoff

 libmicrohttpd is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published
 by the Free Software Foundation; either version 2, or (at your
 option) any later version.

 libmicrohttpd is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with libmicrohttpd; see the file COPYING.  If not, write to the
 Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
 */

/**
 * @file test_options.c
 * @brief  Testcase for libmicrohttpd HTTPS GET operations
 * @author Sagie Amir
 */

#include "platform.h"
#include "microhttpd.h"
#include "mhd_sockets.h"

#define MHD_E_MEM "Error: memory error\n"
#define MHD_E_SERVER_INIT "Error: failed to start server\n"

const int DEBUG_GNUTLS_LOG_LEVEL = 0;
const char *test_file_name = "https_test_file";
const char test_file_data[] = "Hello World\n";

static enum MHD_Result
ahc_echo (void *cls,
          struct MHD_Connection *connection,
          const char *url,
          const char *method,
          const char *version,
          const char *upload_data, size_t *upload_data_size,
          void **req_cls)
{
  (void) cls;
  (void) connection;
  (void) url;
  (void) method;
  (void) version;
  (void) upload_data;
  (void) upload_data_size;
  (void) req_cls;

  return 0;
}


static int
test_wrap_loc (char *test_name, int (*test)(void))
{
  int ret;

  fprintf (stdout, "running test: %s ", test_name);
  ret = test ();
  if (ret == 0)
  {
    fprintf (stdout, "[pass]\n");
  }
  else
  {
    fprintf (stdout, "[fail]\n");
  }
  return ret;
}


/**
 * Test daemon initialization with the MHD_OPTION_SOCK_ADDR option
 */
static int
test_ip_addr_option ()
{
  struct MHD_Daemon *d;
  struct sockaddr_in daemon_ip_addr;
#if HAVE_INET6 && defined (USE_IPV6_TESTING)
  struct sockaddr_in6 daemon_ip_addr6;
#endif

  memset (&daemon_ip_addr, 0, sizeof (struct sockaddr_in));
  daemon_ip_addr.sin_family = AF_INET;
  daemon_ip_addr.sin_port = 0;
  daemon_ip_addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

#if HAVE_INET6 && defined (USE_IPV6_TESTING)
  memset (&daemon_ip_addr6, 0, sizeof (struct sockaddr_in6));
  daemon_ip_addr6.sin6_family = AF_INET6;
  daemon_ip_addr6.sin6_port = 0;
  daemon_ip_addr6.sin6_addr = in6addr_loopback;
#endif

  d = MHD_start_daemon (MHD_USE_ERROR_LOG, 0,
                        NULL, NULL, &ahc_echo, NULL, MHD_OPTION_SOCK_ADDR,
                        &daemon_ip_addr, MHD_OPTION_END);

  if (d == 0)
    return -1;

  MHD_stop_daemon (d);

#if HAVE_INET6 && defined (USE_IPV6_TESTING)
  d = MHD_start_daemon (MHD_USE_ERROR_LOG | MHD_USE_IPv6, 0,
                        NULL, NULL, &ahc_echo, NULL, MHD_OPTION_SOCK_ADDR,
                        &daemon_ip_addr6, MHD_OPTION_END);

  if (d == 0)
    return -1;

  MHD_stop_daemon (d);
#endif

  return 0;
}


/* setup a temporary transfer test file */
int
main (int argc, char *const *argv)
{
  unsigned int errorCount = 0;
  (void) argc; (void) argv; /* Unused. Silent compiler warning. */

  errorCount += test_wrap_loc ("ip addr option", &test_ip_addr_option);

  return errorCount != 0;
}
