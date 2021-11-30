#include <bot.hpp>
#include <database.h>
#include <fstream>
#include <cstdlib>
#include <ctime>

int main()
{
  srand(time(nullptr));

  json configDocument;
  std::ifstream configfile("../config.json");
  configfile >> configdocument;

  if (!configfile) {
    Client::log(LogType::ERROR, "Couldn't find the config file, aborting boot up.");
    return 1;
  }

  json responseDocument;
  std::ifstream responseFile("../responses.json");
  responsefile >> responsedocument;

  if (!responsefile) {
    Client::log(LogType::ERROR, "Couldn't find responses file, aborting boot up.");
  }

  /* Database stuff */
  createAllTables(configdocument["postgresOptions"]);

  /* Login */
  dpp::cluster _bot(configdocument["token"]);
  Client bot((std::shared_ptr<dpp::cluster>(&_bot)), configdocument, responsedocument);

  bot.cluster->start(false);
  return 0;
}