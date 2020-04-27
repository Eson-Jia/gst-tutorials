#include <gst/gst.h>

typedef struct
{
    GMainLoop *loop;
    GstElement *source_pipeline;
    GstElement *sink_pipeline;
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
//            source = gst_bin_get_by_name (GST_BIN (data->sink), "testsource");
//            gst_app_src_end_of_stream (GST_APP_SRC (source));
//            gst_object_unref (source);
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
    g_print("new sample\n");
    return GST_FLOW_OK;
}

int main(int argc, char *argv[])
{
    GstBus * bus;
    CustomData  data;
    GstElement * test_sink;
    gst_init (&argc, &argv);

    data.loop = g_main_loop_new (NULL, FALSE);

    data.sink_pipeline =  gst_parse_launch("filesrc location=/home/ubuntu/Videos/hunter.mkv ! decodebin ! audioconvert ! appsink name=app_sink",NULL);
    test_sink = gst_bin_get_by_name (GST_BIN (data.sink_pipeline), "app_sink");
    bus = gst_element_get_bus (data.sink_pipeline);
    gst_bus_add_watch (bus, (GstBusFunc) on_source_message, &data);
    gst_object_unref (bus);
    g_object_set(test_sink,"emit-signals",TRUE,NULL);
    g_signal_connect(test_sink, "new-sample", G_CALLBACK(new_sample),
                     &data);
    gst_object_unref(test_sink);
    gst_element_set_state (data.sink_pipeline, GST_STATE_PLAYING);
    g_main_run(data.loop);
    gst_element_set_state (data.sink_pipeline, GST_STATE_NULL);
    gst_object_unref (data.sink_pipeline);
    g_main_loop_unref (data.loop);
}
