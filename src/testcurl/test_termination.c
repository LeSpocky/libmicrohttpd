/*
     This file is part of libmicrohttpd
     Copyright (C) 2009 Christian Grothoff
     Copyright (C) 2014-2021 Evgeny Grin (Karlson2k)

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
 * @file daemontest_termination.c
 * @brief  Testcase for libmicrohttpd tolerating client not closing immediately
 * @author hollosig
 * @author Karlson2k (Evgeny Grin)
 */

#include "platform.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <microhttpd.h>
#include <unistd.h>
#include <curl/curl.h>

#ifndef __MINGW32__
#include <sys/select.h>
#include <sys/socket.h>
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif /* !WIN32_LEAN_AND_MEAN */
#include <windows.h>
#endif

static enum MHD_Result
connection_handler (void *cls,
                    struct MHD_Connection *connection,
                    const char *url,
                    const char *method,
                    const char *version,
                    const char *upload_data, size_t *upload_data_size,
                    void **req_cls)
{
  static int i;
  struct MHD_Response *response;
  enum MHD_Result ret;
  (void) cls; (void) url;                        /* Unused. Silent compiler warning. */
  (void) method; (void) version; (void) upload_data; /* Unused. Silent compiler warning. */
  (void) upload_data_size;                       /* Unused. Silent compiler warning. */

  if (*req_cls == NULL)
  {
    *req_cls = &i;
    return MHD_YES;
  }

  if (*upload_data_size != 0)
  {
    (*upload_data_size) = 0;
    return MHD_YES;
  }

  response =
    MHD_create_response_from_buffer (strlen ("Response"), "Response",
                                     MHD_RESPMEM_PERSISTENT);
  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);

  return ret;
}


static size_t
write_data (void *ptr, size_t size, size_t nmemb, void *stream)
{
  (void) ptr; (void) stream;       /* Unused. Silent compiler warning. */
  return size * nmemb;
}


int
main (void)
{
  struct MHD_Daemon *daemon;
  int port;
  char url[255];
  CURL *curl;
  CURLcode success;

  if (MHD_NO != MHD_is_feature_supported (MHD_FEATURE_AUTODETECT_BIND_PORT))
    port = 0;
  else
    port = 1490;


  daemon = MHD_start_daemon (MHD_USE_THREAD_PER_CONNECTION
                             | MHD_USE_INTERNAL_POLLING_THREAD
                             | MHD_USE_ERROR_LOG,
                             port,
                             NULL,
                             NULL, connection_handler, NULL, MHD_OPTION_END);

  if (daemon == NULL)
  {
    fprintf (stderr, "Daemon cannot be started!");
    exit (1);
  }
  if (0 == port)
  {
    const union MHD_DaemonInfo *dinfo;
    dinfo = MHD_get_daemon_info (daemon, MHD_DAEMON_INFO_BIND_PORT);
    if ((NULL == dinfo) || (0 == dinfo->port) )
    {
      MHD_stop_daemon (daemon); return 32;
    }
    port = (int) dinfo->port;
  }

  curl = curl_easy_init ();
  /* curl_easy_setopt(curl, CURLOPT_POST, 1L); */
  snprintf (url,
            sizeof (url),
            "http://127.0.0.1:%d",
            port);
  curl_easy_setopt (curl, CURLOPT_URL, url);
  curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_data);

  success = curl_easy_perform (curl);
  if (success != 0)
  {
    fprintf (stderr, "CURL Error");
    exit (1);
  }
  /* CPU used to go crazy here */
  (void) sleep (1);

  curl_easy_cleanup (curl);
  MHD_stop_daemon (daemon);

  return 0;
}
