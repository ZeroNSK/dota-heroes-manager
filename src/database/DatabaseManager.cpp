#include "database/DatabaseManager.h"
#include <iostream>
#include <sstream>

using namespace std;

DatabaseManager::DatabaseManager() : conn(nullptr) {}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

bool DatabaseManager::connect(const string& host, const string& port, 
                             const string& dbname, const string& user, 
                             const string& password) {
    try {
        if (host.empty() || port.empty() || dbname.empty() || user.empty()) {
            cerr << "Ошибка: Все параметры подключения должны быть заполнены" << endl;
            return false;
        }
        
        ostringstream connStr;
        connStr << "host=" << host 
                << " port=" << port 
                << " dbname=" << dbname 
                << " user=" << user;
        
        if (!password.empty()) {
            connStr << " password=" << password;
        }
        
        connectionString = connStr.str();
        conn = make_unique<pqxx::connection>(connectionString);
        
        if (conn->is_open()) {
            cout << "Подключено к базе данных: " << conn->dbname() << endl;
            
            pqxx::nontransaction test(*conn);
            test.exec("SELECT 1");
            
            return true;
        }
    } catch (const pqxx::broken_connection& e) {
        cerr << "Ошибка подключения к базе данных: Не удалось установить соединение. "
                  << "Проверьте, что PostgreSQL запущен и доступен по указанному адресу." << endl;
        conn.reset();
    } catch (const pqxx::sql_error& e) {
        cerr << "Ошибка SQL при подключении: " << e.what() << endl;
        conn.reset();
    } catch (const exception& e) {
        cerr << "Неожиданная ошибка подключения: " << e.what() << endl;
        conn.reset();
    }
    return false;
}

bool DatabaseManager::isConnected() const {
    return conn && conn->is_open();
}

void DatabaseManager::disconnect() {
    if (conn && conn->is_open()) {
        conn->close();
        cout << "Соединение с базой данных закрыто." << endl;
    }
    conn.reset();
}

bool DatabaseManager::createSchema() {
    if (!isConnected()) {
        cerr << "Нет подключения к базе данных!" << endl;
        return false;
    }
    
    try {
        pqxx::work txn(*conn);
        cout << "Создание нормализованной схемы базы данных (5 таблиц)..." << endl;
        
        txn.exec("DROP TABLE IF EXISTS hero_growth CASCADE");
        txn.exec("DROP TABLE IF EXISTS hero_attributes CASCADE");
        txn.exec("DROP TABLE IF EXISTS hero_damage CASCADE");
        txn.exec("DROP TABLE IF EXISTS hero_stats CASCADE");
        txn.exec("DROP TABLE IF EXISTS heroes CASCADE");
        
        txn.exec(R"(
            CREATE TABLE heroes (
                hero_id SERIAL PRIMARY KEY,
                name VARCHAR(50) UNIQUE NOT NULL,
                display_name VARCHAR(100) NOT NULL,
                attribute_type VARCHAR(20) NOT NULL CHECK (attribute_type IN ('Strength', 'Agility', 'Intelligence')),
                complexity INTEGER NOT NULL CHECK (complexity >= 1 AND complexity <= 3),
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        )");
        
        txn.exec(R"(
            CREATE TABLE hero_stats (
                stat_id SERIAL PRIMARY KEY,
                hero_id INTEGER NOT NULL REFERENCES heroes(hero_id) ON DELETE CASCADE,
                base_health INTEGER NOT NULL CHECK (base_health > 0),
                base_mana INTEGER NOT NULL CHECK (base_mana >= 0),
                base_armor INTEGER NOT NULL,
                base_move_speed INTEGER NOT NULL CHECK (base_move_speed > 0),
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        )");
        
        txn.exec(R"(
            CREATE TABLE hero_damage (
                damage_id SERIAL PRIMARY KEY,
                hero_id INTEGER NOT NULL REFERENCES heroes(hero_id) ON DELETE CASCADE,
                base_damage_min INTEGER NOT NULL CHECK (base_damage_min > 0),
                base_damage_max INTEGER NOT NULL CHECK (base_damage_max >= base_damage_min),
                base_attack_speed REAL NOT NULL CHECK (base_attack_speed > 0),
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        )");
        
        txn.exec(R"(
            CREATE TABLE hero_attributes (
                attribute_id SERIAL PRIMARY KEY,
                hero_id INTEGER NOT NULL REFERENCES heroes(hero_id) ON DELETE CASCADE,
                base_strength INTEGER NOT NULL CHECK (base_strength > 0),
                base_agility INTEGER NOT NULL CHECK (base_agility > 0),
                base_intelligence INTEGER NOT NULL CHECK (base_intelligence > 0),
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        )");
        
        txn.exec(R"(
            CREATE TABLE hero_growth (
                growth_id SERIAL PRIMARY KEY,
                hero_id INTEGER NOT NULL REFERENCES heroes(hero_id) ON DELETE CASCADE,
                strength_gain REAL NOT NULL CHECK (strength_gain > 0),
                agility_gain REAL NOT NULL CHECK (agility_gain > 0),
                intelligence_gain REAL NOT NULL CHECK (intelligence_gain > 0),
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        )");
        
        txn.exec("CREATE INDEX idx_heroes_attribute_type ON heroes(attribute_type)");
        txn.exec("CREATE INDEX idx_heroes_complexity ON heroes(complexity)");
        txn.exec("CREATE INDEX idx_heroes_name ON heroes(name)");
        txn.exec("CREATE INDEX idx_hero_stats_hero_id ON hero_stats(hero_id)");
        txn.exec("CREATE INDEX idx_hero_damage_hero_id ON hero_damage(hero_id)");
        txn.exec("CREATE INDEX idx_hero_attributes_hero_id ON hero_attributes(hero_id)");
        txn.exec("CREATE INDEX idx_hero_growth_hero_id ON hero_growth(hero_id)");
        
        txn.exec(R"(
            CREATE OR REPLACE FUNCTION update_updated_at_column()
            RETURNS TRIGGER AS $
            BEGIN
                NEW.updated_at = CURRENT_TIMESTAMP;
                RETURN NEW;
            END;
            $ language 'plpgsql'
        )");
        
        txn.exec(R"(
            CREATE TRIGGER update_heroes_updated_at 
                BEFORE UPDATE ON heroes 
                FOR EACH ROW 
                EXECUTE FUNCTION update_updated_at_column()
        )");
        
        txn.exec(R"(
            INSERT INTO heroes (name, display_name, attribute_type, complexity) VALUES
            ('pudge', 'Pudge', 'Strength', 2),
            ('invoker', 'Invoker', 'Intelligence', 3),
            ('anti_mage', 'Anti-Mage', 'Agility', 1),
            ('crystal_maiden', 'Crystal Maiden', 'Intelligence', 1),
            ('axe', 'Axe', 'Strength', 1),
            ('meepo', 'Meepo', 'Agility', 3)
        )");
        
        txn.exec(R"(
            INSERT INTO hero_stats (hero_id, base_health, base_mana, base_armor, base_move_speed) VALUES
            (1, 700, 267, 1, 280),
            (2, 492, 195, 1, 280),
            (3, 558, 219, 2, 310),
            (4, 426, 291, -1, 280),
            (5, 625, 234, 2, 310),
            (6, 588, 243, 2, 330)
        )");
        
        txn.exec(R"(
            INSERT INTO hero_damage (hero_id, base_damage_min, base_damage_max, base_attack_speed) VALUES
            (1, 68, 78, 1.7),
            (2, 42, 54, 1.7),
            (3, 53, 57, 1.4),
            (4, 38, 44, 1.7),
            (5, 52, 58, 1.7),
            (6, 39, 45, 1.7)
        )");
        
        txn.exec(R"(
            INSERT INTO hero_attributes (hero_id, base_strength, base_agility, base_intelligence) VALUES
            (1, 25, 14, 16),
            (2, 19, 14, 17),
            (3, 22, 22, 12),
            (4, 16, 16, 16),
            (5, 25, 20, 18),
            (6, 23, 23, 20)
        )");
        
        txn.exec(R"(
            INSERT INTO hero_growth (hero_id, strength_gain, agility_gain, intelligence_gain) VALUES
            (1, 3.2, 1.5, 1.5),
            (2, 2.4, 1.9, 4.6),
            (3, 2.6, 2.8, 1.8),
            (4, 1.7, 1.6, 2.9),
            (5, 2.8, 2.2, 1.6),
            (6, 1.9, 1.9, 1.6)
        )");
        
        txn.commit();
        cout << "Нормализованная схема базы данных (5 таблиц) создана успешно!" << endl;
        return true;
        
    } catch (const pqxx::sql_error& e) {
        cerr << "Ошибка SQL при создании схемы: " << e.what() << endl;
        cerr << "SQL состояние: " << e.sqlstate() << endl;
        return false;
    } catch (const pqxx::broken_connection& e) {
        cerr << "Потеряно соединение при создании схемы: " << e.what() << endl;
        conn.reset();
        return false;
    } catch (const exception& e) {
        cerr << "Неожиданная ошибка создания схемы: " << e.what() << endl;
        return false;
    }
}

pqxx::result DatabaseManager::executeQuery(const string& query) {
    if (!isConnected()) {
        throw DatabaseException("Нет подключения к базе данных");
    }
    
    try {
        pqxx::nontransaction ntxn(*conn);
        return ntxn.exec(query);
    } catch (const pqxx::sql_error& e) {
        string errorMsg = "Ошибка SQL запроса: " + string(e.what());
        cerr << errorMsg << endl;
        throw DatabaseException(errorMsg);
    } catch (const pqxx::broken_connection& e) {
        string errorMsg = "Потеряно соединение с базой данных: " + string(e.what());
        cerr << errorMsg << endl;
        conn.reset();
        throw DatabaseException(errorMsg);
    } catch (const exception& e) {
        string errorMsg = "Ошибка выполнения запроса: " + string(e.what());
        cerr << errorMsg << endl;
        throw DatabaseException(errorMsg);
    }
}

pqxx::result DatabaseManager::executeQuery(const string& query, const vector<string>& params) {
    if (!isConnected()) {
        throw DatabaseException("Нет подключения к базе данных");
    }
    
    try {
        pqxx::nontransaction ntxn(*conn);
        
        if (params.empty()) {
            return ntxn.exec(query);
        }
        
        return ntxn.exec(query, params);
        
    } catch (const pqxx::sql_error& e) {
        string errorMsg = "Ошибка SQL запроса: " + string(e.what());
        cerr << errorMsg << endl;
        throw DatabaseException(errorMsg);
    } catch (const pqxx::broken_connection& e) {
        string errorMsg = "Потеряно соединение с базой данных: " + string(e.what());
        cerr << errorMsg << endl;
        conn.reset();
        throw DatabaseException(errorMsg);
    } catch (const exception& e) {
        string errorMsg = "Ошибка выполнения параметризованного запроса: " + string(e.what());
        cerr << errorMsg << endl;
        throw DatabaseException(errorMsg);
    }
}

unique_ptr<pqxx::work> DatabaseManager::beginTransaction() {
    if (!isConnected()) {
        throw DatabaseException("Нет подключения к базе данных");
    }
    
    try {
        return make_unique<pqxx::work>(*conn);
    } catch (const pqxx::broken_connection& e) {
        string errorMsg = "Потеряно соединение при создании транзакции: " + string(e.what());
        cerr << errorMsg << endl;
        conn.reset();
        throw DatabaseException(errorMsg);
    } catch (const exception& e) {
        string errorMsg = "Ошибка создания транзакции: " + string(e.what());
        cerr << errorMsg << endl;
        throw DatabaseException(errorMsg);
    }
}

void DatabaseManager::commitTransaction(unique_ptr<pqxx::work>& txn) {
    if (!txn) {
        throw DatabaseException("Попытка зафиксировать несуществующую транзакцию");
    }
    
    try {
        txn->commit();
        txn.reset();
    } catch (const pqxx::sql_error& e) {
        string errorMsg = "Ошибка фиксации транзакции: " + string(e.what());
        cerr << errorMsg << endl;
        txn.reset();
        throw DatabaseException(errorMsg);
    } catch (const exception& e) {
        string errorMsg = "Неожиданная ошибка при фиксации транзакции: " + string(e.what());
        cerr << errorMsg << endl;
        txn.reset();
        throw DatabaseException(errorMsg);
    }
}

void DatabaseManager::rollbackTransaction(unique_ptr<pqxx::work>& txn) {
    if (!txn) {
        return;
    }
    
    try {
        txn->abort();
        txn.reset();
    } catch (const exception& e) {
        cerr << "Ошибка отката транзакции: " << e.what() << endl;
        txn.reset();
    }
}

pqxx::connection& DatabaseManager::getConnection() {
    if (!isConnected()) {
        throw DatabaseException("Нет подключения к базе данных");
    }
    return *conn;
}

void DatabaseManager::preparStatement(const string& name, const string& query) {
    if (!isConnected()) {
        throw DatabaseException("Нет подключения к базе данных");
    }
    
    try {
        conn->prepare(name, query);
        cout << "Подготовленный запрос '" << name << "' создан успешно" << endl;
    } catch (const pqxx::sql_error& e) {
        string errorMsg = "Ошибка создания подготовленного запроса '" + name + "': " + string(e.what());
        cerr << errorMsg << endl;
        throw DatabaseException(errorMsg);
    } catch (const exception& e) {
        string errorMsg = "Неожиданная ошибка при создании подготовленного запроса '" + name + "': " + string(e.what());
        cerr << errorMsg << endl;
        throw DatabaseException(errorMsg);
    }
}

pqxx::result DatabaseManager::executePrepared(const string& name, const vector<string>& params) {
    if (!isConnected()) {
        throw DatabaseException("Нет подключения к базе данных");
    }
    
    try {
        pqxx::nontransaction ntxn(*conn);
        
        if (params.empty()) {
            return ntxn.exec_prepared(name);
        } else {
            return ntxn.exec_prepared(name, params);
        }
        
    } catch (const pqxx::sql_error& e) {
        string errorMsg = "Ошибка выполнения подготовленного запроса '" + name + "': " + string(e.what());
        cerr << errorMsg << endl;
        throw DatabaseException(errorMsg);
    } catch (const pqxx::broken_connection& e) {
        string errorMsg = "Потеряно соединение при выполнении подготовленного запроса '" + name + "': " + string(e.what());
        cerr << errorMsg << endl;
        conn.reset();
        throw DatabaseException(errorMsg);
    } catch (const exception& e) {
        string errorMsg = "Неожиданная ошибка при выполнении подготовленного запроса '" + name + "': " + string(e.what());
        cerr << errorMsg << endl;
        throw DatabaseException(errorMsg);
    }
}