#ifndef ALPHYX_PONG_H
#define ALPHYX_PONG_H

#include <message-command.hpp>

class PongCommand: public MessageCommand {
  protected:
    void command_exec(
      const dpp::message_create_t & event,
      std::vector<std::string> & words
    ) const override {
      client->message(event, "This command throws an error to test command error handling... I mean PONG~!");

      throw std::string("Pong encountered some error!!!!!!!!!!");
    }
  public:
    explicit PongCommand(Client *bot): MessageCommand(
      bot,
      CommandType::GENERAL,
      "pong",
      "Throw an error to test that the bot handles errors correctly."
    ) {}
};

#endif