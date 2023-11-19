# decodebin

魔术般的构建一个解码流水线，通过自动插拔使用有效的 decoders 和 demuxers

它有一个 sink pad, 若干个 src_%u pad

详细设计参见 https://gstreamer.freedesktop.org/documentation/additional/design/decodebin.html?gi-language=c#decodebin-design


## FAQ
### not negotiated error

error details: gstbasetransform.c(1415): gst_base_transform_reconfigure (): /GstPipeline:rvsrp/GstDecodeBin:decodebin1/GstCapsFilter:capsfilter6:
not negotiated                        


## referende
* https://gstreamer.freedesktop.org/documentation/additional/design/index.html?gi-language=c
* https://gstreamer.freedesktop.org/documentation/additional/design/decodebin.html?gi-language=c