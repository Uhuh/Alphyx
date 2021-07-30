#include <bot.hpp>

#include <general/ping.hpp>
#include <general/pong.hpp>
#include <general/join-role.hpp>

#include <category/category-create.h>

int Command::commandsRan = 0;

void Client::commandsInit() {
  command_list = {
    { "pong", std::make_shared<PongCommand>(this) },
    { "join-role", std::make_shared<JoinRoleCommand>(this) },
    { "category-create", std::make_shared<CategoryCreateCommand>(this) }
  };

  slash_command_list = {
    { "ping", std::make_shared<PingCommand>(this) },
  };
}

