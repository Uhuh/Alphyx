#ifndef ALPHYX_PONG_H
#define ALPHYX_PONG_H

#include "command.hpp"

class PongCommand: public Command {
  protected:
    void command_exec(
      const dpp::message_create_t & event,
      std::vector<std::string> & words
    ) const override {
      client->message(event, "Going to throw error....");

      throw std::string("Pong encountered some error!!!!!!!!!!");
    }
  public:
    explicit PongCommand(Client *bot): Command(
      bot,
      CommandType::GENERAL,
      "pong",
      "Throw an error to test that the bot handles errors correctly."
    ) {}
};

#endif