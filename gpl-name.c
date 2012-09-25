/*
 * gpl-name.c
 *
 *  Created on: 19.09.2012
 *      Author: remy
 */

/* ------------------------- Секция Объявлений --------------------- */

/* Подключение стандартных библиотек */
#include <stdlib.h>
#include <string.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

/* Типы и перечисления плагина */
#include "gpl-enum.h"
#include "gpl-types.h"

/* Область определений.
 * Задайте нужное имя, название, описание плагина, копирайт и т.д.
 * */

/* Должно соответствовать имени основного файла плагина без расширения */
#define PLUGIN_BINARY                    "gpl-name"
#define PLUGIN_ROLE                      "gimp-gpl-name"
#define AUTHOR                           "Your Name <author@site.com>"
#define COPYRIGHT                        "Copyright (C) 2012. All Rights Reserved"
#define DATE                             "2012"
#define ACCEPTED_COLOR_TYPE              "RGB*, GRAY*"

#define PROCEDURE_NAME_1                "plug-in-gpl-name-one"
#define TEXT_NAME_1                     "Name for effect one"
#define LABEL_NAME_1                    "My effect _one..."
#define BLURB_NAME_1                    "This is a quickly way to add effect one at layer"
#define MENU_PATH_1                     "<Image>/Filters/Misc"

/* Раскомментируйте, если плагин использует несколько вызовов */
/*
#define PROCEDURE_NAME_2                "plug-in-gpl-name-two"
#define TEXT_NAME_2                     "Name for effect two"
#define LABEL_NAME_2                    "My effect _two..."
#define BLURB_NAME_2                    "This is a quickly way to add effect two at layer"
#define MENU_PATH_2                     "<Image>/Filters/Misc"
*/
#define DATA_KEY_VALS                   "plug-in-gpl-name-data"

/* Макросы работы со стоком отмен: */

#define DISABLE_UNDO(IMAGE)    if(gimp_image_undo_is_enabled(IMAGE))    \
                                    gimp_image_undo_disable(IMAGE);

#define ENABLE_UNDO(IMAGE)    if(!gimp_image_undo_is_enabled(IMAGE))    \
                                    gimp_image_undo_enable(IMAGE);

#define FREEZE_UNDO(IMAGE)     if(gimp_image_undo_is_enabled(IMAGE))        \
                                    gimp_image_undo_freeze (IMAGE);


#define THAW_UNDO(IMAGE)       if(!gimp_image_undo_is_enabled(IMAGE))      \
                                      gimp_image_undo_thaw(IMAGE);


#define START_UNDO_GROUP(IMAGE)  if(!gimp_image_undo_is_enabled(IMAGE))      \
                                      gimp_image_undo_thaw(IMAGE);            \
                                  gimp_image_undo_group_start(IMAGE);


#define STOP_UNDO_GROUP(IMAGE)   gimp_image_undo_group_end(IMAGE);          \
                                  if(gimp_image_undo_is_enabled(IMAGE))      \
                                  gimp_image_undo_freeze(IMAGE);

/* Область объявлений функций и переменных.
 *
 * Вначале подключите заголовки исходных файлов вашего плагина.
 * */
#include "gpl-effect-one.h"
#include "gpl-effect-two.h"
#include "gpl-live-preview.h"
#include "gpl-work-image.h"
#include "gpl-state-image.h"
#include "gpl-dialog.h"

/* Область объявлений предопределенных функций и переменных. Не изменять! */
static void     query           (void);

static void     run             (const gchar            *name,
                                   gint                   nparams,
                                   const GimpParam        *param,
                                   gint                   *nreturn_vals,
                                   GimpParam              **return_vals);
static PlugInVals       values;

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,
  NULL,
  query,
  run
};
/* конец объявлений предопределенных функций и переменных. */

/* Область объявлений пользовательских функций и переменных */
static gboolean         plugin_data_is_valid            (gint32 image_id,
                                                             gint32 drawable_id);

void                    crop_layer                        (gint32 image_id,
                                                             gint32 layer_id);

void                    drawable_copy_tiles              (gint32 src_drawable_id,
                                                             gint32 dest_drawable_id,
                                                             gboolean undo);

gint32                  drawable_rewrite                  (gint32 src_drawable_id,
                                                             gint32 dest_drawable_id);

const gchar*            plugin_get_name                  (const gchar *proc_name);

/* конец объявлений объявлений пользовательских функций и переменных */

/* Область констант */
const GimpRGB colorWhite = {1,1,1};
const GimpRGB colorBlack = {0,0,0};

/* Настройки плагина по умолчанию */
const PlugInVals default_values =
{
  100.0,                   /* effect_size */
};

/* ------------------------- Конец Секции Объявлений --------------------- */

MAIN ()

/* ------------------------- Начало Секции Реализаций --------------------- */

/* Область реализации функций.
 *
 * Вначале подключите файлы с реализациями функций вашего плагина.
 * Далее, реализуйте закрытые (static) и открытые функций объявленные
 * в "секций объявлений".
 * */
#include "gpl-live-preview.c"
#include "gpl-state-image.c"
#include "gpl-work-image.c"
#include "gpl-dialog.c"
#include "gpl-effect-one.c"
#include "gpl-effect-two.c"

static void
query (void)
{
  /* Список аргументов для вызова плагина программно (из кода другого плагина)
   *  параметры "run_mode", "image" и "drawable" - обязательны!
   *  Добавьте к концу массива параметры описанные в PlugInVals!
   * */
  static GimpParamDef args[] =
  {
    { GIMP_PDB_INT32,    "run_mode",            "Interactive, non-interactive"          },
    { GIMP_PDB_IMAGE,    "image",               "Input image"                           },
    { GIMP_PDB_DRAWABLE, "drawable",            "Input drawable"                        },

    { GIMP_PDB_FLOAT,    "effect_size",         "Size of effect"                        }
  };

  /* Регистрируем в PDB вызов плагина для первого эффекта */
  gimp_install_procedure (PROCEDURE_NAME_1,
                          TEXT_NAME_1,
                          BLURB_NAME_1,
                          AUTHOR,
                          COPYRIGHT,
                          DATE,
                          LABEL_NAME_1,
                          ACCEPTED_COLOR_TYPE,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (args), 0,
                          args, NULL);

  /* Указываем, где в меню Гимп должен появится плагин */
  gimp_plugin_menu_register (PROCEDURE_NAME_1, MENU_PATH_1);

  /* Раскомментируйте, если плагин использует несколько вызовов */
  /*
  gimp_install_procedure (PROCEDURE_NAME_2,
                          TEXT_NAME_2,
                          BLURB_NAME_2,
                          AUTHOR,
                          COPYRIGHT,
                          DATE,
                          LABEL_NAME_2,
                          ACCEPTED_COLOR_TYPE,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (args), 0,
                          args, NULL);

  gimp_plugin_menu_register (PROCEDURE_NAME_2, MENU_PATH_1);
  */
}

static void
run (const gchar        *name,
     gint               n_params,
     const GimpParam    *param,
     gint               *nreturn_vals,
     GimpParam          **return_vals)
{
  /* Область объявления обязательных переменных */
  static GimpParam  return_values[1];
  GimpPDBStatusType  status = GIMP_PDB_SUCCESS;

  GimpRunMode        run_mode;

  GimpDrawable       *drawable;
  gint32             drawable_id;
  gint32             image_id;

  /* Кол-во и значения передаваемые Гимпу при закрытии плагина. */
  *nreturn_vals =       1;
  *return_vals  =       return_values;

  /* Инициализация обязательных переменных: */
  run_mode              =       param[0].data.d_int32;
  image_id              =       param[1].data.d_image;
  drawable_id           =       param[2].data.d_drawable;
  drawable              =       gimp_drawable_get (param[2].data.d_drawable);

  /*  Инициализация настроек плагина с дефолтными значениями  */
  values          = default_values;

  /* Переменная "на функцию" создающею эффект */
  static EffectFunc effect_func;

  /* проверяем валидность слоя и изображения к которому применяется эффект */
  if (! plugin_data_is_valid (image_id, drawable_id))
    status = GIMP_PDB_EXECUTION_ERROR;

  /* В зависимости от имени вызова плагина, присваиваем переменной effect_func
   * функцию соответствующею вызванному эффекту!
   * Если вызов был с несуществующим именем -  status = GIMP_PDB_CALLING_ERROR;
   * */
  if (strcmp (name, PROCEDURE_NAME_1) == 0)
    effect_func = effect_one;

  /* Раскомментируйте, если плагин использует несколько вызовов: */
  /*
  else if (strcmp (name, PROCEDURE_NAME_2) == 0)
    effect_func = effect_two;
  */
  else
    status = GIMP_PDB_CALLING_ERROR;

  /*
   * Если все еще статус GIMP_PDB_SUCCESS,
   * сохраняем состояние изображения и слоя (выделение, цвета, слой )
   * обрабатываем режимы вызова плагина - run_mode:
   * */
  if(status == GIMP_PDB_SUCCESS)
  {
      /* Отключаем сток отмен */
      FREEZE_UNDO (image_id)

      /* Сохраняем состояние изображения, слоя и настроек Гимпа.
       * (cохраните только те настройки Гимпа, которые могут быть
       * изменены в процессе работы плагина.)
       *  */
      state_image_save (image_id, drawable_id);

      /* Создать скрытое изображение, в котором будет создаваться эффект.
       * (cоздание эффекта в скрытом изображении существенно ускоряет
       * работу плагина и делает его надежнее!)
       * */
      work_image_create(image_id, drawable_id);

      /* Выполняем действия в зависимости от режима вызова: */
        switch (run_mode)
        {
              /* Был вызван из другого плагина: */
              case GIMP_RUN_NONINTERACTIVE:
                    if (n_params != 4)
                    {
                      /* Если вызван с ошибочным кол-вом параметров */
                      status = GIMP_PDB_CALLING_ERROR;
                    }
                    else
                    {
                      /* Присваиваем переданные (при вызове через функцию) параметры
                       * (свойства) плагина
                       * */
                      values.effect_size    = param[3].data.d_float;
                    }
                break;
                /* Был вызан из меню Гимпа: */
                case GIMP_RUN_INTERACTIVE:
                    /* Если возможно - загрузить последние использованные настройки плагина */
                    gimp_get_data (DATA_KEY_VALS,    &values);

                    /* Включаем "живой" предосмотр.
                     * Перед включением предосмотра, необходимо сохранить
                     * состояние изображения! И восстановить его перед выходом из плагина!
                     * */
                     live_preview_init (image_id, drawable_id);

                    /* Открываем диалог плагина.
                     * Если диалог закрыт кнопкой "OK" - status=GIMP_PDB_CANCEL,
                     * иначе - status=GIMP_PDB_SUCCESS
                     * */
                    status = dialog (image_id,
                                            live_preview_get_layer (),
                                            work_image_get (),
                                            effect_func,
                                            &values,
                                            name);

                    /* Отключаем предосмотр */
                    live_preview_destore (image_id);
                break;

                /* Был вызван из меню Гимпа - "Repeat Last" */
                case GIMP_RUN_WITH_LAST_VALS:
                    /* Пытаемся загрузить последние использованные настройки */
                    gimp_get_data (DATA_KEY_VALS, &values);
                break;

                default:
                break;
        }
  }

  /* Если status все еще GIMP_PDB_SUCCESS... */
  if (status == GIMP_PDB_SUCCESS)
    {
      gint32 image_work = work_image_get ();
      gint32 layer_work = work_image_get_work_layer(image_work);

          /* Создаем конечный рендер эффекта */
          gint32 render_layer = effect_func (image_work,
                                             layer_work,
                                             &values);

          /* Если эффект создан успешно: */
          if(render_layer != -1)
          {
              /* Включаем сток отмен и начинаем запись действий в группу */
              START_UNDO_GROUP (image_id)

              /* Копируем изобр. эффекта в слой к которому применяется эффект */
              drawable_copy_tiles (render_layer,
                                   drawable_id, TRUE);

              /* Включаем сток отмен */
              STOP_UNDO_GROUP (image_id)
          }

          /* Восстанавливаем состояние изображения, как оно было
           * до начала работы плагина, КРОМЕ слоя! Т.к. он теперь содержит
           * эффект и может стать больше или меньше прежнего.
           * Для того, чтоб функция state_image_restore не восстанавливала
           * слой, задаем аргумент restore_drawable = FALSE.
           * */
          state_image_restore (image_id, drawable_id, FALSE);

      /* Выполняем обновление картинки на канве Гимпа */
      if (run_mode != GIMP_RUN_NONINTERACTIVE)
          gimp_displays_flush ();

      if (run_mode == GIMP_RUN_INTERACTIVE)
      {
          /* Сохраняем последние использованные настройки */
          gimp_set_data (DATA_KEY_VALS,    &values,    sizeof (values));
      }

      THAW_UNDO(image_id)
    }
  /* Эта ветвь выполняется если состояние status != GIMP_PDB_SUCCESS, т.е.:
   *  - были ошибки в работе плагина
   *  - диалог закрылся кнопкой "Cancel"
   * */
  else
  {
      /* Если диалог закрыт кнопкой  Cancel */
      if (status == GIMP_PDB_CANCEL)
        {
          /* Восстанавливаем изображение и слой */
          state_image_restore(image_id, drawable_id, TRUE);
          THAW_UNDO(image_id)
        }
  }

/* Уничтожаем скрытые изображ. хранящее сохраненный
 * слой и рабочие изображение
 * */
  state_image_remove ();
  work_image_remove ();

  /* "Отстегиваем" слой */
  gimp_drawable_detach (drawable);
  /* Обновляем картинку на канве */
  gimp_displays_flush ();

  return_values[0].type = GIMP_PDB_STATUS;
  return_values[0].data.d_status = status;
}

/**
 * plugin_data_is_valid:
 * @image_id:           id проверяемого изображения
 * @drawable_id:        id проверяемого слоя
 *
 * Проверяет, может ли плагин работать с выбранным изображением и слоем
 *
 * Return: TRUE - если валидно, иначе - FALSE.
 */
static gboolean
plugin_data_is_valid (gint32 image_id,
                         gint32 drawable_id)
{
  if(gimp_image_is_valid(image_id) &&
      gimp_item_is_valid(drawable_id) &&
      gimp_item_is_layer(drawable_id))
    return TRUE;

  return FALSE;
}

/* Public functions */
/**
 * drawable_copy_tiles:
 * @src_drawable_id:    id слоя источника (от куда копируется изображение)
 * @dest_drawable_id:   id слоя приемника (куда копируется изображение)
 * @undo:               TRUE - записывать копирование в сток отмен; FALSE - не записывать;
 *
 * Быстрое копирование изображения со слоя src_drawable_id в
 * слой dest_drawable_id, используя "поплиточное" копирование
 */
void
drawable_copy_tiles (gint32 src_drawable_id,
                        gint32 dest_drawable_id,
                        gboolean undo)
{
        if(!gimp_item_is_valid(src_drawable_id) || !gimp_item_is_valid (dest_drawable_id))
          return;

        GimpDrawable *srcDrawable = NULL;
        GimpDrawable *destDrawable = NULL;

        /* Определяем область копирования или выход - если нечего копировать */
        gint x =0, y=0, w=0, h =0;
        if(!gimp_drawable_mask_intersect(src_drawable_id,
                                                                 &x,
                                                                 &y,
                                                                 &w,&h))
          return;

        /* Подгоняем размер слоя, в который происходит копирование */
        gint src_width = gimp_drawable_width(src_drawable_id);
        gint src_height = gimp_drawable_height(src_drawable_id);
        gimp_layer_resize(dest_drawable_id,
                            src_width,
                            src_height,
                            0,
                            0);
        gint offset_x = 0, offset_y=0;
        gimp_drawable_offsets(src_drawable_id,&offset_x, &offset_y);
        gimp_layer_set_offsets(dest_drawable_id,offset_x,offset_y);

        /* Получаем указатели на структуры слоя
         * (именно здесь, только после изменения размеров и не раньше по коду!)
         */
        srcDrawable = gimp_drawable_get(src_drawable_id);
        destDrawable = gimp_drawable_get(dest_drawable_id);

        /* Объявляем и подготавливаем переменные необходимые для копирования */
        GimpPixelRgn  srcPR, destPR;
        gint          width = srcDrawable->width, height = srcDrawable->height;
        guchar       *buffer;
        gint bytes  = srcDrawable->bpp;

        /* Инициализируем регионы под копирование */
        gimp_pixel_rgn_init(&srcPR, srcDrawable, x, y, w, h, FALSE, FALSE);
        gimp_pixel_rgn_init(&destPR,destDrawable, x, y, w, h,TRUE,TRUE);
        /* Копируем ... */
        buffer = g_malloc (width * height * bytes);
                gimp_pixel_rgn_get_rect(&srcPR, buffer, x, y, w, h);
                gimp_pixel_rgn_set_rect(&destPR,buffer, x, y,w, h);
        g_free (buffer);

       /* Обновляем информацию о скопированных плитках слоя */
       gimp_drawable_flush(destDrawable);
       /* Сводим теневые плитки */
       gimp_drawable_merge_shadow (destDrawable->drawable_id, undo);
       /* Обновляем регион, чтоб отобразить изменения */
       gimp_drawable_update(destDrawable->drawable_id, x, y, w, h);
}

/**
 * drawable_rewrite:
 * @src_drawable_id:     id слоя источника (от куда копируется изображение)
 * @dest_drawable_id:    id слоя приемника (куда копируется изображение)
 *
 * Копирование слоя путем перезаписи слоя src_drawable_id
 * на место слоя dest_drawable_id.
 *
 * Return: новый "перезаписанный" слой
 * */
gint32
drawable_rewrite (gint32 src_drawable_id,
                    gint32 dest_drawable_id)
{
  gint32 new_layer = -1;
  gint32 src_image = gimp_drawable_get_image(src_drawable_id);

  gint32 dest_image = gimp_drawable_get_image(dest_drawable_id);
  const gchar *dest_name = gimp_item_get_name(dest_drawable_id);
  gint pos = gimp_image_get_layer_position(dest_image, dest_drawable_id);
  gint32 parent = gimp_item_get_parent(dest_drawable_id);

  new_layer = gimp_layer_new_from_drawable (src_drawable_id, dest_image);
  gimp_image_insert_layer (dest_image, new_layer, parent, pos+1);
  gimp_image_remove_layer(dest_image, dest_drawable_id);
  gimp_item_set_name(new_layer, dest_name);

  return new_layer;
}

/**
 * crop_layer:
 * @image_id:   id изображения которому принадлежит слой layer_id
 * @layer_id:   id обрезаемого слоя
 *
 * Обрезает слой до размеров содержащегося на нем изображения
 * (аналог "Autocrop Layer")
 * */
void
crop_layer (gint32 image_id,
             gint32 layer_id)
{
        gimp_layer_resize_to_image_size (layer_id);
        gimp_image_select_item (image_id,
                                   GIMP_CHANNEL_OP_REPLACE,
                                   layer_id);

        gint x1, x2, y1, y2;
        gboolean non_empty = TRUE;
        gimp_selection_bounds (image_id,
                                                          &non_empty,
                                                          &x1, &y1,
                                                          &x2, &y2);

        gimp_selection_none (image_id);

        gimp_layer_resize (layer_id,
                             x2 - x1, y2 - y1,
                             x1 * (-1), y1 * (-1));
}

/**
 * plugin_get_name:
 * @proc_name:  уникальное имя вызова функции gimp_install_procedure
 *
 * Return: название плагина в зависимости от имени функции вызова
 * */
const gchar*
plugin_get_name (const gchar *proc_name)
{
  if (g_strcmp0(PROCEDURE_NAME_1, proc_name) == 0)
    return TEXT_NAME_1;
  /*
  else if(g_strcmp0(PROCEDURE_NAME_2, proc_name) == 0)
    return TEXT_NAME_2;
  */
  return "";
}
