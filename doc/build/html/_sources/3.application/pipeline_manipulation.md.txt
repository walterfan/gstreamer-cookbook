# pipeline manipulation

## core questions

* How to insert data from an application into a pipeline
* How to read data from a pipeline
* How to manipulate the pipeline's speed, length and starting point
* How to listen to a pipeline's data processing.

## Using probes

Probing is best envisioned as having access to a pad listener. Technically, a probe is nothing more than a callback that can be attached to a pad using gst_pad_add_probe (). Conversely, you can use gst_pad_remove_probe () to remove the callback. While attached, the probe notifies you of any activity on the pad. You can define what kind of notifications you are interested in when you add the probe.

### Probe types:

1. A buffer is pushed or pulled. 

You want to specify the `GST_PAD_PROBE_TYPE_BUFFER` when registering the probe. 

Because the pad can be scheduled in different ways. It is also possible to specify in what scheduling mode you are interested with the optional GST_PAD_PROBE_TYPE_PUSH and GST_PAD_PROBE_TYPE_PULL flags. You can use this probe to inspect, modify or drop the buffer. See Data probes.

2. A buffer list is pushed. 

Use the `GST_PAD_PROBE_TYPE_BUFFER_LIST` when registering the probe.

3. An event travels over a pad. 

Use the `GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM` and `GST_PAD_PROBE_TYPE_EVENT_UPSTREAM` flags to select downstream and upstream events. 
There is also a convenience `GST_PAD_PROBE_TYPE_EVENT_BOTH` to be notified of events going in both directions. 
By default, flush events do not cause a notification. You need to explicitly enable `GST_PAD_PROBE_TYPE_EVENT_FLUSH` to receive callbacks from flushing events. Events are always only notified in push mode. You can use this type of probe to inspect, modify or drop the event.

3. A query travels over a pad. 

Use the `GST_PAD_PROBE_TYPE_QUERY_DOWNSTREAM` and `GST_PAD_PROBE_TYPE_QUERY_UPSTREAM` flags to select downstream and upstream queries. The convenience `GST_PAD_PROBE_TYPE_QUERY_BOTH` can also be used to select both directions. 
Query probes are notified twice: when the **query travels upstream/downstream** and when the **query result is returned**. You can select in what stage the callback will be called with the `GST_PAD_PROBE_TYPE_PUSH` and `GST_PAD_PROBE_TYPE_PULL`, respectively when the query is performed and when the query result is returned.

You can use a query probe to inspect or modify queries, or even to answer them in the probe callback. To answer a query you place the result value in the query and return GST_PAD_PROBE_DROP from the callback.

4. block the data flow

In addition to notifying you of dataflow, you can also ask the probe to block the dataflow when the callback returns. This is called a blocking probe and is activated by specifying the `GST_PAD_PROBE_TYPE_BLOCK` flag. You can use this flag with the other flags to only block dataflow on selected activity. A pad becomes unblocked again if you remove the probe or when you return GST_PAD_PROBE_REMOVE from the callback. You can let only the currently blocked item pass by returning GST_PAD_PROBE_PASS from the callback, it will block again on the next item.

Blocking probes are used to temporarily block pads because they are unlinked or because you are going to unlink them. If the dataflow is not blocked, the pipeline would go into an error state if data is pushed on an unlinked pad. We will see how to use blocking probes to partially preroll a pipeline. See also Play a section of a media file.

5. check if not activity happening

Be notified when no activity is happening on a pad. You install this probe with the GST_PAD_PROBE_TYPE_IDLE flag. You can specify GST_PAD_PROBE_TYPE_PUSH and/or GST_PAD_PROBE_TYPE_PULL to only be notified depending on the pad scheduling mode. The IDLE probe is also a blocking probe in that it will not let any data pass on the pad for as long as the IDLE probe is installed.

You can use idle probes to dynamically relink a pad. We will see how to use idle probes to replace an element in the pipeline. See also Dynamically changing the pipeline.

## Reference
* https://gstreamer.freedesktop.org/documentation/application-development/advanced/pipeline-manipulation.html?gi-language=c