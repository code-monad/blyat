#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include <boost/beast.hpp>
#include <blyat/blyat.hh>
#include <boost/asio.hpp>
#include <iostream>

struct session_t {
  blyat::uuid id;
  std::string name;
};

void update(entt::registry &registry) {
  auto view = registry.view<const session_t>();
  view.each([](const auto& session){
    spdlog::info("session {}:{}", session.name, session.id);
  });
}

int main(int argc, char* argv[]) {
  //entt::registry registry;
  boost::asio::io_context ioc{};
  std::string config_path = "./config/blyat.toml";
  int thread_cnt = 4;
  if(argc>1) {
    std::string opt = argv[1];
    if(opt == "--help"|| opt == "-h" || opt == "-?") {
      std::cout << fmt::format("Usage: {} [config_path] [threads]\nExample: {} ./blyat.toml 4") << std::endl;
    } else {
      config_path = opt;
    }
    if(argc > 2) {
      try{
	thread_cnt = std::stoll(argv[2]);
      }catch(...) {
	spdlog::error("Failed to parse thread setting! will use default:{}", thread_cnt);
      }
    }
  }
  blyat::config_fields_t configs = {};
  try{
    spdlog::info("Try loading config from {}", config_path);
    auto config = blyat::config_loader::load(config_path);
    if(!config.validate()) {
      return -1;
    }
    configs = config.fields();
  } catch(std::exception& e) {
    spdlog::error("Failed to load! Will use default settings...");
  }
  

  

  //blyat::room_t room;
  auto server = std::make_shared<blyat::server>(ioc);
  server->run(configs);

  std::vector<std::thread> workers{};

  workers.reserve(thread_cnt -1);
  for(auto i = 0; i < thread_cnt; ++i) {
    workers.emplace_back(
			 [&ioc]{
			   while(!ioc.stopped()) {
			     ioc.poll_one();
			     std::this_thread::yield();
			   }
			 }
			 );
  }

  for(auto& worker: workers) {
    worker.join();
  } 

  return 0;
}
