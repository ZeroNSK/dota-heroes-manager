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
    hero.baseDamageMin = row["base_damage_min"].as<int>();
    hero.baseDamageMax = row["base_damage_max"].as<int>();
    hero.baseAttackSpeed = row["base_attack_speed"].as<float>();
    hero.baseMoveSpeed = row["base_move_speed"].as<int>();
    
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
        
        string query = R"(
            INSERT INTO heroes (name, display_name, attribute_type, complexity,
                               base_health, base_mana, base_armor, base_damage_min, base_damage_max,
                               base_attack_speed, base_move_speed, base_strength, base_agility,
                               base_intelligence, strength_gain, agility_gain, intelligence_gain)
            VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17)
        )";
        
        txn->exec_params(query, 
            hero.name, hero.displayName, hero.attributeType, hero.complexity,
            hero.baseHealth, hero.baseMana, hero.baseArmor, hero.baseDamageMin, hero.baseDamageMax,
            hero.baseAttackSpeed, hero.baseMoveSpeed, hero.baseStrength, hero.baseAgility,
            hero.baseIntelligence, hero.strengthGain, hero.agilityGain, hero.intelligenceGain
        );
        
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
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            SELECT hero_id, name, display_name, attribute_type, complexity,
                   base_health, base_mana, base_armor, base_damage_min, base_damage_max,
                   base_attack_speed, base_move_speed, base_strength, base_agility,
                   base_intelligence, strength_gain, agility_gain, intelligence_gain
            FROM heroes 
            WHERE hero_id = $1
        )";
        
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
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            SELECT hero_id, name, display_name, attribute_type, complexity,
                   base_health, base_mana, base_armor, base_damage_min, base_damage_max,
                   base_attack_speed, base_move_speed, base_strength, base_agility,
                   base_intelligence, strength_gain, agility_gain, intelligence_gain
            FROM heroes 
            WHERE name = $1
        )";
        
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

bool HeroService::updateHero(const Hero& hero) {
    if (!hero.isValid() || hero.heroId <= 0) {
        cerr << "Ошибка: Некорректные данные героя для обновления" << endl;
        return false;
    }
    
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            UPDATE heroes 
            SET name = $1, display_name = $2, attribute_type = $3, complexity = $4,
                base_health = $5, base_mana = $6, base_armor = $7, base_damage_min = $8, base_damage_max = $9,
                base_attack_speed = $10, base_move_speed = $11, base_strength = $12, base_agility = $13,
                base_intelligence = $14, strength_gain = $15, agility_gain = $16, intelligence_gain = $17
            WHERE hero_id = $18
        )";
        
        pqxx::result result = txn->exec_params(query,
            hero.name, hero.displayName, hero.attributeType, hero.complexity,
            hero.baseHealth, hero.baseMana, hero.baseArmor, hero.baseDamageMin, hero.baseDamageMax,
            hero.baseAttackSpeed, hero.baseMoveSpeed, hero.baseStrength, hero.baseAgility,
            hero.baseIntelligence, hero.strengthGain, hero.agilityGain, hero.intelligenceGain,
            hero.heroId
        );
        
        dbManager.commitTransaction(txn);
        
        if (result.affected_rows() > 0) {
            cout << "Герой '" << hero.displayName << "' успешно обновлен" << endl;
            return true;
        } else {
            cerr << "Герой с ID " << hero.heroId << " не найден" << endl;
            return false;
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при обновлении героя: " << e.what() << endl;
        return false;
    }
}

bool HeroService::deleteHero(int heroId) {
    try {
        auto txn = dbManager.beginTransaction();
        
        string deleteHeroQuery = "DELETE FROM heroes WHERE hero_id = $1";
        pqxx::result result = txn->exec_params(deleteHeroQuery, heroId);
        
        dbManager.commitTransaction(txn);
        
        if (result.affected_rows() > 0) {
            cout << "Герой с ID " << heroId << " успешно удален" << endl;
            return true;
        } else {
            cerr << "Герой с ID " << heroId << " не найден" << endl;
            return false;
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при удалении героя: " << e.what() << endl;
        return false;
    }
}

vector<Hero> HeroService::getAllHeroes() {
    vector<Hero> heroes;
    
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            SELECT hero_id, name, display_name, attribute_type, complexity,
                   base_health, base_mana, base_armor, base_damage_min, base_damage_max,
                   base_attack_speed, base_move_speed, base_strength, base_agility,
                   base_intelligence, strength_gain, agility_gain, intelligence_gain
            FROM heroes 
            ORDER BY name
        )";
        
        pqxx::result result = txn->exec(query);
        
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
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            SELECT hero_id, name, display_name, attribute_type, complexity,
                   base_health, base_mana, base_armor, base_damage_min, base_damage_max,
                   base_attack_speed, base_move_speed, base_strength, base_agility,
                   base_intelligence, strength_gain, agility_gain, intelligence_gain
            FROM heroes 
            WHERE attribute_type = $1
            ORDER BY name
        )";
        
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
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            SELECT hero_id, name, display_name, attribute_type, complexity,
                   base_health, base_mana, base_armor, base_damage_min, base_damage_max,
                   base_attack_speed, base_move_speed, base_strength, base_agility,
                   base_intelligence, strength_gain, agility_gain, intelligence_gain
            FROM heroes 
            WHERE complexity = $1
            ORDER BY name
        )";
        
        pqxx::result result = txn->exec_params(query, complexity);
        
        for (const auto& row : result) {
            heroes.push_back(resultToHero(row));
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении героев по сложности: " << e.what() << endl;
    }
    
    return heroes;
}

vector<Hero> HeroService::searchHeroes(const HeroSearchCriteria& criteria) {
    vector<Hero> heroes;
    
    if (!criteria.hasAnyFilter()) {
        return getAllHeroes();
    }
    
    try {
        auto txn = dbManager.beginTransaction();
        
        ostringstream query;
        query << R"(
            SELECT hero_id, name, display_name, attribute_type, complexity,
                   base_health, base_mana, base_armor, base_damage_min, base_damage_max,
                   base_attack_speed, base_move_speed, base_strength, base_agility,
                   base_intelligence, strength_gain, agility_gain, intelligence_gain
            FROM heroes 
            WHERE 1=1
        )";
        
        vector<string> params;
        int paramIndex = 1;
        
        if (criteria.namePattern.has_value()) {
            query << " AND (name ILIKE $" << paramIndex << " OR display_name ILIKE $" << paramIndex << ")";
            params.push_back("%" + criteria.namePattern.value() + "%");
            paramIndex++;
        }
        
        if (criteria.attributeType.has_value()) {
            query << " AND attribute_type = $" << paramIndex++;
            params.push_back(criteria.attributeType.value());
        }
        
        if (criteria.complexity.has_value()) {
            query << " AND complexity = $" << paramIndex++;
            params.push_back(to_string(criteria.complexity.value()));
        }
        
        if (criteria.minHealth.has_value()) {
            query << " AND base_health >= $" << paramIndex++;
            params.push_back(to_string(criteria.minHealth.value()));
        }
        
        if (criteria.maxHealth.has_value()) {
            query << " AND base_health <= $" << paramIndex++;
            params.push_back(to_string(criteria.maxHealth.value()));
        }
        
        if (criteria.minDamage.has_value()) {
            query << " AND base_damage_min >= $" << paramIndex++;
            params.push_back(to_string(criteria.minDamage.value()));
        }
        
        if (criteria.maxDamage.has_value()) {
            query << " AND base_damage_max <= $" << paramIndex++;
            params.push_back(to_string(criteria.maxDamage.value()));
        }
        
        query << " ORDER BY name";
        
        pqxx::result result = txn->exec_params(query.str(), params);
        
        for (const auto& row : result) {
            heroes.push_back(resultToHero(row));
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при поиске героев: " << e.what() << endl;
    }
    
    return heroes;
}

vector<Hero> HeroService::getTopHeroesByDamage(int limit) {
    vector<Hero> heroes;
    
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            SELECT hero_id, name, display_name, attribute_type, complexity,
                   base_health, base_mana, base_armor, base_damage_min, base_damage_max,
                   base_attack_speed, base_move_speed, base_strength, base_agility,
                   base_intelligence, strength_gain, agility_gain, intelligence_gain
            FROM heroes 
            ORDER BY (base_damage_min + base_damage_max) / 2.0 DESC
            LIMIT $1
        )";
        
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
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            SELECT hero_id, name, display_name, attribute_type, complexity,
                   base_health, base_mana, base_armor, base_damage_min, base_damage_max,
                   base_attack_speed, base_move_speed, base_strength, base_agility,
                   base_intelligence, strength_gain, agility_gain, intelligence_gain
            FROM heroes 
            ORDER BY base_health DESC
        )";
        
        pqxx::result result = txn->exec(query);
        
        for (const auto& row : result) {
            heroes.push_back(resultToHero(row));
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении героев, отсортированных по здоровью: " << e.what() << endl;
    }
    
    return heroes;
}

vector<Hero> HeroService::getHeroesSortedByMoveSpeed() {
    vector<Hero> heroes;
    
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            SELECT hero_id, name, display_name, attribute_type, complexity,
                   base_health, base_mana, base_armor, base_damage_min, base_damage_max,
                   base_attack_speed, base_move_speed, base_strength, base_agility,
                   base_intelligence, strength_gain, agility_gain, intelligence_gain
            FROM heroes 
            ORDER BY base_move_speed DESC
        )";
        
        pqxx::result result = txn->exec(query);
        
        for (const auto& row : result) {
            heroes.push_back(resultToHero(row));
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении героев, отсортированных по скорости: " << e.what() << endl;
    }
    
    return heroes;
}

bool HeroService::heroExists(int heroId) {
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = "SELECT COUNT(*) FROM heroes WHERE hero_id = $1";
        pqxx::result result = txn->exec_params(query, heroId);
        
        return result[0][0].as<int>() > 0;
        
    } catch (const exception& e) {
        cerr << "Ошибка при проверке существования героя: " << e.what() << endl;
        return false;
    }
}

bool HeroService::heroExistsByName(const string& name) {
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = "SELECT COUNT(*) FROM heroes WHERE name = $1";
        pqxx::result result = txn->exec_params(query, name);
        
        return result[0][0].as<int>() > 0;
        
    } catch (const exception& e) {
        cerr << "Ошибка при проверке существования героя по имени: " << e.what() << endl;
        return false;
    }
}

int HeroService::getHeroCount() {
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = "SELECT COUNT(*) FROM heroes";
        pqxx::result result = txn->exec(query);
        
        return result[0][0].as<int>();
        
    } catch (const exception& e) {
        cerr << "Ошибка при подсчете героев: " << e.what() << endl;
        return 0;
    }
}

vector<AttributeStatistics> HeroService::getAttributeStatistics() {
    vector<AttributeStatistics> statistics;
    
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            SELECT 
                attribute_type,
                COUNT(*) as hero_count,
                AVG(base_health) as avg_health,
                AVG((base_damage_min + base_damage_max) / 2.0) as avg_damage,
                AVG(base_move_speed) as avg_move_speed,
                MIN(complexity) as min_complexity,
                MAX(complexity) as max_complexity
            FROM heroes
            GROUP BY attribute_type
            ORDER BY hero_count DESC, attribute_type
        )";
        
        pqxx::result result = txn->exec(query);
        
        for (const auto& row : result) {
            AttributeStatistics stat;
            stat.attributeType = row["attribute_type"].as<string>();
            stat.heroCount = row["hero_count"].as<int>();
            stat.avgHealth = row["avg_health"].as<double>();
            stat.avgDamage = row["avg_damage"].as<double>();
            stat.avgMoveSpeed = row["avg_move_speed"].as<double>();
            stat.minComplexity = row["min_complexity"].as<int>();
            stat.maxComplexity = row["max_complexity"].as<int>();
            
            statistics.push_back(stat);
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении статистики атрибутов: " << e.what() << endl;
    }
    
    return statistics;
}

HeroStatistics HeroService::getOverallStatistics() {
    HeroStatistics stats = {};
    
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            SELECT 
                COUNT(*) as total_heroes,
                AVG(base_health) as avg_health,
                AVG(base_mana) as avg_mana,
                AVG((base_damage_min + base_damage_max) / 2.0) as avg_damage,
                AVG(base_move_speed) as avg_move_speed,
                SUM(CASE WHEN attribute_type = 'Strength' THEN 1 ELSE 0 END) as strength_heroes,
                SUM(CASE WHEN attribute_type = 'Agility' THEN 1 ELSE 0 END) as agility_heroes,
                SUM(CASE WHEN attribute_type = 'Intelligence' THEN 1 ELSE 0 END) as intelligence_heroes
            FROM heroes
        )";
        
        pqxx::result result = txn->exec(query);
        
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

double HeroService::getAverageHealthByAttribute(const string& attributeType) {
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = "SELECT AVG(base_health) FROM heroes WHERE attribute_type = $1";
        pqxx::result result = txn->exec_params(query, attributeType);
        
        if (!result.empty() && !result[0][0].is_null()) {
            return result[0][0].as<double>();
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении среднего здоровья по атрибуту: " << e.what() << endl;
    }
    
    return 0.0;
}

double HeroService::getAverageDamageByAttribute(const string& attributeType) {
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = "SELECT AVG((base_damage_min + base_damage_max) / 2.0) FROM heroes WHERE attribute_type = $1";
        pqxx::result result = txn->exec_params(query, attributeType);
        
        if (!result.empty() && !result[0][0].is_null()) {
            return result[0][0].as<double>();
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении среднего урона по атрибуту: " << e.what() << endl;
    }
    
    return 0.0;
}

int HeroService::getMinComplexityByAttribute(const string& attributeType) {
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = "SELECT MIN(complexity) FROM heroes WHERE attribute_type = $1";
        pqxx::result result = txn->exec_params(query, attributeType);
        
        if (!result.empty() && !result[0][0].is_null()) {
            return result[0][0].as<int>();
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении минимальной сложности по атрибуту: " << e.what() << endl;
    }
    
    return 0;
}

int HeroService::getMaxComplexityByAttribute(const string& attributeType) {
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = "SELECT MAX(complexity) FROM heroes WHERE attribute_type = $1";
        pqxx::result result = txn->exec_params(query, attributeType);
        
        if (!result.empty() && !result[0][0].is_null()) {
            return result[0][0].as<int>();
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении максимальной сложности по атрибуту: " << e.what() << endl;
    }
    
    return 0;
}

vector<AttributeStatistics> HeroService::getAttributesWithMinHeroCount(int minCount) {
    vector<AttributeStatistics> statistics;
    
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            SELECT 
                attribute_type,
                COUNT(*) as hero_count,
                AVG(base_health) as avg_health,
                AVG((base_damage_min + base_damage_max) / 2.0) as avg_damage,
                AVG(base_move_speed) as avg_move_speed,
                MIN(complexity) as min_complexity,
                MAX(complexity) as max_complexity
            FROM heroes
            GROUP BY attribute_type
            HAVING COUNT(*) >= $1
            ORDER BY hero_count DESC
        )";
        
        pqxx::result result = txn->exec_params(query, minCount);
        
        for (const auto& row : result) {
            AttributeStatistics stat;
            stat.attributeType = row["attribute_type"].as<string>();
            stat.heroCount = row["hero_count"].as<int>();
            stat.avgHealth = row["avg_health"].as<double>();
            stat.avgDamage = row["avg_damage"].as<double>();
            stat.avgMoveSpeed = row["avg_move_speed"].as<double>();
            stat.minComplexity = row["min_complexity"].as<int>();
            stat.maxComplexity = row["max_complexity"].as<int>();
            
            statistics.push_back(stat);
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении атрибутов с минимальным количеством героев: " << e.what() << endl;
    }
    
    return statistics;
}

vector<string> HeroService::getAttributesWithHighAverageDamage(double minAvgDamage) {
    vector<string> attributes;
    
    try {
        auto txn = dbManager.beginTransaction();
        
        string query = R"(
            SELECT attribute_type
            FROM heroes
            GROUP BY attribute_type
            HAVING AVG((base_damage_min + base_damage_max) / 2.0) >= $1
            ORDER BY AVG((base_damage_min + base_damage_max) / 2.0) DESC
        )";
        
        pqxx::result result = txn->exec_params(query, minAvgDamage);
        
        for (const auto& row : result) {
            attributes.push_back(row["attribute_type"].as<string>());
        }
        
    } catch (const exception& e) {
        cerr << "Ошибка при получении атрибутов с высоким средним уроном: " << e.what() << endl;
    }
    
    return attributes;
}