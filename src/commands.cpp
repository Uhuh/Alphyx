#include <bot.hpp>

#include <cppconn/prepared_statement.h>

#include <general/ping.hpp>
#include <general/pong.hpp>
#include <general/join-role.hpp>
#include <category/category-create.h>

int Command::commandsRan = 0;

void Client::commandsInit() {
  command_list = {
    { "ping", std::make_shared<PingCommand>(this) },
    { "pong", std::make_shared<PongCommand>(this) },
    { "join-role", std::make_shared<JoinRoleCommand>(this) },
    { "category-create", std::make_shared<CategoryCreateCommand>(this) }
  };
}

