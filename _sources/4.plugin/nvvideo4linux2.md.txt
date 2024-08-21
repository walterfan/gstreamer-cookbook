# nvvideo4linux2

DeepStream extends the open source V4L2 codec plugins (here called Gst-v4l2) to support hardware-accelerated codecs.

![nvvideo4linux2](nvvideo4linux2.png)

## Decoder

The OSS Gst-nvvideo4linux2 plugin leverages the hardware decoding engines on Jetson and DGPU platforms by interfacing with libv4l2 plugins on those platforms.

It supports H.264, H.265, AV1, JPEG and MJPEG formats.

The plugin accepts an encoded bitstream and uses the NVDEC hardware engine to decode the bitstream. The decoded output can be NV12 or YUV444 format which depends on the encoded stream content.


## Inputs

- An encoded bitstream. Supported formats are H.264, H.265, AV1, JPEG and MJPEG

- H264 encoded bitstream in 8bit 4:2:0 format.

- H265 encoded bitstream in 8/10/12 bit 4:2:0 and 8/10/12 bit 4:4:4 format.

- AV1 encoded bitstream in 8bit 4:2:0 format.

## Output

- H264 decoder outputs GstBuffer in 8bit semi-planar(NV12) 4:2:0 format.

- H265 decoder outputs GstBuffer in 8/10/12 bit semi-planar(NV12) 4:2:0 format, 8/10/12 bit planar(YUV444) 4:4:4 format.

- AV1 decoder outputs GstBuffer in 8bit semi-planar(NV12) 4:2:0 format.


## Reference

* https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvvideo4linux2.html