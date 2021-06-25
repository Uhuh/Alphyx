#include <bot.hpp>
#include <fstream>

int main()
{
  json configdocument;
  std::ifstream configfile("../config.json");
  configfile >> configdocument;

  /* Login */
  dpp::cluster _bot(configdocument["token"]);
  Client bot((std::shared_ptr<dpp::cluster>(&_bot)));

  bot.cluster->start(false);
  return 0;
}