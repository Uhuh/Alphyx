#ifndef ALPHYX_CATEGORY_CREATE_H
#define ALPHYX_CATEGORY_CREATE_H

#include <command.hpp>

class CategoryCreateCommand : public Command {
  protected:
    void command_exec(const dpp::message_create_t & event, std::vector<std::string> & words) const override;

  public:
    explicit CategoryCreateCommand(const Client *bot): Command(
      bot,
      CommandType::CATEGORY,
      "category-create",
      "Create a new category, even give it a description!"
    ) {
    permission = dpp::role_permissions::p_manage_roles;
  }
};

#endif