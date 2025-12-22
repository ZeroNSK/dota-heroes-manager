CREATE DATABASE dota_heroes
    WITH 
    OWNER = postgres
    ENCODING = 'UTF8'
    LC_COLLATE = 'en_US.UTF-8'
    LC_CTYPE = 'en_US.UTF-8'
    TABLESPACE = pg_default
    CONNECTION LIMIT = -1;

CREATE USER dota_user WITH PASSWORD 'password';
GRANT ALL PRIVILEGES ON DATABASE dota_heroes TO dota_user;

\c dota_heroes;

GRANT ALL ON SCHEMA public TO dota_user;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO dota_user;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO dota_user;

CREATE TABLE heroes (
    hero_id SERIAL PRIMARY KEY,
    name VARCHAR(50) UNIQUE NOT NULL,
    display_name VARCHAR(100) NOT NULL,
    attribute_type VARCHAR(20) NOT NULL CHECK (attribute_type IN ('Strength', 'Agility', 'Intelligence')),
    complexity INTEGER NOT NULL CHECK (complexity >= 1 AND complexity <= 3),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE hero_stats (
    stat_id SERIAL PRIMARY KEY,
    hero_id INTEGER NOT NULL REFERENCES heroes(hero_id) ON DELETE CASCADE,
    base_health INTEGER NOT NULL CHECK (base_health > 0),
    base_mana INTEGER NOT NULL CHECK (base_mana >= 0),
    base_armor INTEGER NOT NULL,
    base_move_speed INTEGER NOT NULL CHECK (base_move_speed > 0),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE hero_damage (
    damage_id SERIAL PRIMARY KEY,
    hero_id INTEGER NOT NULL REFERENCES heroes(hero_id) ON DELETE CASCADE,
    base_damage_min INTEGER NOT NULL CHECK (base_damage_min > 0),
    base_damage_max INTEGER NOT NULL CHECK (base_damage_max >= base_damage_min),
    base_attack_speed REAL NOT NULL CHECK (base_attack_speed > 0),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE hero_attributes (
    attribute_id SERIAL PRIMARY KEY,
    hero_id INTEGER NOT NULL REFERENCES heroes(hero_id) ON DELETE CASCADE,
    base_strength INTEGER NOT NULL CHECK (base_strength > 0),
    base_agility INTEGER NOT NULL CHECK (base_agility > 0),
    base_intelligence INTEGER NOT NULL CHECK (base_intelligence > 0),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE hero_growth (
    growth_id SERIAL PRIMARY KEY,
    hero_id INTEGER NOT NULL REFERENCES heroes(hero_id) ON DELETE CASCADE,
    strength_gain REAL NOT NULL CHECK (strength_gain > 0),
    agility_gain REAL NOT NULL CHECK (agility_gain > 0),
    intelligence_gain REAL NOT NULL CHECK (intelligence_gain > 0),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_heroes_attribute_type ON heroes(attribute_type);
CREATE INDEX idx_heroes_complexity ON heroes(complexity);
CREATE INDEX idx_heroes_name ON heroes(name);
CREATE INDEX idx_hero_stats_hero_id ON hero_stats(hero_id);
CREATE INDEX idx_hero_damage_hero_id ON hero_damage(hero_id);
CREATE INDEX idx_hero_attributes_hero_id ON hero_attributes(hero_id);
CREATE INDEX idx_hero_growth_hero_id ON hero_growth(hero_id);

CREATE OR REPLACE FUNCTION update_updated_at_column()
RETURNS TRIGGER AS $
BEGIN
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$ language 'plpgsql';

CREATE TRIGGER update_heroes_updated_at 
    BEFORE UPDATE ON heroes 
    FOR EACH ROW 
    EXECUTE FUNCTION update_updated_at_column();

INSERT INTO heroes (name, display_name, attribute_type, complexity) VALUES
('pudge', 'Pudge', 'Strength', 2),
('invoker', 'Invoker', 'Intelligence', 3),
('anti_mage', 'Anti-Mage', 'Agility', 1),
('crystal_maiden', 'Crystal Maiden', 'Intelligence', 1),
('axe', 'Axe', 'Strength', 1),
('meepo', 'Meepo', 'Agility', 3);

INSERT INTO hero_stats (hero_id, base_health, base_mana, base_armor, base_move_speed) VALUES
(1, 700, 267, 1, 280),
(2, 492, 195, 1, 280),
(3, 558, 219, 2, 310),
(4, 426, 291, -1, 280),
(5, 625, 234, 2, 310),
(6, 588, 243, 2, 330);

INSERT INTO hero_damage (hero_id, base_damage_min, base_damage_max, base_attack_speed) VALUES
(1, 68, 78, 1.7),
(2, 42, 54, 1.7),
(3, 53, 57, 1.4),
(4, 38, 44, 1.7),
(5, 52, 58, 1.7),
(6, 39, 45, 1.7);

INSERT INTO hero_attributes (hero_id, base_strength, base_agility, base_intelligence) VALUES
(1, 25, 14, 16),
(2, 19, 14, 17),
(3, 22, 22, 12),
(4, 16, 16, 16),
(5, 25, 20, 18),
(6, 23, 23, 20);

INSERT INTO hero_growth (hero_id, strength_gain, agility_gain, intelligence_gain) VALUES
(1, 3.2, 1.5, 1.5),
(2, 2.4, 1.9, 4.6),
(3, 2.6, 2.8, 1.8),
(4, 1.7, 1.6, 2.9),
(5, 2.8, 2.2, 1.6),
(6, 1.9, 1.9, 1.6);