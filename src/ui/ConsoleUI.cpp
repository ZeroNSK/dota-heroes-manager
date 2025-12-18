#include "ui/ConsoleUI.h"
#include <iostream>
#include <limits>
#include <iomanip>

using namespace std;

ConsoleUI::ConsoleUI(unique_ptr<DatabaseManager> db) : dbManager(move(db)) {
    heroService = make_unique<HeroService>(*dbManager);
}

void ConsoleUI::run() {
    cout << "\n=== Добро пожаловать в упрощенную систему управления героями Dota 2! ===" << endl;
    
    int choice;
    do {
        showMainMenu();
        choice = getIntInput("");
        handleMenuChoice(choice);
        
    } while (choice != 0);
    
    cout << "До свидания!" << endl;
}

void ConsoleUI::showMainMenu() {
    cout << "\n==================== ГЛАВНОЕ МЕНЮ ====================" << endl;
    cout << "1. Информация о базе данных" << endl;
    cout << "2. Тест подключения" << endl;
    cout << "3. Управление героями" << endl;
    cout << "4. Статистика и агрегация" << endl;
    cout << "5. Демонстрация возможностей PostgreSQL" << endl;
    cout << "0. Выход" << endl;
    cout << "======================================================" << endl;
    cout << "Выберите пункт меню: ";
}

void ConsoleUI::handleMenuChoice(int choice) {
    switch (choice) {
        case 1:
            showDatabaseInfo();
            break;
        case 2:
            testConnection();
            break;
        case 3:
            showHeroMenu();
            break;
        case 4:
            showStatisticsMenu();
            break;
        case 5:
            demonstratePostgreSQLFeatures();
            break;
        case 0:
            break;
        default:
            cout << "Неверный выбор. Попробуйте снова." << endl;
            break;
    }
}

void ConsoleUI::showDatabaseInfo() {
    cout << "\n=== ИНФОРМАЦИЯ О БАЗЕ ДАННЫХ ===" << endl;
    
    if (!dbManager->isConnected()) {
        cout << "Нет подключения к базе данных!" << endl;
        return;
    }
    
    try {
        auto result = dbManager->executeQuery(R"(
            SELECT table_name 
            FROM information_schema.tables 
            WHERE table_schema = 'public' 
            ORDER BY table_name
        )");
        
        cout << "Созданные таблицы:" << endl;
        for (const auto& row : result) {
            cout << "  - " << row[0].c_str() << endl;
        }
        
        auto countResult = dbManager->executeQuery("SELECT COUNT(*) FROM heroes");
        if (!countResult.empty()) {
            cout << "\nКоличество героев в базе: " << countResult[0][0].c_str() << endl;
        }
        
        auto structureResult = dbManager->executeQuery(R"(
            SELECT column_name, data_type, is_nullable 
            FROM information_schema.columns 
            WHERE table_name = 'heroes' 
            ORDER BY ordinal_position
        )");
        
        cout << "\nСтруктура таблицы heroes:" << endl;
        for (const auto& row : structureResult) {
            cout << "  " << row[0].c_str() << " (" << row[1].c_str() << ")" << endl;
        }
        
    } catch (const exception& e) {
        cout << "Ошибка получения информации о базе данных: " << e.what() << endl;
    }
}

void ConsoleUI::testConnection() {
    cout << "\n=== ТЕСТ ПОДКЛЮЧЕНИЯ ===" << endl;
    
    if (dbManager->isConnected()) {
        cout << "✓ Подключение к базе данных активно" << endl;
        
        try {
            auto result = dbManager->executeQuery("SELECT version()");
            if (!result.empty()) {
                cout << "✓ Версия PostgreSQL: " << result[0][0].c_str() << endl;
            }
            
            auto timeResult = dbManager->executeQuery("SELECT NOW()");
            if (!timeResult.empty()) {
                cout << "✓ Текущее время сервера: " << timeResult[0][0].c_str() << endl;
            }
            
        } catch (const exception& e) {
            cout << "✗ Ошибка выполнения тестового запроса: " << e.what() << endl;
        }
    } else {
        cout << "✗ Нет подключения к базе данных!" << endl;
    }
}

void ConsoleUI::showHeroMenu() {
    int choice;
    do {
        cout << "\n================= УПРАВЛЕНИЕ ГЕРОЯМИ =================" << endl;
        cout << "1. Добавить нового героя" << endl;
        cout << "2. Просмотреть героя" << endl;
        cout << "3. Обновить героя" << endl;
        cout << "4. Удалить героя" << endl;
        cout << "5. Поиск героев" << endl;
        cout << "6. Список всех героев" << endl;
        cout << "7. Герои по атрибуту" << endl;
        cout << "8. Герои по сложности" << endl;
        cout << "9. Топ героев по урону" << endl;
        cout << "10. Герои по здоровью (сортировка)" << endl;
        cout << "11. Герои по скорости (сортировка)" << endl;
        cout << "0. Вернуться в главное меню" << endl;
        cout << "======================================================" << endl;
        
        choice = getIntInput("Выберите пункт меню: ");
        handleHeroMenuChoice(choice);
        
    } while (choice != 0);
}

void ConsoleUI::handleHeroMenuChoice(int choice) {
    switch (choice) {
        case 1: addNewHero(); break;
        case 2: viewHero(); break;
        case 3: updateHero(); break;
        case 4: deleteHero(); break;
        case 5: searchHeroes(); break;
        case 6: listAllHeroes(); break;
        case 7: showHerosByAttribute(); break;
        case 8: showHerosByComplexity(); break;
        case 9: showTopHeroesByDamage(); break;
        case 10: showHeroesSortedByHealth(); break;
        case 11: showHeroesSortedByMoveSpeed(); break;
        case 0: break;
        default: cout << "Неверный выбор!" << endl; break;
    }
}

void ConsoleUI::addNewHero() {
    cout << "\n=== ДОБАВЛЕНИЕ НОВОГО ГЕРОЯ ===" << endl;
    
    string name = getStringInput("Введите системное имя героя (например, pudge): ");
    string displayName = getStringInput("Введите отображаемое имя героя (например, Pudge): ");
    
    cout << "Доступные атрибуты:" << endl;
    cout << "1. Strength" << endl;
    cout << "2. Agility" << endl;
    cout << "3. Intelligence" << endl;
    int attrChoice = getIntInput("Выберите основной атрибут (1-3): ");
    
    string attributeType;
    switch (attrChoice) {
        case 1: attributeType = "Strength"; break;
        case 2: attributeType = "Agility"; break;
        case 3: attributeType = "Intelligence"; break;
        default:
            cout << "Неверный выбор атрибута!" << endl;
            return;
    }
    
    int complexity = getIntInput("Введите сложность героя (1-3): ");
    
    int baseHealth = getIntInput("Введите базовое здоровье: ");
    int baseMana = getIntInput("Введите базовую ману: ");
    int baseArmor = getIntInput("Введите базовую броню: ");
    int baseDamageMin = getIntInput("Введите минимальный урон: ");
    int baseDamageMax = getIntInput("Введите максимальный урон: ");
    float baseAttackSpeed = getFloatInput("Введите скорость атаки: ");
    int baseMoveSpeed = getIntInput("Введите скорость передвижения: ");
    
    int baseStrength = getIntInput("Введите базовую силу: ");
    int baseAgility = getIntInput("Введите базовую ловкость: ");
    int baseIntelligence = getIntInput("Введите базовый интеллект: ");
    float strengthGain = getFloatInput("Введите прирост силы за уровень: ");
    float agilityGain = getFloatInput("Введите прирост ловкости за уровень: ");
    float intelligenceGain = getFloatInput("Введите прирост интеллекта за уровень: ");
    
    Hero hero(name, displayName, attributeType, complexity,
              baseHealth, baseMana, baseArmor, baseDamageMin, baseDamageMax,
              baseAttackSpeed, baseMoveSpeed, baseStrength, baseAgility,
              baseIntelligence, strengthGain, agilityGain, intelligenceGain);
    
    if (heroService->addHero(hero)) {
        cout << "Герой успешно добавлен!" << endl;
    } else {
        cout << "Ошибка при добавлении героя!" << endl;
    }
}

void ConsoleUI::viewHero() {
    cout << "\n=== ПРОСМОТР ГЕРОЯ ===" << endl;
    
    int heroId = getIntInput("Введите ID героя: ");
    
    auto hero = heroService->getHero(heroId);
    if (hero.has_value()) {
        displayHero(hero.value());
    } else {
        cout << "Герой с ID " << heroId << " не найден!" << endl;
    }
}

void ConsoleUI::updateHero() {
    cout << "\n=== ОБНОВЛЕНИЕ ГЕРОЯ ===" << endl;
    
    int heroId = getIntInput("Введите ID героя для обновления: ");
    
    auto existingHero = heroService->getHero(heroId);
    if (!existingHero.has_value()) {
        cout << "Герой с ID " << heroId << " не найден!" << endl;
        return;
    }
    
    cout << "Текущие данные героя:" << endl;
    displayHero(existingHero.value());
    
    cout << "\nВведите новые данные:" << endl;
    
    string name = getStringInput("Новое системное имя: ");
    string displayName = getStringInput("Новое отображаемое имя: ");
    
    cout << "Доступные атрибуты:" << endl;
    cout << "1. Strength" << endl;
    cout << "2. Agility" << endl;
    cout << "3. Intelligence" << endl;
    int attrChoice = getIntInput("Выберите основной атрибут (1-3): ");
    
    string attributeType;
    switch (attrChoice) {
        case 1: attributeType = "Strength"; break;
        case 2: attributeType = "Agility"; break;
        case 3: attributeType = "Intelligence"; break;
        default:
            cout << "Неверный выбор атрибута!" << endl;
            return;
    }
    
    int complexity = getIntInput("Новая сложность героя (1-3): ");
    int baseHealth = getIntInput("Новое базовое здоровье: ");
    int baseMana = getIntInput("Новая базовая мана: ");
    int baseArmor = getIntInput("Новая базовая броня: ");
    int baseDamageMin = getIntInput("Новый минимальный урон: ");
    int baseDamageMax = getIntInput("Новый максимальный урон: ");
    float baseAttackSpeed = getFloatInput("Новая скорость атаки: ");
    int baseMoveSpeed = getIntInput("Новая скорость передвижения: ");
    int baseStrength = getIntInput("Новая базовая сила: ");
    int baseAgility = getIntInput("Новая базовая ловкость: ");
    int baseIntelligence = getIntInput("Новый базовый интеллект: ");
    float strengthGain = getFloatInput("Новый прирост силы за уровень: ");
    float agilityGain = getFloatInput("Новый прирост ловкости за уровень: ");
    float intelligenceGain = getFloatInput("Новый прирост интеллекта за уровень: ");
    
    Hero updatedHero = existingHero.value();
    updatedHero.name = name;
    updatedHero.displayName = displayName;
    updatedHero.attributeType = attributeType;
    updatedHero.complexity = complexity;
    updatedHero.baseHealth = baseHealth;
    updatedHero.baseMana = baseMana;
    updatedHero.baseArmor = baseArmor;
    updatedHero.baseDamageMin = baseDamageMin;
    updatedHero.baseDamageMax = baseDamageMax;
    updatedHero.baseAttackSpeed = baseAttackSpeed;
    updatedHero.baseMoveSpeed = baseMoveSpeed;
    updatedHero.baseStrength = baseStrength;
    updatedHero.baseAgility = baseAgility;
    updatedHero.baseIntelligence = baseIntelligence;
    updatedHero.strengthGain = strengthGain;
    updatedHero.agilityGain = agilityGain;
    updatedHero.intelligenceGain = intelligenceGain;
    
    if (heroService->updateHero(updatedHero)) {
        cout << "Герой успешно обновлен!" << endl;
    } else {
        cout << "Ошибка при обновлении героя!" << endl;
    }
}

void ConsoleUI::deleteHero() {
    cout << "\n=== УДАЛЕНИЕ ГЕРОЯ ===" << endl;
    
    int heroId = getIntInput("Введите ID героя для удаления: ");
    
    auto hero = heroService->getHero(heroId);
    if (!hero.has_value()) {
        cout << "Герой с ID " << heroId << " не найден!" << endl;
        return;
    }
    
    cout << "Вы действительно хотите удалить героя:" << endl;
    displayHero(hero.value());
    
    string confirmation = getStringInput("Подтвердите удаление (да/нет): ");
    if (confirmation == "да" || confirmation == "yes" || confirmation == "y") {
        if (heroService->deleteHero(heroId)) {
            cout << "Герой успешно удален!" << endl;
        } else {
            cout << "Ошибка при удалении героя!" << endl;
        }
    } else {
        cout << "Удаление отменено." << endl;
    }
}

void ConsoleUI::searchHeroes() {
    cout << "\n=== ПОИСК ГЕРОЕВ ===" << endl;
    
    HeroSearchCriteria criteria;
    
    string namePattern = getStringInput("Поиск по имени (пустая строка для пропуска): ");
    if (!namePattern.empty()) {
        criteria.namePattern = namePattern;
    }
    
    cout << "Фильтр по атрибуту:" << endl;
    cout << "1. Strength" << endl;
    cout << "2. Agility" << endl;
    cout << "3. Intelligence" << endl;
    cout << "0. Пропустить" << endl;
    int attrChoice = getIntInput("Выберите атрибут (0-3): ");
    
    switch (attrChoice) {
        case 1: criteria.attributeType = "Strength"; break;
        case 2: criteria.attributeType = "Agility"; break;
        case 3: criteria.attributeType = "Intelligence"; break;
    }
    
    int complexity = getIntInput("Фильтр по сложности (0 для пропуска): ");
    if (complexity > 0) {
        criteria.complexity = complexity;
    }
    
    int minHealth = getIntInput("Минимальное здоровье (0 для пропуска): ");
    if (minHealth > 0) {
        criteria.minHealth = minHealth;
    }
    
    int maxHealth = getIntInput("Максимальное здоровье (0 для пропуска): ");
    if (maxHealth > 0) {
        criteria.maxHealth = maxHealth;
    }
    
    auto heroes = heroService->searchHeroes(criteria);
    
    if (heroes.empty()) {
        cout << "Героев по заданным критериям не найдено." << endl;
    } else {
        cout << "Найдено героев: " << heroes.size() << endl;
        displayHeroesList(heroes);
    }
}

void ConsoleUI::listAllHeroes() {
    cout << "\n=== СПИСОК ВСЕХ ГЕРОЕВ ===" << endl;
    
    auto heroes = heroService->getAllHeroes();
    
    if (heroes.empty()) {
        cout << "В базе данных нет героев." << endl;
    } else {
        cout << "Всего героев в базе: " << heroes.size() << endl;
        displayHeroesList(heroes);
    }
}

void ConsoleUI::showHerosByAttribute() {
    cout << "\n=== ГЕРОИ ПО АТРИБУТУ ===" << endl;
    
    cout << "Доступные атрибуты:" << endl;
    cout << "1. Strength" << endl;
    cout << "2. Agility" << endl;
    cout << "3. Intelligence" << endl;
    int attrChoice = getIntInput("Выберите атрибут (1-3): ");
    
    string attributeType;
    switch (attrChoice) {
        case 1: attributeType = "Strength"; break;
        case 2: attributeType = "Agility"; break;
        case 3: attributeType = "Intelligence"; break;
        default:
            cout << "Неверный выбор!" << endl;
            return;
    }
    
    auto heroes = heroService->getHeroesByAttribute(attributeType);
    
    if (heroes.empty()) {
        cout << "Героев с атрибутом " << attributeType << " не найдено." << endl;
    } else {
        cout << "Героев с атрибутом " << attributeType << ": " << heroes.size() << endl;
        displayHeroesList(heroes);
    }
}

void ConsoleUI::showHerosByComplexity() {
    cout << "\n=== ГЕРОИ ПО СЛОЖНОСТИ ===" << endl;
    
    int complexity = getIntInput("Введите сложность (1-3): ");
    
    if (complexity < 1 || complexity > 3) {
        cout << "Неверная сложность!" << endl;
        return;
    }
    
    auto heroes = heroService->getHeroesByComplexity(complexity);
    
    if (heroes.empty()) {
        cout << "Героев со сложностью " << complexity << " не найдено." << endl;
    } else {
        cout << "Героев со сложностью " << complexity << ": " << heroes.size() << endl;
        displayHeroesList(heroes);
    }
}

void ConsoleUI::showTopHeroesByDamage() {
    cout << "\n=== ТОП ГЕРОЕВ ПО УРОНУ ===" << endl;
    
    int limit = getIntInput("Введите количество героев для показа: ");
    
    auto heroes = heroService->getTopHeroesByDamage(limit);
    
    if (heroes.empty()) {
        cout << "Героев не найдено." << endl;
    } else {
        cout << "Топ " << heroes.size() << " героев по урону:" << endl;
        displayHeroesList(heroes);
    }
}

void ConsoleUI::showHeroesSortedByHealth() {
    cout << "\n=== ГЕРОИ ПО ЗДОРОВЬЮ (СОРТИРОВКА) ===" << endl;
    
    auto heroes = heroService->getHeroesSortedByHealth();
    
    if (heroes.empty()) {
        cout << "Героев не найдено." << endl;
    } else {
        cout << "Герои, отсортированные по здоровью (по убыванию):" << endl;
        displayHeroesList(heroes);
    }
}

void ConsoleUI::showHeroesSortedByMoveSpeed() {
    cout << "\n=== ГЕРОИ ПО СКОРОСТИ (СОРТИРОВКА) ===" << endl;
    
    auto heroes = heroService->getHeroesSortedByMoveSpeed();
    
    if (heroes.empty()) {
        cout << "Героев не найдено." << endl;
    } else {
        cout << "Герои, отсортированные по скорости передвижения (по убыванию):" << endl;
        displayHeroesList(heroes);
    }
}

void ConsoleUI::showStatisticsMenu() {
    int choice;
    do {
        cout << "\n================= СТАТИСТИКА И АГРЕГАЦИЯ =================" << endl;
        cout << "1. Общая статистика" << endl;
        cout << "2. Статистика по атрибутам" << endl;
        cout << "3. Средние характеристики по атрибутам" << endl;
        cout << "4. Атрибуты с минимальным количеством героев" << endl;
        cout << "5. Атрибуты с высоким средним уроном" << endl;
        cout << "0. Вернуться в главное меню" << endl;
        cout << "======================================================" << endl;
        
        choice = getIntInput("Выберите пункт меню: ");
        handleStatisticsMenuChoice(choice);
        
    } while (choice != 0);
}

void ConsoleUI::handleStatisticsMenuChoice(int choice) {
    switch (choice) {
        case 1: showOverallStatistics(); break;
        case 2: showAttributeStatistics(); break;
        case 3: showAverageStatsByAttribute(); break;
        case 4: showAttributesWithMinHeroCount(); break;
        case 5: showAttributesWithHighDamage(); break;
        case 0: break;
        default: cout << "Неверный выбор!" << endl; break;
    }
}

void ConsoleUI::showOverallStatistics() {
    cout << "\n=== ОБЩАЯ СТАТИСТИКА ===" << endl;
    
    auto stats = heroService->getOverallStatistics();
    displayOverallStatistics(stats);
}

void ConsoleUI::showAttributeStatistics() {
    cout << "\n=== СТАТИСТИКА ПО АТРИБУТАМ ===" << endl;
    
    auto stats = heroService->getAttributeStatistics();
    displayAttributeStatistics(stats);
}

void ConsoleUI::showAverageStatsByAttribute() {
    cout << "\n=== СРЕДНИЕ ХАРАКТЕРИСТИКИ ПО АТРИБУТАМ ===" << endl;
    
    cout << "Доступные атрибуты:" << endl;
    cout << "1. Strength" << endl;
    cout << "2. Agility" << endl;
    cout << "3. Intelligence" << endl;
    int attrChoice = getIntInput("Выберите атрибут (1-3): ");
    
    string attributeType;
    switch (attrChoice) {
        case 1: attributeType = "Strength"; break;
        case 2: attributeType = "Agility"; break;
        case 3: attributeType = "Intelligence"; break;
        default:
            cout << "Неверный выбор!" << endl;
            return;
    }
    
    double avgHealth = heroService->getAverageHealthByAttribute(attributeType);
    double avgDamage = heroService->getAverageDamageByAttribute(attributeType);
    int minComplexity = heroService->getMinComplexityByAttribute(attributeType);
    int maxComplexity = heroService->getMaxComplexityByAttribute(attributeType);
    
    cout << "Средние характеристики для атрибута " << attributeType << ":" << endl;
    cout << "Среднее здоровье: " << fixed << setprecision(1) << avgHealth << endl;
    cout << "Средний урон: " << fixed << setprecision(1) << avgDamage << endl;
    cout << "Минимальная сложность: " << minComplexity << endl;
    cout << "Максимальная сложность: " << maxComplexity << endl;
}

void ConsoleUI::showAttributesWithMinHeroCount() {
    cout << "\n=== АТРИБУТЫ С МИНИМАЛЬНЫМ КОЛИЧЕСТВОМ ГЕРОЕВ ===" << endl;
    
    int minCount = getIntInput("Введите минимальное количество героев: ");
    
    auto stats = heroService->getAttributesWithMinHeroCount(minCount);
    
    if (stats.empty()) {
        cout << "Атрибутов с минимальным количеством " << minCount << " героев не найдено." << endl;
    } else {
        cout << "Атрибуты с количеством героев >= " << minCount << ":" << endl;
        displayAttributeStatistics(stats);
    }
}

void ConsoleUI::showAttributesWithHighDamage() {
    cout << "\n=== АТРИБУТЫ С ВЫСОКИМ СРЕДНИМ УРОНОМ ===" << endl;
    
    float minAvgDamage = getFloatInput("Введите минимальный средний урон: ");
    
    auto attributes = heroService->getAttributesWithHighAverageDamage(minAvgDamage);
    
    if (attributes.empty()) {
        cout << "Атрибутов со средним уроном >= " << minAvgDamage << " не найдено." << endl;
    } else {
        cout << "Атрибуты со средним уроном >= " << minAvgDamage << ":" << endl;
        for (const auto& attr : attributes) {
            cout << "  - " << attr << endl;
        }
    }
}

void ConsoleUI::demonstratePostgreSQLFeatures() {
    cout << "\n=== ДЕМОНСТРАЦИЯ ВОЗМОЖНОСТЕЙ POSTGRESQL ===" << endl;
    
    cout << "1. CRUD операции - реализованы в управлении героями" << endl;
    cout << "2. WHERE условия - используются в поиске и фильтрации" << endl;
    cout << "3. ORDER BY - сортировка героев по различным критериям" << endl;
    cout << "4. GROUP BY - группировка в статистике по атрибутам" << endl;
    cout << "5. HAVING - фильтрация групп в статистике" << endl;
    cout << "6. Агрегатные функции - COUNT, AVG, MIN, MAX в статистике" << endl;
    cout << "7. CHECK ограничения - валидация данных на уровне БД" << endl;
    cout << "8. UNIQUE ограничения - уникальность имен героев" << endl;
    cout << "9. Индексы - оптимизация запросов по атрибутам" << endl;
    cout << "10. Триггеры - автоматическое обновление updated_at" << endl;
    
    cout << "\nВсе эти возможности активно используются в системе!" << endl;
}

void ConsoleUI::displayHero(const Hero& hero) {
    cout << "\n--- Информация о герое ---" << endl;
    cout << "ID: " << hero.heroId << endl;
    cout << "Системное имя: " << hero.name << endl;
    cout << "Отображаемое имя: " << hero.displayName << endl;
    cout << "Основной атрибут: " << hero.attributeType << endl;
    cout << "Сложность: " << hero.complexity << "/3" << endl;
    
    cout << "\n--- Базовые характеристики ---" << endl;
    cout << "Здоровье: " << hero.baseHealth << endl;
    cout << "Мана: " << hero.baseMana << endl;
    cout << "Броня: " << hero.baseArmor << endl;
    cout << "Урон: " << hero.baseDamageMin << "-" << hero.baseDamageMax << endl;
    cout << "Скорость атаки: " << hero.baseAttackSpeed << endl;
    cout << "Скорость передвижения: " << hero.baseMoveSpeed << endl;
    
    cout << "\n--- Атрибуты ---" << endl;
    cout << "Сила: " << hero.baseStrength << " (+" << hero.strengthGain << "/уровень)" << endl;
    cout << "Ловкость: " << hero.baseAgility << " (+" << hero.agilityGain << "/уровень)" << endl;
    cout << "Интеллект: " << hero.baseIntelligence << " (+" << hero.intelligenceGain << "/уровень)" << endl;
    cout << "-------------------------" << endl;
}

void ConsoleUI::displayHeroesList(const vector<Hero>& heroes) {
    cout << "\n" << setw(5) << "ID" 
              << setw(15) << "Имя" 
              << setw(20) << "Отображаемое имя" 
              << setw(12) << "Атрибут" 
              << setw(8) << "Сложн." 
              << setw(8) << "HP"
              << setw(10) << "Урон" << endl;
    cout << string(78, '-') << endl;
    
    for (const auto& hero : heroes) {
        cout << setw(5) << hero.heroId
                  << setw(15) << hero.name.substr(0, 14)
                  << setw(20) << hero.displayName.substr(0, 19)
                  << setw(12) << hero.attributeType.substr(0, 11)
                  << setw(8) << hero.complexity
                  << setw(8) << hero.baseHealth
                  << setw(10) << (hero.baseDamageMin + hero.baseDamageMax) / 2 << endl;
    }
}

void ConsoleUI::displayAttributeStatistics(const vector<AttributeStatistics>& stats) {
    cout << "\n" << setw(15) << "Атрибут" 
              << setw(10) << "Героев" 
              << setw(12) << "Ср.здоровье" 
              << setw(10) << "Ср.урон"
              << setw(12) << "Ср.скорость"
              << setw(8) << "Мин.сл."
              << setw(8) << "Макс.сл." << endl;
    cout << string(75, '-') << endl;
    
    for (const auto& stat : stats) {
        cout << setw(15) << stat.attributeType
                  << setw(10) << stat.heroCount
                  << setw(12) << fixed << setprecision(1) << stat.avgHealth
                  << setw(10) << fixed << setprecision(1) << stat.avgDamage
                  << setw(12) << fixed << setprecision(1) << stat.avgMoveSpeed
                  << setw(8) << stat.minComplexity
                  << setw(8) << stat.maxComplexity << endl;
    }
}

void ConsoleUI::displayOverallStatistics(const HeroStatistics& stats) {
    cout << "Общее количество героев: " << stats.totalHeroes << endl;
    cout << "Среднее здоровье: " << fixed << setprecision(1) << stats.avgHealth << endl;
    cout << "Средняя мана: " << fixed << setprecision(1) << stats.avgMana << endl;
    cout << "Средний урон: " << fixed << setprecision(1) << stats.avgDamage << endl;
    cout << "Средняя скорость: " << fixed << setprecision(1) << stats.avgMoveSpeed << endl;
    
    cout << "\nРаспределение по атрибутам:" << endl;
    cout << "Strength героев: " << stats.strengthHeroes << endl;
    cout << "Agility героев: " << stats.agilityHeroes << endl;
    cout << "Intelligence героев: " << stats.intelligenceHeroes << endl;
}

int ConsoleUI::getIntInput(const string& prompt) {
    int value;
    cout << prompt;
    while (!(cin >> value)) {
        cout << "Неверный ввод. Введите число: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

string ConsoleUI::getStringInput(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

float ConsoleUI::getFloatInput(const string& prompt) {
    float value;
    cout << prompt;
    while (!(cin >> value)) {
        cout << "Неверный ввод. Введите число: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}