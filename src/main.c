#include <string.h>
#include <gtk/gtk.h>


static void (*pre_hijacked_menu_bar_show)(GtkWidget *widget) = NULL;
static GObject* (*pre_hijacked_menu_bar_constructor)(GType type,
						     guint n_construct_properties,
						     GObjectConstructParam *construct_params);


static void toggle_menu(GtkWidget *window, GdkEventKey *event, gpointer user_data)
{
  GtkWidget *widget = GTK_WIDGET(user_data);
 
  if (event->keyval == 65513) {
    GTK_WIDGET_GET_CLASS(widget)->show = pre_hijacked_menu_bar_show;
    if (gtk_widget_get_visible(widget))
      gtk_widget_hide(widget);
    else
      gtk_widget_show(widget);
  }
}


static void anchor_event(GtkWidget *widget, GtkWidget *previous_toplevel, gpointer user_data)
{
  GtkWidget *toplevel = gtk_widget_get_toplevel(widget);
  
  g_return_if_fail(GTK_IS_MENU_BAR(widget));

  if (gtk_widget_is_toplevel(toplevel)) {
    if (previous_toplevel)
      g_signal_handlers_disconnect_by_func(previous_toplevel, G_CALLBACK(toggle_menu), widget);
    g_signal_connect(toplevel, "key-release-event", G_CALLBACK(toggle_menu), widget);
  }
}


static GObject* hijacked_menu_bar_constructor(GType type,
					      guint n_construct_properties,
					      GObjectConstructParam *construct_params)
{
  GObject *object = (*pre_hijacked_menu_bar_constructor)(type, n_construct_properties, construct_params);

  g_signal_connect(object, "hierarchy-changed", G_CALLBACK(anchor_event), NULL);
  g_signal_connect(object, "can-activate-accel", G_CALLBACK(gtk_true), NULL);

  return object;
}


static void hijacked_menu_bar_show(GtkWidget *widget)
{
  pre_hijacked_menu_bar_show(widget);
  gtk_widget_hide(widget);
}


G_MODULE_EXPORT
void gtk_module_init(gint *argc, gchar ***argv)
{
   GObjectClass *menu_bar_class;
   const gchar *app_whitelist = "gedit, file-roller, eog, transmission-gtk";

   if (strstr(app_whitelist, g_get_prgname()) == NULL)
     return;

   menu_bar_class = g_type_class_ref(GTK_TYPE_MENU_BAR);
      
   pre_hijacked_menu_bar_constructor = menu_bar_class->constructor;   
   menu_bar_class->constructor = hijacked_menu_bar_constructor;

   pre_hijacked_menu_bar_show = GTK_WIDGET_CLASS(menu_bar_class)->show;
   GTK_WIDGET_CLASS(menu_bar_class)->show = hijacked_menu_bar_show;
}
