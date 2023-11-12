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


Overview
=======================================
Caps negotiation is the act of finding a media format (GstCaps) between elements that they can handle. This process in GStreamer can in most cases find an optimal solution for the complete pipeline. In this section we explain how this works.

Caps negotiation basics
=======================================
In GStreamer, negotiation of the media format always follows the following simple rules:

1. A downstream element suggest a format on its sinkpad and places the suggestion in the result of the CAPS query performed on the sinkpad. See also Implementing a CAPS query function.

2. An upstream element decides on a format. It sends the selected media format downstream on its source pad with a CAPS event. Downstream elements reconfigure themselves to handle the media type in the CAPS event on the sinkpad.

3. A downstream element can inform upstream that it would like to suggest a new format by sending a RECONFIGURE event upstream. The RECONFIGURE event simply instructs an upstream element to restart the negotiation phase. Because the element that sent out the RECONFIGURE event is now suggesting another format, the format in the pipeline might change.

In addition to the CAPS and RECONFIGURE event and the CAPS query, there is an ACCEPT_CAPS query to quickly check if a certain caps can be accepted by an element.



Caps negotiation use cases
===================================
In what follows we will look at some use cases for push-mode scheduling. The pull-mode scheduling negotiation phase is discussed in Pull-mode Caps negotiation and is actually similar as we will see.

Since the sink pads only suggest formats and the source pads need to decide, the most complicated work is done in the source pads. We can identify 3 caps negotiation use cases for the source pads:

1. Fixed negotiation. An element can output one format only. See Fixed negotiation.

2. Transform negotiation. There is a (fixed) transform between the input and output format of the element, usually based on some element property. The caps that the element will produce depend on the upstream caps and the caps that the element can accept depend on the downstream caps. See Transform negotiation.

3. Dynamic negotiation. An element can output many formats. See Dynamic negotiation.