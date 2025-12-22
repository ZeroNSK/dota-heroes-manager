#pragma once

#include <string>
#include <optional>

using namespace std;

struct Hero {
    int heroId;
    string name;
    string displayName;
    string attributeType;
    int complexity;
    int baseHealth;
    int baseMana;
    int baseArmor;
    int baseDamageMin;
    int baseDamageMax;
    float baseAttackSpeed;
    int baseMoveSpeed;
    int baseStrength;
    int baseAgility;
    int baseIntelligence;
    float strengthGain;
    float agilityGain;
    float intelligenceGain;
    
    Hero() : heroId(0), attributeType("Strength"), complexity(1),
             baseHealth(500), baseMana(200), baseArmor(0),
             baseDamageMin(50), baseDamageMax(60), baseAttackSpeed(1.7f), baseMoveSpeed(300),
             baseStrength(20), baseAgility(20), baseIntelligence(20),
             strengthGain(2.0f), agilityGain(2.0f), intelligenceGain(2.0f) {}
             
    Hero(const string& name, const string& displayName, const string& attributeType, int complexity,
         int baseHealth, int baseMana, int baseArmor, int baseDamageMin, int baseDamageMax,
         float baseAttackSpeed, int baseMoveSpeed, int baseStrength, int baseAgility, int baseIntelligence,
         float strengthGain, float agilityGain, float intelligenceGain)
        : heroId(0), name(name), displayName(displayName), attributeType(attributeType), complexity(complexity),
          baseHealth(baseHealth), baseMana(baseMana), baseArmor(baseArmor),
          baseDamageMin(baseDamageMin), baseDamageMax(baseDamageMax), baseAttackSpeed(baseAttackSpeed),
          baseMoveSpeed(baseMoveSpeed), baseStrength(baseStrength), baseAgility(baseAgility),
          baseIntelligence(baseIntelligence), strengthGain(strengthGain), agilityGain(agilityGain),
          intelligenceGain(intelligenceGain) {}
          
    bool isValid() const {
        return !name.empty() && 
               !displayName.empty() && 
               complexity >= 1 && complexity <= 3 &&
               (attributeType == "Strength" || attributeType == "Agility" || attributeType == "Intelligence") &&
               baseHealth > 0 &&
               baseMana >= 0 &&
               baseDamageMin > 0 &&
               baseDamageMax >= baseDamageMin &&
               baseAttackSpeed > 0 &&
               baseMoveSpeed > 0 &&
               baseStrength > 0 &&
               baseAgility > 0 &&
               baseIntelligence > 0 &&
               strengthGain > 0 &&
               agilityGain > 0 &&
               intelligenceGain > 0;
    }
};

struct HeroSearchCriteria {
    optional<string> namePattern;
    optional<string> attributeType;
    optional<int> complexity;
    optional<int> minHealth;
    optional<int> maxHealth;
    optional<int> minDamage;
    optional<int> maxDamage;
    
    bool hasAnyFilter() const {
        return namePattern.has_value() || 
               attributeType.has_value() || 
               complexity.has_value() ||
               minHealth.has_value() ||
               maxHealth.has_value() ||
               minDamage.has_value() ||
               maxDamage.has_value();
    }
};

struct AttributeStatistics {
    string attributeType;
    int heroCount;
    double avgHealth;
    double avgDamage;
    double avgMoveSpeed;
    int minComplexity;
    int maxComplexity;
};

struct HeroStatistics {
    int totalHeroes;
    double avgHealth;
    double avgMana;
    double avgDamage;
    double avgMoveSpeed;
    int strengthHeroes;
    int agilityHeroes;
    int intelligenceHeroes;
};