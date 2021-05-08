/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* dbus-print-message.h  Utility function to print out a message
 *
 * Copyright (C) 2003 Philip Blundell <philb@gnu.org>
 * Copyright (C) 2003 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <config.h>
#include "dbus-print-message.h"

#ifdef DBUS_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#endif

#include <stdlib.h>
#include "config.h"

static const char*
type_to_name (int message_type)
{
  switch (message_type)
    {
    case DBUS_MESSAGE_TYPE_SIGNAL:
      return "signal";
    case DBUS_MESSAGE_TYPE_METHOD_CALL:
      return "method call";
    case DBUS_MESSAGE_TYPE_METHOD_RETURN:
      return "method return";
    case DBUS_MESSAGE_TYPE_ERROR:
      return "error";
    default:
      return "(unknown message type)";
    }
}

#define INDENT 3

static void
indent (int depth)
{
  while (depth-- > 0)
    printf ("   "); /* INDENT spaces. */
}

static void
print_hex (unsigned char *bytes, unsigned int len, int depth)
{
  unsigned int i, columns;

  printf ("array of bytes [\n");

  indent (depth + 1);

  /* Each byte takes 3 cells (two hexits, and a space), except the last one. */
  columns = (80 - ((depth + 1) * INDENT)) / 3;

  if (columns < 8)
    columns = 8;

  i = 0;

  while (i < len)
    {
      printf ("%02x", bytes[i]);
      i++;

      if (i != len)
        {
          if (i % columns == 0)
            {
              printf ("\n");
              indent (depth + 1);
            }
          else
            {
              printf (" ");
            }
        }
    }

  printf ("\n");
  indent (depth);
  printf ("]\n");
}

#define DEFAULT_SIZE 100

static void
print_ay (DBusMessageIter *iter, int depth)
{
  /* Not using DBusString because it's not public API. It's 2009, and I'm
   * manually growing a string chunk by chunk.
   */
  unsigned char *bytes = malloc (DEFAULT_SIZE + 1);
  unsigned int len = 0;
  unsigned int max = DEFAULT_SIZE;
  dbus_bool_t all_ascii = TRUE;
  int current_type;

  while ((current_type = dbus_message_iter_get_arg_type (iter))
          != DBUS_TYPE_INVALID)
    {
      unsigned char val;

      dbus_message_iter_get_basic (iter, &val);
      bytes[len] = val;
      len++;

      if (val < 32 || val > 126)
        all_ascii = FALSE;

      if (len == max)
        {
          max *= 2;
          bytes = realloc (bytes, max + 1);
        }

      dbus_message_iter_next (iter);
    }

  if (all_ascii)
    {
      bytes[len] = '\0';
      printf ("array of bytes \"%s\"\n", bytes);
    }
  else
    {
      print_hex (bytes, len, depth);
    }

  free (bytes);
}

static void
print_fd (int fd, int depth)
{
#ifdef DBUS_UNIX
  int ret;
  struct stat statbuf = {0,};
  union {
      struct sockaddr sa;
      struct sockaddr_storage storage;
      struct sockaddr_un un;
      struct sockaddr_in ipv4;
      struct sockaddr_in6 ipv6;
  } addr, peer;
  char hostip[INET6_ADDRSTRLEN];
  int addrlen = sizeof (addr);
  int peerlen = sizeof (peer);
  int has_peer;
#endif

  /* Don't print the fd number: it is different in every process and since
   * dbus-monitor closes the fd after reading it, the same number would be
   * printed again and again.
   */
  printf ("file descriptor\n");
  if (fd == -1)
    return;

#ifdef DBUS_UNIX
  ret = fstat (fd, &statbuf);
  if (ret == -1)
    return;

  indent (depth+1);
  printf ("inode: %d\n", (int) statbuf.st_ino);

  indent (depth+1);
  printf ("type: ");
  if (S_ISREG(statbuf.st_mode))
    printf ("file\n");
  if (S_ISDIR(statbuf.st_mode))
    printf ("directory\n");
  if (S_ISCHR(statbuf.st_mode))
    printf ("char\n");
  if (S_ISBLK(statbuf.st_mode))
    printf ("block\n");
  if (S_ISFIFO(statbuf.st_mode))
    printf ("fifo\n");
  if (S_ISLNK(statbuf.st_mode))
    printf ("link\n");
  if (S_ISSOCK(statbuf.st_mode))
    printf ("socket\n");

  /* If it's not a socket, getsockname will just return -1 with errno
   * ENOTSOCK. */

  memset (&addr, 0, sizeof (addr));
  memset (&peer, 0, sizeof (peer));

  if (getsockname(fd, &addr.sa, &addrlen))
    return;

  has_peer = !getpeername(fd, &peer.sa, &peerlen);

  indent (depth+1);
  printf ("address family: ");
  switch (addr.sa.sa_family)
    {
      case AF_UNIX:
        printf("unix\n");
        if (addr.un.sun_path[0] == '\0')
          {
            /* Abstract socket might not be zero-terminated and length is
             * variable. Who designed this interface?
             * Write the name in the same way as /proc/net/unix
             * See manual page unix(7)
             */
            indent (depth+1);
            printf ("name @%.*s\n",
                    (int) (addrlen - sizeof (sa_family_t) - 1),
                    &(addr.un.sun_path[1]));

            if (has_peer)
              {
                indent (depth+1);
                printf ("peer @%.*s\n",
                        (int) (addrlen - sizeof (sa_family_t) - 1),
                        &(addr.un.sun_path[1]));
              }
          }
        else
          {
            indent (depth+1);
            printf ("name %s\n", addr.un.sun_path);
            if (has_peer)
              {
                indent (depth+1);
                printf ("peer %s\n", peer.un.sun_path);
              }
          }
        break;

      case AF_INET:
        printf ("inet\n");
        if (inet_ntop (AF_INET, &addr.ipv4.sin_addr, hostip, sizeof (hostip)))
          {
            indent (depth+1);
            printf ("name %s port %u\n", hostip, ntohs (addr.ipv4.sin_port));
          }
        if (has_peer && inet_ntop (AF_INET, &peer.ipv4.sin_addr, hostip, sizeof (hostip)))
          {
            indent (depth+1);
            printf ("peer %s port %u\n", hostip, ntohs (peer.ipv4.sin_port));
          }

        break;

#ifdef AF_INET6
      case AF_INET6:
        printf ("inet6\n");
        if (inet_ntop (AF_INET6, &addr.ipv6.sin6_addr, hostip, sizeof (hostip)))
          {
            indent (depth+1);
            printf ("name %s port %u\n", hostip, ntohs (addr.ipv6.sin6_port));
          }
        if (has_peer && inet_ntop (AF_INET6, &peer.ipv6.sin6_addr, hostip, sizeof (hostip)))
          {
            indent (depth+1);
            printf ("peer %s port %u\n", hostip, ntohs (peer.ipv6.sin6_port));
          }
        break;
#endif

#ifdef AF_BLUETOOTH
      case AF_BLUETOOTH:
        printf ("bluetooth\n");
        break;
#endif

      default:
        printf ("unknown (%d)\n", addr.sa.sa_family);
        break;
    }
#endif
}

static void
print_iter (DBusMessageIter *iter, dbus_bool_t literal, int depth)
{
  do
    {
      int type = dbus_message_iter_get_arg_type (iter);

      if (type == DBUS_TYPE_INVALID)
	break;
      
      indent(depth);

      switch (type)
	{
	case DBUS_TYPE_STRING:
	  {
	    char *val;
	    dbus_message_iter_get_basic (iter, &val);
	    if (!literal)
	      printf ("string \"");
	    printf ("%s", val);
	    if (!literal)
	      printf ("\"\n");
	    break;
	  }

	case DBUS_TYPE_SIGNATURE:
	  {
	    char *val;
	    dbus_message_iter_get_basic (iter, &val);
	    if (!literal)
	      printf ("signature \"");
	    printf ("%s", val);
	    if (!literal)
	      printf ("\"\n");
	    break;
	  }

	case DBUS_TYPE_OBJECT_PATH:
	  {
	    char *val;
	    dbus_message_iter_get_basic (iter, &val);
	    if (!literal)
	      printf ("object path \"");
	    printf ("%s", val);
	    if (!literal)
	      printf ("\"\n");
	    break;
	  }

	case DBUS_TYPE_INT16:
	  {
	    dbus_int16_t val;
	    dbus_message_iter_get_basic (iter, &val);
	    printf ("int16 %d\n", val);
	    break;
	  }

	case DBUS_TYPE_UINT16:
	  {
	    dbus_uint16_t val;
	    dbus_message_iter_get_basic (iter, &val);
	    printf ("uint16 %u\n", val);
	    break;
	  }

	case DBUS_TYPE_INT32:
	  {
	    dbus_int32_t val;
	    dbus_message_iter_get_basic (iter, &val);
	    printf ("int32 %d\n", val);
	    break;
	  }

	case DBUS_TYPE_UINT32:
	  {
	    dbus_uint32_t val;
	    dbus_message_iter_get_basic (iter, &val);
	    printf ("uint32 %u\n", val);
	    break;
	  }

	case DBUS_TYPE_INT64:
	  {
	    dbus_int64_t val;
	    dbus_message_iter_get_basic (iter, &val);
#ifdef DBUS_INT64_PRINTF_MODIFIER
        printf ("int64 %" DBUS_INT64_PRINTF_MODIFIER "d\n", val);
#else
        printf ("int64 (omitted)\n");
#endif
	    break;
	  }

	case DBUS_TYPE_UINT64:
	  {
	    dbus_uint64_t val;
	    dbus_message_iter_get_basic (iter, &val);
#ifdef DBUS_INT64_PRINTF_MODIFIER
        printf ("uint64 %" DBUS_INT64_PRINTF_MODIFIER "u\n", val);
#else
        printf ("uint64 (omitted)\n");
#endif
	    break;
	  }

	case DBUS_TYPE_DOUBLE:
	  {
	    double val;
	    dbus_message_iter_get_basic (iter, &val);
	    printf ("double %g\n", val);
	    break;
	  }

	case DBUS_TYPE_BYTE:
	  {
	    unsigned char val;
	    dbus_message_iter_get_basic (iter, &val);
	    printf ("byte %d\n", val);
	    break;
	  }

	case DBUS_TYPE_BOOLEAN:
	  {
	    dbus_bool_t val;
	    dbus_message_iter_get_basic (iter, &val);
	    printf ("boolean %s\n", val ? "true" : "false");
	    break;
	  }

	case DBUS_TYPE_VARIANT:
	  {
	    DBusMessageIter subiter;

	    dbus_message_iter_recurse (iter, &subiter);

	    printf ("variant ");
	    print_iter (&subiter, literal, depth+1);
	    break;
	  }
	case DBUS_TYPE_ARRAY:
	  {
	    int current_type;
	    DBusMessageIter subiter;

	    dbus_message_iter_recurse (iter, &subiter);

	    current_type = dbus_message_iter_get_arg_type (&subiter);

	    if (current_type == DBUS_TYPE_BYTE)
	      {
		print_ay (&subiter, depth);
		break;
	      }

	    printf("array [\n");
	    while (current_type != DBUS_TYPE_INVALID)
	      {
		print_iter (&subiter, literal, depth+1);

		dbus_message_iter_next (&subiter);
		current_type = dbus_message_iter_get_arg_type (&subiter);

		if (current_type != DBUS_TYPE_INVALID)
		  printf (",");
	      }
	    indent(depth);
	    printf("]\n");
	    break;
	  }
	case DBUS_TYPE_DICT_ENTRY:
	  {
	    DBusMessageIter subiter;

	    dbus_message_iter_recurse (iter, &subiter);

	    printf("dict entry(\n");
	    print_iter (&subiter, literal, depth+1);
	    dbus_message_iter_next (&subiter);
	    print_iter (&subiter, literal, depth+1);
	    indent(depth);
	    printf(")\n");
	    break;
	  }
	    
	case DBUS_TYPE_STRUCT:
	  {
	    int current_type;
	    DBusMessageIter subiter;

	    dbus_message_iter_recurse (iter, &subiter);

	    printf("struct {\n");
	    while ((current_type = dbus_message_iter_get_arg_type (&subiter)) != DBUS_TYPE_INVALID)
	      {
		print_iter (&subiter, literal, depth+1);
		dbus_message_iter_next (&subiter);
		if (dbus_message_iter_get_arg_type (&subiter) != DBUS_TYPE_INVALID)
		  printf (",");
	      }
	    indent(depth);
	    printf("}\n");
	    break;
	  }

	case DBUS_TYPE_UNIX_FD:
	  {
	    int fd;
	    dbus_message_iter_get_basic (iter, &fd);

	    print_fd (fd, depth+1);

            /* dbus_message_iter_get_basic() duplicated the fd, we need to
             * close it after use. The original fd will be closed when the
             * DBusMessage is released.
             */
            close (fd);

	    break;
	  }

	default:
	  printf (" (dbus-monitor too dumb to decipher arg type '%c')\n", type);
	  break;
	}
    } while (dbus_message_iter_next (iter));
}

void
print_message (DBusMessage *message, dbus_bool_t literal)
{
  DBusMessageIter iter;
  const char *sender;
  const char *destination;
  int message_type;

  message_type = dbus_message_get_type (message);
  sender = dbus_message_get_sender (message);
  destination = dbus_message_get_destination (message);
  
  if (!literal)
    {
      printf ("%s sender=%s -> dest=%s",
	      type_to_name (message_type),
	      sender ? sender : "(null sender)",
	      destination ? destination : "(null destination)");
  
      switch (message_type)
	{
	case DBUS_MESSAGE_TYPE_METHOD_CALL:
	case DBUS_MESSAGE_TYPE_SIGNAL:
	  printf (" serial=%u path=%s; interface=%s; member=%s\n",
                  dbus_message_get_serial (message),
		  dbus_message_get_path (message),
		  dbus_message_get_interface (message),
		  dbus_message_get_member (message));
	  break;
      
	case DBUS_MESSAGE_TYPE_METHOD_RETURN:
	  printf (" reply_serial=%u\n",
          dbus_message_get_reply_serial (message));
	  break;

	case DBUS_MESSAGE_TYPE_ERROR:
	  printf (" error_name=%s reply_serial=%u\n",
		  dbus_message_get_error_name (message),
          dbus_message_get_reply_serial (message));
	  break;

	default:
	  printf ("\n");
	  break;
	}
    }

  dbus_message_iter_init (message, &iter);
  print_iter (&iter, literal, 1);
  fflush (stdout);
  
}

