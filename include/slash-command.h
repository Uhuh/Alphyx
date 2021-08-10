#ifndef ALPHYX_SLASHCOMMAND_H
#define ALPHYX_SLASHCOMMAND_H
#include <command.hpp>
#include <dpp/fmt/format.h>

class SlashCommand : protected Command<dpp::interaction_create_t, std::string> {
protected:
    void command_exec(
      const dpp::interaction_create_t & event,
      std::string& name
    ) const override {
      // Default response for a command with no implementation.
      const std::string cmd_name = std::get<std::string>(event.get_parameter(name));
      event.reply(dpp::ir_channel_message_with_source, fmt::format("You ran slash command {}", cmd_name));
    }

    [[nodiscard]] bool can_run(const dpp::interaction_create_t &event) const override {
      return true;
    }

    /**
     * Use this to detect slash commands failing to be created.
     * @param e - HTTP response from Discord API, if the slash command was created or not.
     */
    void command_create(const dpp::confirmation_callback_t& e);

    virtual void slash_command_create() = 0;
public:
    SlashCommand(const Client *bot, CommandType type , std::string name, std::string desc):
      Command(bot, type, std::move(name), std::move(desc)) {
    }

    inline void run(const dpp::interaction_create_t& e, std::string& name) {
      command_exec(e, name);
    }
};


#endif
