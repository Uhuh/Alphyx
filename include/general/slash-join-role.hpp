#ifndef ALPHYX_SLASH_JOIN_ROLE_H
#define ALPHYX_SLASH_JOIN_ROLE_H

#include <slash-command.h>
#include <dpp/message.h>

class SlashJoinRoleCommand: public SlashCommand {
protected:
    void command_exec(const dpp::interaction_create_t &event, std::string &name) const override {
      event.reply(dpp::ir_channel_message_with_source, "Join role slash command is up and running.");
    }

    void slash_command_create() override {
      dpp::slashcommand command;

      dpp::command_option role = dpp::command_option(dpp::co_role, "role", "The role you want users to get when they join.", true);

      command.set_name(this->m_name)
        .set_description(this->m_description)
        .set_application_id(this->client->cluster->me.id)
        .add_option(
          dpp::command_option(dpp::co_sub_command_group, "role", "Are we adding a new role?")
            .add_option(
              dpp::command_option(dpp::co_sub_command, "add", "Add a role to your auto-join roles.").add_option(role)
            )
            .add_option(
              dpp::command_option(dpp::co_sub_command, "remove", "Remove a role from your auto-join roles.").add_option(role)
            )
            .add_option(dpp::command_option(dpp::co_sub_command, "list", "List your auto-join roles."))
        );

      this->client->cluster->global_command_create(command, [&](const auto &e){
        this->command_create(e);
      });
    }
public:
    explicit SlashJoinRoleCommand(const Client *bot): SlashCommand(
      bot,
      CommandType::GENERAL,
      "join",
      "Add, remove or list your servers join roles."
    ) {
      slash_command_create();
    }
};

#endif