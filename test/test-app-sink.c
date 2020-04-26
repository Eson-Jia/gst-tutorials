#include <gst/gst.h>

/* The appsink has received a buffer */
static GstFlowReturn
new_sample(GstElement *sink, GstElement *data)
{
    GstSample *sample;
    GstBuffer *buffer;

    /* Retrieve the buffer */
    g_signal_emit_by_name(sink, "pull-sample", &sample);
    if (sample)
    {
        buffer = gst_sample_get_buffer(sample);
        /* The only thing we do in this example is print a * to indicate a received buffer */
        g_print("*");
//        g_signal_emit_by_name(data,"push-buffer",buffer);
//        gst_buffer_unref(buffer);
        gst_sample_unref(sample);
        return GST_FLOW_OK;
    }

    return GST_FLOW_ERROR;
}

int main(int argc, char *argv[])
{
    GstElement *pipeline,*file_src,*caps_filter,*queue,* app_sink, *app_src,*fake_sink;
    GstBus *bus;
    GstMessage *msg;
    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Build the pipeline */
    pipeline = gst_pipeline_new("test-pipeline");

    file_src = gst_element_factory_make("filesrc","file_src");
    caps_filter =gst_element_factory_make("capsfilter","caps_filter");
    queue = gst_element_factory_make("queue","queue");
    app_sink= gst_element_factory_make("appsink","app_sink");
    app_src = gst_element_factory_make("appsrc","app_src");
    fake_sink = gst_element_factory_make("fakesink","fake_sink");
    if(!pipeline||
    !file_src||
    !caps_filter||
    !queue ||
    !app_sink ||
    !app_src ||
    !fake_sink
    )
    {
        g_printerr("some element init failed\n");
    }
    g_object_set(file_src, "location","/home/ubuntu/Videos/hunter.mkv",NULL);
    g_object_set(caps_filter, "caps",gst_caps_from_string("audio/x-raw"),NULL);
    g_object_set(app_sink, "emit-signals", TRUE,NULL);
    gst_bin_add_many(GST_BIN(pipeline),file_src,caps_filter,queue,app_sink,app_src,NULL);
    g_signal_connect(app_sink, "new-sample", G_CALLBACK(new_sample),
                     app_src);

    if(gst_element_link_many(file_src,caps_filter, queue,app_sink,NULL)!=TRUE
//       ||gst_element_link(app_src,fake_sink)!=TRUE
            ){
        g_printerr("some element linked error");
        return -1;
    }

    /* Start playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg =
            gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                       GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* Parse message */
    if (msg != NULL)
    {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(msg))
        {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("Error received from element %s: %s\n",
                           GST_OBJECT_NAME(msg->src), err->message);
                g_printerr("Debugging information: %s\n",
                           debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End-Of-Stream reached.\n");
                break;
            default:
                /* We should not reach here because we only asked for ERRORs and EOS */
                g_printerr("Unexpected message received.\n");
                break;
        }
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}
