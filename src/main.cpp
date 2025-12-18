#include <iostream>
#include <memory>
#include <cstdlib>
#include "database/DatabaseManager.h"
#include "services/HeroService.h"
#include "ui/ConsoleUI.h"

using namespace std;

int main() {
    try {
        cout << "=== Система управления героями Dota 2 ===" << endl;
        
        auto dbManager = make_unique<DatabaseManager>();
        
        string host = getenv("DB_HOST") ? getenv("DB_HOST") : "localhost";
        string port = getenv("DB_PORT") ? getenv("DB_PORT") : "5432";
        string dbname = getenv("DB_NAME") ? getenv("DB_NAME") : "dota_heroes";
        string user = getenv("DB_USER") ? getenv("DB_USER") : "ivan";
        string password = getenv("DB_PASSWORD") ? getenv("DB_PASSWORD") : "";
        
        if (!dbManager->connect(host, port, dbname, user, password)) {
            cerr << "Ошибка: Не удалось подключиться к базе данных!" << endl;
            cerr << "Проверьте настройки подключения и убедитесь, что PostgreSQL запущен." << endl;
            return 1;
        }
        
        cout << "Подключение к базе данных установлено успешно!" << endl;
        
        if (!dbManager->createSchema()) {
            cerr << "Ошибка: Не удалось создать схему базы данных!" << endl;
            return 1;
        }
        
        cout << "Схема базы данных создана успешно!" << endl;
        
        ConsoleUI ui(move(dbManager));
        ui.run();
        
    } catch (const exception& e) {
        cerr << "Критическая ошибка: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}