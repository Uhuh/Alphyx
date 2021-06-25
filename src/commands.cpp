#include <bot.hpp>

#include <cppconn/prepared_statement.h>

#include <ping.hpp>
#include <pong.hpp>
#include <join_role.hpp>

int Command::commandsRan = 0;

/**
 *
 * @param event Should have mentioned role or passed ID as well with saying what join event.
 * @param words AAAAAAAAAAAAA
 */
void JoinRoleCommand::command_exec(const dpp::message_create_t &event, std::vector<std::string> &words) const {
  Client::log(LogType::INFO, "Attempting to do joinrole command");

  if (words.empty()) {
    client->message(event, "Hey listen! You forgot some parameters.", dpp::mt_reply);
    return;
  }

  /**
   * Handle join operation(?) EG: add / remove / list
   */

  uint64_t roleId;
  std::string query = "SELECT role_id FROM join_roles WHERE guild_id=?";
  // Must be add or remove
  std::string type = words.front();

  if (type != "add" && type != "remove") {
    client->message(event, "You need to specify either `add` or `remove` for each role.");
    return;
  }

  if (!event.msg->mention_roles.empty()) {
    roleId = event.msg->mention_roles.front();
  } else {
    // Remove the add/remove portion.
    words.erase(words.begin());

    try {
      // Cast user input to verify it's a real ID;
      roleId = std::stoull(words.front());

      dpp::cache *role_cache = dpp::get_role_cache();
      dpp::cache_container &rc = role_cache->get_container();

      if (rc.find(roleId) == rc.end()) {
        client->message(event, "That doesn't appear to be a real role ID... try again.");
        return;
      }
    } catch (const std::invalid_argument & e) {
      Client::log(LogType::INFO, "User passed in a non uint type for role ID.");
      client->message(event, "That's not a role ID!");
      return;
    }
  }

  // Prepare statements for following query
  if (type == "add") {
    query = "INSERT INTO join_roles(role_id, guild_id) VALUES (?, ?)";
  } else if (type == "remove") {
    query = "DELETE FROM join_roles WHERE role_id=?";
  }

  sql::PreparedStatement *prep_stmt = client->con->prepareStatement(query);
  prep_stmt->setBigInt(1, std::to_string(roleId));

  if (type == "add") {
    prep_stmt->setBigInt(2, std::to_string(event.msg->guild_id));
  }

  try {
    prep_stmt->execute();
  } catch(std::exception & e) {
    /**
     * @TODO - Figure out how to parse error message. Assuming it only throws on duplicates right now
     */
     Client::log(LogType::ERROR, "User tried to add existing role id.");
     client->message(event, "That role is already in the join role list. View the list by running `rb join-role list`");
     return;
  }

  /**
   * @TODO - Implement listing join roles.
   */

  Client::log(LogType::SUCCESS, "Successfully executed " + type + " for guild[" + std::to_string(event.msg->guild_id) + "]");
}

void Client::commandsInit() {
  command_list = {
    { "ping", std::make_shared<PingCommand>(this) },
    { "pong", std::make_shared<PongCommand>(this) },
    { "join-role", std::make_shared<JoinRoleCommand>(this)}
  };
}

