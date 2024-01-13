#include "gtest/gtest.h"
#include "time_util.h"
#include "time.h"
#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>

#include <fstream>
#include "yaml-cpp/yaml.h"

using namespace std;
using namespace testing;

class YamlParserTest: public testing::Test
{
public:

    virtual void SetUp() {

        
    }

protected:
    
};

TEST_F(YamlParserTest, parse) {
    std::map<std::string, std::vector<std::string>> config;
    YAML::Node node = YAML::LoadFile("../example/etc/pipeline.yaml");
    if (node["pipelines"]) {
        YAML::Node pipelines = node["pipelines"]; 
        YAML::const_iterator it=pipelines.begin();     
        for (; it!=pipelines.end(); ++it) {
            std::string key = it->first.as<std::string>();
        
            YAML::Node elements = it->second;
            std::cout << key << "[elements]:\n";
            std::vector<std::string> vals = elements["elements"].as<std::vector<std::string>>();
            for(auto& val: vals) {
                std::cout << "\t" << val << "\n";
            }
            config.emplace(std::make_pair(key, vals));

        }
    }
}