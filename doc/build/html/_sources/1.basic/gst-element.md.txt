# gst element overview



## gst_element_get_static_pad
通过 name 获取一个已经存在的 element

```c
GstPad *
gst_element_get_static_pad (GstElement * element,
                            const gchar * name)
```
Retrieves a pad from element by name. 

This version only retrieves already-existing (i.e. 'static') pads.