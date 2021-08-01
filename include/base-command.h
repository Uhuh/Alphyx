#ifndef ALPHYX_BASECOMMAND_H
#define ALPHYX_BASECOMMAND_H

#include <dpp/dpp.h>
#include <sstream>

class CommandBase {
public:
    static int commandsRan;

    /**
     * Break message events up by spaces and return the vector of strings.
     */
    inline static std::vector<std::string> content(const dpp::message_create_t & event) {
      std::stringstream ss(event.msg->content);
      const std::istream_iterator<std::string> begin(ss);
      const std::istream_iterator<std::string> end;
      std::vector<std::string> words(begin, end);

      return words;
    }
};

#endif