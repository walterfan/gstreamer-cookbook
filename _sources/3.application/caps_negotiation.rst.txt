######################
Caps negotiation
######################

.. include:: ../links.ref
.. include:: ../tags.ref
.. include:: ../abbrs.ref

============ ==========================
**Abstract** Caps negotiation
**Category** Learning note
**Authors**  Walter Fan
**Status**   WIP
**Updated**  |date|
============ ==========================

.. contents::
   :local:


Caps Negotiation 能力协商概述
=======================================
Caps negotiation is the act of finding a media format (GstCaps) between elements that they can handle. This process in GStreamer can in most cases find an optimal solution for the complete pipeline. 

能力协商是指在两个元素之间寻找一个彼此可以处理的媒体格式，在 GStreamer 中多数情况下是为了整个管道寻找一个优化的方案。

Caps negotiation basics
=======================================
In GStreamer, negotiation of the media format always follows the following simple rules:

在 GStreamer 中， 媒体格式协商总是遵循下面几条简单的规则：

1. A downstream element suggest a format on its sinkpad and places the suggestion in the result of the CAPS query performed on the sinkpad. See also Implementing a CAPS query function.

下游的元素在它的 sinkpad 上建议一个格式，并在 sinkpad 的 CAPS query 的结果放置这个建议


2. An upstream element decides on a format. It sends the selected media format downstream on its source pad with a CAPS event. Downstream elements reconfigure themselves to handle the media type in the CAPS event on the sinkpad.

上游的元素决定用哪种格式。它由其 source pad 发送所选择的媒体格式通过 CAPS event 到下游
下游的元素在其 sink pad 上重新配置以处理 CAPS event 中指定的媒体类型

3. A downstream element can inform upstream that it would like to suggest a new format by sending a RECONFIGURE event upstream. The RECONFIGURE event simply instructs an upstream element to restart the negotiation phase. Because the element that sent out the RECONFIGURE event is now suggesting another format, the format in the pipeline might change.

下游的元素可通知上游，即通过发送一个 RECONFIGURE event 到上游元素来建议一个新的格式
RECONFIGURE event 简单地指示上游的元素重启协商过程，因为发送 RECONFIGURE event 的元素要建议另外的格式，管道中的格式可能会改变。

In addition to the CAPS and RECONFIGURE event and the CAPS query, there is an ACCEPT_CAPS query to quickly check if a certain caps can be accepted by an element.

除了 CAPS 和 RECONFIGURE event, 还有 CAPS query, 还有 ACCEPT_CAPS query 来快速检查是否一个特定的 caps 可以被另外一个元素接受.


能力协商用例
===================================
In what follows we will look at some use cases for push-mode scheduling. The pull-mode scheduling negotiation phase is discussed in Pull-mode Caps negotiation and is actually similar as we will see.

我们先看看一些 push-mode 会商的例子, pull-mode 会商的例子后面再说，其实它们很类似。
sink pads 只建议格式, 而 source pads 需要做决定，最复杂的工作在 source pads 中完成。
我们可以识别 source pads 的 3 个能力协商的用例 

1. Fixed negotiation. An element can output one format only. See Fixed negotiation.

固定协商：一个元素只可以输出一种格式

2. Transform negotiation. There is a (fixed) transform between the input and output format of the element, usually based on some element property. The caps that the element will produce depend on the upstream caps and the caps that the element can accept depend on the downstream caps. See Transform negotiation.

转换协商：元素可以有一个输入与输出格式的固定转换，通常基于一些元素属性。元素将产生的能力取决于上游的能力, 而元素可接受的能力取决于下游的能力

3. Dynamic negotiation. An element can output many formats. ß
   
动态协商：一个元素可以输出许多格式

.. code-block:: 

              src              sink
               |                 |
               |  querycaps?     |
               |---------------->|
               |     caps        |
   select caps |< - - - - - - - -|
   from the    |                 |
   candidates  |                 |
               |                 |-.
               |  accepts?       | |
   type A      |---------------->| | optional
               |      yes        | |
               |< - - - - - - - -| |
               |                 |-'
               |  send_event()   |
   send CAP    |---------------->| Receive type A, reconfigure to
   event A     |                 | process type A.
               |                 |
               |  push           |
   push buffe  |---------------->| Process buffer of type A
               |                 |