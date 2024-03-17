# Gstreamer Debug


To enable debug output, set the GST_DEBUG environment variable to the desired debug level. All levels below that will also be shown (i.e., if you set `GST_DEBUG=2`, you will get both ERROR and WARNING messages).

Furthermore, each plugin or part of the GStreamer defines its own category, so you can specify a debug level for each individual category. For example, `GST_DEBUG=2,audiotestsrc:6`, will use Debug Level 6 for the audiotestsrc element, and 2 for all the others.

```sh
export GST_DEBUG=<some element name>:<1-6?>,<some otherelement>:<1-6>
```

如果不太清楚问题出在哪儿，就启用 debug

```sh
export GST_DEBUG=*:3 
```

## Debug Level

| # | Name    | Description                                                    |
|---|---------|----------------------------------------------------------------|
| 0 | none    | No debug information is output.                                |
| 1 | ERROR   | Logs all fatal errors. These are errors that do not allow the  |
|   |         | core or elements to perform the requested action. The          |
|   |         | application can still recover if programmed to handle the      |
|   |         | conditions that triggered the error.                           |
| 2 | WARNING | Logs all warnings. Typically these are non-fatal, but          |
|   |         | user-visible problems are expected to happen.                  |
| 3 | FIXME   | Logs all "fixme" messages. Those typically that a codepath that|
|   |         | is known to be incomplete has been triggered. It may work in   |
|   |         | most cases, but may cause problems in specific instances.      |
| 4 | INFO    | Logs all informational messages. These are typically used for  |
|   |         | events in the system that only happen once, or are important   |
|   |         | and rare enough to be logged at this level.                    |
| 5 | DEBUG   | Logs all debug messages. These are general debug messages for  |
|   |         | events that happen only a limited number of times during an    |
|   |         | object's lifetime; these include setup, teardown, change of    |
|   |         | parameters, etc.                                               |
| 6 | LOG     | Logs all log messages. These are messages for events that      |
|   |         | happen repeatedly during an object's lifetime; these include   |
|   |         | streaming and steady-state conditions. This is used for log    |
|   |         | messages that happen on every buffer in an element for example.|
| 7 | TRACE   | Logs all trace messages. Those are message that happen very    |
|   |         | very often. This is for example is each time the reference     |
|   |         | count of a GstMiniObject, such as a GstBuffer or GstEvent, is  |
|   |         | modified.                                                      |
| 9 | MEMDUMP | Logs all memory dump messages. This is the heaviest logging and|
|   |         | may include dumping the content of blocks of memory.           |
+------------------------------------------------------------------------------+


## Conclusion

* How to get more debug information from GStreamer

  - using the GST_DEBUG environment variable.

* How to print your own debug information into the GStreamer log?

 - with the GST_ERROR() macro and relatives.

* How to get pipeline graphs?

 - with the GST_DEBUG_DUMP_DOT_DIR environment variable.
