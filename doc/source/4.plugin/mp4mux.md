# mp4mux plugin
## Overview

mp4mux element merges streams (audio and video) into ISO MPEG-4 (.mp4) files.

The following background intends to explain why various similar muxers are present in this plugin.

The QuickTime file format specification served as basis for the MP4 file format specification (mp4mux), and as such the QuickTime file structure is nearly identical to the so-called ISO Base Media file format defined in ISO 14496-12 (except for some media specific parts).

In turn, the latter ISO Base Media format was further specialized as a Motion JPEG-2000 file format in ISO 15444-3 (mj2mux) and in various 3GPP(2) specs (3gppmux). The fragmented file features defined (only) in ISO Base Media are used by ISMV files making up (a.o.) Smooth Streaming (ismlmux).

A few properties (#GstBaseQTMux:movie-timescale, trak-timescale) allow adjusting some technical parameters, which might be useful in (rare) cases to resolve compatibility issues in some situations.

Some other properties influence the result more fundamentally. A typical mov/mp4 file's metadata (aka moov) is located at the end of the file, somewhat contrary to this usually being called "the header". However, a faststart file will (with some effort) arrange this to be located near start of the file, which then allows it e.g. to be played while downloading. Alternatively, rather than having one chunk of metadata at start (or end), there can be some metadata at start and most of the other data can be spread out into fragments of fragment-duration. If such fragmented layout is intended for streaming purposes, then streamable allows foregoing to add index metadata (at the end of file).

## Example pipelines
```
 gst-launch-1.0 gst-launch-1.0 v4l2src num-buffers=50 ! queue ! x264enc ! mp4mux ! filesink location=video.mp4
```

Records a video stream captured from a v4l2 device, encodes it into H.264 and muxes it into an mp4 file.

## Hierarchy

```
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstElement
                ╰──GstAggregator
                    ╰──GstBaseQTMux
                        ╰──mp4mux
```