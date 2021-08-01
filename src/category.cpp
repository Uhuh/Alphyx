#include <bot.hpp>
#include <category/category-create.h>

void CategoryCreateCommand::command_exec(const dpp::message_create_t &event, std::vector<std::string> &words) const {
  dpp::embed embed = dpp::embed();

  embed.set_title("Reaction role setup!")
       .set_description(
         "Testing out embed stufffffffffffff"
       )
       .set_color(Colors::AQUA);

  client->message(event, embed);
}