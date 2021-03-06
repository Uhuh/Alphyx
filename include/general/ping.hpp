#ifndef ALPHYX_PING_H
#define ALPHYX_PING_H

#include "commands/command.hpp"
#include <dpp/message.h>

class PingCommand: public SlashCommand {
  protected:
    void command_exec(const dpp::interaction_create_t & event) const override {
      event.reply(dpp::ir_channel_message_with_source, dpp::message("Ping pong. I work.. I think?").set_flags(dpp::m_ephemeral));
    }

    void slash_command_create() override {
      dpp::slashcommand command;

      command.set_name(this->m_name)
        .set_description(this->m_description)
        .set_application_id(this->client->cluster->me.id);

      this->client->cluster->global_command_create(command, [&](const auto& e) {
          this->command_create(e);
      });
    }

  public:
    explicit PingCommand(Client *bot): SlashCommand(
      bot,
      CommandType::GENERAL,
      "ping",
      "stupid description"
    ) {
      slash_command_create();
    }
};

#endif