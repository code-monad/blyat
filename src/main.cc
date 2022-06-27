#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include <boost/beast.hpp>
#include <blyat/blyat.hh>
#include <boost/asio.hpp>
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

int main() {
  //entt::registry registry;
  boost::asio::io_context ioc{};
  auto config = blyat::config_loader::load("../config/blyat.toml");

  if(!config.validate()) {
    return -1;
  }

  //blyat::room_t room;
  auto server = std::make_shared<blyat::server>(ioc);
  server->run(config.fields());

  std::thread work([&ioc]{
    while(!ioc.stopped()) {
      ioc.poll_one();
      std::this_thread::yield();
    }
    
  });

  std::this_thread::sleep_for(std::chrono::seconds(100));
  ioc.reset();
  ioc.stop();
  work.join();
  return 0;
}
