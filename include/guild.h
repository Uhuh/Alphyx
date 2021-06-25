#ifndef ALPHYX_GUILD_H
#define ALPHYX_GUILD_H

#include <vector>
#include <unordered_map>

class Guild {
  private:
    const uint64_t guild_id;
    std::vector<string> categories;
    std::unordered_map<string, string> react_message;

  public:
    Guild(const uint64_t & id): guild_id(id) {
      /**
       * Probably handle all SQL calls in here when given the guild ID
       */
    }

};

#endif