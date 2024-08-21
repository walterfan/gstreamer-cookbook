# nvstreammux

The Gst-nvstreammux plugin forms a batch of frames from multiple input sources.
它从多个输入源形成对于多个视频帧的批处理。

When connecting a source to nvstreammux (the muxer), a new pad must be requested from the muxer using gst_element_get_request_pad() and the pad template sink_%u.

当将一个源连接到 nvstreammux（muxer）时，必须使用 gst_element_get_request_pad() 从muxer 请求一个新的pad，并使用pad模板sink_%u。


For more information, see link_element_to_streammux_sink_pad() in the DeepStream app source code.
The muxer forms a batched buffer of batch-size frames. (batch-size is specified using the gst object property.)



If the muxer’s output format and input format are the same, the muxer forwards the frames from that source as a part of the muxer’s output batched buffer.

The frames are returned to the source when muxer gets back its output buffer. If the resolution is not the same, the muxer scales frames from the input into the batched buffer and then returns the input buffers to the upstream component.

The muxer pushes the batch downstream when the batch is filled, or the batch formation timeout batched-pushed-timeout is reached. The timeout starts running when the first buffer for a new batch is collected.

The muxer uses a round-robin algorithm to collect frames from the sources. It tries to collect an average of (batch-size/num-source) frames per batch from each source (if all sources are live and their frame rates are all the same). The number varies for each source, though, depending on the sources’ frame rates.

The muxer outputs a single resolution (i.e. all frames in the batch have the same resolution). This resolution can be specified using the width and height properties. The muxer scales all input frames to this resolution.

The enable-padding property can be set to true to preserve the input aspect ratio while scaling by padding with black bands.

For DGPU platforms, the GPU to use for scaling and memory allocations can be specified with the gpu-id property. For each source that needs scaling to the muxer’s output resolution, the muxer creates a buffer pool and allocates four buffers each of size:

```
output width*output height*f
```


## Reference
* https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvstreammux.html