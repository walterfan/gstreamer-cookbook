# pipeline manipulation

管道的操作有多种方法, 例如

* 向管道中插入数据
* 从管道中读取数据
* 控制管道的速率, 长度和起始点
* 侦听管道的数据处理

## 问题

* 如何将数据从应用程序插入到管道中
* 如何从管道中读取数据
* 如何操纵管道的速度、长度和起点
* 如何监听管道的数据处理。

## 使用探针

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



## 动态修改管道

当管道处在 "PLAYING" 状态时, 如何在不打断数据流的前提下修改管道呢?

以下事项需要重点注意

* insertbin 和 switchbin 元素可用于管道的动态修改
* 当要从管道中移除元件时, 要确保断开连接的 pads 上没有数据流, 否则会引发严重的管道错误. 
在断开连接前要阻塞 source pad(在 push 模式下) 或者 sink pads(拉模式)

* 当添加元件到管道中, 确保在允许数据流通前设置元件为正确的状态, 通常和父元件保持相同状态
当一个元件新建出来, 它处于 "NULL" 状态, 当它收到数据时会抛出错误

* 当添加元件到管道中, GStreamer 会在这个元件上设置时钟 clock 和 基准时间 base-time 为管道的当前值.
它意味着

## Reference
* https://gstreamer.freedesktop.org/documentation/application-development/advanced/pipeline-manipulation.html?gi-language=c