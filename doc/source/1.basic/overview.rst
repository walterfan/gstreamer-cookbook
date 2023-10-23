######################
GStreamer 概论
######################

.. include:: ../links.ref
.. include:: ../tags.ref
.. include:: ../abbrs.ref

============ ==========================
**Abstract** GStreamer 概论
**Authors**  Walter Fan
**Status**   WIP as draft
**Updated**  |date|
============ ==========================

.. contents::
   :local:


DeepStream
===================
DeepStream是NVIDIA基于GStreamer的插件系统开发的，继承了GStreamer的特性。DeepSream提供超过15种以上的GStreamer插件模块，以此建造一个高效的视频分析管道（Pipeline）。

NVIDIA将TensorRT, cuDNN，CUDA, Video SDK等以插件的形式集成进GStreamer当中，以管道的形式进行智能视频分析应用的开发，将各个功能封装成组件，通过将对应功能的组件插入管道中，启动管道使数据按照要求在管道内流动，数据经过解析、编解码、预处理、算法处理后进行图像渲染或者发送到云端。

例如

* nvv4l2decoder 用于调用GPU硬件加速解码h264文件 （解码帧的格式是NV12（YUV420））
* nvstreammux 用于把输入按照参数处理成一系列的视频帧 
* nvinfer 通过推理的配置文件对输入图像进行推理 
* nvvideoconvert 视频颜色格式转换 
* nvdsosd 处理RGBA buffer 绘制ROI等 识别对象的Bounding Box，边框， 以及识别对象的文字标签（字体、颜色、标示框） 
* nvegltransform 转换成 EGLImage instance 给nveglglessink使用
* nveglglessink 渲染