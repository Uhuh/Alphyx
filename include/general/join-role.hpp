#ifndef ALPHYX_JOIN_ROLE_H
#define ALPHYX_JOIN_ROLE_H

#include <command.hpp>

class JoinRoleCommand: public Command {
  protected:
    void command_exec(const dpp::message_create_t &event, std::vector<std::string> &words) const override;

  public:
    explicit JoinRoleCommand(Client *bot): Command(
      bot,
      CommandType::REACTION,
      "join-role",
      "Add, remove or view the current join roles for the server."
    ) {
      permission = dpp::role_permissions::p_manage_roles;
    }
};

#endif