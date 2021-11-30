#include <string>
#include <bot.hpp>
#include <pqxx/pqxx>

void createTable(const std::string & postgresConfig, const std::string & sql, const std::string & table) {
  try {
    pqxx::connection c(postgresConfig);

    if (c.is_open()) {
      Client::log(LogType::SUCCESS, fmt::format("Connected to Postgres successfully. Going to create {} table.", table));
    } else {
      Client::log(LogType::ERROR, "Couldn't connect to Postgres. Stopping.");
      return;
    }

    pqxx::work w(c);

    w.exec(sql);
    w.commit();

    Client::log(LogType::SUCCESS, fmt::format("Created {} table successfully.", table));
  } catch (std::exception const &e) {
    Client::log(LogType::ERROR, fmt::format("Encountered an error while trying to create {} table.", table));
    Client::log(LogType::ERROR, e.what());
  }
}

void dropAllTables(const std::string & postgresConfig) {
  try {
    pqxx::connection c(postgresConfig);

    if (c.is_open()) {
      Client::log(LogType::SUCCESS, "Connected to Postgres. Starting table dropping");
    } else {
      Client::log(LogType::ERROR, "Couldn't connect to Postgres.");
      return;
    }

    pqxx::work w(c);

    w.exec("DROP TABLE IF EXISTS pet_achievements");
    w.exec("DROP TABLE IF EXISTS pet_guild_info");
    w.exec("DROP TABLE IF EXISTS achievements");
    w.exec("DROP TABLE IF EXISTS pet");
    w.commit();

  } catch (std::exception const &e) {
    Client::log(LogType::ERROR, "Encountered an error when dropping all tables. Oops");
    Client::log(LogType::ERROR, e.what());
  }
}

void createAllTables(const std::string & postgresConfig) {
  dropAllTables(postgresConfig);

  std::string sql;

  // Our pet! It's pretty generic for now! But just you wait.
  sql = "CREATE TABLE pet(" \
        "\"id\"         bigint NOT NULL," \
        "ownerId        bigint NOT NULL," \
        "name           varchar(50) NOT NULL," \
        "description    varchar(250) NULL," \
        "creationDate   date NOT NULL," \
        "totalExp       int NOT NULL," \
        "type           bit(6) NOT NULL," \
        "title          varchar(50) NULL," \
        "reputation     int NULL," \
        "questCompleted int NOT NULL," \
        "timesDefeated  int NOT NULL," \
        "CONSTRAINT PK_5 PRIMARY KEY ( \"id\" ));";

  // Invoke
  createTable(postgresConfig, sql, "pet");

  // Create achievements. Pets can earn these.
  sql = "CREATE TABLE achievements(" \
          "\"id\"   int NOT NULL," \
          "name varchar(250) NOT NULL," \
          "CONSTRAINT PK_23 PRIMARY KEY ( \"id\" ));";

  // Invoke
  createTable(postgresConfig, sql, "achievements");

  // Pet achievements is how we track what pet has what achievement.
  sql = "CREATE TABLE pet_achievements(" \
        "petId         bigint NOT NULL," \
        "achievementId int NOT NULL," \
        "achievedDate  date NOT NULL," \
        "CONSTRAINT FK_12 FOREIGN KEY ( petId ) REFERENCES pet ( \"id\" )," \
        "CONSTRAINT FK_25 FOREIGN KEY ( achievementId ) REFERENCES achievements ( \"id\" ));" \
        "CREATE INDEX FK_14 ON pet_achievements( petId );" \
        "CREATE INDEX FK_27 ON pet_achievements( achievementId );";

  // Invoke
  createTable(postgresConfig, sql, "pet_achievements");

  // Guild info will helps us track pets in different Discord servers
  sql = "CREATE TABLE pet_guild_info(" \
        "guildId        bigint NOT NULL," \
        "petId          bigint NOT NULL," \
        "gold           int NOT NULL," \
        "guildRank      bit(16) NOT NULL," \
        "guildExp       int NOT NULL," \
        "questCompleted int NOT NULL," \
        "CONSTRAINT PK_31 PRIMARY KEY ( guildId )," \
        "CONSTRAINT FK_32 FOREIGN KEY ( petId ) REFERENCES pet ( \"id\" ));" \
        "CREATE INDEX FK_34 ON pet_guild_info( petId );";

  // Invoke
  createTable(postgresConfig, sql, "pet_guild_info");
}

void prepareFindPet(pqxx::connection & c) {
  // Prepare a SQL statement to help find an existing pet.
  c.prepare(
    "find_pet",
    "SELECT * FROM pet WHERE ownerId = $1"
  );
}

void prepareNewPet(pqxx::connection & c) {
  // Prepare the LONGGGG SQL statement to create a pet.
  c.prepare(
    "new_pet",
    "INSERT INTO pet (id, ownerId, name, description, creationDate, totalExp, type, title, reputation, questCompleted, timesDefeated) VALUES ($1, $1, $2, 'hello world', '2021-01-01', 0, B'000001', 'bob title', 0, 0, 0)"
  );
}