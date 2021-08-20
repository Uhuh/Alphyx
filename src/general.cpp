#include <bot.hpp>
#include <cppconn/prepared_statement.h>
#include <general/join-role.hpp>

/**
 * Add or remove a role to a guilds join list, or list the roles to the user.
 * @param event Slash command with types and possible role id.
 * @param name
 */
void JoinRoleCommand::command_exec(const dpp::interaction_create_t &event) const {
  dpp::command_interaction cmd_data = std::get<dpp::command_interaction>(event.command.data);
  /**
   * This can be add / remove / list.
   */
  std::string action = cmd_data.options.begin()->options.begin()->name;

  if (action == "add" || action == "remove") {
    dpp::snowflake role_id = std::get<dpp::snowflake>(cmd_data.options.begin()->options.begin()->options.begin()->value);
    std::string query = "INSERT INTO join_roles(role_id, guild_id) VALUES (?, ?)";

    if (action == "remove") {
      query = "DELETE FROM join_roles WHERE role_id=?";
    }

    sql::PreparedStatement *prep_stmt = client->con->prepareStatement(query);
    prep_stmt->setBigInt(1, std::to_string(role_id));

    // If we're adding a role then set the guild ID
    if (action == "add") {
      prep_stmt->setBigInt(2, std::to_string(event.command.guild_id));
    }

    // Wording :)
    std::string response = "now";
    if (action == "remove") {
      response = "no longer";
    }

    try {
      prep_stmt->execute();
      event.reply(
        dpp::ir_channel_message_with_source,
        dpp::message(fmt::format("Users will {} get <@&{}> when they join the server.", response, std::to_string(role_id)))
          .set_flags(dpp::m_ephemeral)
      );
    } catch(std::exception & e) {
      /**
       * @TODO - Figure out how to parse error message. Assuming it only throws on duplicates right now
       */
      Client::log(LogType::ERROR, "User tried to add existing role id.");

      event.reply(
        dpp::ir_channel_message_with_source,
        dpp::message("That role is already in the join role list. View the list by running `/join role list`")
        .set_flags(dpp::m_ephemeral)
      );
    }
    return;
  }
  else if (action == "list") {
    client->stmt = client->con->createStatement();
    client->res = client->stmt->executeQuery("SELECT role_id FROM join_roles WHERE guild_id="+std::to_string(event.command.guild_id));

    std::string rolesMentions;

    // Make it so it mentions each role in the embed.
    while(client->res->next()) {
      rolesMentions += fmt::format("<@&{}>\n", client->res->getInt64(1));
    }

    dpp::embed embed;

    embed
      .set_title("Roles given when users join")
      .set_color(Colors::AQUA)
      .set_description(rolesMentions);

    event.reply(
      dpp::ir_channel_message_with_source,
      dpp::message(event.command.channel_id, embed).set_flags(dpp::m_ephemeral)
    );
    return;
  }

  // Users shouldn't be able to get here. If they do then wtf lol
  event.reply(
    dpp::ir_channel_message_with_source,
    dpp::message("You shouldn't be able to see this.......")
    .set_flags(dpp::m_ephemeral)
  );
}