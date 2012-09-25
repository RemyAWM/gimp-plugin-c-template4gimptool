/*
 * gpl-dialog.c
 *
 *  Created on: 20.09.2012
 *      Author: remy
 */


#define MAIN_BOX_SPACING        12
#define CONTAINER_BORDER        3
#define ALIGNMENT_PADDING       6

#define SCALE_WIDTH                     120
#define SPIN_BUTTON_WIDTH               75

#define SOFT_CONNECT(GOBJECT, DATA)    g_signal_connect (GOBJECT, \
                                                        "button_press_event",              \
                                                        G_CALLBACK (event_changed),        \
                                                        DATA);                             \
                                        g_signal_connect (GOBJECT, \
                                                        "button_release_event",            \
                                                        G_CALLBACK (event_changed),        \
                                                        DATA);


static gboolean btPressed       =       FALSE;
static int *lastEddectCode      =       NULL;


static void             dialog_toggle_update    (GtkWidget *widget,
                                                     gpointer data);

static void             options_changed         (GtkWidget *widget,
                                                    gpointer data);

static gboolean         event_changed           (GtkWidget* widget,
                                                    GdkEvent * event,
                                                    gpointer user_data);

/* Включить/Отключить систему "мягкого" изменения
 * параметров плагина
 * */
const id_soft_changing = FALSE;


/* -------------    Секция реализации   ------------- */

/**
 * dialog:
 * @image_preview:      id изображения, которое содержит слой предосмотра
 * @drawable_preview:   id слоя предосмотра
 * @image_work:         id изображения, внутри которого будет создаваться эффект
 * @effect_func:        указатель на функцию, которая создает эффект
 * @values:             указатель на структуру с параметрами (свойствами) плагина
 * @name:               уникальное имя функции вызова плагина
 *
 * Функция создает диалоговое окно плагина
 *
 * Return: результат закрытия диалогового окна.
 * Если без сохранения результатов (кнопка Cancel), тогда - GIMP_PDB_CANCEL
 * Иначе (кнопка OK) - GIMP_PDB_SUCCESS
 */
GimpPDBStatusType
dialog  (gint32 image_preview,
          gint32 drawable_preview,
          gint32 image_work,
          gpointer effect_func,
          PlugInVals *values,
          const gchar *name)
{
    GtkWidget               *dlg;
    GtkWidget               *main_vbox;
    GtkWidget               *frame;
    GtkWidget               *alignment;
    GtkWidget               *table;
    GtkWidget               *prop_widget;
    GtkObject               *prop_adj;

    gboolean                run;

    /* Указатель на переменную, которая хранит данные передаваемые
     * от диалога в функцию предосмотра - live_preview_update()
     * */
    PlugInPreviewData       *data   =       g_new(PlugInPreviewData,1);
    data->image_work                =       image_work;
    data->image_preview             =       image_preview;
    data->layer_preview             =       drawable_preview;
    data->values                    =       values;
    data->effect_func               =       effect_func;

    gimp_ui_init (PLUGIN_BINARY, TRUE);

    dlg = gimp_dialog_new (plugin_get_name (name), PLUGIN_ROLE,
                                                     NULL, 0,
                                                     gimp_standard_help_func, name,
                                                     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                     GTK_STOCK_OK,     GTK_RESPONSE_OK,
                                                     NULL);

    gimp_window_set_transient (GTK_WINDOW (dlg));
    gtk_window_set_keep_above(GTK_WINDOW(dlg),TRUE);

    main_vbox = gtk_vbox_new (FALSE, MAIN_BOX_SPACING);
    gtk_container_set_border_width (GTK_CONTAINER (main_vbox), CONTAINER_BORDER);
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dlg)->vbox), main_vbox);

    /* Добавляем фрейм */
    frame = gtk_frame_new (NULL);
    gtk_container_set_border_width (GTK_CONTAINER (frame), CONTAINER_BORDER);
    gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 0);
    gtk_widget_show (frame);

    alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment),
                                  ALIGNMENT_PADDING, ALIGNMENT_PADDING,
                                  ALIGNMENT_PADDING, ALIGNMENT_PADDING);
    gtk_container_add (GTK_CONTAINER (frame), alignment);
    gtk_widget_show (alignment);

    /* Добавляем таблицу в которой будут расположены
     * виджеты настроек параметров плагина
     * */
    table =  gtk_table_new (4, 3, FALSE);
    gtk_container_add (GTK_CONTAINER (alignment), table);
    gtk_widget_show (table);

    /*
     * Добавляем виджеты настроек плагина
     * ****************************** */

    /* Настройка свойства Size effect */
    prop_adj = gimp_scale_entry_new (GTK_TABLE (table),
                                      0,
                                      0,
                                      "Size effect:",
                                      SCALE_WIDTH,
                                      SPIN_BUTTON_WIDTH,
                                      values->effect_size,
                                      0,
                                      100,
                                      0.1,
                                      1,
                                      2,
                                      TRUE,
                                      0,0,
                                      "Set amount size for image effect. (Range: 0% - 100%)",
                                      NULL);

    g_signal_connect (prop_adj, "value_changed",
                      G_CALLBACK (gimp_double_adjustment_update),
                      &values->effect_size);

    SOFT_CONNECT(GIMP_SCALE_ENTRY_SCALE(prop_adj), data)

    /* При изменении свойства  "Size effect" обновляем предосмотр  */
    g_signal_connect (prop_adj, "value_changed",
                      G_CALLBACK (options_changed),
                      data);

    /* Отображаем в предосмотре эффект с данными установленными при вызове плагина: */
    live_preview_update (NULL, data);

    /* Отображаем диалог */
    gtk_widget_show (main_vbox);
    gtk_widget_show (dlg);

    run = (gimp_dialog_run (GIMP_DIALOG (dlg)) == GTK_RESPONSE_OK);

    /* Освобождаем выделенную память */
    g_free (data);
    gtk_widget_destroy (dlg);

    /* Возвращаем соответствующее значение,
     * в зависимости от того, какой кнопкой был закрыт диалог
     * */
    if(run)
        return GIMP_PDB_SUCCESS;
    else
        return GIMP_PDB_CANCEL;
}

/**
 * dialog_toggle_update:
 * @widget:             кнопка toggle или чекбокс
 * @data:               указатель на связанную переменную типа gboolean (bool)
 *
 * Слот для обработки событий от toggle-кнопок
 */
static void
dialog_toggle_update (GtkWidget *widget,
                      gpointer data)
{
    GtkToggleButton *button = GTK_TOGGLE_BUTTON (widget);
    gboolean *state = data;
    *state = !gtk_toggle_button_get_active (button);
}

/* Функции реализующие систему "мягкого" изменения параметров */
static void
options_changed (GtkWidget *widget,
                   gpointer data)
{
        if(btPressed && id_soft_changing)
                return;

        live_preview_update(widget,data);
}

static gboolean
event_changed (GtkWidget* widget,
                 GdkEvent * event,
                 gpointer user_data)
{
        if (event->type == GDK_BUTTON_PRESS)
        {
                btPressed=TRUE;
                lastEddectCode=user_data;
        }
        else if(event->type == GDK_BUTTON_RELEASE)
        {
                btPressed = FALSE;
                options_changed(widget, lastEddectCode);
                lastEddectCode=NULL;
        }

        return FALSE;
}
