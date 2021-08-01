#ifndef ALPHYX_PING_H
#define ALPHYX_PING_H

#include "command.hpp"
#include <dpp/message.h>

class PingCommand: public SlashCommand {
  protected:
    void command_exec(
      const dpp::interaction_create_t & event,
      std::string & content
    ) const override {
      event.reply(dpp::ir_channel_message_with_source, "Ping pong. I work.. I think?");
    }

    void slash_command_create() override {
      dpp::slashcommand command;

      command.set_name(this->m_name)
        .set_description(this->m_description)
        .set_application_id(this->client->cluster->me.id)
        .add_option(
          dpp::command_option(dpp::co_string, "ping", "The type of ping", true)
            .add_choice(dpp::command_option_choice("Ping", std::string("normal_ping")))
            .add_choice(dpp::command_option_choice("Pong", std::string("ping_pong")))
            .add_choice(dpp::command_option_choice("Pyng", std::string("pyng")))
        );

      this->client->cluster->global_command_create(command, [](const dpp::confirmation_callback_t & e) {
          Client::log(LogType::DEBUG, e.type);
          Client::log(LogType::DEBUG, e.http_info.body);
      });
      Client::log(LogType::INFO, "Initialized Ping slash command.");
    }

  public:
    explicit PingCommand(const Client *bot): SlashCommand(
      bot,
      CommandType::GENERAL,
      "ping",
      "stupid description"
    ) {
      slash_command_create();
    }
};

#endif