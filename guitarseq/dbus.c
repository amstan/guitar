#include <dbus/dbus-glib.h>
#include <glib-object.h>
#include <stdio.h>

#include "dbus.h"

DBusGConnection *dbus_connection;
GError *dbus_error;

int dbus_list (void)
{
	DBusGProxy *proxy;
	char **name_list;
	char **name_list_ptr;
	
	/* Create a proxy object for the "bus driver" (name "org.freedesktop.DBus") */
	
	proxy = dbus_g_proxy_new_for_name (dbus_connection, DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);
	
	/* Call ListNames method, wait for reply */
	dbus_error = NULL;
	if (!dbus_g_proxy_call (proxy, "ListNames", &dbus_error, G_TYPE_INVALID,G_TYPE_STRV, &name_list, G_TYPE_INVALID)) {
		/* Just do demonstrate remote exceptions versus regular GError */
		if (dbus_error->domain == DBUS_GERROR && dbus_error->code == DBUS_GERROR_REMOTE_EXCEPTION)
			g_printerr ("Caught remote method exception %s: %s", dbus_g_error_get_name (dbus_error), dbus_error->message);
		else
			g_printerr ("Error: %s\n", dbus_error->message);
		g_error_free (dbus_error);
		return 1;
	}
	
	/* Print the results */
	g_print ("Names on the message bus:\n");
	for (name_list_ptr = name_list; *name_list_ptr; name_list_ptr++) {
		g_print ("  %s\n", *name_list_ptr);
	}
	g_strfreev (name_list);
	
	g_object_unref (proxy);
	
	return 0;
}

int dbus_init() {
	g_type_init ();
	
	dbus_error = NULL;
	dbus_connection = dbus_g_bus_get (DBUS_BUS_SESSION, &dbus_error);
	if (dbus_connection == NULL) {
		g_printerr ("Failed to open connection to bus: %s\n", dbus_error->message);
		g_error_free (dbus_error);
		return 1;
	}
	fprintf(stdout,"DBus connection established.\n");
	
	//dbus_list();
	dbus_g_object_type_install_info (NULL, dbus_glib_guitarseq_object_info);
	
	return 0;
}