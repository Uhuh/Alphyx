#include <bot.hpp>
#include <thread>
#include "mysql_connection.h"
#include <cppconn/prepared_statement.h>

void Client::connectDb(const std::string &db) {
  try {
    Client::log(LogType::INFO, "Attempt to connect to SQL DB");

    /* Create a connection */
    driver = get_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "");

    /* Connect to the MySQL test database */
    con->setSchema(db);
    Client::log(LogType::SUCCESS, "Successfully connected to " + db );
  } catch(int e) {
    Client::log(LogType::ERROR, "Oops, couldn't connect to the DB for some reason.");
  }
}

void Client::log(const LogType type, const std::string &message) {
  std::string colorType;

  switch (type) {
    case LogType::ERROR: colorType = "\033[31m[ ERROR ]\033[0m - ";     break;
    case LogType::SUCCESS: colorType = "\033[32m[ SUCCESS ]\033[0m - "; break;
    case LogType::INFO: colorType = "\033[33m[ INFO ]\033[0m - ";       break;
    case LogType::DEBUG: colorType = "\033[35m[ DEBUG ]\033[0m - ";     break;
    default: colorType = "\033[32m[ SUCCESS ]\033[0m - ";
  }

  std::cout << colorType << message << std::endl;
}

void Client::loadJoinRoles() {
  Client::log(LogType::INFO, "Loading join roles...");
  usleep(1000000);
  dpp::cache *guild_cache = dpp::get_guild_cache();
  dpp::cache_container &gc = guild_cache->get_container();

  std::vector<uint64_t> roleIds;

  con->setSchema("beta");

  for (auto &g: gc) {
    auto* gp = (dpp::guild*)g.second;
    roleIds = {};

    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT role_id FROM join_roles WHERE guild_id="+std::to_string(gp->id));

    Client::log(LogType::INFO, "Loading join roles for guild " + std::to_string(gp->id));

    while(res->next()) {
      roleIds.push_back(res->getInt64(1));
    }

    join_roles[gp->id] = std::move(roleIds);
  }

  Client::log(LogType::SUCCESS, "Loaded " + std::to_string(gc.size()) + " guild join_roles");
}

void Client::createJoinRole(uint64_t guildId, uint64_t roleId, const std::string& emojiId) const {
  sql::PreparedStatement *prep_stmt;

  try {
    prep_stmt = con->prepareStatement("INSERT INTO join_roles(guild_id, role_id) VALUES (?, ?)");

    prep_stmt->setBigInt(1, std::to_string(guildId));
    prep_stmt->setBigInt(2, std::to_string(roleId));
    prep_stmt->execute();
  } catch(const int &e) {
    const std::string errorMessage = "Failed to insert role_id[" + std::to_string(roleId) + "] for guild_id[" + std::to_string(guildId) + "]";
    Client::log(LogType::ERROR, errorMessage);
  }

  delete prep_stmt;
}

void Client::createMessageButton(
  const dpp::message_create_t & event,
  const std::string & content,
  const std::string & label, 
  const std::string & emoji,
  const dpp::component_style & style,
  const std::string & id
) const {
  const std::string user_id = std::to_string(event.msg->author->id);
  
  // Just wanted to remove this bulky mess from being called 20 times.
  cluster->message_create(
    dpp::message(event.msg->channel_id, content).add_component(
      dpp::component().add_component(
        dpp::component().set_label(label)
          .set_emoji(emoji)
          .set_style(style)
          .set_id(id + user_id)
      )
    )
  );
}

void Client::setPresence() const {
  Client::log(LogType::INFO, "Setting bot presence...");

  cluster->set_presence(dpp::presence(
    dpp::presence_status::ps_dnd, dpp::activity_type::at_listening, "Learning..."
  ));
}

void Client::onButtonClicked() const {
  cluster->on_button_click([](const dpp::button_click_t & event) {
    const json raw_event = json::parse(event.raw_event);
    const std::string user_id = raw_event["d"]["member"]["user"]["id"];

    if (event.custom_id.find(user_id) != std::string::npos) {
      event.reply(dpp::ir_channel_message_with_source, "<@" + user_id + "> clicked me.");
    }
  });
}

void Client::onMessage() const {
  cluster->on_message_create([this](const dpp::message_create_t & event) {
    std::vector<std::string> content(std::move(Command::content(event)));

    if (content.empty() ||
        !event.msg->guild_id ||
        event.msg->author->is_bot()
      ) return;

    /**::
     * Break down message and look for a command and prefix.
     */
    const std::string command = content.front();
    const char prefix = command[0];
    const std::string commandName = command.substr(1);

    if (prefix != '!' || this->command_list.find(commandName) == this->command_list.end()) return;

    std::shared_ptr<Command> commandPtr = this->command_list.at(commandName);
    if (!commandPtr) return;

    // Remove the command portion since we already parsed it out above.
    content.erase(content.begin());

    try {
      Client::log(LogType::DEBUG, "Message content size: " + std::to_string(content.size()));
      commandPtr->run(event, content);
    } catch (const std::string &error) {
      Client::log(LogType::ERROR, error);
    }
  });
}

void Client::onLog() const {
  /* Log event */
  cluster->on_log([](const dpp::log_t &event) {
    if (event.severity >= dpp::ll_debug)
    {
      std::cout << dpp::utility::current_date_time() << " [" << dpp::utility::loglevel(event.severity) << "] " << event.message << "\n";
    }
  });
}

void Client::onReady() {
  cluster->on_ready([this](const dpp::ready_t & event) {
    this->setPresence();

    Client::log(LogType::SUCCESS, "Shard [" + std::to_string(event.shard_id) + "] connected successfully.");
    Client::log(LogType::SUCCESS, "Logged in as " + cluster->me.username);

    // Run loads on separate thread to ensure that guild cache is loaded.
    std::thread t1([this](){
      loadData();
    });

    t1.detach();
  });
}

void Client::message(const dpp::message_create_t &event, const std::string &content, dpp::message_type type) const {
  try {
    switch (type) {
      case dpp::mt_default:
        cluster->message_create(dpp::message(
          event.msg->channel_id,
          content,
          type
        ));
        break;
      case dpp::mt_reply:
        // Create message for channel with correct content.
        dpp::message m(event.msg->channel_id, content);
        // Set messages original ref ID so we can reply to the user
        m.set_reference(event.msg->id).set_type(dpp::mt_reply);
        cluster->message_create(m);
        break;
    }
  } catch (std::exception &e) {
    Client::log(LogType::ERROR, "============ Message sending error start ============");
    Client::log(LogType::ERROR, e.what());
    Client::log(LogType::ERROR, "============  Message sending error end  ============");
  }
}

