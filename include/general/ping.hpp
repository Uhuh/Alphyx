#ifndef ALPHYX_PING_H
#define ALPHYX_PING_H

#include "command.hpp"

class PingCommand: public Command {
  protected:
    void command_exec(
      const dpp::message_create_t & event,
      std::vector<std::string> & words
    ) const override {
      client->message(event, "Ping pong! I work.. I think?");
    }
  public:
    explicit PingCommand(const Client *bot): Command(
      bot,
      CommandType::GENERAL,
      "ping",
      "stupid description"
    ) {}
};

#endif