#include "gui.h"

struct WindowAndStream {
    GtkWidget* window = nullptr;
    GtkMediaStream* stream = nullptr;
};

static gboolean timeout_func_(gpointer data) {

    WindowAndStream* window_stream = (WindowAndStream*)data;

    gtk_media_stream_set_playing(window_stream->stream, FALSE);

    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    GtkCssProvider* css = gtk_css_provider_new();
    gtk_css_provider_load_from_string(css, "label { font-size: 50pt;}");
    gtk_style_context_add_provider_for_display(gtk_widget_get_display(box), GTK_STYLE_PROVIDER(css), 1);

    GtkWidget* label = gtk_label_new("Жопа взломана");

    gtk_box_append(GTK_BOX(box), label);

    gtk_window_set_child(GTK_WINDOW(window_stream->window), box);

    free(data);

    return FALSE;
}

static void activate_(GtkApplication* app, gpointer user_data) {
    (void) user_data;

    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Взлом жопы");
    gtk_window_set_default_size(GTK_WINDOW(window), 700, 700);

    GtkMediaStream* stream = gtk_media_file_new_for_filename("video.mp4");

    GtkWidget* video =  gtk_image_new_from_paintable(GDK_PAINTABLE(stream));

    gtk_media_stream_set_playing(stream, TRUE);
    gtk_media_stream_set_loop(stream, TRUE);

    gtk_window_set_child (GTK_WINDOW(window), video);

    gtk_window_present (GTK_WINDOW(window));

    WindowAndStream* window_stream = (WindowAndStream*)calloc(1, sizeof(WindowAndStream));
    *window_stream = {.window = window, .stream = stream};

    g_timeout_add(10000, timeout_func_, window_stream);
}

bool start_gtk_window() {

    GtkApplication* app = gtk_application_new("org.gtk.patcher", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate_), NULL);

    int status = g_application_run(G_APPLICATION(app), 0, nullptr);
    g_object_unref(app);

    return status == 0;
}
