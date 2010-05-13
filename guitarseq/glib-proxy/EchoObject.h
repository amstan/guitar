/*
 * Author: Ross Burton <ross@openedhand.com>
 * Copyright (C) 2005 Opened Hand Ltd
 */

#include <glib-object.h>

typedef struct Echo Echo;
typedef struct EchoClass EchoClass;

GType echo_get_type (void);

struct Echo
{
  GObject parent;
};

struct EchoClass
{
  GObjectClass parent;
};

#define ECHO_TYPE              (echo_get_type ())
#define ECHO(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), ECHO_TYPE, Echo))
#define ECHO_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), ECHO_TYPE, EchoClass))
#define IS_ECHO(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), ECHO_TYPE))
#define IS_ECHO_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), ECHO_TYPE))
#define ECHO_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), ECHO_TYPE, EchoClass))

typedef enum
{
  ECHO_ERROR_GENERIC
} EchoError;

GQuark echo_error_quark (void);
#define ECHO_ERROR echo_error_quark ()

/**
 * Take @string, reverse it, and return it as @echo_string.
 */
gboolean echo_echo(Echo *echo, const char* string, char **echo_string, GError **error);
