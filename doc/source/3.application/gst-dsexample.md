# gst-dsexample

The GStreamer example plugin (gst-dsexample) demonstrates the following:

* Processing the entire frame, with downscaling / color conversion if required.
  处理整个视频帧, 包括所需的大小和色彩转换

* Processing objects detected by the Primary Detector, specifically, cropping these objects from the frame and then processing the crops.
  处理被主检测器检测到的对象, 尤其是从视频帧中裁剪这些对象, 并处理这些裁剪过的片段

* In-place modification of the buffer frame contents using OpenCV
  通过 opencv 就地修改缓冲区的视频帧内容

* Two versions of the plugin are included. Refer to the plugin’s Makefile and README to switch between them
  这里包括 两个版本的 plugin

  - Simple (gstdsexample.cpp) - Sequential pre-processing and processing
    简单版本

  - Optimized (gstdsexample_optimized.cpp) - Parallel batch pre-processing and processing
    优化版本

This release includes a simple static library dsexample_lib that demonstrates the interface between custom libraries and this Gstreamer plugin. The library generates simple labels of the form “Obj_label”. The library implements these functions:

这个 plugin 包括以下这些函数

- DsExampleCtxInit—Initalizes the custom library

- DsExampleCtxDeinit—De-initalizes the custom library

- DsExampleProcess – Process on an input frame

The GStreamer plugin itself is a standard in-place transform plugin. Because it does not generate new buffers but only adds / updates existing metadata, the plugin implements an in-place transform. Some of the code is standard GStreamer plugin boilerplate (e.g. plugin_init, class_init, instance_init). Other functions of interest are as follows:


## GstBaseTransfrom Class Functions
* start—Acquires resources, allocate memory, initialize example library.

* stop—De-initializes the example library and frees up resources and memory.

* set_caps—Gets the capabilities of the video (i.e. resolution, color format, framerate) that flow through this element. Allocations / initializations that depend on input video format can be done here.

* transform_ip—Implemented in the simple version. Called when the plugin receives a buffer from upstream element.

* Finds the metadata of the primary detector.

* Use get_converted_mat to pre-process frame/object crop to get the required buffer for pushing to library. Push the data to the example library. Pop the example library output.

* Attach / update metadata using attach_metadata_full_frame or attach_metadata_object.

* Alternatively, modify frame contents in-place to blur objects using blur_objects.

* submit_input_buffer — Implemented in the optimized version. Called when the plugin receives a buffer from upstream element. Works in parallel with gst_dsexample_output_loop to improve performance.

* Finds the metadata of the primary detector.

* Create a batch of frames/objects to pre-process. Pre-process the batches and push the pre-processed output to the processing thread.

* Pre-process on the next batch while the processing thread works on an older batch.