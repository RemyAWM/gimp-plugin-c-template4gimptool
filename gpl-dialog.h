/*
 * gpl-dialog.h
 *
 *  Created on: 20.09.2012
 *      Author: remy
 */

#ifndef GPL_DIALOG_H_
#define GPL_DIALOG_H_

GimpPDBStatusType               dialog          (gint32 image_preview,
                                                  gint32 drawable_preview,
                                                  gint32 image_work,
                                                  gpointer effect_func,
                                                  PlugInVals *values,
                                                  const gchar *name);

#endif /* GPL_DIALOG_H_ */
