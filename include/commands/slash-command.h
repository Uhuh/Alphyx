#ifndef ALPHYX_SLASHCOMMAND_H
#define ALPHYX_SLASHCOMMAND_H
#include <commands/command.hpp>
#include <dpp/fmt/format.h>

class SlashCommand : protected Command<dpp::interaction_create_t> {
protected:
    void command_exec(const dpp::interaction_create_t & event) const override {
      // Default response for a command with no implementation.
      dpp::command_interaction cmd_name = std::get<dpp::command_interaction>(event.command.data);
      event.reply(dpp::ir_channel_message_with_source, fmt::format("You ran slash command {}", cmd_name.name));
    }

    /**
     * Check if a user has the correct perms within one of their roles.
     * @param event command run for the user info.
     * @return true if user has perms, false if not.
     */
    [[nodiscard]] UserPermResponse user_perms(const dpp::interaction_create_t &event) const override {
      dpp::user *user = dpp::find_user(event.command.usr.id);
      if (user == nullptr) return UserPermResponse::user_missing;
      dpp::channel *channel = dpp::find_channel(event.command.channel_id);
      if (channel == nullptr) return UserPermResponse::channel_missing;
      const uint64_t memberPerms = channel->get_user_permissions(user);

      return ((memberPerms & permission) == permission) ? UserPermResponse::has_required_perms : UserPermResponse::missing_required_perms;
    }

    /**
     * Use this to detect slash commands failing to be created.
     * @param e - HTTP response from Discord API, if the slash command was created or not.
     */
    void command_create(const dpp::confirmation_callback_t& e);

    /**
     * Setup what options the slash command will have and globally create.
     */
    virtual void slash_command_create() = 0;

    /**
     * Going to use this as a generic error message to reply to users.
     * @param e slash interaction to use reply with.
     * @param message What I want to reply to the user with
     */
    static void ephemeral_message(const dpp::interaction_create_t& e, const std::string & message) {
      e.reply(
        dpp::ir_channel_message_with_source,
        dpp::message(message).set_flags(dpp::m_ephemeral)
      );
    }
public:
    SlashCommand(Client *bot, CommandType type , std::string name, std::string desc):
      Command(bot, type, std::move(name), std::move(desc)) {
    }

    /**
     * Check if user has correct perms and run the command.
     * @param e - slash command run.
     * @param name - name of slash command used for parsing data later on.
     */
    inline void run(const dpp::interaction_create_t& e) {
      UserPermResponse userPerms = user_perms(e);

      switch(userPerms) {
        case UserPermResponse::has_required_perms:
          command_exec(e);
          break;
        case UserPermResponse::missing_required_perms:
          ephemeral_message(e, "You don't have the permissions to run this command.");
          break;
        case UserPermResponse::user_missing:
          ephemeral_message(e, "I had an issue finding you. This may be because of Discord caching. Please send a normal message then try again.");
          break;
        case UserPermResponse::channel_missing:
          ephemeral_message(e, "Somehow I couldn't find this channel. Please try again.");
          break;
        default:
          throw "Got unexpected user perm response.";
      }
    }
};


#endif
