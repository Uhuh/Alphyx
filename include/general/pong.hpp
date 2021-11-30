#ifndef ALPHYX_PONG_H
#define ALPHYX_PONG_H

#include <commands/message-command.hpp>

class PongCommand: public MessageCommand {
  protected:
    void command_exec(
      const dpp::message_create_t & event
    ) const override {
      client->message(
        event.msg->channel_id,
        event.msg->id,
        "This command throws an error to test command error handling... I mean PONG~!"
      );

      dpp::embed embed;

      embed
        .set_title("Category here")
        .set_description("<@&811680018698600478>\n<@&811680018698600478>\n<@&811680018698600478>\n<@&811680018698600478>")
        .set_color(Colors::AQUA);

      client->cluster->message_create(
        dpp::message(event.msg->channel_id, embed).add_component(
          dpp::component().add_component(
            dpp::component().set_label("The real that one guy")
                .set_style(dpp::cos_primary)
                .set_id("811680018698600478")
            )
          )
      );

      dpp::message m(event.msg->channel_id, "Select a role from the dropdown below.");
      m.add_component(
        dpp::component().add_component(
          dpp::component().set_type(dpp::cot_selectmenu)
            .set_placeholder("Select role")
            .add_select_option(dpp::select_option("The real that one guy", "8116800186598600478s", "Self role"))
            .set_id("select34rtrwetwre")
          )
        );

      client->cluster->message_create(m);

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