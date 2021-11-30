#include <bot.hpp>
#include <pqxx/pqxx>
#include <database.h>
#include <pet-commands/adopt.h>
#include <iostream>

void AdoptCommand::command_exec(const dpp::interaction_create_t &event) const {
  dpp::snowflake ownerId = event.command.usr.id;

  pqxx::connection c(client->getPostgresConfig());
  prepareFindPet(c);
  prepareNewPet(c);

  pqxx::work w(c);

  pqxx::result pet = w.exec_prepared("find_pet", ownerId);
  w.commit();

  if (!pet.empty()) {
    event.reply(dpp::ir_channel_message_with_source, "Turns out you have a pet bud!");
    return;
  }

  try {
    event.reply(dpp::ir_channel_message_with_source, "Searching for a lonely spirit to adopt...");
    const int spiritTypeNum = rand()%6 + 1;

    const std::string spiritType = client->response[fmt::format("ADOPT:SPIRIT_TYPE{}", spiritTypeNum)]["name"];

    dpp::embed embed = dpp::embed()
      .set_title(fmt::format("Adopted a {} spirit!", spiritType))
      .set_description(fmt::format(static_cast<std::string>(client->response[fmt::format("ADOPT:SPIRITTYPE{}", spiritTypeNum)]["desc"]),  spiritType + " spirit"));

    client->message(
      event.command.channel_id,
      embed
    );

    w.exec_prepared("new_pet", ownerId, "Bob");
    w.commit();

  } catch(std::exception & e) {
    Client::log(LogType::ERROR, fmt::format("Failed to create pet for user[{}]", ownerId));
    std::cout << e.what() << std::endl;
  }
}