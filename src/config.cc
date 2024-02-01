#include <blyat/config.hh>
#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>
#include <spdlog/spdlog.h>

namespace blyat {
  config_loader::config_loader(const std::string path) : _fields{} {
    parse(path);
  }
  
  void config_loader::parse(const std::string path) {
    if(path.empty()) return;
    spdlog::info("Loading configs from {}", path);
    auto parse_result = toml::parse_file(path);
    if(!parse_result) {
      spdlog::error("Failed to load config:{}!Please check the file.", path);
      return;
    }

    _raw_node = std::move(parse_result.table());
    
    _parsed = true;

    if(auto service_config = _raw_node.get_as<toml::table>("service")) {
      if(auto address = service_config->get("listen_addr"))  {
	_fields.service_field.listen_addr = address->value_or("127.0.0.1");
      }
      spdlog::info("service.listen_addr:{}", _fields.service_field.listen_addr);
      if(auto port = service_config->get("listen_port")) {
	_fields.service_field.listen_port = port->value_or(65472);
      }
      spdlog::info("service.listen_port:{}", _fields.service_field.listen_port);
      if(auto session_limit = service_config->get("session_limit"))  {
	_fields.service_field.session_limit = session_limit->value_or(0);
      }
      spdlog::info("service.session_limit:{}", _fields.service_field.session_limit);
    }

    
    if(auto common_config = _raw_node.get_as<toml::table>("common")) {
      if(auto secrets = common_config->get("secrets"))  {
	_fields.common_field.secrets = secrets->value_or("");
      }
      if(auto doc_root = common_config->get("doc_root"))  {
	_fields.common_field.doc_root = doc_root->value_or("./dist");
      }
    }

    
    
  }

  config_loader config_loader::load(const std::string path) {
    return config_loader(path);
  }
} // namespace blyat
