```{contents} Table of Contents
:depth: 3
```

# glib Overview

GLib is a general-purpose, portable utility library, which provides many useful data types, macros, type conversions, string utilities, file utilities, a mainloop abstraction, and so on.


## g_signal_connect

将信号绑定于回调函数

refer to https://docs.gtk.org/gobject/func.signal_connect.html

### Declaration
```
#define g_signal_connect (
  instance,
  detailed_signal,
  c_handler,
  data
)
```

### Description

Connects a GCallback function to a signal for a particular object.

The handler will be called synchronously, before the default handler of the signal. g_signal_emit() will not return control until all handlers are called.

See [memory management of signal handlers][signal-memory-management] for details on how to handle the return value and memory management of data.

This function is not directly available to language bindings.

## g_timeout_add
refer to https://docs.gtk.org/glib/func.timeout_add.html

### Declaration

```
guint
g_timeout_add (
  guint interval,
  GSourceFunc function,
  gpointer data
)
```


### Description

Sets a function to be called at regular intervals, with the default priority, G_PRIORITY_DEFAULT.

The given function is called repeatedly until it returns G_SOURCE_REMOVE or FALSE, at which point the timeout is automatically destroyed and the function will not be called again. The first call to the function will be at the end of the first interval.

Note that timeout functions may be delayed, due to the processing of other event sources. Thus they should not be relied on for precise timing. After each call to the timeout function, the time of the next timeout is recalculated based on the current time and the given interval (it does not try to ‘catch up’ time lost in delays).

See [memory management of sources][mainloop-memory-management] for details on how to handle the return value and memory management of data.

If you want to have a timer in the “seconds” range and do not care about the exact time of the first call of the timer, use the g_timeout_add_seconds() function; this function allows for more optimizations and more efficient system power usage.

This internally creates a main loop source using g_timeout_source_new() and attaches it to the global GMainContext using g_source_attach(), so the callback will be invoked in whichever thread is running that main context. You can do these steps manually if you need greater control or to use a custom main context.

It is safe to call this function from any thread.

The interval given is in terms of monotonic time, not wall clock time. See g_get_monotonic_time().

This function is not directly available to language bindings.

The implementation of this method is provided by g_timeout_add_full() in language bindings


## g_source_remove
### Declaration
```
gboolean
g_source_remove (
  guint tag
)
```

### Description

Removes the source with the given ID from the default main context. You must use g_source_destroy() for sources added to a non-default main context.

The ID of a GSource is given by g_source_get_id(), or will be returned by the functions g_source_attach(), g_idle_add(), g_idle_add_full(), g_timeout_add(), g_timeout_add_full(), g_child_watch_add(), g_child_watch_add_full(), g_io_add_watch(), and g_io_add_watch_full().

It is a programmer error to attempt to remove a non-existent source.

More specifically: source IDs can be reissued after a source has been destroyed and therefore it is never valid to use this function with a source ID which may have already been removed. An example is when scheduling an idle to run in another thread with g_idle_add(): the idle may already have run and been removed by the time this function is called on its (now invalid) source ID. This source ID may have been reissued, leading to the operation being performed against the wrong source.
