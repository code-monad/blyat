#ifndef __BLYAT_CONFIG_HH__
#define __BLYAT_CONFIG_HH__


#include <blyat/uuid.hh>
#include <toml++/impl/std_optional.h>
#include <toml++/impl/table.h>
#include <vector>
#include <string>
namespace blyat {
  struct service_field_t {
    std::string listen_addr = "127.0.0.1";
    unsigned short listen_port = 65472; // Respect for "同济上单";From: https://jikipedia.com/definition/907563605
    std::uint64_t session_limit = 0;
    service_field_t() = default;
    service_field_t(service_field_t& other) = default;
  };
  
  struct log_field_t {
    bool enable = false;
    std::string log_path = "./logs";
    std::string log_prefix = "blyat_";
    log_field_t() = default;
    log_field_t(log_field_t& other) = default;
  };

  struct common_field_t {
    std::string secrets = "";
    std::string doc_root = "./dist";
  };

  struct room_field_t {
    std::uint64_t id = 0;
    std::uint64_t session_limit = 0;
    std::string name = "ROOM";
  };

  struct db_field_t {
    bool enable = false;
  };

  struct config_fields_t {
    service_field_t service_field;
    common_field_t common_field;
    db_field_t db_field;
    std::vector<room_field_t> rooms_field;
  };

  class config_loader {
  public:
    static config_loader load(const std::string path);
    config_loader() = default;
    config_loader(const std::string path);
    void parse(const std::string path);
    bool validate() const { return _parsed; }
    config_fields_t& fields() { return _fields; }
    
  private:
    bool _parsed = false;
    toml::table _raw_node;
    config_fields_t _fields;
  };
  
} // blyat
#endif
