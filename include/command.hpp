#ifndef ALPHYX_COMMAND_H
#define ALPHYX_COMMAND_H

#include <dpp/dpp.h>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <iterator>
#include <iostream>

class Client;

enum CommandType {
    GENERAL = 0,
    CATEGORY,
    REACTION,
    OWNER
};

class Command {
  protected:
    const Client *client;
    /**
     * Derived classes functionality, the core of the commands.
     * @param event Message event mostly for channel_id
     * @param words Content of message broken up by spaces.
     */
    virtual void command_exec(
            const dpp::message_create_t & event,
            std::vector<std::string> & words
    ) const = 0;

    /**
     * Check if a user has the correct perms to run a command.
     * @param event message event to grab user roles from.
     * @return true/false based on if user has correct perms.
     */
    [[nodiscard]] inline virtual bool can_run(const dpp::message_create_t & event) const {
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
    inline void run(const dpp::message_create_t & event, std::vector<std::string> & words) const {
      // Prevent users who shouldn't be running these commands.
      if (!can_run(event)) return;

      command_exec(event, words);

      commandsRan++;
    }

    /**
     * Break message events up by spaces and return the vector of strings.
     */
    inline static std::vector<std::string> content(const dpp::message_create_t & event) {
      std::stringstream ss(event.msg->content);
      const std::istream_iterator<std::string> begin(ss);
      const std::istream_iterator<std::string> end;
      std::vector<std::string> words(begin, end);
      
      return words;
    }

  private:
    static int commandsRan;
};

#endif