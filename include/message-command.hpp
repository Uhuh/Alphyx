#ifndef ALPHYX_MESSAGECOMMAND_H
#define ALPHYX_MESSAGECOMMAND_H
#include <command.hpp>
#include <utility>
#include <dpp/fmt/format.h>

class MessageCommand : protected Command<dpp::message_create_t, std::vector<std::string>> {
protected:
    void command_exec(
      const dpp::message_create_t & event,
      std::vector<std::string> & words
    ) const override {
      // Default response for a command with no implementation.

    }

    [[nodiscard]] bool can_run(const dpp::message_create_t &event) const override {
      bool hasPerms = false;

      for (const uint64_t r: event.msg->member.roles) {
        dpp::role *role = dpp::find_role(r);
        if (role != nullptr && (role->permissions & permission) == permission) {
          hasPerms = true;
          break;
        }
      }

      return hasPerms;
    }

public:
    MessageCommand(const Client *bot, CommandType type , std::string name, std::string desc):
      Command(bot, type, std::move(name), std::move(desc)) {}

    inline void run(const dpp::message_create_t& e, std::vector<std::string>& name) {
      command_exec(e, name);
    }
};


#endif