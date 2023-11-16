#include "gst_util.h"
#include "logger.h"

GstCaps* get_default_caps() 
{
    
    GstCaps *caps = gst_caps_new_full (
      gst_structure_new ("video/x-raw",
            "format", G_TYPE_STRING, "I420",
             "width", G_TYPE_INT, 320,
             "height", G_TYPE_INT, 240,
             "framerate", GST_TYPE_FRACTION, 30, 1,
             NULL),
      gst_structure_new ("video/x-raw",
             "format", G_TYPE_STRING, "NV12",
             "width", G_TYPE_INT, 320,
             "height", G_TYPE_INT, 240,
             "framerate", GST_TYPE_FRACTION, 30, 1,
             NULL),
      NULL);
    
    return caps;
}

gboolean link_elements_with_filter (GstElement *element1, GstElement *element2)
{
  gboolean link_ok;
  GstCaps *caps = get_default_caps();

  link_ok = gst_element_link_filtered (element1, element2, caps);
  gst_caps_unref (caps);

  if (!link_ok) {
    g_warning ("Failed to link element1 and element2!");
  }

  return link_ok;
}


/* Functions below print the Capabilities in a human-friendly format */
gboolean print_field (GQuark field, const GValue * value, gpointer pfx) 
{
  gchar *str = gst_value_serialize (value);

  INFO_TRACE("{}  {}: {}\n", (gchar *) pfx, g_quark_to_string (field), str);
  g_free (str);
  return TRUE;
}

void print_caps (const GstCaps * caps, const gchar * pfx) 
{
  guint i;

  g_return_if_fail (caps != NULL);

  if (gst_caps_is_any (caps)) {
    INFO_TRACE ("{}ANY\n", pfx);
    return;
  }
  if (gst_caps_is_empty (caps)) {
    INFO_TRACE ("{}EMPTY\n", pfx);
    return;
  }

  for (i = 0; i < gst_caps_get_size (caps); i++) {
    GstStructure *structure = gst_caps_get_structure (caps, i);

    INFO_TRACE ("{}{}\n", pfx, gst_structure_get_name (structure));
    gst_structure_foreach (structure, print_field, (gpointer) pfx);
  }
}

void print_pad_capabilities (GstPad *pad, gchar *pad_name) 
{

  GstCaps *caps = NULL;

  /* Retrieve negotiated caps (or acceptable caps if negotiation is not finished yet) */
  caps = gst_pad_get_current_caps (pad);
  if (!caps)
    caps = gst_pad_query_caps (pad, NULL);

  /* Print and free */
  INFO_TRACE ("Caps for the {} pad:\n", pad_name);
  print_caps (caps, "      ");
  gst_caps_unref (caps);

}
