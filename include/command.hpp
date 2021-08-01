#ifndef ALPHYX_COMMAND_H
#define ALPHYX_COMMAND_H

#include <string>
#include <utility>
#include <vector>
#include <iterator>
#include <iostream>
#include <base-command.h>

class Client;

enum CommandType {
    GENERAL = 0,
    CATEGORY,
    REACTION,
    OWNER
};

template <typename T, typename U>
class Command : public CommandBase {
  protected:
    const Client *client;
    /**
     * Derived classes functionality, the core of the commands.
     * @param event Message event mostly for channel_id
     * @param words Content of message broken up by spaces.
     */
    virtual void command_exec(const T & event, U & content) const {
      // Do nothing by default.
      std::cout << "= = = = = = = Command " << m_name << " has not implemented the execute function = = = = = = =" << std::endl;
    }

    /**
     * Check if a user has the correct perms to run a command.
     * @param event message event to grab user roles from.
     * @return true/false based on if user has correct perms.
     */
    virtual bool can_run(const T & event) const = 0;

  public:
    Command(
      const Client *_client,
      CommandType type,
      std::string name,
      std::string desc = ""
    ): m_name(std::move(name)),
       m_description(std::move(desc)),
       command_type(type),
       client(_client),
       permission(dpp::role_permissions::p_send_messages) {}

    std::string m_name;
    std::string m_description;
    uint32_t permission;
    CommandType command_type;

    /**
     * Run commands functionality and increase commandRan
     * @param event Message event to break up by spaces.
     */
    inline void command_run(const T & event, U & content) const {
      // Prevent users who shouldn't be running these commands.
      if (!can_run(event)) return;

      command_exec(event, content);

      commandsRan++;
    }
};

#endif