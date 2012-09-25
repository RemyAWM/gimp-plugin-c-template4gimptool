/*
 * gpl-state-image.c
 *
 *  Created on: 19.09.2012
 *      Author: remy
 */

#define SAVE_LAYER_NAME         "save-layer"

static gint32 save_image_id = -1;
static gint32 save_selection_channel = -1;
static GimpRGB save_fg = {1,1,1}, save_bg = {0,0,0};
static gint save_height = 0, save_width = 0, save_offset_x = 0, save_offset_y = 0;

/**
 * state_image_save:
 * @image_id:           id сохраняемого изображения
 * @drawable_id:        id сохраняемого слоя
 *
 * Cохраняет настройки Гимпа перед началом работы плагина.
 * (сохраняет слой к которому применяется эффект в скрытое
 * изображение, сохраняет выделение, цвета и т.д. ...)
 * */
void
state_image_save (gint32 image_id,
                    gint32 drawable_id)
{
        if(!gimp_image_is_valid(image_id) || !gimp_item_is_valid(drawable_id))
                return;

        gint32          save_layer              =       -1;
        gint            width                   =       gimp_image_width (image_id);
        gint            height                  =       gimp_image_height (image_id);

        /* Если изображение с сохранением уже существует - удалить! */
        if(save_image_id != -1)
        {
                gimp_image_delete(save_image_id);
                save_image_id = -1;
        }

        /* Создаем новое изображение */
        save_image_id = gimp_image_new (width,
                                        height,
                                        gimp_image_base_type (image_id));

        /* Откл. сток отмен для скорости и экономии памяти */
        DISABLE_UNDO (save_image_id)

        /* Сохраняем слой */
        save_layer = gimp_layer_new_from_drawable (drawable_id, save_image_id);
        gimp_item_set_name (save_layer, SAVE_LAYER_NAME);
        gimp_image_insert_layer (save_image_id, save_layer, 0, -1);

        /* Сохраняем выделенный регион */
        save_selection_channel = gimp_selection_save(image_id);

        /* Сохраняем цвета */
        gimp_context_get_background (&save_bg);
        gimp_context_get_foreground (&save_fg);

        /* Размер слоя и положение */
        save_height = gimp_drawable_height(drawable_id);
        save_width = gimp_drawable_width(drawable_id);
        gimp_drawable_offsets(drawable_id, &save_offset_x, &save_offset_y);
}

/**
 * state_image_restore:
 * @restore_image_id:           восстанавливаемое изображение
 * @restore_drawable_id:        восстанавливаемый слой
 * @restore_drawable:           TRUE - восстанавливать слой; FALSE - нет (только настройки)
 *
 * Восстанавливаем настройки Гимпа.
 * (вызывается, как правило, перед окончанием работы плагина)
 * */
void
state_image_restore (gint32 restore_image_id,
                        gint32 restore_drawable_id,
                        gboolean restore_drawable)
{
        gint32  save_layer      = -1;

        /* Восстановить слой */
        if(gimp_image_is_valid(save_image_id))
        {

                if(restore_drawable)
                {
                        save_layer = gimp_image_get_layer_by_name (save_image_id, SAVE_LAYER_NAME);
                        if(save_layer != -1 && gimp_item_is_layer(save_layer))
                        {
                            drawable_copy_tiles(save_layer,
                                                restore_drawable_id,
                                                TRUE);
                        }
                }

                /* Удалить изображение */
                gimp_image_delete(save_image_id);
                save_image_id = -1;
        }

        /* Восстановить цвета */
        gimp_context_set_background (&save_bg);
        gimp_context_set_foreground (&save_fg);

        /* Восстановить выделенное */
        if(save_selection_channel != -1)
        {
                gimp_image_select_item (restore_image_id,
                                GIMP_CHANNEL_OP_REPLACE,
                                save_selection_channel);
                gimp_image_remove_channel(restore_image_id, save_selection_channel);
        }

        gimp_image_set_active_layer(restore_image_id, restore_drawable_id);
}

/**
 * state_image_remove:
 *
 * Удаляет сохраненные настройки и скрытое изображение
 * со слоем-копией
 * */
void
state_image_remove (void)
{
  if(save_image_id != -1)
  {
          gimp_image_delete(save_image_id);
          save_image_id = -1;
  }
}
