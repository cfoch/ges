/* GStreamer Editing Services
 * Copyright (C) 2016 Fabian Orccon <cfoch.fabian@gmail.com>
 * Copyright (C) 2013 Lubosz Sarnecki <lubosz@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/**
 * SECTION:gesimagesequencesrc
 * @short_description: outputs the video stream from a sequence of images.
 *
 * Outputs the video stream from a given image sequence. The start frame
 * chosen will be determined by the in-point property on the track element.
 */
#include <stdlib.h>
#include <string.h>
#include "ges-internal.h"
#include "ges-timeline-element.h"
#include "ges-track-element.h"
#include "ges-image-sequence-source.h"
#include "ges-extractable.h"
#include "ges-uri-asset.h"


/* Extractable interface implementation */

static gchar *
ges_extractable_check_id (GType type, const gchar * id, GError ** error)
{
  return g_strdup (id);
}

static void
ges_extractable_interface_init (GESExtractableInterface * iface)
{
  iface->check_id = ges_extractable_check_id;
}

G_DEFINE_TYPE_WITH_CODE (GESImageSequenceSource, ges_image_sequence_source,
    GES_TYPE_VIDEO_SOURCE,
    G_IMPLEMENT_INTERFACE (GES_TYPE_EXTRACTABLE,
        ges_extractable_interface_init));

struct _GESImageSequenceSourcePrivate
{
  /*  Dummy variable */
  void *nothing;
  GstElement *src;
};

enum
{
  PROP_0,
  PROP_URI
};

static void
ges_image_sequence_source_update_properties (GESImageSequenceSource * self)
{
  GESImageSequenceURI *st;

  g_assert (self->priv->src != NULL);

  st = ges_image_sequence_uri_new (self->uri);
  g_object_set (self->priv->src, "start-index", st->start, "stop-index",
      st->end, "location", st->location, NULL);
  g_free (st);
}

static char *
ges_image_sequence_source_get_uri (GESImageSequenceSource * self)
{
  if (self->priv->src) {
    gint start_index, stop_index;
    g_object_get (self->priv->src, "start-index", &start_index, "stop-index",
        &stop_index, NULL);
    return g_strdup_printf ("%s://%d:%d@%s", GES_IMAGE_SEQUENCE_URI_PREFIX,
        start_index, stop_index, self->location);
  }
  return g_strdup (self->uri);
}

static void
ges_image_sequence_source_set_uri (GESImageSequenceSource * self, gchar * uri)
{
  if (self->uri)
    g_free (self->uri);
  self->uri = uri;

  /* Update the imagesequencesrc properties according URI specifications */
  if (self->priv->src)
    ges_image_sequence_source_update_properties (self);
}

static void
ges_image_sequence_source_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GESImageSequenceSource *self = GES_IMAGE_SEQUENCE_SOURCE (object);

  switch (property_id) {
    case PROP_URI:
      self->uri = ges_image_sequence_source_get_uri (self);
      g_value_set_string (value, self->uri);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
ges_image_sequence_source_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GESImageSequenceSource *self = GES_IMAGE_SEQUENCE_SOURCE (object);

  switch (property_id) {
    case PROP_URI:
      ges_image_sequence_source_set_uri (self, g_value_dup_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
ges_image_sequence_source_dispose (GObject * object)
{
  GESImageSequenceSource *self = GES_IMAGE_SEQUENCE_SOURCE (object);

  if (self->location)
    g_free (self->location);
  g_free (self->uri);

  G_OBJECT_CLASS (self)->dispose (object);
}

static void
pad_added_cb (GstElement * decodebin, GstPad * pad, GstElement * bin)
{
  GstPad *srcpad;
  srcpad = gst_element_get_static_pad (bin, "src");
  if (srcpad) {
    gst_ghost_pad_set_target (GST_GHOST_PAD (srcpad), pad);
  } else {
    srcpad = gst_ghost_pad_new ("src", pad);
    gst_pad_set_active (srcpad, TRUE);
    gst_element_add_pad (bin, srcpad);
  }
}

static void
update_duration (GstElement * element, GParamSpec * pspec, GESTrackElement *
    track_element)
{
  guint64 duration;
  g_object_get (element, "duration", &duration, NULL);
  ges_timeline_element_set_max_duration (GES_TIMELINE_ELEMENT (track_element),
      duration);
}

static GstElement *
ges_image_sequence_source_create_source (GESTrackElement * track_element)
{
  GstElement *bin, *src, *decodebin;
  GESImageSequenceSource *self;
  const gchar *props[] = { "start-index", "stop-index", "framerate",
    "location", "duration", NULL
  };

  self = GES_IMAGE_SEQUENCE_SOURCE (track_element);

  bin = GST_ELEMENT (gst_bin_new ("image-sequence-bin"));
  src = gst_element_factory_make ("imagesequencesrc", NULL);
  decodebin = gst_element_factory_make ("decodebin", NULL);

  g_object_set (src, "location", self->location, NULL);

  gst_bin_add_many (GST_BIN (bin), src, decodebin, NULL);
  gst_element_link (src, decodebin);

  g_signal_connect (G_OBJECT (decodebin), "pad-added",
      G_CALLBACK (pad_added_cb), bin);
  g_signal_connect (G_OBJECT (src), "notify::start-index",
      G_CALLBACK (update_duration), track_element);
  g_signal_connect (G_OBJECT (src), "notify::stop-index",
      G_CALLBACK (update_duration), track_element);
  g_signal_connect (G_OBJECT (src), "notify::framerate",
      G_CALLBACK (update_duration), track_element);

  ges_track_element_add_children_props (track_element, src, NULL, NULL, props);
  self->priv->src = src;

  ges_image_sequence_source_update_properties (self);

  return bin;
}

static void
ges_image_sequence_source_class_init (GESImageSequenceSourceClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GESVideoSourceClass *source_class = GES_VIDEO_SOURCE_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GESImageSequenceSourcePrivate));
  object_class->dispose = ges_image_sequence_source_dispose;

  object_class->get_property = ges_image_sequence_source_get_property;
  object_class->set_property = ges_image_sequence_source_set_property;
  source_class->create_source = ges_image_sequence_source_create_source;

  /**
   * GESImageSequenceSource:uri:
   *
   * The uri of the file/resource to use. You can set a start index,
   * a stop index and a sequence pattern.
   * The format is &lt;imagesequence://start:stop\@location-pattern&gt;.
   * The pattern uses printf string formating.
   *
   * Example uris:
   *
   * imagesequence:///home/you/image\%03d.jpg
   *
   * imagesequence://20:50@/home/you/sequence/\%04d.png
   *
   */
  g_object_class_install_property (object_class, PROP_URI,
      g_param_spec_string ("uri", "URI", "imagesequence uri",
          NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
  source_class->create_source = ges_image_sequence_source_create_source;
}

/**
  * ges_image_sequence_uri_new: (skip)
  *
  * Reads start/stop index and location from a gesimagesequence uri.
  *
  */
GESImageSequenceURI *
ges_image_sequence_uri_new (const gchar * uri)
{
  gchar *colon = NULL;
  gchar *at = NULL;
  gchar *indices;
  int charpos;
  GESImageSequenceURI *uri_data;
  const int prefix_size = strlen (GES_IMAGE_SEQUENCE_URI_PREFIX);

  uri_data = malloc (sizeof (GESImageSequenceURI));

  uri_data->start = 0;
  uri_data->end = -1;

  at = strchr (uri, '@');
  if (at != NULL) {
    charpos = (int) (at - uri);
    indices = g_strdup_printf ("%.*s", charpos, uri);
    indices = &indices[prefix_size];
    colon = strchr (indices, ':');
    if (colon != NULL) {
      charpos = (int) (colon - indices);
      uri_data->end = atoi (colon + 1);
      uri_data->start = atoi (g_strdup_printf ("%.*s", charpos, indices));
      GST_DEBUG ("indices start: %d end %d\n", uri_data->start, uri_data->end);
    } else {
      GST_ERROR ("Malformated imagesequence uri. You are using '@' and are "
          "missing ':'");
    }
    uri_data->location = at + 1;
  } else {
    uri_data->location = g_strdup (&uri[prefix_size]);
  }
  GST_DEBUG ("location: %s\n", uri_data->location);

  return uri_data;
}

static void
ges_image_sequence_source_init (GESImageSequenceSource * self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
      GES_TYPE_IMAGE_SEQUENCE_SOURCE, GESImageSequenceSourcePrivate);
  self->priv->src = NULL;
}

/**
 * ges_image_sequence_source_new:
 * @uri: the URI the source should control
 *
 * Creates a new #GESImageSequenceSource for the provided @uri.
 *
 * Returns: (transfer floating): A new #GESImageSequenceSource.
 */
GESImageSequenceSource *
ges_image_sequence_source_new (const gchar * uri)
{
  return g_object_new (GES_TYPE_IMAGE_SEQUENCE_SOURCE, "uri", uri,
      "track-type", GES_TRACK_TYPE_VIDEO, NULL);
}
