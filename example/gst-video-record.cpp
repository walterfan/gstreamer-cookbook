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

static const std::string VIDEO_SRC_FACTORY = "video_src";
static const std::string VIDEO_SINK_FACTORY = "video_sink";
static const std::string VIDEO_DEVICE_NAME = "device";
static const std::string VIDEO_TEST_SRC = "videotestsrc";
static const std::string VIDEO_AUTO_SINK = "autovideosink";
static const std::string SPLIT_MUX_SINK = "splitmuxsink";
static const std::string HLS_SINK = "hlssink2";
static const std::string FILE_SINK = "filesink";

static const std::string DELIMITER = " ";

/*
 * 
gst-launch-1.0 -vv filesrc location=talk.mp4 \
 ! decodebin name=dec ! videoconvert ! clockoverlay ! x264enc ! h264parse \
 ! hlssink2 max-files=10 playlist-location=/tmp/v_playlist.m3u8 location=/tmp/v_record_%05d.ts playlist-root=/tmp target-duration=10
 

gst-launch-1.0 videotestsrc is-live=true ! x264enc ! h264parse ! hlssink2 max-files=5 
 
 */
template<class T>
T str_to_num(const std::string timestr) {
    T value;
    std::istringstream iss(timestr);
    iss >> value;
    return value;
}


static std::string str_tolower(std::string s)
{
    for(char &c : s)
        c = tolower(c);
    return s;
}

static bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

static inline std::string &trim(std::string &str)
{
    const char *spaces = " \n\r\t";
    str.erase(str.find_last_not_of(spaces) + 1);
    str.erase(0, str.find_first_not_of(spaces));
    return str;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

struct ElementDesc {
    ElementDesc(const std::string& name, const std::string& factory, GstElement*& element)
    : m_name(name)
    , m_factory(factory)
    , m_element(element)
    , m_desc("") {
        
    }

    ElementDesc(const std::string& name, 
        const std::string& factory, 
        const std::string& desc, 
        GstElement*& element)
    : m_name(name)
    , m_factory(factory)
    , m_element(element)
    , m_desc(desc) {
        parse_desc(m_desc);
    }

    bool create_element() {
        m_element = gst_element_factory_make(m_factory.c_str(), m_name.c_str());
        if (!m_element) {
            std::cerr << "create failed for " << m_name << std::endl;
            return false;
        }
        std::cout << "create_element: " << m_name << std::endl;
        return true;
    }

    void set_desc(const std::string& desc) { m_desc = desc; parse_desc(m_desc);}
    void parse_desc(const std::string& desc);
    void parse_prop(const std::string& prop);
    

    std::string m_name;
    std::string m_factory;
    GstElement*& m_element;
    std::string m_desc;
    std::map<std::string, std::string> m_props;
};


void ElementDesc::parse_prop(const std::string& token) {
    if (token.empty()) {
        return;
    }
    
    size_t pos = 0;
    if ((pos = token.find("=")) != std::string::npos) {
        m_props.insert(std::make_pair<std::string, std::string>(trim_copy(token.substr(0, pos)),
            trim_copy(token.substr(pos+1))));
    } else {
        
        if (m_factory.empty()) {
            m_factory = token;
        }

        if (m_name.empty()) {
            m_name = token;
        }
    }
}
void ElementDesc::parse_desc(const std::string& desc) {
    size_t last = 0; 
    size_t next = 0; 
    while ((next = desc.find(DELIMITER, last)) != std::string::npos) {  
        std::string token = desc.substr(last, next-last);
        trim(token);
        parse_prop(token);
        last = next + 1; 
    } 
    parse_prop(desc.substr(last));
}

class GstVideoRecorder  {
public:
    GstVideoRecorder() {
    }
    virtual ~GstVideoRecorder() = default;
    void set_option(const std::string& option_name, const std::string& option_value);
    
    int init() ;
    void clean();
    int start();

protected:
    int define_elements();
    int make_elements();
    int add_elements();
    int link_elements();
    int add_probe();

private:
    std::string m_video_src_factory = VIDEO_TEST_SRC;
    std::string m_video_device_name = "";
    std::string m_video_sink_factory = VIDEO_AUTO_SINK;

    GstElement *m_pipeline = nullptr;
    GstBus     *m_bus = nullptr;
    //GstMessage *m_msg = nullptr;


    GstElement *m_source = nullptr;
    GstElement *m_clock_overlay = nullptr;
    GstElement *m_video_encode = nullptr;
    GstElement *m_video_parse = nullptr;
    //GstElement *m_clock_overlay2 = nullptr;
    GstElement *m_sink = nullptr;

    std::vector<ElementDesc> m_elements_desc;
};


void GstVideoRecorder::set_option(const std::string& option_name, const std::string& option_value) {
    if(VIDEO_SRC_FACTORY == option_name) {
        m_video_src_factory = option_value;
    } else if(VIDEO_DEVICE_NAME == option_name) {
        m_video_device_name = option_value;
    } else if(VIDEO_SINK_FACTORY == option_name) {
        m_video_sink_factory = option_value;
    } else {
        std::cerr << "unknown option name" << option_name << std::endl;
    }
}


int GstVideoRecorder::define_elements() {
    m_elements_desc.emplace_back(ElementDesc("videotestsrc", "videotestsrc", "is-live=true num-buffers=500", m_source));
    m_elements_desc.emplace_back(ElementDesc("clockoverlay", "clockoverlay", "halignment=0 valignment=2", m_clock_overlay));
    m_elements_desc.emplace_back(ElementDesc("x264enc", "x264enc", "", m_video_encode));
    m_elements_desc.emplace_back(ElementDesc("h264parse", "h264parse", "", m_video_parse));
    //m_elements_desc.emplace_back(ElementDesc("clockoverlay2", "clockoverlay", "halignment=2 valignment=2", m_clock_overlay2));
    m_elements_desc.emplace_back(ElementDesc("hlssink2", "hlssink2", "max-files=10 location=/tmp/w_record_%05d.ts playlist-location=/tmp/w_playlist.m3u8", m_sink));
    return 0;
}

int GstVideoRecorder::make_elements() {
    for(auto& desc: m_elements_desc) {
        desc.create_element();

        for(auto& kv: desc.m_props) {
            if(is_number(kv.second)) {
                g_object_set(desc.m_element, kv.first.c_str(), str_to_num<int32_t>(kv.second), NULL);    
            } else {
                std::string lower_value = str_tolower(kv.second);
                if(lower_value == "true") {
                    g_object_set(desc.m_element, kv.first.c_str(), 1, NULL);
                } else if(lower_value == "false") {
                    g_object_set(desc.m_element, kv.first.c_str(), 0, NULL);
                } else {
                    g_object_set(desc.m_element, kv.first.c_str(), kv.second.c_str(), NULL);
                }
            }
            std::cout << "set prop: " << kv.first << "=" << kv.second << std::endl;
        }
    }
    return 0;
}

int GstVideoRecorder::add_elements() {
    for(auto& desc: m_elements_desc) {
        if(!gst_bin_add(GST_BIN(m_pipeline), desc.m_element)) { 
            std::cerr << "add element failed: " << desc.m_name << std::endl;
            return -1;
        } 
        std::cout << "add_element: " << desc.m_name << std::endl;
    }
    return 0;
}

int GstVideoRecorder::link_elements() {
    bool ret = gst_element_link_many(m_source, 
        m_clock_overlay, 
        m_video_encode, 
        m_video_parse, 
        //m_clock_overlay2,
        m_sink, NULL);

    if (!ret) {
        std::cerr << "link elements failed" << std::endl;
        return -1;
    }
    std::cout << "link_elements succeed" << std::endl;

    return 0;
}


static GstPadProbeReturn cb_have_event(GstPad *pad, GstPadProbeInfo *info,
                                    gpointer user_data)
{

    GstEvent *event = gst_pad_probe_info_get_event(info);
    if (event)
      g_print("cb_hava_event: event type=%s from %s\n", GST_EVENT_TYPE_NAME(event),
              (char *)user_data);

    return GST_PAD_PROBE_OK;
}

static GstPadProbeReturn cb_have_query(GstPad *pad, GstPadProbeInfo *info,
                                gpointer user_data)
{

    GstQuery *query = gst_pad_probe_info_get_query(info);

    g_print("cb_hava_query: query type=%s from %s\n", GST_QUERY_TYPE_NAME(query),
          (char *)user_data);

    return GST_PAD_PROBE_OK;
}

static void add_pad_probe(GstPad *pad_to_probe, const char *evt_pad_name)
{


    // GST_PAD_PROBE_TYPE_EVENT_BOTH
    gulong probe_id_2 = gst_pad_add_probe(pad_to_probe,
                                          GST_PAD_PROBE_TYPE_EVENT_BOTH, cb_have_event,
                                          (gpointer)evt_pad_name, NULL);

    // GST_PAD_PROBE_TYPE_QUERY_BOTH
    gulong probe_id_3 = gst_pad_add_probe(pad_to_probe,
                                          GST_PAD_PROBE_TYPE_QUERY_BOTH, cb_have_query,
                                          (gpointer)evt_pad_name, NULL);

}

int GstVideoRecorder::add_probe() {
    GstPad* sink_pad = gst_element_get_static_pad(m_sink, "video");
    if (!sink_pad) {
        std::cerr << "gst_element_get_static_pad failed." << std::endl;
        return -1;
    }
    add_pad_probe(sink_pad, "video_sink");
    return 0;
}

int GstVideoRecorder::init() {
    gst_init(NULL, NULL);

    /* Create the empty pipeline */
    m_pipeline = gst_pipeline_new("test-pipeline");

    if (!m_pipeline) {
        std::cerr << "pipeline could be created." << std::endl;
        return -1;
    }
    std::cout << "define_elements start..." << std::endl;
    int ret = define_elements();
    if (ret !=0 ) std::cerr << "define_elements failed." << std::endl;

    ret = make_elements();
    if (ret !=0 ) std::cerr << "make_elements failed." << std::endl;
    ret = add_elements();
    if (ret !=0 ) std::cerr << "add_elements failed." << std::endl;

    ret = link_elements();
    if (ret !=0 ) std::cerr << "link_elements failed." << std::endl;  

    add_probe();

    return ret;
}


void GstVideoRecorder::clean() {
    //TODO: remove elements from pipeline
    //TODO: unlink elements
    //TODO: release elememts
    /* Free resources */
    if (m_bus) {
        gst_object_unref(m_bus);
    }
    
    if (m_pipeline) {
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
        gst_object_unref(m_pipeline);
    }
    
}


int GstVideoRecorder::start() {
    gboolean terminate = FALSE;

    if (!m_pipeline) {
        std::cerr << "Have not created pipeline." << std::endl;
        return -1;
    }
    /* Start playing */
    GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        gst_object_unref (m_pipeline);
        std::cerr << "Unable to set the pipeline to the playing state." << std::endl;
        return -2;
    }
    std::cout << "start pipeline succeed" << std::endl;
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN_CAST(m_pipeline), GST_DEBUG_GRAPH_SHOW_VERBOSE, "record_pipeline");

    m_bus = gst_element_get_bus(m_pipeline);
    do {
        GstMessage* msg = gst_bus_timed_pop_filtered(m_bus, GST_CLOCK_TIME_NONE,
            (GstMessageType)(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_QOS | GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

        /* Parse message */
        if (msg != NULL) {
            GError *err;
            gchar *debug_info;

            switch (GST_MESSAGE_TYPE(msg)) {
                case GST_MESSAGE_ERROR:
                    gst_message_parse_error(msg, &err, &debug_info);
                    g_printerr("Error received from element %s: %s\n",
                            GST_OBJECT_NAME(msg->src), err->message);
                    g_printerr("Debugging information: %s\n",
                            debug_info ? debug_info : "none");
                    g_clear_error(&err);
                    g_free(debug_info);
                    break;
                case GST_MESSAGE_QOS:
                    g_print("QOS message\n");
                    break;
                case GST_MESSAGE_EOS:
                    g_print("End-Of-Stream reached.\n");
                    terminate = TRUE;
                    break;
                case GST_MESSAGE_STATE_CHANGED:
                    /* We are only interested in state-changed messages from the pipeline */
                    if (GST_MESSAGE_SRC (msg) == GST_OBJECT (m_pipeline)) {
                        GstState old_state, new_state, pending_state;
                        gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                        g_print ("Pipeline state changed from %s to %s:\n",
                            gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
                    }
                    break;
                default:
                    /* We should not reach here because we only asked for ERRORs and EOS */
                    g_printerr("Unexpected message received.\n");
                    break;
            }
            gst_message_unref(msg);
            
        }
    } while (!terminate);
    return 0;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cout << "Usage  : " << argv[0] 
            << " --video_src=filesrc|uridecodebin|videotestsrc"
            << " --video_sink=<splitmuxsink|hlssink2>" 
            << std::endl;
        std::cout << "Example: " << argv[0] 
            << " --video_src=videotestsrc"
            << " --video_sink=hlssink2" 
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


    auto recorder = std::make_unique<GstVideoRecorder>();
    
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

