#pragma once

#include <memory>
#include <vector>
#include <optional>
#include "models/Hero.h"
#include "database/DatabaseManager.h"

using namespace std;

class HeroService {
private:
    DatabaseManager& dbManager;
    Hero resultToHero(const pqxx::row& row);
    
public:
    explicit HeroService(DatabaseManager& dbManager);
    bool addHero(const Hero& hero);
    optional<Hero> getHero(int heroId);
    optional<Hero> getHeroByName(const string& name);
    bool updateHero(const Hero& hero);
    bool deleteHero(int heroId);
    vector<Hero> getAllHeroes();
    vector<Hero> getHeroesByAttribute(const string& attributeType);
    vector<Hero> getHeroesByComplexity(int complexity);
    vector<Hero> searchHeroes(const HeroSearchCriteria& criteria);
    vector<Hero> getTopHeroesByDamage(int limit);
    vector<Hero> getHeroesSortedByHealth();
    vector<Hero> getHeroesSortedByMoveSpeed();
    bool heroExists(int heroId);
    bool heroExistsByName(const string& name);
    int getHeroCount();
    vector<AttributeStatistics> getAttributeStatistics();
    HeroStatistics getOverallStatistics();
    double getAverageHealthByAttribute(const string& attributeType);
    double getAverageDamageByAttribute(const string& attributeType);
    int getMinComplexityByAttribute(const string& attributeType);
    int getMaxComplexityByAttribute(const string& attributeType);
    vector<AttributeStatistics> getAttributesWithMinHeroCount(int minCount);
    vector<string> getAttributesWithHighAverageDamage(double minAvgDamage);
};