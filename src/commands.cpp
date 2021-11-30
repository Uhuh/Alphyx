#include <bot.hpp>

#include <general/ping.hpp>
#include <general/pong.hpp>
#include <pet-commands/adopt.h>

int CommandBase::commandsRan = 0;

void Client::commandsInit() {
  message_command_list = {
    { "pong", std::make_shared<PongCommand>(this) },
  };

  slash_command_list = {
    { "adopt", std::make_shared<AdoptCommand>(this) },
    { "ping", std::make_shared<PingCommand>(this) }
  };
}

void SlashCommand::command_create(const dpp::confirmation_callback_t& e) const {
  if (e.is_error()) {
    Client::log(LogType::ERROR, "Slash command: " + this->m_name + " threw an error upon creation.");
    Client::log(LogType::DEBUG, e.http_info.body);
  }
}