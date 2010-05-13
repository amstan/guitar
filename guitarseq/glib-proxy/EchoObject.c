/*
 * Author: Ross Burton <ross@openedhand.com>
 * Copyright (C) 2005 Opened Hand Ltd
 */

#include <stdlib.h>
#include <dbus/dbus-glib-bindings.h>
#include <glib-object.h>
#include "EchoObject.h"
#include "EchoObjectGlue.h"

/* Convenience macro to test and set a GError/return on failure */
#define g_set_error_val_if_fail(test, returnval, error, domain, code) G_STMT_START{ \
 if G_LIKELY (test) {} else { \
   g_set_error (error, domain, code, #test); \
   g_warning(#test " failed"); \
   return (returnval); \
 } \
}G_STMT_END


/* Generate the GObject boilerplate */
G_DEFINE_TYPE(Echo, echo, G_TYPE_OBJECT)

/* Create the Echo error quark */
GQuark
echo_error_quark (void)
{
  static GQuark quark = 0;
  if (!quark)
    quark = g_quark_from_static_string ("echo_error");
  return quark;
}

/* Class init */
static void
echo_class_init (EchoClass *echo_class)
{
  dbus_g_object_class_install_info (G_OBJECT_CLASS (echo_class), &dbus_glib_echo_object_info);
}

/* Instance init */
static void
echo_init (Echo *echo)
{
}

/* The echo method */
gboolean
echo_echo (Echo *echo, const char* string, char **echo_string, GError **error)
{
  g_set_error_val_if_fail (string != NULL, FALSE, error, ECHO_ERROR, ECHO_ERROR_GENERIC);
  g_set_error_val_if_fail (echo_string != NULL, FALSE, error, ECHO_ERROR, ECHO_ERROR_GENERIC);
  *echo_string = g_utf8_strreverse (string, -1);
  return TRUE;
}


#if STANDALONE

int main (int argc, char **argv)
{
  Echo *echo;
  char *s;
  g_type_init();
  echo = g_object_new (ECHO_TYPE, NULL);
  g_assert (echo != NULL);
  echo_echo (echo, "Hello, World!", &s, NULL);
  g_print("Got the echo '%s'\n", s);
  g_object_unref (echo);
  return 0;
}

#else

/* Convenience function to print an error and exit */
static void
die (const char *prefix, GError *error) 
{
  g_error("%s: %s", prefix, error->message);
  g_error_free (error);
  exit(1);
}

#define ECHO_SERVICE_NAME "com.openedhand.DBus.Tests.Echo"

int
main (int argc, char **argv)
{
  GMainLoop *loop;
  DBusGConnection *connection;
  GError *error = NULL;
  GObject *obj;
  DBusGProxy *driver_proxy;
  guint32 request_name_ret;

  g_type_init ();
  loop = g_main_loop_new (NULL, FALSE);

  g_print ("Launching EchoObject\n");  

  /* Obtain a connection to the session bus */
  connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
  if (connection == NULL)
    die ("Failed to open connection to bus", error);

  obj = g_object_new (ECHO_TYPE, NULL);
  dbus_g_connection_register_g_object (connection,
                                       "/com/openedhand/DBus/Tests/Echo",
                                       obj);

  driver_proxy = dbus_g_proxy_new_for_name (connection,
                                            DBUS_SERVICE_DBUS,
                                            DBUS_PATH_DBUS,
                                            DBUS_INTERFACE_DBUS);

  if (!org_freedesktop_DBus_request_name (driver_proxy, ECHO_SERVICE_NAME,
					  0, &request_name_ret, &error))
    die ("Failed to get name", error);

  if (request_name_ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
    g_error ("Got result code %u from requesting name", request_name_ret);
    exit (1);
  }

  g_print ("GLib test service has name '%s'\n", ECHO_SERVICE_NAME);

  g_print ("GLib test service entering main loop\n");
  g_main_loop_run (loop);
  g_print ("Successfully completed %s\n", argv[0]);
  
  return 0;
}
#endif
