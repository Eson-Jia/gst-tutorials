#include <gst/gst.h>
#include <gst/app/app.h>

typedef struct
{
    GMainLoop *loop;
    GstElement *src_pipeline;
    GstElement *sink_pipeline;
    GstElement *app_src;
} CustomData;


static gboolean
on_source_message (GstBus * bus, GstMessage * message, CustomData * data)
{
    GError *err;
    gchar *debug_info;
    g_print ("%s\n", __func__);

    switch (GST_MESSAGE_TYPE (message)) {
        case GST_MESSAGE_EOS:
            g_print ("The source got dry\n");
            gst_app_src_end_of_stream (GST_APP_SRC (data->app_src));
            gst_object_unref (data->app_src);
            break;
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(message, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n",
                       GST_OBJECT_NAME(message->src), err->message);
            g_printerr("Debugging information: %s\n",
                       debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            g_print ("Received error\n");
            g_main_loop_quit (data->loop);
            break;
        default:
            g_print("default\n");
            break;
    }
    return TRUE;
}

static gboolean
on_sink_message (GstBus * bus, GstMessage * message, CustomData * data)
{
    GError *err;
    gchar *debug_info;
    g_print ("%s\n", __func__);

    switch (GST_MESSAGE_TYPE (message)) {
        case GST_MESSAGE_EOS:
            g_print ("The source got dry\n");
            g_main_loop_quit(data->loop);
            break;
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(message, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n",
                       GST_OBJECT_NAME(message->src), err->message);
            g_printerr("Debugging information: %s\n",
                       debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            g_print ("Received error\n");
            g_main_loop_quit (data->loop);
            break;
        default:
            g_print("default\n");
            break;
    }
    return TRUE;
}

static GstFlowReturn
new_sample(GstElement *sink, CustomData *data)
{
    GstBuffer * buffer,*app_buffer;
    GstSample *sample;
    GstFlowReturn ret;

    /* Retrieve the buffer */
//    g_print("new sample\n");
    g_signal_emit_by_name(sink, "pull-sample", &sample);
    buffer = gst_sample_get_buffer(sample);
    app_buffer = gst_buffer_copy_deep(buffer);
    gst_sample_unref(sample);
    ret = gst_app_src_push_buffer (GST_APP_SRC (data->app_src), app_buffer);
    return ret;
}

int main(int argc, char *argv[])
{
    GstBus * bus;
    GError *err;
    CustomData  data;
    GstElement * app_sink;
    gst_init (&argc, &argv);

    data.loop = g_main_loop_new (NULL, FALSE);

    data.sink_pipeline =  gst_parse_launch("filesrc location=/home/ubuntu/Videos/angle.mp4 ! decodebin ! audioconvert ! avenc_aac ! aacparse ! appsink name=app_sink",NULL);
    bus = gst_element_get_bus (data.sink_pipeline);
    gst_bus_add_watch (bus, (GstBusFunc) on_source_message, &data);
    gst_object_unref (bus);
    app_sink = gst_bin_get_by_name (GST_BIN (data.sink_pipeline), "app_sink");
    g_object_set(app_sink, "emit-signals", TRUE, NULL);
    g_signal_connect(app_sink, "new-sample", G_CALLBACK(new_sample),
                     &data);
    gst_object_unref(app_sink);
    data.src_pipeline = gst_parse_launch("appsrc name=app_src ! filesink location=/home/ubuntu/Videos/hunter.audio", NULL);
    data.app_src = gst_bin_get_by_name(GST_BIN(data.src_pipeline), "app_src");
    bus = gst_element_get_bus(data.src_pipeline);
    gst_bus_add_watch (bus, (GstBusFunc) on_sink_message, &data);
    gst_object_unref (bus);
    gst_element_set_state (data.sink_pipeline, GST_STATE_PLAYING);
    gst_element_set_state (data.src_pipeline, GST_STATE_PLAYING);
    g_main_run(data.loop);
    gst_element_set_state (data.sink_pipeline, GST_STATE_NULL);
    gst_element_set_state (data.src_pipeline, GST_STATE_NULL);
    gst_object_unref (data.sink_pipeline);
    gst_object_unref(data.src_pipeline);
    g_main_loop_unref (data.loop);
}
