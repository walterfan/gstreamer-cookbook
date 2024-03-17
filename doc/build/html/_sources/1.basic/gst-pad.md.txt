# GstPad Overview

Pad 是元件之间的连接器， 它有两个方向 GstPadDirection
* source pads 产生数据
* sink pads 消费数据

Pad 有三种有效性

* always: pad 在元件中永远存在 
* sometimes: pad 只在特定的条件下存在
* on request: pad 只在应用程序要求时才创建(存在)


## 连接 pad


### request pad

An element can also have request pads. These pads are not created automatically but are only created on demand. 

This is very useful for multiplexers, aggregators and tee elements. Aggregators are elements that merge the content of several input streams together into one output stream.

Tee elements are the reverse: they are elements that have one input stream and copy this stream to each of their output pads, which are created on request. 

Whenever an application needs another copy of the stream, it can simply request a new output pad from the tee element.

The following piece of code shows how you can request a new output pad from a “tee” element:


元件 还可以具有 request pad. 这些 pad 不会自动创建，而是仅根据需要创建。

这对于多路复用器 multiplexers、聚合器 aggregators 和 Tee 元件非常有用。 

聚合器是将多个输入流的内容合并到一个输出流中的元素。

Tee 元素则相反 -- 它是具有一个输入流的元素，并将该流复制到每个输出 pad（根据请求创建）。

每当应用程序需要流的另一个副本时，它可以简单地从 tee 元素请求新的 request pad。

下面的代码展示了如何从 `tee` 元件 请求新的 sink pad：

```c
static void
some_function (GstElement * tee)
{
  GstPad *pad;
  gchar *name;

  pad = gst_element_request_pad_simple (tee, "src%d");
  name = gst_pad_get_name (pad);
  g_print ("A new pad %s was created\n", name);
  g_free (name);

  /* here, you would link the pad */

  /* [..] */

  /* and, after doing that, free our reference */
  gst_object_unref (GST_OBJECT (pad));
}
```


`gst_element_request_pad_simple()` 方法可用于根据 pad 模板的名称从元素获取 pad。

还可以请求与其他 pad template 兼容的 pad。 如果您想要将一个元件链接到多路复用器元素并且需要请求兼容的 pad，那么这个方法非常有用。

方法 `gst_element_get_known_pad()` 可用于请求兼容的 pad ，如下一个示例所示。 它将从任何输入请求来自 Ogg 多路复用器的兼容 pad。

```c

static void
link_to_multiplexer (GstPad * tolink_pad, GstElement * mux)
{
  GstPad *pad;
  gchar *srcname, *sinkname;

  srcname = gst_pad_get_name (tolink_pad);
  pad = gst_element_get_compatible_pad (mux, tolink_pad, NULL);
  gst_pad_link (tolink_pad, pad);
  sinkname = gst_pad_get_name (pad);
  gst_object_unref (GST_OBJECT (pad));

  g_print ("A new pad %s was created and linked to %s\n", sinkname, srcname);
  g_free (sinkname);
  g_free (srcname);
}
```

注意 `gst_element_request_pad_simple` 只能以 GStreamer 1.20 及其以上版本才可使用，低版本的还是要使用 `gst_element_request_pad`

```c

GstPad *
gst_element_request_pad (GstElement * element,
                         GstPadTemplate * templ,
                         const gchar * name,
                         const GstCaps * caps)

```

Retrieves a request pad from the element according to the provided template. 
Pad templates can be looked up using `gst_element_factory_get_static_pad_templates`.

The pad should be released with gst_element_release_request_pad.

Parameters:

* element – a GstElement to find a request pad of.
* templ – a GstPadTemplate of which we want a pad of.
* name ( [transfer: none][allow-none]) – the name of the request GstPad to retrieve. Can be NULL.
* caps ( [transfer: none][allow-none]) – the caps of the pad we want to request. Can be NULL.

Returns ( [transfer: full][nullable]) – requested GstPad if found, otherwise NULL. Release after usage.

```c

   #include <gst/gst.h>

   GstElement *src;
   GstElementFactory *srcfactory;

   gst_init (&argc, &argv);

   srcfactory = gst_element_factory_find ("filesrc");
   g_return_if_fail (srcfactory != NULL);
   src = gst_element_factory_create (srcfactory, "src");
   g_return_if_fail (src != NULL);
   ...

```

## gstelementfactory

```c
gst_element_factory_get_static_pad_templates 
const GList *
gst_element_factory_get_static_pad_templates (GstElementFactory * factory)
Gets the GList of GstStaticPadTemplate for this factory.

//Parameters: factory – a GstElementFactory
  
//Returns ( [transfer: none][element-typeGst.StaticPadTemplate]) – the static pad templates
```


## GstPad
A GstElement is linked to other elements via "pads", which are extremely light-weight generic link points.

> GstPad 是 GstElement 之间的连接点

Pads have a GstPadDirection, source pads produce data, sink pads consume data.

> GstPad 是有方向的 GstPadDirection, source pads 产生数据， sink pads 消费数据

Pads are typically created from a GstPadTemplate with gst_pad_new_from_template and are then added to a GstElement. This usually happens when the element is created but it can also happen dynamically based on the data that the element is processing or based on the pads that the application requests.

> gst_pad_new_from_template 用来从 GstPadTemplate 创建 Pad, 它可以动态创建

Pads without pad templates can be created with gst_pad_new, which takes a direction and a name as an argument. If the name is NULL, then a guaranteed unique name will be assigned to it.

A GstElement creating a pad will typically use the various gst_pad_set_*_function() calls to register callbacks for events, queries or dataflow on the pads.

gst_pad_get_parent will retrieve the GstElement that owns the pad.

After two pads are retrieved from an element by gst_element_get_static_pad, the pads can be linked with gst_pad_link. (For quick links, you can also use gst_element_link, which will make the obvious link for you if it's straightforward.). Pads can be unlinked again with gst_pad_unlink. gst_pad_get_peer can be used to check what the pad is linked to.

Before dataflow is possible on the pads, they need to be activated with gst_pad_set_active.

gst_pad_query and gst_pad_peer_query can be used to query various properties of the pad and the stream.

To send a GstEvent on a pad, use gst_pad_send_event and gst_pad_push_event. Some events will be sticky on the pad, meaning that after they pass on the pad they can be queried later with gst_pad_get_sticky_event and gst_pad_sticky_events_foreach. gst_pad_get_current_caps and gst_pad_has_current_caps are convenience functions to query the current sticky CAPS event on a pad.

> gst_pad_send_event 和 gst_pad_push_event 可用来发送 GstEvent

GstElements will use gst_pad_push and gst_pad_pull_range to push out or pull in a buffer.

> gst_pad_push and gst_pad_pull_range 也可从推或拉数据

The dataflow, events and queries that happen on a pad can be monitored with probes that can be installed with gst_pad_add_probe. gst_pad_is_blocked can be used to check if a block probe is installed on the pad. gst_pad_is_blocking checks if the blocking probe is currently blocking the pad. gst_pad_remove_probe is used to remove a previously installed probe and unblock blocking probes if any.

> 在 pad 中经过的数据流，events 以及 queries 可通过 probes 来监测，方法是 gst_pad_add_probe

Pad have an offset that can be retrieved with gst_pad_get_offset. This offset will be applied to the running_time of all data passing over the pad. gst_pad_set_offset can be used to change the offset.

Convenience functions exist to start, pause and stop the task on a pad with gst_pad_start_task, gst_pad_pause_task and gst_pad_stop_task respectively.


## gst_pad_add_probe 
```
gulong
gst_pad_add_probe (GstPad * pad,
                   GstPadProbeType mask,
                   GstPadProbeCallback callback,
                   gpointer user_data,
                   GDestroyNotify destroy_data)
```

Be notified of different states of pads. The provided callback is called for every state that matches mask.

Probes are called in groups: First GST_PAD_PROBE_TYPE_BLOCK probes are called, then others, then finally GST_PAD_PROBE_TYPE_IDLE. The only exception here are GST_PAD_PROBE_TYPE_IDLE probes that are called immediately if the pad is already idle while calling gst_pad_add_probe. In each of the groups, probes are called in the order in which they were added.

### Parameters:

* pad – the GstPad to add the probe to
* mask – the probe mask
* callback – GstPadProbeCallback that will be called with notifications of the pad state
* user_data ( [closure]) – user data passed to the callback
* destroy_data – GDestroyNotify for user_data


GstPadProbeType 有如下定义

```
typedef enum
{
  GST_PAD_PROBE_TYPE_INVALID          = 0,
  /* flags to control blocking */
  GST_PAD_PROBE_TYPE_IDLE             = (1 << 0),
  GST_PAD_PROBE_TYPE_BLOCK            = (1 << 1),
  /* flags to select datatypes */
  GST_PAD_PROBE_TYPE_BUFFER           = (1 << 4),
  GST_PAD_PROBE_TYPE_BUFFER_LIST      = (1 << 5),
  GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM = (1 << 6),
  GST_PAD_PROBE_TYPE_EVENT_UPSTREAM   = (1 << 7),
  GST_PAD_PROBE_TYPE_EVENT_FLUSH      = (1 << 8),
  GST_PAD_PROBE_TYPE_QUERY_DOWNSTREAM = (1 << 9),
  GST_PAD_PROBE_TYPE_QUERY_UPSTREAM   = (1 << 10),
  /* flags to select scheduling mode */
  GST_PAD_PROBE_TYPE_PUSH             = (1 << 12),
  GST_PAD_PROBE_TYPE_PULL             = (1 << 13),

  /* flag combinations */
  GST_PAD_PROBE_TYPE_BLOCKING         = GST_PAD_PROBE_TYPE_IDLE | GST_PAD_PROBE_TYPE_BLOCK,
  GST_PAD_PROBE_TYPE_DATA_DOWNSTREAM  = GST_PAD_PROBE_TYPE_BUFFER | GST_PAD_PROBE_TYPE_BUFFER_LIST | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM,
  GST_PAD_PROBE_TYPE_DATA_UPSTREAM    = GST_PAD_PROBE_TYPE_EVENT_UPSTREAM,
  GST_PAD_PROBE_TYPE_DATA_BOTH        = GST_PAD_PROBE_TYPE_DATA_DOWNSTREAM | GST_PAD_PROBE_TYPE_DATA_UPSTREAM,
  GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM = GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_DATA_DOWNSTREAM,
  GST_PAD_PROBE_TYPE_BLOCK_UPSTREAM   = GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_DATA_UPSTREAM,
  GST_PAD_PROBE_TYPE_EVENT_BOTH       = GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM | GST_PAD_PROBE_TYPE_EVENT_UPSTREAM,
  GST_PAD_PROBE_TYPE_QUERY_BOTH       = GST_PAD_PROBE_TYPE_QUERY_DOWNSTREAM | GST_PAD_PROBE_TYPE_QUERY_UPSTREAM,
  GST_PAD_PROBE_TYPE_ALL_BOTH         = GST_PAD_PROBE_TYPE_DATA_BOTH | GST_PAD_PROBE_TYPE_QUERY_BOTH,
  GST_PAD_PROBE_TYPE_SCHEDULING       = GST_PAD_PROBE_TYPE_PUSH | GST_PAD_PROBE_TYPE_PULL
} GstPadProbeType;

```

## GstPadTemplate

Padtemplates describe the possible media types a pad or an elementfactory can handle. This allows for both inspection of handled types before loading the element plugin as well as identifying pads on elements that are not yet created (request or sometimes pads).

Pad and PadTemplates have GstCaps attached to it to describe the media type they are capable of dealing with. gst_pad_template_get_caps or GST_PAD_TEMPLATE_CAPS are used to get the caps of a padtemplate. It's not possible to modify the caps of a padtemplate after creation.

PadTemplates have a GstPadPresence property which identifies the lifetime of the pad and that can be retrieved with GST_PAD_TEMPLATE_PRESENCE. Also the direction of the pad can be retrieved from the GstPadTemplate with GST_PAD_TEMPLATE_DIRECTION.

The `GST_PAD_TEMPLATE_NAME_TEMPLATE()` is important for GST_PAD_REQUEST pads because it has to be used as the name in the gst_element_request_pad_simple call to instantiate a pad from this template.

Padtemplates can be created with `gst_pad_template_new` or with `gst_static_pad_template_get()`, which creates a GstPadTemplate from a GstStaticPadTemplate that can be filled with the convenient GST_STATIC_PAD_TEMPLATE macro.

A padtemplate can be used to create a pad (see gst_pad_new_from_template or gst_pad_new_from_static_template ()) or to add to an element class (see gst_element_class_add_static_pad_template ()).

The following code example shows the code to create a pad from a padtemplate.

```c
   GstStaticPadTemplate my_template =
   GST_STATIC_PAD_TEMPLATE (
     "sink",          // the name of the pad
     GST_PAD_SINK,    // the direction of the pad
     GST_PAD_ALWAYS,  // when this pad will be present
     GST_STATIC_CAPS (        // the capabilities of the padtemplate
       "audio/x-raw, "
         "channels = (int) [ 1, 6 ]"
     )
   );
   void
   my_method (void)
   {
     GstPad *pad;
     pad = gst_pad_new_from_static_template (&my_template, "sink");
     ...
   }
```

The following example shows you how to add the padtemplate to an element class, this is usually done in the class_init of the class:

```c
   static void
   my_element_class_init (GstMyElementClass *klass)
   {
     GstElementClass *gstelement_class = GST_ELEMENT_CLASS (klass);

     gst_element_class_add_static_pad_template (gstelement_class, &my_template);
   }
```

### GstStaticPadTemplate 
It is Structure describing the GstStaticPadTemplate.

Members:

* name_template (const gchar *) – the name of the template
* direction (GstPadDirection) – the direction of the template
* presence (GstPadPresence) – the presence of the template
* static_caps (GstStaticCaps) – the caps of the template.


## Reference
* https://gstreamer.freedesktop.org/documentation/application-development/basics/pads.html?gi-language=c
* https://gstreamer.freedesktop.org/documentation/plugin-development/advanced/request.html?gi-language=c