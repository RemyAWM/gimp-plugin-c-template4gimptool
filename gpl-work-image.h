/*
 * gpl-work-image.h
 *
 *  Created on: 19.09.2012
 *      Author: remy
 */

#ifndef GPL_WORK_IMAGE_H_
#define GPL_WORK_IMAGE_H_


void            work_image_create               (gint32 image_id, gint32 layer_id);

gint32          work_image_get                  (void);

gint32          work_image_get_work_layer      (gint32 image_id);

void            work_image_remove               (void);

#endif /* GPL_WORK_IMAGE_H_ */
