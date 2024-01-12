#include "gst_util.h"
#include "file_util.h"
#include "string_util.h"
#include "pipeline_builder.h"

using namespace std;

static const std::string DELIMITER = " ";

ElementConfig::ElementConfig(const std::string& desc) {
    parse_desc(desc);
}

void ElementConfig::parse_prop(const std::string& token) {
    if (token.empty()) {
        return;
    }
    
    size_t pos = 0;
    if ((pos = token.find("=")) != string::npos) {
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
void ElementConfig::parse_desc(const std::string& desc) {
    size_t last = 0; 
    size_t next = 0; 
    while ((next = desc.find(DELIMITER, last)) != string::npos) {  
        std::string token = desc.substr(last, next-last);
        trim(token);
        parse_prop(token);
        last = next + 1; 
    } 
    parse_prop(desc.substr(last));
}


PipelineConfig::PipelineConfig(std::string name, std::vector<std::string> elements)
: m_name(name)
, m_elements_desc(elements) {
    for(auto& desc: m_elements_desc) {
        m_elements_config.push_back(std::make_shared<ElementConfig>(desc));
    }
}

void PipelineConfig::insert_desc(const std::string& desc) {
    m_elements_desc.push_back(desc);
    m_elements_config.push_back(std::make_shared<ElementConfig>(desc));
}

std::shared_ptr<ElementConfig> PipelineConfig::get_element_config(const std::string& name) {
    for(auto& ele_cfg_ptr: m_elements_config) {
        if (ele_cfg_ptr->m_name == name) {
            return ele_cfg_ptr;
        }
    }
    return nullptr;
}

int PipelineBuilder::init(int argc, char *argv[]) {
    gst_init(&argc, &argv);
    
    const std::vector<std::string_view> args(argv, argv + argc);
    auto pipeline_config_file = get_option(args, "-f");
    if (pipeline_config_file.empty()) {
        DEBUG_TRACE("PipelineBuilder init not give pipeline yaml" );
        return -1;
    }

    m_pipeline_name = get_option(args, "-p");
    if (m_pipeline_name.empty()) {
        m_pipeline_name = "pipeline_mp4_rtmp";
    }
    std::map<std::string, std::vector<std::string>> pipelines;
    int ret = yaml_to_str_vec_map(std::string(pipeline_config_file), "pipelines", pipelines);
    if (ret < 0 || pipelines.empty()) {
        DEBUG_TRACE("PipelineBuilder init not found pipeline yaml: " << pipeline_config_file);
        return -1;
    }

    auto it = pipelines.find(std::string(m_pipeline_name));
    if (it == pipelines.end()) {
        DEBUG_TRACE("PipelineBuilder init not found pipeline " << m_pipeline_name);
        return -1;
    }

    m_pipeline = gst_pipeline_new(m_pipeline_name.c_str());
    m_bus = gst_element_get_bus(m_pipeline);
    gst_bus_add_watch(m_bus, on_bus_msg, this);
    //use the global-default main context, and not running
    m_loop = g_main_loop_new(NULL, FALSE);

    m_pipelie_config = std::make_shared<PipelineConfig>(it->first, it->second);
    DEBUG_TRACE("PipelineBuilder init success for pipeline " << m_pipeline_name);
    return 0;
}

int PipelineBuilder::build() {

    for(auto& ele_cfg_ptr: m_pipelie_config->m_elements_config) {        
        create_element(ele_cfg_ptr->m_factory, ele_cfg_ptr->m_name);
    }

    for(auto& ele_cfg_ptr: m_pipelie_config->m_elements_config) {
        add_element(ele_cfg_ptr->m_name);
    }

    link_elements();
    return 0;
}


int PipelineBuilder::clean() {
    //don't forget gst_object_unref (GST_OBJECT (element)) if not use it
    unlink_elements();
    for(auto& ele_cfg_ptr: m_pipelie_config->m_elements_config) {
        del_element(ele_cfg_ptr->m_name);
    }
    return 0;
}

int PipelineBuilder::start() {
    std::string dot_file = "test_pipeline";
    //set environment variable, such as export GST_DEBUG_DUMP_DOT_DIR=/tmp
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN_CAST(m_pipeline), GST_DEBUG_GRAPH_SHOW_VERBOSE, dot_file.c_str());

    DEBUG_TRACE("start playing...");
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    g_main_loop_run(m_loop);
    return 0;
}

int PipelineBuilder::stop() {
    DEBUG_TRACE("stop playing...");
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    return 0;
}

GstElement* PipelineBuilder::create_element(const std::string& factory, 
    const std::string& name) {
    GstElement* element = gst_element_factory_make (factory.c_str(), name.c_str());
    if (!element) {
        DEBUG_TRACE("create failed for element " << factory << ", name=" << name);
        return nullptr;
    }
    DEBUG_TRACE("create succeed for element " << factory << ", name=" << name);
    m_elements.emplace(std::make_pair(name, element));
    return element;
}

GstElement* PipelineBuilder::get_element(const std::string& name) {
    auto it = m_elements.find(name);
    if (it != m_elements.end()) {
        return it->second;
    }
    return nullptr;
}

bool PipelineBuilder::add_element(const std::string& name) {
    auto it = m_elements.find(name);
    if (it == m_elements.end()) {
        return false;
    }

    auto ret = gst_bin_add(GST_BIN(m_pipeline), it->second);
    if(ret) {     
        DEBUG_TRACE("add succeed for element " << it->first);
        //set element prop
        auto ele_cfg_ptr = m_pipelie_config->get_element_config(name);
        if (ele_cfg_ptr) {
            for(auto& kv: ele_cfg_ptr->m_props) {
                DEBUG_TRACE("set element prop: " << kv.first << " = " << kv.second);

                if(kv.first == "caps") {
                    //GstCaps* caps = gst_caps_from_string(kv.second.c_str());
                    
                    GstCaps *caps = gst_caps_new_simple ("audio/x-raw",
                    "format", G_TYPE_STRING, "S16LE",
                    "rate", G_TYPE_INT, 16000,
                    "channels", G_TYPE_INT, 1,
                    "layout", G_TYPE_STRING, "interleaved",
                    NULL);
                    
                    g_object_set(G_OBJECT(it->second), kv.first.c_str(), caps, nullptr);
                } else if(is_number(kv.second)) {
                    g_object_set(it->second, kv.first.c_str(), str_to_num<int32_t>(kv.second), NULL);    
                } else {
                    std::string lower_value = str_tolower(kv.second);
                    if(lower_value == "true") {
                        g_object_set(it->second, kv.first.c_str(), 1, NULL);
                    } else if(lower_value == "false") {
                        g_object_set(it->second, kv.first.c_str(), 0, NULL);
                    } else {
                        g_object_set(it->second, kv.first.c_str(), kv.second.c_str(), NULL);
                    }
                    
                }
                
            }
        }
        return true;                               
    } 
    DEBUG_TRACE("add failed for element " << it->first);   
    return false;
}

bool PipelineBuilder::del_element(const std::string& name) {
    auto it = m_elements.find(name);
    if (it == m_elements.end()) {
        return false;
    }
    GstElement* e = it->second;
    GstStateChangeReturn s = gst_element_set_state(e, GST_STATE_NULL);                                   
    if(s == GST_STATE_CHANGE_SUCCESS) {                    
        if(gst_bin_remove(GST_BIN(m_pipeline), (e))) {             
            DEBUG_TRACE("remove/delete succeed for element " << name);
            m_elements.erase(it);
            //TODO: need unrefe here?
            //gst_object_unref(GST_OBJECT(e));
            //e = nullptr;
            return true;
        } else {                                           
            DEBUG_TRACE("remove failed for element " << name);           
        }                                                  
    } else {                                               
        DEBUG_TRACE("remove elem failed for set state null: " << name);  
    }                                                      

    return false;
}

bool PipelineBuilder::link_elements() {
    auto& elements_configs = m_pipelie_config->m_elements_config;
    auto it = elements_configs.begin();

    while( it != elements_configs.end()) {
        GstElement* e1 = get_element((*it)->m_name);
        //for decodebin, cannot link directly
        if ((*it)->m_factory == "decodebin") {
            g_signal_connect(e1, "pad-added", G_CALLBACK(on_pad_added), this);
        }

        ++it;
        if (it == elements_configs.end()) {
            break;
        }
        
        GstElement* e2 = get_element((*it)->m_name);
        if (e1 != nullptr && e2 != nullptr) {      
            gchar* e1n = gst_element_get_name(e1);
            gchar* e2n = gst_element_get_name(e2);
            auto link_ret = gst_element_link(e1, e2);
            if(link_ret) {                                 
                DEBUG_TRACE("link succed for " << e1n << " and " << e2n);
            } else {
                ERROR_TRACE("link failed for " << e1n << " and " << e2n);
            }
            g_free(e1n);        
            g_free(e2n);                                         
        }
    }
    
    return true;
}

bool PipelineBuilder::unlink_elements() {
    auto& pipe_configs = m_pipelie_config->m_elements_config;
    auto it = pipe_configs.begin();
    
    while( it != pipe_configs.end()) {
        GstElement* e1 = get_element((*it)->m_name);
        ++it;
        if (it == pipe_configs.end()) {
            break;
        }
        GstElement* e2 = get_element((*it)->m_name);
        if (e1 != nullptr && e2 != nullptr) {      
                gst_element_unlink(e1, e2);
                                     
                gchar* e1n = gst_element_get_name((e1));
                gchar* e2n = gst_element_get_name((e2));
                DEBUG_TRACE("unlink successfully for " << e1n << " and " << e2n);
                g_free(e1n);                            
                g_free(e2n); 
                                                    
        }
    }

    return false;
}


gboolean PipelineBuilder::on_bus_msg(GstBus* bus, GstMessage* msg, gpointer data) {
    
    DEBUG_TRACE("on_bus_msg: type=" << GST_MESSAGE_TYPE(msg)
        << ", type name=" << GST_MESSAGE_TYPE_NAME(msg)
        << ", source="  << GST_OBJECT_NAME(msg->src));

    PipelineBuilder* builder = (PipelineBuilder*)data;

    switch(GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            builder->on_bus_msg_eos();
            break;
        case GST_MESSAGE_ERROR:
            builder->on_bus_msg_error(msg);
            break;
        case GST_MESSAGE_WARNING:
            builder->on_bus_msg_warning(msg);
            break;
        case GST_MESSAGE_STATE_CHANGED:
            builder->on_state_changed(msg);
            break;
        case GST_MESSAGE_STREAM_START:
            builder->on_stream_started(msg);
            break;
        default:
            break;
    }
    return TRUE;
}


void PipelineBuilder::on_pad_added(GstElement* element, GstPad* pad, gpointer data) {
    DEBUG_TRACE("on_pad_added:");
}

void PipelineBuilder::on_bus_msg_eos() {
    ERROR_TRACE("on_bus_msg_eos:");
    g_main_loop_quit(m_loop);
}

void PipelineBuilder::on_bus_msg_error(GstMessage* msg) {
    ERROR_TRACE("on_bus_msg_error:");
    gchar* debug = nullptr;
    GError* error = nullptr;
    gst_message_parse_error(msg, &error, &debug);
    DEBUG_TRACE("there is error " << error->message 
        << " for " << GST_OBJECT_NAME(msg->src));
    if (debug) {
        ERROR_TRACE("error details: " << debug);
    }

    g_free(debug);
    g_error_free(error);
 
    g_main_loop_quit(m_loop);
}

void PipelineBuilder::on_bus_msg_warning(GstMessage* msg) {
    ERROR_TRACE("on_bus_msg_warning:");
}

void PipelineBuilder::on_state_changed(GstMessage* msg) {

    GstState old_state, new_state, pending_state;
    gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);

    DEBUG_TRACE("--> state changed from " 
                << gst_element_state_get_name (old_state)
                << " to " << gst_element_state_get_name(new_state)
                << " for " << GST_OBJECT_NAME(msg->src));
 
}

void PipelineBuilder::on_stream_started(GstMessage* msg) {
    DEBUG_TRACE("on_stream_started:");
}