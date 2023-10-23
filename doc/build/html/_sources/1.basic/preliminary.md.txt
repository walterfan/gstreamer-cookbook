# GStreamer 预备知识

GStreamer 采用了 GObject 编程模型，有关 GObject 和 glib 的编程知识需要了解

1. GObject instantiation

2. GObject properties (set/get)

3. GObject casting

4. GObject referencing/dereferencing

5. glib memory management

6. glib signals and callbacks

7. glib main loop


推荐阅读
* [GObject reference manual](https://developer-old.gnome.org/gobject/stable/)

## Basic Types
### Description
GLib defines a number of commonly used types, which can be divided into several groups:

New types which are not part of standard C (but are defined in various C standard library header files) — gboolean, gssize.

* Integer types which are guaranteed to be the same size across all platforms — gint8, guint8, gint16, guint16, gint32, guint32, gint64, guint64.

* Types which are easier to use than their standard C counterparts - gpointer, gconstpointer, guchar, guint, gushort, gulong.

* Types which correspond exactly to standard C types, but are included for completeness — gchar, gint, gshort, glong, gfloat, gdouble.

* Types which correspond exactly to standard C99 types, but are available to use even if your compiler does not support C99 — gsize, goffset, gintptr, guintptr.

GLib also defines macros for the limits of some of the standard integer and floating point types, as well as macros for suitable printf() formats for these types.

Note that depending on the platform and build configuration, the format macros might not be compatible with the system provided printf() function, because GLib might use a different printf() implementation internally. The format macros will always work with GLib API (like g_print()), and with any C99 compatible printf() implementation.

## GLib Core Application Support

* The Main Event Loop — manages all available sources of events
* Threads — portable support for threads, mutexes, locks, conditions and thread private data
* Thread Pools — pools of threads to execute work concurrently
* Asynchronous Queues — asynchronous communication between threads
* Dynamic Loading of Modules — portable method for dynamically loading 'plug-ins'
* Memory Allocation — general memory-handling
* Memory Slices — efficient way to allocate groups of equal-sized chunks of memory
* IO Channels — portable support for using files, pipes and sockets
* Error Reporting — a system for reporting errors
* Warnings and Assertions — warnings and assertions to use in runtime code
* Message Output and Debugging Functions — functions to output messages and help debug applications