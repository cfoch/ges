/* GStreamer Editing Services
 * Copyright (C) 2009 Edward Hervey <edward.hervey@collabora.co.uk>
 *               2009 Nokia Corporation
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

#ifndef __GES_INTERNAL_H__
#define __GES_INTERNAL_H__

#include <gst/gst.h>
#include <gio/gio.h>

#include "ges-timeline.h"
#include "ges-track-object.h"

#include "ges-asset.h"
#include "ges-base-xml-formatter.h"

GST_DEBUG_CATEGORY_EXTERN (_ges_debug);
#define GST_CAT_DEFAULT _ges_debug

gboolean
timeline_ripple_object         (GESTimeline *timeline, GESTrackObject *obj,
                                    GList * layers, GESEdge edge,
                                    guint64 position);

gboolean
timeline_slide_object          (GESTimeline *timeline, GESTrackObject *obj,
                                    GList * layers, GESEdge edge, guint64 position);

gboolean
timeline_roll_object           (GESTimeline *timeline, GESTrackObject *obj,
                                    GList * layers, GESEdge edge, guint64 position);

gboolean
timeline_trim_object           (GESTimeline *timeline, GESTrackObject * object,
                                    GList * layers, GESEdge edge, guint64 position);
gboolean
ges_timeline_trim_object_simple (GESTimeline * timeline, GESTrackObject * obj,
                                 GList * layers, GESEdge edge, guint64 position, gboolean snapping);

gboolean
ges_timeline_move_object_simple (GESTimeline * timeline, GESTrackObject * object,
                                 GList * layers, GESEdge edge, guint64 position);

gboolean
timeline_move_object           (GESTimeline *timeline, GESTrackObject * object,
                                    GList * layers, GESEdge edge, guint64 position);

gboolean
timeline_context_to_layer      (GESTimeline *timeline, gint offset);

G_GNUC_INTERNAL void
ges_asset_cache_init (void);

G_GNUC_INTERNAL void
ges_asset_set_id (GESAsset *asset, const gchar *id);

G_GNUC_INTERNAL void
ges_asset_cache_put (GESAsset * asset, GSimpleAsyncResult *res);

G_GNUC_INTERNAL gboolean
ges_asset_cache_set_loaded(GType extractable_type, const gchar * id, GError *error);

GESAsset*
ges_asset_cache_lookup(GType extractable_type, const gchar * id);

gboolean
ges_asset_set_proxy (GESAsset *asset, const gchar *new_id);

G_GNUC_INTERNAL gboolean
ges_asset_request_id_update (GESAsset *asset, gchar **proposed_id,
    GError *error);

/* GESExtractable internall methods
 *
 * FIXME Check if that should be public later
 */
GType
ges_extractable_type_get_asset_type              (GType type);

G_GNUC_INTERNAL gchar *
ges_extractable_type_check_id                    (GType type, const gchar *id, GError **error);

GParameter *
ges_extractable_type_get_parameters_from_id      (GType type, const gchar *id,
                                                  guint *n_params);
GType
ges_extractable_get_real_extractable_type_for_id (GType type, const gchar * id);

gboolean ges_extractable_register_metas          (GType extractable_type, GESAsset *asset);

/************************************************
 *                                              *
 *        GESFormatter internal methods         *
 *                                              *
 ************************************************/
void
ges_formatter_set_project                        (GESFormatter *formatter,
                                                  GESProject *project);
GESProject *
ges_formatter_get_project                        (GESFormatter *formatter);
GESAsset * _find_formatter_asset_for_uri         (const gchar *uri);



/************************************************
 *                                              *
 *        GESProject internal methods           *
 *                                              *
 ************************************************/

/* FIXME This should probably become public, but we need to make sure it
 * is the right API before doing so*/
gboolean ges_project_set_loaded                  (GESProject * project,
                                                  GESFormatter *formatter);
gchar * ges_project_try_updating_id              (GESProject *self,
                                                  GESAsset *asset,
                                                  GError *error);

/************************************************
 *                                              *
 *   GESBaseXmlFormatter internal methods       *
 *                                              *
 ************************************************/

/* FIXME GESBaseXmlFormatter is all internal for now, the API is not stable
 * fo now, so do not expose it */
G_GNUC_INTERNAL void ges_base_xml_formatter_add_timeline_object (GESBaseXmlFormatter * self,
                                                                 const gchar *id,
                                                                 const char *asset_id,
                                                                 GType type,
                                                                 GstClockTime start,
                                                                 GstClockTime inpoint,
                                                                 GstClockTime duration,
                                                                 gdouble rate,
                                                                 guint layer_prio,
                                                                 GESTrackType track_types,
                                                                 GstStructure *properties,
                                                                 const gchar *metadatas,
                                                                 GError **error);
G_GNUC_INTERNAL void ges_base_xml_formatter_add_asset        (GESBaseXmlFormatter * self,
                                                                 const gchar * id,
                                                                 GType extractable_type,
                                                                 GstStructure *properties,
                                                                 const gchar *metadatas,
                                                                 GError **error);
G_GNUC_INTERNAL void ges_base_xml_formatter_add_layer           (GESBaseXmlFormatter *self,
                                                                 GType extractable_type,
                                                                 guint priority,
                                                                 GstStructure *properties,
                                                                 const gchar *metadatas,
                                                                 GError **error);
G_GNUC_INTERNAL void ges_base_xml_formatter_add_track           (GESBaseXmlFormatter *self,
                                                                 GESTrackType track_type,
                                                                 GstCaps *caps,
                                                                 const gchar *id,
                                                                 GstStructure *properties,
                                                                 const gchar *metadatas,
                                                                 GError **error);
void ges_base_xml_formatter_add_encoding_profile               (GESBaseXmlFormatter * self,
                                                                 const gchar *type,
                                                                 const gchar *parent,
                                                                 const gchar * name,
                                                                 const gchar * description,
                                                                 GstCaps * format,
                                                                 const gchar * preset,
                                                                 const gchar * preset_name,
                                                                 guint id,
                                                                 guint presence,
                                                                 GstCaps * restriction,
                                                                 guint pass,
                                                                 gboolean variableframerate,
                                                                 GstStructure * properties,
                                                                 GError ** error);
G_GNUC_INTERNAL void ges_base_xml_formatter_add_track_object    (GESBaseXmlFormatter *self,
                                                                 GType effect_type,
                                                                 const gchar *asset_id,
                                                                 const gchar * track_id,
                                                                 const gchar *timeline_obj_id,
                                                                 GstStructure *children_properties,
                                                                 GstStructure *properties,
                                                                 const gchar *metadatas,
                                                                 GError **error);
G_GNUC_INTERNAL void set_property_foreach                       (GQuark field_id,
                                                                 const GValue * value,
                                                                 GObject * object);;

/* Function to initialise GES */
G_GNUC_INTERNAL void _init_standard_transition_assets        (void);
G_GNUC_INTERNAL void _init_formatter_assets                  (void);

#endif /* __GES_INTERNAL_H__ */
