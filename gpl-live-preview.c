/*
 * gpl-live-preview.c
 *
 *  Created on: 19.09.2012
 *      Author: remy
 */


static gint32 preview_layer = -1;
static gint32 origin_layer = -1;

/* Система лайв-просмотра
 * */

/**
 * live_preview_init:
 * @image_id:
 * @layer_id:
 * Инициализирует и создает слой лайв-предосмотра
 * */
gboolean
live_preview_init (gint32 image_id,
                     gint32 layer_id)
{
  if (!gimp_image_is_valid(image_id) ||
      !gimp_item_is_valid(layer_id) ||
      !gimp_item_is_layer(layer_id))
    return FALSE;

  origin_layer = layer_id;

  /* Если слой предосмотра уже существует, удалить его */
  if (preview_layer != -1)
    live_preview_destore (image_id);

  /* Создаем слой предосмотра */
  gint32    parent  =       gimp_item_get_parent(layer_id);
  gint      pos     =       gimp_image_get_layer_position(image_id,
                                                              layer_id);
  preview_layer = gimp_layer_copy(layer_id);
  gimp_item_set_name(preview_layer,"<Preview>");
  gimp_image_insert_layer(image_id, preview_layer, parent, pos);

  /* Подготавливаем изображеие и слой для работы с лайв-просмотром: */

  /* Удаляем выделние (если оно есть) */
  gimp_selection_none (image_id);

  /* Скрываем выбранный для эффекта слой */
  gimp_drawable_set_visible(layer_id, FALSE);

  /* Инициализируем tiles cache: */
  GimpDrawable *drawable = gimp_drawable_get (layer_id);
  gimp_tile_cache_ntiles (3 * (MAX (drawable->width, drawable->height) /
                                                         gimp_tile_width() + 1));

  return TRUE;
}

/**
 * live_preview_get_layer:
 *
 * Return: id слоя-предосмотра или -1, если он не был создан
 * вызовом live_preview_init
 * */
gint32
live_preview_get_layer (void)
{
  return preview_layer;
}

/**
 * live_preview_destore:
 * @image_id:   id изображения, которое содержит слой предосмотра
 *
 * Уничтожает слой предосмотра
 * */
void
live_preview_destore (gint32 image_id)
{
  if (gimp_item_is_valid(preview_layer))
    {
        gimp_drawable_set_visible(origin_layer, TRUE);
        gimp_image_remove_layer(image_id, preview_layer);
    }

  preview_layer = -1;
  origin_layer =  -1;
}

/**
 * live_preview_update:
 * @widget:     указатель на объект вызвавший этот слот
 * @data:       указатель на структуру PlugInPreviewData которая
 *              содержит необходимые для работы предосмотра данные
 *
 * Слот вызывается при изменении свойств плагина и
 * (по переданным данным в аргументе data) обновляет изображение
 *  слоя предосмотра.
 *
 * (подробнее о PlugInPreviewData см. в gpl-types.h)
 * */
void
live_preview_update (GtkWidget *widget,
                        gpointer data)
{
        PlugInPreviewData *work_data    =       (PlugInPreviewData *) data;
        gint32 layer_work               =       work_image_get_work_layer (work_data->image_work);
        EffectFunc effect               =       work_data->effect_func;

        /* Создаем эффект с текущими настройками (свойствами) плагина */
        gint32 render_layer             =       effect (work_data->image_work,
                                                         layer_work,
                                                         work_data->values);

        if(render_layer == -1)
                return;

        /* Копируем слой с рендером эффекта в слой предосмотра */
        drawable_copy_tiles (render_layer,
                        work_data->layer_preview,
                        FALSE);

        gimp_displays_flush();
}
