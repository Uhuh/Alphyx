#ifndef ALPHYX_REACT_MESSAGE_H
#define ALPHYX_REACT_MESSAGE_H

#include <unordered_map>

class ReactMessage {
  private:
    /**
     * uint64_t saves memory
     */
    const uint64_t guild_id;
    /**
     * Emojis can be an ID or unicode so just keep it as a string
     */
    std::unordered_map<string, uint64_t> react_roles;
  public:
    ReactMessage(const uint64_t id): guild_id(id) {
      /**
       * Query DB for info and setup roles.
       */
    }
};

#endif