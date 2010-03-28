/*
 * Author: Ross Burton <ross@openedhand.com>
 * Copyright (C) 2005 Opened Hand Ltd
 */

#include <stdlib.h>
#include <dbus/dbus-glib-bindings.h>
#include "EchoObjectBindings.h"

/* Convenience function to print an error and exit */
static void
die (const char *prefix, GError *error) 
{
  g_error("%s: %s", prefix, error->message);
  g_error_free (error);
  exit(1);
}

static GMainLoop *loop = NULL;

int
main (int argc, char **argv)
{
  GError *error = NULL;
  DBusGConnection *connection;
  DBusGProxy *proxy;
  char *s_out = NULL;

  g_type_init ();
  
  loop = g_main_loop_new (NULL, FALSE);

  connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (connection == NULL)
    die ("Failed to open connection to bus", error);

  proxy = dbus_g_proxy_new_for_name_owner (connection,
                                           "com.openedhand.DBus.Tests.Echo",
                                           "/com/openedhand/DBus/Tests/Echo",
                                           "com.openedhand.DBus.Tests.Echo",
                                           &error);
  if (proxy == NULL)
    die ("Failed to create proxy for name owner", error);

  if (!com_openedhand_DBus_Tests_Echo_echo (proxy, "Hello, World", &s_out, &error))
    die ("Call to echo failed", error);

  g_print("Got '%s'\n", s_out);
  g_free (s_out);

  return 0;
}
