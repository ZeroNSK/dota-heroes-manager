#pragma once

#include <memory>
#include "database/DatabaseManager.h"
#include "services/HeroService.h"

using namespace std;

class ConsoleUI {
private:
    unique_ptr<DatabaseManager> dbManager;
    unique_ptr<HeroService> heroService;
    
    void showMainMenu();
    void handleMenuChoice(int choice);
    void showDatabaseInfo();
    void testConnection();
    void showHeroMenu();
    void handleHeroMenuChoice(int choice);
    void addNewHero();
    void viewHero();
    void updateHero();
    void deleteHero();
    void searchHeroes();
    void listAllHeroes();
    void showHerosByAttribute();
    void showHerosByComplexity();
    void showTopHeroesByDamage();
    void showHeroesSortedByHealth();
    void showHeroesSortedByMoveSpeed();
    void showStatisticsMenu();
    void handleStatisticsMenuChoice(int choice);
    void showOverallStatistics();
    void showAttributeStatistics();
    void showAverageStatsByAttribute();
    void showAttributesWithMinHeroCount();
    void showAttributesWithHighDamage();
    void demonstratePostgreSQLFeatures();
    void displayHero(const Hero& hero);
    void displayHeroesList(const vector<Hero>& heroes);
    void displayAttributeStatistics(const vector<AttributeStatistics>& stats);
    void displayOverallStatistics(const HeroStatistics& stats);

    int getIntInput(const string& prompt);
    string getStringInput(const string& prompt);
    float getFloatInput(const string& prompt);
    
public:
    explicit ConsoleUI(unique_ptr<DatabaseManager> db);
    ~ConsoleUI() = default;
    
    void run();
};