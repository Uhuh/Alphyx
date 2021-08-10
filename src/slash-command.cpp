#include <bot.hpp>
#include <slash-command.h>

void SlashCommand::command_create(const dpp::confirmation_callback_t& e) {
  if (e.is_error()) {
    Client::log(LogType::ERROR, "Slash command: " + this->m_name + " threw an error upon creation.");
    Client::log(LogType::DEBUG, e.http_info.body);
  }
}