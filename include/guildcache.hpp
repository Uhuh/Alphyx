#ifndef ALPHYX_GUILD_CACHE_H
#define ALPHYX_GUILD_CACHE_H

#include <unordered_map>

class GuildCache {
protected:
    std::unordered_map <uint64_t, uint64_t> role_permissions;
    std::unordered_map <uint64_t, uint64_t> role_permissions;
    std::unordered_map <uint64_t, uint64_t> channel_permissions;
};

#endif