#ifndef DATABASE_HANDLER_H
#define DATABASE_HANDLER_H
#include "common_key.h"
#include <map>
#include <mutex>
#include <condition_variable>

class DatabaseHandler{
    std::condition_variable _cv;
    std::mutex _mtx;
    std::map<std::string, Key> _clients;
    int _next_id;
    std::string _databasePath;

    void process_line(std::string line,
                            std::map<std::string, Key> &map);

    public:
    DatabaseHandler();
    ~DatabaseHandler();
    void initializeData(std::string dbPath);
    void insert(const std::string &subject, Key &publicKey);
    bool lookup(std::string &subject);
    void remove(const std::string &subject);
    int getNextId();
    Key getPublicKey(std::string &subject);
};

#endif
