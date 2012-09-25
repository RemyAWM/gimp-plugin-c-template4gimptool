/*
 * gpl-effect_one.c
 *
 *  Created on: 19.09.2012
 *      Author: remy
 */


#define RENDER_LAYER    "render-layer"

const GimpRGB colorText = {0.7,0.84,1};

/* Это пример создания демонстрационного эффекта.
 * Эффект представляет собой текст "Effect One Do!", который
 * добавляется к изображению обрабатываемого слоя и центрируется по
 * горизонтали и вертикали относительно него.
 * Параметр effect_size меняет размер текста от 0 до 100 пик.
 *
 * Параметр image_work_id - уникальный id на скрытое изображение
 * в котором создается эффект (то, что мы создали функцией work_image_create() в gpl-name.c).
 *  */

/**
 * effect_one:
 * @image_work_id:      id изображения в котором будет создаваться эффект
 * @drawable_work_id:   id копии слоя к изображению которого будет применяться эффект
 * @values:             id указатель на параметры (свойства) плагина
 *
 * Return: id слоя содержащего отрендереный эффект
 */
gint32
effect_one (gint32 image_work_id,
             gint32 drawable_work_id,
             PlugInVals *values)
{
  if(!gimp_image_is_valid(image_work_id) || !gimp_item_is_valid(drawable_work_id))
          return -1;

  gint32 render_text = -1;
  gint32 render_grv_black = -1;
  gint32 render_grv_white = -1;
  gint32 render_copy = -1;

  /* Получаем слой с последним рендером, если такой имеется */
  gint32 layer_render = gimp_image_get_layer_by_name (image_work_id, RENDER_LAYER);
  /* Если имеется - удаляем */
  if(layer_render != -1)
            gimp_image_remove_layer (image_work_id, layer_render);

  /* Получаем размеры обрабатываемого слоя */
  gint width = gimp_drawable_width(drawable_work_id);
  gint height = gimp_drawable_height(drawable_work_id);
  gint x_offset = 0;
  gint y_offset = 0;
  gint parent = gimp_item_get_parent(drawable_work_id);
  gimp_drawable_offsets(drawable_work_id, &x_offset, &y_offset);

  /* Создаем копию work-layer, которую потом объединим со слоем эффекта */
  render_copy = gimp_layer_copy (drawable_work_id);
  gimp_image_insert_layer(image_work_id, render_copy, parent, -1);

  /* Собственно эффект - просто текст ... */
  gimp_context_set_foreground(&colorText);
  render_text = gimp_text_layer_new (image_work_id,
      "Effect One Do!",
      "Serif",
      values->effect_size,
      GIMP_UNIT_PIXEL);
  gimp_image_insert_layer(image_work_id, render_text, parent, -1);
  gint render_pos = gimp_image_get_layer_position(image_work_id, render_text);

  /* Центрируем текст относительно исходного изображения обрабатываемого слоя
   * (который хранится в work-layer и был передан через drawable_work_id!)
   * */
  gint render_width = gimp_drawable_width(render_text);
  gint render_height = gimp_drawable_height(render_text);
  gint render_offset_x = x_offset+((width/2)-(render_width/2));
  gint render_offset_y = y_offset+((height/2)-(render_height/2));

  gimp_layer_set_offsets (render_text,
                            render_offset_x,
                            render_offset_y);
/* Отблеск сверху */
  gimp_context_set_foreground(&colorWhite);
  render_grv_white = gimp_text_layer_new (image_work_id,
      "Effect One Do!",
      "Serif",
      values->effect_size,
      GIMP_UNIT_PIXEL);

  gimp_image_insert_layer(image_work_id, render_grv_white, parent, render_pos+1);
  gimp_layer_set_offsets (render_grv_white,
      render_offset_x-1,
      render_offset_y-1);

  /* Тень снизу */
  gimp_context_set_foreground(&colorBlack);
  render_grv_black = gimp_text_layer_new (image_work_id,
      "Effect One Do!",
      "Serif",
      values->effect_size,
      GIMP_UNIT_PIXEL);

  gimp_image_insert_layer(image_work_id, render_grv_black, parent, render_pos+2);
  gimp_layer_set_offsets (render_grv_black,
      render_offset_x+1,
      render_offset_y+1);

  render_text = gimp_image_merge_down (image_work_id, render_text, GIMP_EXPAND_AS_NECESSARY);
  render_text = gimp_image_merge_down (image_work_id, render_text, GIMP_EXPAND_AS_NECESSARY);
  layer_render = gimp_image_merge_down (image_work_id, render_text, GIMP_EXPAND_AS_NECESSARY);

  gimp_item_set_name(layer_render, RENDER_LAYER);

  return layer_render;
}

