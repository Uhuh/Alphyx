#ifndef ALPHYX_ADOPT_H
#define ALPHYX_ADOPT_H

#include <commands/command.hpp>

class AdoptCommand : public SlashCommand {
protected:
    void command_exec(const dpp::interaction_create_t & event) const override;

    void slash_command_create() override {
      dpp::slashcommand command;

      command.set_name(this->m_name)
        .set_description(this->m_description)
        .set_application_id(this->client->cluster->me.id);

      this->client->cluster->guild_command_create(command, 756395872811483177, [&](const auto& e) {
          this->command_create(e);
      });
    }

public:
    explicit AdoptCommand(
      Client* bot
    ): SlashCommand(
        bot,
        CommandType::GENERAL,
        "adopt",
        "Adopt your pet. Once in a life time opportunity. ;)"
      ) {
      slash_command_create();
    }
};


#endif