#ifndef ALPHYX_DATABASE_H
#define ALPHYX_DATABASE_H

/**
 * Calls other table generation methods to make sure tables exist and are created in the proper order.
 * @param postgresConfig contains postgres user host and password.
 */
void createAllTables(const std::string &postgresConfig);

/**
 * Prepare a SQL statement to return a pet based on an owners ID.
 * @param c Postgres connection
 */
void prepareFindPet(pqxx::connection & c);

/**
 * The SQL for this is stupid long because of a lot of default values.
 * @param c Postgres connection
 */
void prepareNewPet(pqxx::connection & c);

#endif