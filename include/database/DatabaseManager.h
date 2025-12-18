#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <stdexcept>
#include <pqxx/pqxx>

using namespace std;

class DatabaseException : public runtime_error {
public:
    explicit DatabaseException(const string& message) : runtime_error(message) {}
};

class DatabaseManager {
private:
    unique_ptr<pqxx::connection> conn;
    string connectionString;

public:
    DatabaseManager();
    ~DatabaseManager();

    bool connect(const string& host, const string& port, 
                 const string& dbname, const string& user, 
                 const string& password);
    
    bool isConnected() const;
    void disconnect();
    bool createSchema();
    pqxx::result executeQuery(const string& query);
    pqxx::result executeQuery(const string& query, const vector<string>& params);
    void preparStatement(const string& name, const string& query);
    pqxx::result executePrepared(const string& name, const vector<string>& params = {});
    unique_ptr<pqxx::work> beginTransaction();
    void commitTransaction(unique_ptr<pqxx::work>& txn);
    void rollbackTransaction(unique_ptr<pqxx::work>& txn);
    pqxx::connection& getConnection();
};