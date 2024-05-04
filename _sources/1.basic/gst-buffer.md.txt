# GstBuffer

A GstBuffer is the object that is passed from an upstream element to a downstream element and contains memory and metadata information.


## Members
* mini_object (GstMiniObject) – the parent structure 
父结构类型

* pool (GstBufferPool *) – pointer to the pool owner of the buffer 
缓冲池的指针

* pts (GstClockTime) – presentation timestamp of the buffer, can be GST_CLOCK_TIME_NONE when the pts is not known or relevant. The pts contains the timestamp when the media should be presented to the user.

缓冲数据的播放时间戳，当不知道或者无关时为GST_CLOCK_TIME_NONE
pts 包含了缓冲数据应该被播放的时间。

* dts (GstClockTime) – decoding timestamp of the buffer, can be GST_CLOCK_TIME_NONE when the dts is not known or relevant. The dts contains the timestamp when the media should be processed.

缓冲数据的解码时间戳，如果不知道或无关，则为GST_CLOCK_TIME_NONE。
dts 包含了缓冲数据应该被处理的时间。

* duration (GstClockTime) – duration in time of the buffer data, can be GST_CLOCK_TIME_NONE when the duration is not known or relevant.
缓冲区数据的时长，如果不知道或无关，则为GST_CLOCK_TIME_NONE。

* offset (guint64) – a media specific offset for the buffer data. For video frames, this is the frame number of this buffer. For audio samples, this is the offset of the first sample in this buffer. For file data or compressed data this is the byte offset of the first byte in this buffer.

对于缓冲数据的媒体的特定的位移
 - 对于视频帧，这是缓冲区数据的帧号。
 - 对于音频采样, 这是缓冲区数据的第一个采样的位移。
 - 对于文件数据或者压缩数据，这是缓冲区数据的第一个字节的位

* offset_end (guint64) – the last offset contained in this buffer. It has the same format as offset.

缓冲区数据的最后位移，与offset相同格式。

## Requirements

* It must be fast
  * allocation, free, low fragmentation
* Must be able to attach multiple memory blocks to the buffer
* Must be able to attach arbitrary metadata to buffers
* efficient handling of subbuffer, copy, span, trim


## Reference
* https://gstreamer.freedesktop.org/documentation/additional/design/buffer.html?gi-language=c