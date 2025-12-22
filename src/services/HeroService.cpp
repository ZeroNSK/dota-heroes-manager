#include "services/HeroService.h"
#include <iostream>
#include <sstream>

using namespace std;

HeroService::HeroService(DatabaseManager& dbManager) : dbManager(dbManager) {}

Hero HeroService::resultToHero(const pqxx::row& row) {
    Hero hero;
    hero.heroId = row["hero_id"].as<int>();
    hero.name = row["name"].as<string>();
    hero.displayName = row["display_name"].as<string>();
    hero.attributeType = row["attribute_type"].as<string>();
    hero.complexity = row["complexity"].as<int>();
    
    hero.baseHealth = row["base_health"].as<int>();
    hero.baseMana = row["base_mana"].as<int>();
    hero.baseArmor = row["base_armor"].as<int>();
    hero.baseMoveSpeed = row["base_move_speed"].as<int>();
    
    hero.baseDamageMin = row["base_damage_min"].as<int>();
    hero.baseDamageMax = row["base_damage_max"].as<int>();
    hero.baseAttackSpeed = row["base_attack_speed"].as<float>();
    
    hero.baseStrength = row["base_strength"].as<int>();
    hero.baseAgility = row["base_agility"].as<int>();
    hero.baseIntelligence = row["base_intelligence"].as<int>();
    
    hero.strengthGain = row["strength_gain"].as<float>();
    hero.agilityGain = row["agility_gain"].as<float>();
    hero.intelligenceGain = row["intelligence_gain"].as<float>();
    
    return hero;
}

bool HeroService::addHero(const Hero& hero) {
    if (!hero.isValid()) {
        cerr << "Ошибка: Некорректные данные героя" << endl;
        return false;
    }
    
    try {
        auto txn = dbManager.beginTransaction();
        
        string heroQuery = R"(
            INSERT INTO heroes (name, display_name, attribute_type, complexity)
            VALUES ($1, $2, $3, $4) RETURNING hero_id
        )";
        
        pqxx::result heroResult = txn->exec_params(heroQuery, 
            hero.name, hero.displayName, hero.attributeType, hero.complexity);
        
        int heroId = heroResult[0]["hero_id"].as<int>();
        
        string statsQuery = R"(
            INSERT INTO hero_stats (hero_id, base_health, base_mana, base_armor, base_move_speed)
            VALUES ($1, $2, $3, $4, $5)
        )";
        
        txn->exec_params(statsQuery, heroId, hero.baseHealth, hero.baseMana, hero.baseArmor, hero.baseMoveSpeed);
        
        string damageQuery = R"(
            INSERT INTO hero_damage (hero_id, base_damage_min, base_damage_max, base_attack_speed)
            VALUES ($1, $2, $3, $4)
        )";
        
        txn->exec_params(damageQuery, heroId, hero.baseDamageMin, hero.baseDamageMax, hero.baseAttackSpeed);
        
        string attributesQuery = R"(
            INSERT INTO hero_attributes (hero_id, base_strength, base_agility, base_intelligence)
            VALUES ($1, $2, $3, $4)
        )";
        
        txn->exec_params(attributesQuery, heroId, hero.baseStrength, hero.baseAgility, hero.baseIntelligence);
        
        string growthQuery = R"(
            INSERT INTO hero_growth (hero_id, strength_gain, agility_gain, intelligence_gain)
            VALUES ($1, $2, $3, $4)
        )";
        
        txn->exec_params(growthQuery, heroId, hero.strengthGain, hero.agilityGain, hero.intelligenceGain);
        
        dbManager.commitTransaction(txn);
        cout << "Герой '" << hero.displayName << "' успешно добавлен" << endl;
        return true;
        
    } catch (const exception& e) {
        cerr << "Ошибка при добавлении героя: " << e.what() << endl;
        return false;
    }
}

optional<Hero> HeroService::getHero(int heroId) {
    try {
        string query = R"(
            SELECT h.hero_id, h.name, h.display_name, h.attribute_type, h.complexity,
                   s.base_health, s.base_mana, s.base_armor, s.base_move_speed,
                   d.base_damage_min, d.base_damage_max, d.base_attack_speed,
                   a.base_strength, a.base_agility, a.base_intelligence,
                   g.strength_gain, g.agility_gain, g.intelligence_gain
            FROM heroes h
            JOIN hero_stats s ON h.hero_id = s.hero_id
            JOIN hero_damage d ON h.hero_id = d.hero_id
            JOIN hero_attributes a ON h.hero_id = a.hero_id
            JOIN hero_growth g ON h.hero_id = g.hero_id
            WHERE h.hero_id = $1
        )";
        
        auto txn = dbManager.beginTransaction();
        pqxx::result result = txn->exec_params(query, heroId);
        
        if (result.empty()) {
            return nullopt;
        }
        
        return resultToHero(result[0]);
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении героя: " << e.what() << endl;
        return nullopt;
    }
}

optional<Hero> HeroService::getHeroByName(const string& name) {
    try {
        string query = R"(
            SELECT h.hero_id, h.name, h.display_name, h.attribute_type, h.complexity,
                   s.base_health, s.base_mana, s.base_armor, s.base_move_speed,
                   d.base_damage_min, d.base_damage_max, d.base_attack_speed,
                   a.base_strength, a.base_agility, a.base_intelligence,
                   g.strength_gain, g.agility_gain, g.intelligence_gain
            FROM heroes h
            JOIN hero_stats s ON h.hero_id = s.hero_id
            JOIN hero_damage d ON h.hero_id = d.hero_id
            JOIN hero_attributes a ON h.hero_id = a.hero_id
            JOIN hero_growth g ON h.hero_id = g.hero_id
            WHERE h.name = $1
        )";
        
        auto txn = dbManager.beginTransaction();
        pqxx::result result = txn->exec_params(query, name);
        
        if (result.empty()) {
            return nullopt;
        }
        
        return resultToHero(result[0]);
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении героя по имени: " << e.what() << endl;
        return nullopt;
    }
}

vector<Hero> HeroService::getAllHeroes() {
    vector<Hero> heroes;
    try {
        string query = R"(
            SELECT h.hero_id, h.name, h.display_name, h.attribute_type, h.complexity,
                   s.base_health, s.base_mana, s.base_armor, s.base_move_speed,
                   d.base_damage_min, d.base_damage_max, d.base_attack_speed,
                   a.base_strength, a.base_agility, a.base_intelligence,
                   g.strength_gain, g.agility_gain, g.intelligence_gain
            FROM heroes h
            JOIN hero_stats s ON h.hero_id = s.hero_id
            JOIN hero_damage d ON h.hero_id = d.hero_id
            JOIN hero_attributes a ON h.hero_id = a.hero_id
            JOIN hero_growth g ON h.hero_id = g.hero_id
            ORDER BY h.display_name
        )";
        
        pqxx::result result = dbManager.executeQuery(query);
        
        for (const auto& row : result) {
            heroes.push_back(resultToHero(row));
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении всех героев: " << e.what() << endl;
    }
    return heroes;
}

vector<Hero> HeroService::getHeroesByAttribute(const string& attributeType) {
    vector<Hero> heroes;
    try {
        string query = R"(
            SELECT h.hero_id, h.name, h.display_name, h.attribute_type, h.complexity,
                   s.base_health, s.base_mana, s.base_armor, s.base_move_speed,
                   d.base_damage_min, d.base_damage_max, d.base_attack_speed,
                   a.base_strength, a.base_agility, a.base_intelligence,
                   g.strength_gain, g.agility_gain, g.intelligence_gain
            FROM heroes h
            JOIN hero_stats s ON h.hero_id = s.hero_id
            JOIN hero_damage d ON h.hero_id = d.hero_id
            JOIN hero_attributes a ON h.hero_id = a.hero_id
            JOIN hero_growth g ON h.hero_id = g.hero_id
            WHERE h.attribute_type = $1
            ORDER BY h.display_name
        )";
        
        auto txn = dbManager.beginTransaction();
        pqxx::result result = txn->exec_params(query, attributeType);
        
        for (const auto& row : result) {
            heroes.push_back(resultToHero(row));
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении героев по атрибуту: " << e.what() << endl;
    }
    return heroes;
}

vector<Hero> HeroService::getHeroesByComplexity(int complexity) {
    vector<Hero> heroes;
    try {
        string query = R"(
            SELECT h.hero_id, h.name, h.display_name, h.attribute_type, h.complexity,
                   s.base_health, s.base_mana, s.base_armor, s.base_move_speed,
                   d.base_damage_min, d.base_damage_max, d.base_attack_speed,
                   a.base_strength, a.base_agility, a.base_intelligence,
                   g.strength_gain, g.agility_gain, g.intelligence_gain
            FROM heroes h
            JOIN hero_stats s ON h.hero_id = s.hero_id
            JOIN hero_damage d ON h.hero_id = d.hero_id
            JOIN hero_attributes a ON h.hero_id = a.hero_id
            JOIN hero_growth g ON h.hero_id = g.hero_id
            WHERE h.complexity = $1
            ORDER BY h.display_name
        )";
        
        auto txn = dbManager.beginTransaction();
        pqxx::result result = txn->exec_params(query, complexity);
        
        for (const auto& row : result) {
            heroes.push_back(resultToHero(row));
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении героев по сложности: " << e.what() << endl;
    }
    return heroes;
}

vector<Hero> HeroService::getTopHeroesByDamage(int limit) {
    vector<Hero> heroes;
    try {
        string query = R"(
            SELECT h.hero_id, h.name, h.display_name, h.attribute_type, h.complexity,
                   s.base_health, s.base_mana, s.base_armor, s.base_move_speed,
                   d.base_damage_min, d.base_damage_max, d.base_attack_speed,
                   a.base_strength, a.base_agility, a.base_intelligence,
                   g.strength_gain, g.agility_gain, g.intelligence_gain
            FROM heroes h
            JOIN hero_stats s ON h.hero_id = s.hero_id
            JOIN hero_damage d ON h.hero_id = d.hero_id
            JOIN hero_attributes a ON h.hero_id = a.hero_id
            JOIN hero_growth g ON h.hero_id = g.hero_id
            ORDER BY (d.base_damage_min + d.base_damage_max) DESC
            LIMIT $1
        )";
        
        auto txn = dbManager.beginTransaction();
        pqxx::result result = txn->exec_params(query, limit);
        
        for (const auto& row : result) {
            heroes.push_back(resultToHero(row));
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении топ героев по урону: " << e.what() << endl;
    }
    return heroes;
}

vector<Hero> HeroService::getHeroesSortedByHealth() {
    vector<Hero> heroes;
    try {
        string query = R"(
            SELECT h.hero_id, h.name, h.display_name, h.attribute_type, h.complexity,
                   s.base_health, s.base_mana, s.base_armor, s.base_move_speed,
                   d.base_damage_min, d.base_damage_max, d.base_attack_speed,
                   a.base_strength, a.base_agility, a.base_intelligence,
                   g.strength_gain, g.agility_gain, g.intelligence_gain
            FROM heroes h
            JOIN hero_stats s ON h.hero_id = s.hero_id
            JOIN hero_damage d ON h.hero_id = d.hero_id
            JOIN hero_attributes a ON h.hero_id = a.hero_id
            JOIN hero_growth g ON h.hero_id = g.hero_id
            ORDER BY s.base_health DESC
        )";
        
        pqxx::result result = dbManager.executeQuery(query);
        
        for (const auto& row : result) {
            heroes.push_back(resultToHero(row));
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении героев по здоровью: " << e.what() << endl;
    }
    return heroes;
}

vector<Hero> HeroService::getHeroesSortedByMoveSpeed() {
    vector<Hero> heroes;
    try {
        string query = R"(
            SELECT h.hero_id, h.name, h.display_name, h.attribute_type, h.complexity,
                   s.base_health, s.base_mana, s.base_armor, s.base_move_speed,
                   d.base_damage_min, d.base_damage_max, d.base_attack_speed,
                   a.base_strength, a.base_agility, a.base_intelligence,
                   g.strength_gain, g.agility_gain, g.intelligence_gain
            FROM heroes h
            JOIN hero_stats s ON h.hero_id = s.hero_id
            JOIN hero_damage d ON h.hero_id = d.hero_id
            JOIN hero_attributes a ON h.hero_id = a.hero_id
            JOIN hero_growth g ON h.hero_id = g.hero_id
            ORDER BY s.base_move_speed DESC
        )";
        
        pqxx::result result = dbManager.executeQuery(query);
        
        for (const auto& row : result) {
            heroes.push_back(resultToHero(row));
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении героев по скорости: " << e.what() << endl;
    }
    return heroes;
}

int HeroService::getHeroCount() {
    try {
        pqxx::result result = dbManager.executeQuery("SELECT COUNT(*) FROM heroes");
        return result[0][0].as<int>();
    } catch (const exception& e) {
        cerr << "Ошибка при подсчете героев: " << e.what() << endl;
        return 0;
    }
}

vector<AttributeStatistics> HeroService::getAttributeStatistics() {
    vector<AttributeStatistics> stats;
    try {
        string query = R"(
            SELECT 
                h.attribute_type,
                COUNT(h.hero_id) as hero_count,
                AVG(s.base_health) as avg_health,
                AVG((d.base_damage_min + d.base_damage_max) / 2.0) as avg_damage,
                AVG(s.base_move_speed) as avg_move_speed,
                MIN(h.complexity) as min_complexity,
                MAX(h.complexity) as max_complexity
            FROM heroes h
            JOIN hero_stats s ON h.hero_id = s.hero_id
            JOIN hero_damage d ON h.hero_id = d.hero_id
            GROUP BY h.attribute_type
            ORDER BY h.attribute_type
        )";
        
        pqxx::result result = dbManager.executeQuery(query);
        
        for (const auto& row : result) {
            AttributeStatistics stat;
            stat.attributeType = row["attribute_type"].as<string>();
            stat.heroCount = row["hero_count"].as<int>();
            stat.avgHealth = row["avg_health"].as<double>();
            stat.avgDamage = row["avg_damage"].as<double>();
            stat.avgMoveSpeed = row["avg_move_speed"].as<double>();
            stat.minComplexity = row["min_complexity"].as<int>();
            stat.maxComplexity = row["max_complexity"].as<int>();
            stats.push_back(stat);
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении статистики атрибутов: " << e.what() << endl;
    }
    return stats;
}

HeroStatistics HeroService::getOverallStatistics() {
    HeroStatistics stats = {};
    try {
        string query = R"(
            SELECT 
                COUNT(*) as total_heroes,
                AVG(s.base_health) as avg_health,
                AVG(s.base_mana) as avg_mana,
                AVG((d.base_damage_min + d.base_damage_max) / 2.0) as avg_damage,
                AVG(s.base_move_speed) as avg_move_speed,
                SUM(CASE WHEN h.attribute_type = 'Strength' THEN 1 ELSE 0 END) as strength_heroes,
                SUM(CASE WHEN h.attribute_type = 'Agility' THEN 1 ELSE 0 END) as agility_heroes,
                SUM(CASE WHEN h.attribute_type = 'Intelligence' THEN 1 ELSE 0 END) as intelligence_heroes
            FROM heroes h
            JOIN hero_stats s ON h.hero_id = s.hero_id
            JOIN hero_damage d ON h.hero_id = d.hero_id
        )";
        
        pqxx::result result = dbManager.executeQuery(query);
        
        if (!result.empty()) {
            const auto& row = result[0];
            stats.totalHeroes = row["total_heroes"].as<int>();
            stats.avgHealth = row["avg_health"].as<double>();
            stats.avgMana = row["avg_mana"].as<double>();
            stats.avgDamage = row["avg_damage"].as<double>();
            stats.avgMoveSpeed = row["avg_move_speed"].as<double>();
            stats.strengthHeroes = row["strength_heroes"].as<int>();
            stats.agilityHeroes = row["agility_heroes"].as<int>();
            stats.intelligenceHeroes = row["intelligence_heroes"].as<int>();
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении общей статистики: " << e.what() << endl;
    }
    return stats;
}

bool HeroService::heroExists(int heroId) {
    try {
        string query = "SELECT 1 FROM heroes WHERE hero_id = $1";
        auto txn = dbManager.beginTransaction();
        pqxx::result result = txn->exec_params(query, heroId);
        return !result.empty();
    } catch (const exception& e) {
        cerr << "Ошибка при проверке существования героя: " << e.what() << endl;
        return false;
    }
}

bool HeroService::heroExistsByName(const string& name) {
    try {
        string query = "SELECT 1 FROM heroes WHERE name = $1";
        auto txn = dbManager.beginTransaction();
        pqxx::result result = txn->exec_params(query, name);
        return !result.empty();
    } catch (const exception& e) {
        cerr << "Ошибка при проверке существования героя по имени: " << e.what() << endl;
        return false;
    }
}

vector<Hero> HeroService::searchHeroes(const HeroSearchCriteria& criteria) {
    return getAllHeroes();
}

bool HeroService::updateHero(const Hero& hero) {
    return false;
}

bool HeroService::deleteHero(int heroId) {
    return false;
}

double HeroService::getAverageHealthByAttribute(const string& attributeType) {
    return 0.0;
}

double HeroService::getAverageDamageByAttribute(const string& attributeType) {
    return 0.0;
}

int HeroService::getMinComplexityByAttribute(const string& attributeType) {
    return 0;
}

int HeroService::getMaxComplexityByAttribute(const string& attributeType) {
    return 0;
}

vector<AttributeStatistics> HeroService::getAttributesWithMinHeroCount(int minCount) {
    return vector<AttributeStatistics>();
}

vector<string> HeroService::getAttributesWithHighAverageDamage(double minAvgDamage) {
    return vector<string>();
}