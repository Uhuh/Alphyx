#ifndef ALPHYX_JOIN_ROLE_H
#define ALPHYX_JOIN_ROLE_H

#include <command.hpp>

class JoinRoleCommand: public Command {
  protected:
    void command_exec(const dpp::message_create_t &event, std::vector<std::string> &words) const override;

    bool can_run(const dpp::message_create_t & event) const override {
      bool hasPerms = false;

      for (const auto r: event.msg->member.roles) {
        if (client->role_cache->find(r) != client->role_cache->end()) {
          dpp::role *role = (dpp::role*)client->role_cache->find(r)->second;
          if (role->has_manage_roles()) {
            hasPerms = true;
            break;
          }
        }
      }

      return hasPerms;
    }

  public:
    explicit JoinRoleCommand(Client *bot): Command(
      bot,
      CommandType::REACTION,
      "join-role",
      "Add, remove or view the current join roles for the server."
    ) {}
};

#endif