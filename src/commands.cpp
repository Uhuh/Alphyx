#include <bot.hpp>

#include <general/ping.hpp>
#include <general/pong.hpp>
#include <general/join-role.hpp>

int CommandBase::commandsRan = 0;

void Client::commandsInit() {
  message_command_list = {
    { "pong", std::make_shared<PongCommand>(this) },
  };

  slash_command_list = {
    { "ping", std::make_shared<PingCommand>(this) },
    { "join", std::make_shared<JoinRoleCommand>(this) }
  };
}

void SlashCommand::command_create(const dpp::confirmation_callback_t& e) {
  if (e.is_error()) {
    Client::log(LogType::ERROR, "Slash command: " + this->m_name + " threw an error upon creation.");
    Client::log(LogType::DEBUG, e.http_info.body);
  }
}