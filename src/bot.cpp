#include <bot.hpp>
#include <thread>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <pqxx/pqxx>

void Client::slashCommandHandler() {
  this->cluster->on_interaction_create([&](const dpp::interaction_create_t & event) {
    if (event.command.type == dpp::it_application_command) {
      dpp::command_interaction cmd_data = std::get<dpp::command_interaction>(event.command.data);

      std::shared_ptr<SlashCommand> slash_command = this->slash_command_list.at(cmd_data.name);

      if (!slash_command) return;

      slash_command->run(event);
    }
  });

  Client::log(LogType::INFO, "Finished setting up slash command handler.");
}

void Client::connectDb() {
  Client::log(LogType::INFO, "Testing if server is accepting Postgres connection...");

  pqxx::connection c (this->getPostgresConfig());

  if (c.is_open()) {
    Client::log(LogType::SUCCESS, "Connection test successful.");
  } else {
    Client::log(LogType::ERROR, "Could not connect to Postgres. Exiting.");
    throw "Unable to connect to Postgres.";
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
    std::vector<std::string> content(std::move(CommandBase::content(event)));

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

    if (prefix != '!' || this->message_command_list.find(commandName) == this->message_command_list.end()) return;

    std::shared_ptr<MessageCommand> commandPtr = this->message_command_list.at(commandName);
    if (!commandPtr) return;

    // Remove the command portion since we already parsed it out above.
    content.erase(content.begin());

    try {
      Client::log(LogType::DEBUG, "Message content size: " + std::to_string(content.size()));
      commandPtr->run(event);
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
  });

  // Run loads on separate thread to ensure that guild cache is loaded.
  std::thread t1([this](){
      usleep(1000000);
      commandsInit();
  });

  t1.detach();
}

void Client::message(
  const dpp::snowflake &channel_id,
  const dpp::snowflake &message_id,
  const std::string &content,
  dpp::message_type type
) const {
  try {
    switch (type) {
      case dpp::mt_default:
        cluster->message_create(dpp::message(
          channel_id,
          content,
          type
        ));
        break;
      case dpp::mt_reply:
        // Create message for channel with correct content.
        dpp::message m(channel_id, content);
        // Set messages original ref ID so we can reply to the user
        m.set_reference(message_id).set_type(dpp::mt_reply);
        cluster->message_create(m);
        break;
    }
  } catch (std::exception &e) {
    Client::log(LogType::ERROR, "============ Message sending error start ============");
    Client::log(LogType::ERROR, e.what());
    Client::log(LogType::ERROR, "============  Message sending error end  ============");
  }
}

/**
 * @TODO - To attach buttons to embeds, use an ENUM to identify which command is being used
 * @TODO - to determine which buttons to add and how to process them.
 */
void Client::message(
  const dpp::snowflake &channel_id,
  dpp::embed & embed,
  std::function<void(const dpp::confirmation_callback_t&)> cb
) const {
  try {
    cluster->message_create(
      dpp::message(
        channel_id,
        embed
      ),
      std::move(cb)
    );
  } catch (std::exception &e) {
    Client::log(LogType::ERROR, "============ Message sending error start ============");
    Client::log(LogType::ERROR, e.what());
    Client::log(LogType::ERROR, "============  Message sending error end  ============");
  }
}
