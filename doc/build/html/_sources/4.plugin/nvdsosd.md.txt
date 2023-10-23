# nvdsosd plugin
## Overview

[nvdsosd 插件](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvdsosd.html)绘制边界框、文本、箭头、线条、圆形和感兴趣区域 (RoI) 多边形。 （多边形显示为一组线。）

该插件在 CPU 处理模式下接受来自上游组件的带有附加元数据的 RGBA 缓冲区。 当 GPU 处理模式时，它可以接受 RGBA/NV12 PL/NV12 BL 缓冲区。 

它绘制边界框，边界框可能会根据给定边界框的配置（例如宽度、颜色和不透明度）进行着色。 它还在帧中的指定位置绘制文本和 RoI 多边形。 文本和多边形参数可通过元数据进行配置。

It can accepts RGBA/NV12 PL/NV12 BL buffer when GPU process mode. It draws bounding boxes, which may be shaded depending on the configuration (e.g. width, color, and opacity) of a given bounding box. It also draws text and RoI polygons at specified locations in the frame. Text and polygon parameters are configurable through metadata.

当 GPU 处理模式时，它可以接受 RGBA/NV12 PL/NV12 BL 缓冲区。 它绘制边界框，边界框可能会根据给定边界框的配置（例如宽度、颜色和不透明度）进行着色。 它还在帧中的指定位置绘制文本和 RoI 多边形。 文本和多边形参数可通过元数据进行配置。

## Reference
https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvdsosd.html
