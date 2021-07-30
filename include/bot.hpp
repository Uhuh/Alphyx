#ifndef ALPHYX_BOT_H
#define ALPHYX_BOT_H

#include <command.hpp>
#include <dpp/dpp.h>
#include <dpp/message.h>
#include <dpp/nlohmann/json.hpp>
#include <unordered_map>
#include <utility>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

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
  public:
    explicit Client(std::shared_ptr<dpp::cluster> _cluster): cluster(std::move(_cluster)) {

      // Prepare and connect to db
      driver = nullptr;
      connectDb("beta");

      // Load each event handler
      onLog();
      onReady();
      onMessage();
      commandsInit();
      onButtonClicked();
      slashCommandHandler();
    }

    ~Client() {
      /**
       * Have to destroy these, don't wanna lose memory.
       */
      delete con;
      delete stmt;
      delete res;
    }

    /**
     * Keep general guild information in memory so we don't need to constantly query the DB.
     */
    std::unordered_map<uint64_t, std::vector<uint64_t>> join_roles;

    /**
     * D++'s bot client. It is the main driver.
     */
    std::shared_ptr<dpp::cluster> cluster;

    /**
     * Ugly pointers required for the MySQL connection stuff
     */
    sql::Driver *driver;
    sql::Connection *con{};
    sql::Statement *stmt{};
    sql::ResultSet *res{};

    /**
     * All commands that the bot has access to.
     */
    std::unordered_map<std::string, std::shared_ptr<Command>> command_list;
    std::unordered_map<std::string, std::shared_ptr<Command>> slash_command_list;

    /**
     * Check all events and find a slash command that correlates with the interaction
     */
    void slashCommandHandler();

    /**
     * Connect to given DB
     * @param db: beta | prod database
     */
    void connectDb(const std::string &db);

    /**
     * Making messages for this lib is ugly and I need a shortcut
     */
    void message(
      const dpp::message_create_t &event,
      const std::string &content,
      dpp::message_type type = dpp::message_type::mt_default
    ) const;

    /**
     * Send a Discord embed.
     * @param event
     * @param embed
     */
    void message(
      const dpp::message_create_t & event,
      dpp::embed & embed,
      std::function<void(const dpp::confirmation_callback_t&)> cb = [](const dpp::confirmation_callback_t & e){}
    ) const;

    /**
     * Load all relevant information, like setting up guild cache, db calls, etc.
     */
    void loadData() {
      loadJoinRoles();
    }

    /**
     * Load each guilds join roles
     */
    void loadJoinRoles();

    /**
     * Log any message to console with color!
     * @param type Helps decide what color is used
     * @param message Message to log to console
     */
    static void log(LogType type, const std::string &message);

    /**
     *
     * @param guildId
     * @param roleId
     * @param emojiId
     */
    void createJoinRole(uint64_t guildId, uint64_t roleId, const std::string& emojiId) const;

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
     * Just output some info like the bot has logged in correctly.
     */
    void onReady();
};

#endif