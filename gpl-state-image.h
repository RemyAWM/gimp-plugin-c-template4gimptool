/*
 * gpl-state-image.h
 *
 *  Created on: 19.09.2012
 *      Author: remy
 */

#ifndef GPL_STATE_IMAGE_H_
#define GPL_STATE_IMAGE_H_


void            state_image_save                (gint32 image_id,
                                                   gint32 drawable_id);

void             state_image_restore             (gint32 restore_image_id,
                                                    gint32 restore_drawable_id,
                                                    gboolean restore_drawable);

void            state_image_remove              (void);

#endif /* GPL_STATE_IMAGE_H_ */
