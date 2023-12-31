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

Overview
=======================================

GStreamer 是一个用于创建流媒体应用程序的框架。 基本设计来自俄勒冈研究生院的视频管道 video pipline, 以及 DirectShow 的一些想法。

GStreamer的开发框架使得编写任何类型的流媒体应用程序成为可能。 GStreamer 框架旨在使编写处理音频或视频或两者的应用程序变得容易。 它不限于音频和视频, 可以处理任何类型的数据流。 管道设计的开销几乎不高于所应用的过滤器引起的开销。 这使得 GStreamer 成为设计对延迟有很高要求的高端音频应用程序的良好框架。

GStreamer 最明显的用途之一是用它来构建媒体播放器。 GStreamer 已经包含用于构建媒体播放器的组件, 该播放器可以支持多种格式, 包括 MP3、Ogg/Vorbis、MPEG-1/2、AVI、Quicktime、mod 等。 然而, GStreamer 不仅仅是另一个媒体播放器。 它的主要优点是可插入组件可以混合并匹配到任意管道中, 以便可以编写成熟的视频或音频编辑应用程序。

该框架基于提供各种编解码器和其他功能的插件。 插件可以链接并排列在管道中。 该管道定义了数据的流向。 还可以使用 GUI 编辑器编辑管道并将其保存为 XML, 以便可以轻松地创建管道库。

GStreamer 的核心功能是为插件、数据流和媒体类型处理/协商提供框架。 它还提供了一个 API 来使用各种插件编写应用程序。


具体来说, GStreamer 提供


* 一套用于多媒体应用程序的 API
* 一套插件架构
* 一套管道架构
* 一套媒体类型处理和协商的机制
* 一套同步的机制
* 超过 250 个以上的插件提供了上千个元素
* 一套命令行工具

GStreamer 插件可被分类为
---------------------------------------------------
1. 协议处理 protocols handling
2. 媒体源: 音频和视频源 (涉及协议插件)
3. 媒体格式: 解析器、格式化器、复用器、解复用器、元数据、字幕
4. 编解码器: 编码器和解码器
5. 滤波器: 转换器、混音器、效果器...
6. 接收器: 用于音频和视频 (涉及协议插件)


.. image:: ../_static/gstreamer-overview.png

GStreamer 被封装成
---------------------------------------------------
* gstreamer: 核心包
* gst-plugins-base: 一组重要的示例元素
* gst-plugins-good: LGPL下的一组优质插件
* gst-plugins-ugly: 一组可能会造成分发问题的优质插件
* gst-plugins-bad: 一组需要更高质量的插件
* gst-libav: 一组封装libav进行解码和编码的插件
* 其他一些包

