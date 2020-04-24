#include <gst/gst.h>
#include <gst/audio/audio.h>
#include <stdio.h>
#include <stdlib.h>

// uridecodebin >> audio_0 >> appsink >> queue >> appsrc  >> udpsink

static void
start_feed(GstElement *source, guint size)
{
    g_print("start feed\n");
}

static void
stop_feed(GstElement *source, guint size)
{
    g_print("stop feed\n");
}

static GstFlowReturn
new_sample(GstElement *sink, GstElement *src)
{
    GstSample *sample;
    GstBuffer *buffer;
    GstFlowReturn ret;
    g_print("We got new sample.\n");
    /* Retrieve the buffer */
    g_signal_emit_by_name(sink, "pull-sample", &sample);

    if (sample)
    {
        buffer = gst_sample_get_buffer(sample);
        g_signal_emit_by_name(src, "push-buffer", buffer, &ret);
        gst_sample_unref(sample);
        gst_buffer_unref(buffer);
        if (ret != GST_FLOW_OK)
        {
            return GST_FLOW_ERROR;
        }
    }
    return GST_FLOW_ERROR;
}

static void pad_added_handler(GstElement *src, GstPad *new_pad, GstElement *app_src)
{
    GstPad *sink_pad = gst_element_get_static_pad(app_src, "sink");
    GstPadLinkReturn ret;
    GstCaps *new_pad_caps = NULL;
    GstStructure *new_pad_struct = NULL;
    const gchar *new_pad_type = NULL;

    g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));

    /* If our converter is already linked, we have nothing to do here */
    if (gst_pad_is_linked(sink_pad))
    {
        g_print("We are already linked. Ignoring.\n");
        goto exit;
    }

    /* Check the new pad's type */
    new_pad_caps = gst_pad_get_current_caps(new_pad);
    new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    new_pad_type = gst_structure_get_name(new_pad_struct);
    if (!g_str_has_prefix(new_pad_type, "audio/x-raw"))
    {
        g_print("It has type '%s' which is not raw audio. Ignoring.\n", new_pad_type);
        goto exit;
    }

    /* Attempt the link */
    ret = gst_pad_link(new_pad, sink_pad);
    if (GST_PAD_LINK_FAILED(ret))
    {
        g_print("Type is '%s' but link failed.\n", new_pad_type);
    }
    else
    {
        g_print("Link succeeded (type '%s').\n", new_pad_type);
    }

exit:
    /* Unreference the new pad's caps, if we got them */
    if (new_pad_caps != NULL)
        gst_caps_unref(new_pad_caps);

    /* Unreference the sink pad */
    gst_object_unref(sink_pad);
}

static void
error_cb(GstBus *bus, GstMessage *msg, GMainLoop *main_loop)
{
    GError *err;
    gchar *debug_info;

    /* Print error details on the screen */
    gst_message_parse_error(msg, &err, &debug_info);
    g_printerr("Error received from element %s: %s\n",
               GST_OBJECT_NAME(msg->src), err->message);
    g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
    g_clear_error(&err);
    g_free(debug_info);

    g_main_loop_quit(main_loop);
}

int main(int argc, char *argv[])
{
    GstElement *pipeline, *src, *queue, *app_sink, *app_src, *udp_sink;
    GstBus *bus;
    GstMessage *msg;
    GstAudioInfo info;
    GstStateChangeReturn ret;
    GMainLoop *main_loop;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    src = gst_element_factory_make("uridecodebin", "source");
    queue = gst_element_factory_make("queue", "audio_queue");
    app_sink = gst_element_factory_make("appsink", "app_sink");
    app_src = gst_element_factory_make("appsrc", "app_src");
    udp_sink = gst_element_factory_make("udpsink", "udp_sink");
    /* Create the empty pipeline */
    pipeline = gst_pipeline_new("test-pipeline");

    if (
        !src ||
        !queue ||
        !app_sink ||
        !app_src ||
        !udp_sink ||
        !pipeline)
    {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }
    g_object_set(app_sink, "emit-signals", TRUE, NULL);
    g_signal_connect(app_sink, "new-sample", G_CALLBACK(new_sample),
                     app_src);
    g_signal_connect(app_src, "need-data", G_CALLBACK(start_feed),
                     NULL);
    g_signal_connect(app_src, "enough-data", G_CALLBACK(stop_feed),
                     NULL);

    g_signal_connect(src, "pad-added", G_CALLBACK(pad_added_handler), queue);

    g_object_set(src, "uri",
                 "file:///home/ubuntu/Videos/hunter.mkv",
                 //  "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
                 NULL);
    g_object_set(udp_sink, "bind-port",
                 1234,
                 NULL);
    g_object_set(udp_sink, "host",
                 "localhost",
                 NULL);
    g_object_set(udp_sink, "port",
                 8224,
                 NULL);
    gst_bin_add_many(GST_BIN(pipeline),
                     src,
                     queue,
                     app_sink,
                     app_src,
                     udp_sink,
                     NULL);
    if (
        gst_element_link_many(queue, app_sink, NULL) != TRUE ||
        gst_element_link_many(app_src, udp_sink, NULL) != TRUE)
    {
        g_printerr("Not all elements could be linked.\n");
        return -1;
    }

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    gst_bus_add_signal_watch(bus);
    g_signal_connect(G_OBJECT(bus), "message::error", (GCallback)error_cb,
                     main_loop);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);
    g_print("end loop\n");
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}