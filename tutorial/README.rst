Basic tutorial
======================

Basic tutorial 1: Hello world! – The mandatory 'Hello world' example
---------------------------------------------------------------------------------------


Basic tutorial 2: GStreamer concepts – Gstreamer Elements, Pipeline and the Bus
---------------------------------------------------------------------------------------

Basic tutorial 3: Dynamic pipelines
---------------------------------------------------------------------------------------
Goal
~~~~~~~~~
This tutorial shows the rest of the basic concepts required to use GStreamer, which allow building the pipeline "on the fly", as information becomes available, instead of having a monolithic pipeline defined at the beginning of your application.

After this tutorial, you will have the necessary knowledge to start the Playback tutorials. The points reviewed here will be:

* How to attain finer control when linking elements.

* How to be notified of interesting events so you can react in time.

The various states in which an element can be.

Basic tutorial 4: Time management
---------------------------------------------------------------------------------------

Goal
~~~~~~~~~~
This tutorial shows how to use GStreamer time-related facilities. In particular:

* How to query the pipeline for information like stream position or duration.
* How to seek (jump) to a different position (time) inside the stream.


Basic tutorial 5: GUI toolkit integration
---------------------------------------------------------------------------------------

Basic tutorial 6: Media formats and Pad Capabilities
---------------------------------------------------------------------------------------

Basic tutorial 7: Multithreading and Pad Availability
---------------------------------------------------------------------------------------

Basic tutorial 8: Short-cutting the pipeline
---------------------------------------------------------------------------------------
Goal
~~~~~~~~
Pipelines constructed with GStreamer do not need to be completely closed. Data can be injected into the pipeline and extracted from it at any time, in a variety of ways. This tutorial shows:

* How to inject external data into a general GStreamer pipeline.

* How to extract data from a general GStreamer pipeline.

* How to access and manipulate this data.

Playback tutorial 3: Short-cutting the pipeline explains how to achieve the same goals in a playbin-based pipeline.




Basic tutorial 9: Media information gathering
---------------------------------------------------------------------------------------

Basic tutorial 10: GStreamer tools
---------------------------------------------------------------------------------------

Basic tutorial 11: Debugging tools
---------------------------------------------------------------------------------------

Basic tutorial 12: Streaming
---------------------------------------------------------------------------------------
Goal
~~~~~~~~
Playing media straight from the Internet without storing it locally is known as Streaming. We have been doing it throughout the tutorials whenever we used a URI starting with http://. This tutorial shows a couple of additional points to keep in mind when streaming. In particular:

* How to enable buffering (to alleviate network problems)
* How to recover from interruptions (lost clock)



Basic tutorial 13: Playback speed
---------------------------------------------------------------------------------------
Goal
~~~~~~~~
Fast-forward, reverse-playback and slow-motion are all techniques collectively known as trick modes and they all have in common that modify the normal playback rate. This tutorial shows how to achieve these effects and adds frame-stepping into the deal. In particular, it shows:

* How to change the playback rate, faster and slower than normal, forward and backwards.
* How to advance a video frame-by-frame

Basic tutorial 14: Handy elements
---------------------------------------------------------------------------------------

Basic tutorial 16: Platform-specific elements
---------------------------------------------------------------------------------------

