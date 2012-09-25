/*
 * gpl-types.h
 *
 *  Created on: 19.09.2012
 *      Author: remy
 */

#ifndef GPL_TYPES_H_
#define GPL_TYPES_H_

/* Объявление типа структуры
 * содержащей значения настроек (свойств) плагина.
 * (effect_size - для примера)
 * */
typedef struct _PlugInVals PlugInVals;
struct _PlugInVals
{
  gdouble               effect_size;
};

/* Объявление типа указателя на функцию
 * создающею эффект.
 * */
typedef gint32 (* EffectFunc) (gint32 image_id,
                                gint32 drawable_id,
                                PlugInVals *values);

/* Объявление типа структуры
 * передающей данные системе лайв-просмотра.
 * */
typedef struct _PlugInPreviewData PlugInPreviewData;
struct _PlugInPreviewData
{
    /* Изображение (как правило скрытое) в котором создается эффект */
    gint32          image_work;

    /* Изображение содержащее слой предосмотра.
    * (слой предосмотра - как правило слой созданный live_preview_init)
    * */
    gint32          image_preview;

    /* Собственно - слой предосмотра */
    gint32          layer_preview;

    /* Указатель на структуру содержащую настройки (свойства) плагина */
    PlugInVals      *values;

    /* Указатель на функцию создающею эффект */
    gpointer        effect_func;
};

#endif /* GPL_TYPES_H_ */
