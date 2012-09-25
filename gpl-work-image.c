/*
 * gpl-work-image.c
 *
 *  Created on: 19.09.2012
 *      Author: remy
 */

#define WORK_LAYER_NAME         "work-layer"

static work_image = -1;


/**
 *  work_image_create:
 *  @image_id:  id изображения на котором находится слой @layer_id
 *  @layer_id:  id слоя к которому применяется эффект
 *
 * Создает скрытое изображение в котором будем создаваться эффект.
 * Скрытое изображение содержит копию слоя к которому применяется эффект.
 * Этот слой называется work-layer "рабочий" слой.
 *
 * (если используется система лайв-просмотра тогда,
 * он не должен изменяться в процессе создания эффекта)
 */
void
work_image_create (gint32 image_id,
                     gint32 layer_id)
{
    gint32  image      =       -1;
    gint32  layer      =       -1;
    gint    width           =       gimp_image_width (image_id);
    gint    height          =       gimp_image_height (image_id);

    if(!gimp_image_is_valid(image_id) || !gimp_item_is_valid(layer_id))
          return;

        image      = gimp_image_new (width,
                                       height,
                                       gimp_image_base_type(image_id));

        /* Отключаем сток отмен, для экономии памяти и скорости */
        DISABLE_UNDO (image)

        layer = gimp_layer_new_from_drawable (layer_id, image);

        gimp_item_set_name(layer,"work-layer");

        gimp_image_insert_layer(image, layer,0,-1);

        /* Обрезаем слой по изображение - "Автокадрирование слоя" */
        crop_layer(image, layer);

         work_image = image;
}

/**
 * work_image_get:
 *
 * Return: индиф. скрытого изображения
 * или -1 если оно не было создано вызовом - work_image_create ()
 */
gint32
work_image_get (void)
{
  return work_image;
}

/**
 * work_image_get_work_layer:
 * @image_id:   скрытое изображение содержащее "рабочий" слой
 *
 * Return: id "рабочего" слоя
 * (копия слоя к которому применяется эффект)
 * */
gint32
work_image_get_work_layer (gint32 image_id)
{
        gint32 work_layer = -1;

        if(gimp_image_is_valid(image_id))
                work_layer = gimp_image_get_layer_by_name (image_id, WORK_LAYER_NAME);

        return work_layer;
}

/**
 * work_image_remove:
 *
 * Удаляет скрытое изображение с "рабочим" и другими слоями
 * */
void
work_image_remove (void)
{
  if (gimp_image_is_valid(work_image))
    gimp_image_delete(work_image);

  work_image = -1;
}
