/**
 * Gstreamer Editing Services
 *
 * Copyright (C) <2012> Thibault Saunier <thibault.saunier@collabora.com>
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

#include "test-utils.h"
#include <gio/gio.h>

gchar *
ges_test_get_audio_only_uri (void)
{
  gchar *uri;
  GFile *cfile, *fdir, *f_audio_only;

  cfile = g_file_new_for_path (__FILE__);
  fdir = g_file_get_parent (cfile);

  f_audio_only = g_file_get_child (fdir, "audio_only.ogg");
  uri = g_file_get_uri (f_audio_only);

  gst_object_unref (cfile);
  gst_object_unref (fdir);
  gst_object_unref (f_audio_only);

  return uri;
}

gchar *
ges_test_get_audio_video_uri (void)
{
  gchar *uri;
  GFile *cfile, *fdir, *f_audio_video;

  cfile = g_file_new_for_path (__FILE__);
  fdir = g_file_get_parent (cfile);

  f_audio_video = g_file_get_child (fdir, "audio_video.ogg");
  uri = g_file_get_uri (f_audio_video);

  gst_object_unref (cfile);
  gst_object_unref (fdir);
  gst_object_unref (f_audio_video);

  return uri;
}

gchar *
ges_test_get_image_uri (void)
{
  return ges_test_file_uri ("image.png");
}

gchar *
ges_test_file_uri (const gchar * filename)
{
  gchar *uri;
  GFile *cfile, *fdir, *f;

  cfile = g_file_new_for_path (__FILE__);
  fdir = g_file_get_parent (cfile);

  f = g_file_get_child (fdir, filename);
  uri = g_file_get_uri (f);

  gst_object_unref (cfile);
  gst_object_unref (fdir);
  gst_object_unref (f);

  return uri;
}

GESTimelinePipeline *
ges_test_create_pipeline (GESTimeline * timeline)
{
  GESTimelinePipeline *pipeline;

  pipeline = ges_timeline_pipeline_new ();
  fail_unless (ges_timeline_pipeline_add_timeline (pipeline, timeline));

  g_object_set (pipeline, "audio-sink", gst_element_factory_make ("fakesink",
          "test-audiofakesink"), "video-sink",
      gst_element_factory_make ("fakesink", "test-videofakesink"), NULL);

  return pipeline;

}
