#ifndef ALPHYX_MESSAGECOMMAND_H
#define ALPHYX_MESSAGECOMMAND_H
#include <commands/command.hpp>
#include <utility>
#include <dpp/fmt/format.h>

class MessageCommand : protected Command<dpp::message_create_t> {
protected:
    void command_exec(const dpp::message_create_t & event) const override {
      // Default response for a command with no implementation.
    }

    /**
     * Check if a user has the correct perms within one of their roles.
     * @param event command run for the user info.
     * @return true if user has perms, false if not.
     */
    [[nodiscard]] UserPermResponse user_perms(const dpp::message_create_t &event) const override {
      dpp::user *user = dpp::find_user(event.msg->member.user_id);
      if (user == nullptr) return UserPermResponse::user_missing;
      dpp::channel *channel = dpp::find_channel(event.msg->channel_id);
      if (channel == nullptr) return UserPermResponse::channel_missing;
      const uint64_t memberPerms = channel->get_user_permissions(user);

      return ((memberPerms & permission) == permission) ? UserPermResponse::has_required_perms : UserPermResponse::missing_required_perms;
    }

public:
    MessageCommand(Client *bot, CommandType type , std::string name, std::string desc):
      Command(bot, type, std::move(name), std::move(desc)) {}

    inline void run(const dpp::message_create_t& e) {
      command_exec(e);
    }
};


#endif