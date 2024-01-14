#include <gst/gst.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <sstream>
#include <ctime>   // localtime
#include <iomanip> // put_time

static const char* DEFAUT_TIME_FMT = "%Y%m%d_%H%M%S";
//unit is ns, it is 60s by default
static const uint64_t MAX_SIZE_TIME = 60000000000;
static const std::string AUDIO_SRC_FACTORY = "audio_src";
static const std::string AUDIO_SINK_FACTORY = "audio_sink";
static const std::string AUDIO_DEVICE_NAME = "device";
static const std::string AUDIO_TEST_SRC = "audiotestsrc";
static const std::string AUDIO_AUTO_SINK = "autoaudiosink";
static const std::string SPLIT_MUX_SINK = "splitmuxsink";
static const std::string HLS_SINK = "hlssink2";
static const std::string FILE_SINK = "filesink";

/*
 * 
 * audio_src -> audioconvert -> audioresample -> capsfilter -> tee -> queue -> voaaacenc -> hlssink2
 * tee -> queue -> wavescope -> videoconvert -> autovideosink
 * tee -> queue -> autoaudiosink
 *
 * 
 gst-launch-1.0 -vv alsasrc device=$MIC_DEV_ID ! audioconvert ! audioresample \
   ! capsfilter caps="audio/x-raw,format=S16LE,rate=16000,channels=1" \
   ! voaacenc ! hlssink2 max-files=5 playlist-location=/tmp/asr_playlist.m3u8 location=/tmp/asr_record_%05d.ts playlist-root=/tmp target-duration=10    
 
 gst-launch-1.0 -vv osxaudiosrc device=1826 ! audioconvert ! audioresample \
   ! capsfilter caps="audio/x-raw,format=S16LE,rate=16000,channels=1" ! tee name=at ! queue \
   ! avenc_aac ! audiosink.audio ! hlssink2 name=audiosink max-files=5 playlist-location=/tmp/asr_playlist.m3u8 location=/tmp/asr_record_%05d.ts playlist-root=/tmp target-duration=10    
 
 gst-launch-1.0 osxaudiosrc ! audioconvert ! audioresample 
 ! capsfilter caps="audio/x-raw,format=S16LE,rate=16000,channels=1"  
 ! tee ! queue ! audioconvert ! avenc_aac ! hlssink2 max-files=5

 */

struct ElementDesc {
    ElementDesc(const std::string& name, const std::string& factory, GstElement*& element)
    : m_name(name)
    , m_factory(factory)
    , m_element(element)
    , m_desc("") {
        
    }

    bool create_element() {
        m_element = gst_element_factory_make(m_factory.c_str(), m_name.c_str());
        if (!m_element) {
            std::cerr << "create failed for " << m_name << std::endl;
            return false;
        }
        return true;
    }
    void set_desc(const std::string& desc) { m_desc = desc; }

    std::string m_name;
    std::string m_factory;
    GstElement*& m_element;

    std::string m_desc;
};

class GstAudioRecorder  {
public:
    GstAudioRecorder() {
    }
    virtual ~GstAudioRecorder() = default;
    void set_option(const std::string& option_name, const std::string& option_value);
    
    int init() ;
    void clean();
    int start();

protected:

    int make_elements();
    int add_elements();
    int link_elements();

private:
    std::string m_audio_src_factory = AUDIO_TEST_SRC;
    std::string m_audio_device_name = "";
    std::string m_audio_sink_factory = AUDIO_AUTO_SINK;

    GstElement *m_pipeline = nullptr;
    GstBus     *m_bus = nullptr;
    GstMessage *m_msg = nullptr;

    GstElement *m_source = nullptr;
    GstElement *m_audio_convert = nullptr;
    GstElement *m_audio_resample = nullptr;
    GstElement *m_caps_filter = nullptr;

    GstElement *m_tee = nullptr;
    GstElement *m_record_queue = nullptr;
    GstElement *m_sink = nullptr;

    GstElement *m_audio_convert2 = nullptr;
    GstElement *m_aac_enc = nullptr;
    GstElement *m_wav_enc = nullptr;
    GstElement *m_play_queue = nullptr;
    GstElement *m_play_sink = nullptr;

    std::vector<ElementDesc> m_elements_desc;
};


void GstAudioRecorder::set_option(const std::string& option_name, const std::string& option_value) {
    if(AUDIO_SRC_FACTORY == option_name) {
        m_audio_src_factory = option_value;
    } else if(AUDIO_DEVICE_NAME == option_name) {
        m_audio_device_name = option_value;
    } else if(AUDIO_SINK_FACTORY == option_name) {
        m_audio_sink_factory = option_value;
    } else {
        std::cerr << "unknown option name" << option_name << std::endl;
    }
}

int GstAudioRecorder::make_elements() {
            /* Create the elements */
        m_source = gst_element_factory_make(m_audio_src_factory.c_str(), "source");
        
        if (m_audio_src_factory == AUDIO_TEST_SRC) {
            g_object_set(m_source, "wave", 5, NULL);
            g_object_set(m_source, "num-buffers", 100, NULL);
        } else {
            if (!m_audio_device_name.empty()) {
                g_object_set(m_source, "device", m_audio_device_name.c_str(), NULL);
            }
        }

        m_audio_convert = gst_element_factory_make("audioconvert", "audioconvert");
        m_audio_convert2 = gst_element_factory_make("audioconvert", "audioconvert2");

        m_audio_resample = gst_element_factory_make("audioresample", "audioresample");
        m_caps_filter = gst_element_factory_make("capsfilter", "capsfilter");

        GstCaps *caps = gst_caps_new_simple ("audio/x-raw",
                        "format", G_TYPE_STRING, "S16LE",
                        "rate", G_TYPE_INT, 16000,
                        "channels", G_TYPE_INT, 1,
                        NULL);
        g_object_set(G_OBJECT(m_caps_filter), "caps", caps, nullptr);


        m_tee = gst_element_factory_make("tee", "tee");
        m_record_queue = gst_element_factory_make("queue", "record_queue");
        m_aac_enc = gst_element_factory_make("voaacenc", "aac_enc");
        m_sink = gst_element_factory_make(m_audio_sink_factory.c_str(), "sink");
        
        //m_play_queue = gst_element_factory_make("queue", "play_queue");
        //m_play_sink =  gst_element_factory_make("autoaudiosink", "play_sink");

        if (m_audio_sink_factory == SPLIT_MUX_SINK) {
            m_wav_enc = gst_element_factory_make("wavenc", "wavenc");

            //g_object_set(G_OBJECT(m_sink), "async-finalize", true, nullptr);
            g_object_set(G_OBJECT(m_sink), "location", "test_audio.wav", nullptr);
            g_object_set(G_OBJECT(m_sink), "muxer", m_wav_enc, nullptr);
            g_object_set(G_OBJECT(m_sink), "max-size-time", MAX_SIZE_TIME, nullptr);

        } else if (m_audio_sink_factory == HLS_SINK) {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), DEFAUT_TIME_FMT);
    
            std::string playlist_filename = "/tmp/playlist_";
            playlist_filename.append(ss.str());
            playlist_filename.append(".m3u8");

            std::string record_filename = "/tmp/record_";
            playlist_filename.append(ss.str());
            playlist_filename.append("_%03d.ts");

            g_object_set(m_sink, "location", record_filename.c_str(), NULL);
            g_object_set(m_sink, "playlist-location", playlist_filename.c_str(), NULL);
            //g_object_set(m_sink, "playlist-root", "/tmp", NULL);
            g_object_set(m_sink, "playlist-length", 20, NULL);
            g_object_set(m_sink, "max-files", 20, NULL);
            g_object_set(m_sink, "target-duration", 10, NULL);
        }

        if (!m_source || !m_audio_convert || !m_audio_resample || !m_caps_filter || !m_tee || !m_record_queue || !m_aac_enc || !m_sink) {
            std::cerr << "Not all elements could be created." << std::endl;
            return -1;
        }

        return 0;
}

int GstAudioRecorder::add_elements() {
    /* add the elements */
    gst_bin_add_many(GST_BIN(m_pipeline), m_source, m_audio_convert, m_audio_convert2, m_audio_resample, m_caps_filter,
         m_tee, m_record_queue, m_aac_enc, m_sink, NULL);
    return 0;
}

int GstAudioRecorder::link_elements() {
    /* link the elements as record pipeline */
    bool link_ret = gst_element_link_many(m_source, m_audio_convert, m_audio_resample, 
         m_caps_filter, m_tee, m_record_queue, m_audio_convert2, m_aac_enc, m_sink, NULL);

    if (!link_ret) {
        std::cerr << "Elements could not be linked" << std::endl;
        return -2;
    }
    
    /* 
    link_ret = gst_element_link_many(m_tee, m_play_queue, m_play_sink, NULL);

    if (!link_ret) {
        std::cerr << "Elements could not be linked for tee->queue->autoaudiosink" << std::endl;
        return -3;
    }*/
    return 0;
}

int GstAudioRecorder::init() {
        gst_init(NULL, NULL);

        /* Create the empty pipeline */
        m_pipeline = gst_pipeline_new("test-pipeline");

        if (!m_pipeline) {
            std::cerr << "pipeline could be created." << std::endl;
            return -1;
        }
        
        int ret = make_elements();
        if (ret !=0 ) std::cerr << "make_elements failed." << std::endl;
        ret = add_elements();
        if (ret !=0 ) std::cerr << "add_elements failed." << std::endl;

        ret = link_elements();
        if (ret !=0 ) std::cerr << "link_elements failed." << std::endl;  

        return ret;
}


void GstAudioRecorder::clean() {
    /* Free resources */
    gst_object_unref(m_bus);
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    gst_object_unref(m_pipeline);
}


int GstAudioRecorder::start() {

    /* Start playing */
    GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        gst_object_unref (m_pipeline);
        std::cerr << "Unable to set the pipeline to the playing state." << std::endl;
        return -1;
    }

    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN_CAST(m_pipeline), GST_DEBUG_GRAPH_SHOW_VERBOSE, "record_pipeline");

    m_bus = gst_element_get_bus(m_pipeline);
    m_msg = gst_bus_timed_pop_filtered(m_bus, GST_CLOCK_TIME_NONE,
            (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Parse message */
    if (m_msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(m_msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(m_msg, &err, &debug_info);
                g_printerr("Error received from element %s: %s\n",
                        GST_OBJECT_NAME(m_msg->src), err->message);
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
        gst_message_unref(m_msg);
    }
    return 0;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cout << "Usage  : " << argv[0] 
            << " --audio_src=osxaudiosrc|alsasrc|audiotestsrc"
            << " --device=<device_id>"
            << " --audio_sink=<splitmuxsink|hlssink2>" 
            << std::endl;
        std::cout << "Example: " << argv[0] 
            << " --audio_src=osxaudiosrc"
            << " --device=1832"
            << " --audio_sink=hlssink2" 
            << std::endl;
        return 1; 
    }

    std::map<std::string, std::string> arguments;


    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Check if the argument is of the form "--name=value"
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string name = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            arguments[name] = value;
        } else {
            // Handle other types of arguments if needed
            std::cout << "Unknown argument: " << arg << std::endl;
        }
    }


    auto recorder = std::make_unique<GstAudioRecorder>();
    
    for (const auto& pair : arguments) {
        std::cout << "Argument: " << pair.first << ", Value: " << pair.second << std::endl;
        recorder->set_option(pair.first, pair.second);
    }

    int ret = recorder->init();
    if (ret < 0) {
        std::cerr <<"initialize pipeline failed:" << ret << std::endl;
        return ret;
    }

    ret = recorder->start();
    if (ret < 0) {
        std::cerr <<"start pipeline failed:" << ret << std::endl;
        return ret;
    }
    recorder->clean();
    return 0;
}

