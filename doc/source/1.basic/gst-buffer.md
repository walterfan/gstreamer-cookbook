# GstBuffer

A GstBuffer is the object that is passed from an upstream element to a downstream element and contains memory and metadata information.

## Requirements
* It must be fast
  * allocation, free, low fragmentation
* Must be able to attach multiple memory blocks to the buffer
* Must be able to attach arbitrary metadata to buffers
* efficient handling of subbuffer, copy, span, trim


## Reference
* https://gstreamer.freedesktop.org/documentation/additional/design/buffer.html?gi-language=c