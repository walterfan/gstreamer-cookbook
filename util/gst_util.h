#pragma once

#include <gst/gst.h>
#include <gst/sdp/sdp.h>
#include <gst/rtp/rtp.h>
#include <gst/webrtc/webrtc.h>

GstCaps* get_default_caps();

gboolean link_elements_with_filter (GstElement *element1, GstElement *element2);

gboolean print_field (GQuark field, const GValue * value, gpointer pfx);

void print_caps (const GstCaps * caps, const gchar * pfx);

void print_pad_capabilities (GstPad *pad, gchar *pad_name);