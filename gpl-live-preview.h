/*
 * gpl-live-preview.h
 *
 *  Created on: 19.09.2012
 *      Author: remy
 */

#ifndef GPL_LIVE_PREVIEW_H_
#define GPL_LIVE_PREVIEW_H_

gboolean        live_preview_init               (gint32 image_id,
                                                   gint32 drawable_id);

gint32          live_preview_get_layer          (void);

void            live_preview_destore            (gint32 image_id);

void            live_preview_update             (GtkWidget *widget,
                                                    gpointer data);

#endif /* GPL_LIVE_PREVIEW_H_ */
