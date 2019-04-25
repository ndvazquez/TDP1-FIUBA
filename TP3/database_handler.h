#ifndef DATABASE_HANDLER_H
#define DATABASE_HANDLER_H
#include "key.h"
#include <map>

class DatabaseHandler{
    std::map<std::string, Key> _clients;
    int _next_id;
    std::string _databasePath;

    void process_line(std::string line,
                            std::map<std::string, Key> &map);

    public:
    DatabaseHandler(std::string databasePath);
    ~DatabaseHandler();
    void insert(const std::string &subject, const Key &publicKey);
    bool lookup(std::string &subject) const;
    void remove(const std::string &subject);
    int getNextId() const;
    Key getPublicKey(std::string &subject);
};

#endif
