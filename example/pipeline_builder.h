#pragma once

#include <gst/gst.h>
#include <glib.h>
#include <memory>
#include <thread>
#include <vector>
#include <map>


struct ElementConfig {
    ElementConfig(const std::string& desc);
    void parse_desc(const std::string& desc);
    void parse_prop(const std::string& prop);
    std::string m_name;
    std::string m_factory;
    std::map<std::string, std::string> m_props;
};

struct PipelineConfig {
    PipelineConfig(std::string name, std::vector<std::string> elements);
    
    void insert_desc(const std::string& desc);
    void parse_desc(const std::string& desc);
    
    std::shared_ptr<ElementConfig> get_element_config(const std::string& name);

    std::string m_name;
    std::vector<std::string> m_elements_desc;
    std::vector<std::shared_ptr<ElementConfig>> m_elements_config;
};

struct PipelineRepo {
    void insert(const std::string& name, PipelineConfig& PipelineConfig);
    

    std::map<std::string, PipelineConfig> m_pipelines;
};


class PipelineBuilder {
public:
    int init(int argc, char *argv[]);
    int clean();
    int build();
    int start();
    int stop();

private:

    GstElement* create_element(const std::string& factory, const std::string& name);
    GstElement* get_element(const std::string& name);
    
    bool add_element(const std::string& name);
    bool del_element(const std::string& name);

    bool link_elements();
    bool unlink_elements();

    std::map<std::string, GstElement*> m_elements;
    GstBus* m_bus;
    GMainLoop* m_loop;
    GstElement* m_pipeline;
    std::string m_pipeline_name;
    std::shared_ptr<PipelineConfig> m_pipelie_config;
};
