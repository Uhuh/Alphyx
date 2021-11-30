#ifndef ALPHYX_BOT_H
#define ALPHYX_BOT_H

#include <dpp/dpp.h>
#include <dpp/message.h>
#include <dpp/nlohmann/json.hpp>
#include <unordered_map>
#include <utility>
#include <pqxx/pqxx>
#include <commands/slash-command.h>
#include <commands/message-command.hpp>

enum Colors {
    AQUA = 0x00FFFF,
    BLACK = 0x000000,
    WHITE = 0xFFFFFF,
    YELLOW = 0xFFFF00,
    RED = 0xFF0000,
    GREEN = 0x00FF00,
    BLUE = 0x0000FF,
};

enum LogType {
    ERROR = 0,
    INFO,
    SUCCESS,
    DEBUG,
};

class Client {
protected:
    /**
     * Stuff related to the bot. Database, token etc.
     */
    json config;

public:
  explicit Client(std::shared_ptr<dpp::cluster> _cluster, json _config, json _response): cluster(std::move(_cluster)), config(std::move(_config)), response(std::move(_response)) {

    // Text the connection of the postgres on the main server
    connectDb();

    // Load each event handler
    onLog();
    onReady();
    onMessage();
    onButtonClicked();
    slashCommandHandler();
  }

  /**
   * Big JSON file that will make it easier read large bodies of text instead of having all that text in code.
   */
  json response;

  /**
   * Keep general guild information in memory so we don't need to constantly query the DB.
   */
  std::unordered_map<uint64_t, std::vector<uint64_t>> join_roles;

  /**
   * D++'s bot client. It is the main driver.
   */
  std::shared_ptr<dpp::cluster> cluster;

  /**
   * All commands that the bot has access to.
   */
  std::unordered_map<std::string, std::shared_ptr<MessageCommand>> message_command_list;
  std::unordered_map<std::string, std::shared_ptr<SlashCommand>> slash_command_list;

  /**
   * Check all events and find a slash command that correlates with the interaction
   */
  void slashCommandHandler();

  /**
   * Connect to given DB
   * @param db: beta | prod database
   */
  void connectDb();

  /**
   * Making messages for this lib is ugly and I need a shortcut
   */
  void message(
    const dpp::snowflake &channel_id,
    const dpp::snowflake &message_id,
    const std::string &content,
    dpp::message_type type = dpp::message_type::mt_default
  ) const;

  /**
   * Send a Discord embed.
   * @param event
   * @param embed
   */
  void message(
    const dpp::snowflake &channel_id,
    dpp::embed & embed,
    std::function<void(const dpp::confirmation_callback_t&)> cb = [](const dpp::confirmation_callback_t & e){}
  ) const;

  /**
   * Log any message to console with color!
   * @param type Helps decide what color is used
   * @param message Message to log to console
   */
  static void log(LogType type, const std::string &message);

  /**
   * Set bots presence to DND and "Listening"
   */
  void setPresence() const;

  /**
   * How to handle buttons that are clicked.
   * For now just restrict the clicker to be the original button command caller.
   */
  void onButtonClicked() const;

  /**
   * Parse messages and see if it's invoking a command.
   */
  void onMessage() const;

  /**
   * Setup command_list
   */
  void commandsInit();

  /**
   * Show debug logging in console.
   */
  void onLog() const;

  /**
   * Get the current configs Postgres options.
   * @return string that contains postgres host information
   */
  std::string getPostgresConfig() { return this->config["postgresOptions"]; }

  /**
   * Just output some info like the bot has logged in correctly.
   */
  void onReady();
};

#endif