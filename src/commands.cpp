#include <bot.hpp>

#include <general/ping.hpp>
#include <general/pong.hpp>
#include <general/join-role.hpp>
#include <general/slash-join-role.hpp>

#include <category/category-create.h>

int CommandBase::commandsRan = 0;

void Client::commandsInit() {
  message_command_list = {
    { "pong", std::make_shared<PongCommand>(this) },
    { "join-role", std::make_shared<JoinRoleCommand>(this) },
    { "category-create", std::make_shared<CategoryCreateCommand>(this) }
  };

  slash_command_list = {
    { "ping", std::make_shared<PingCommand>(this) },
    { "join", std::make_shared<SlashJoinRoleCommand>(this) }
  };
}

