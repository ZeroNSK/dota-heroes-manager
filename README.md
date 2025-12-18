# Система управления героями Dota 2

Упрощенная система управления героями Dota 2 с использованием PostgreSQL и C++.

## Требования

- PostgreSQL (версия 12+)
- libpqxx (библиотека для работы с PostgreSQL в C++)
- C++17 компилятор (g++ или clang++)

## Установка зависимостей

### macOS (с Homebrew):
```bash
brew install postgresql libpqxx
```

### Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install postgresql postgresql-contrib libpqxx-dev
```

## Настройка базы данных

1. Запустите PostgreSQL:
```bash
# macOS
brew services start postgresql

# Ubuntu/Debian
sudo systemctl start postgresql
```

2. Создайте базу данных и пользователя:
```bash
# Войдите в PostgreSQL как суперпользователь
sudo -u postgres psql

# Или на macOS
psql postgres

# Выполните команды из файла setup_database.sql
\i scripts/setup_database.sql
```

Или выполните команды вручную:
```sql
CREATE DATABASE dota_heroes;
CREATE USER dota_user WITH PASSWORD 'password';
GRANT ALL PRIVILEGES ON DATABASE dota_heroes TO dota_user;
```

## Компиляция и запуск

1. Скомпилируйте проект:
```bash
make clean
make
```

2. Настройте переменные окружения (опционально):
```bash
export DB_HOST=localhost
export DB_PORT=5432
export DB_NAME=dota_heroes
export DB_USER=ivan
export DB_PASSWORD=
```

3. Запустите приложение:
```bash
make run
# или
./bin/dota_heroes_system
```

## Возможности системы

### CRUD операции
- Добавление новых героев
- Просмотр информации о героях
- Обновление характеристик героев
- Удаление героев

### Поиск и фильтрация
- Поиск по имени
- Фильтрация по атрибуту (Strength, Agility, Intelligence)
- Фильтрация по сложности (1-3)
- Фильтрация по характеристикам (здоровье, урон)

### Статистика и агрегация
- Общая статистика по всем героям
- Статистика по атрибутам с GROUP BY
- Средние характеристики с AVG()
- Группировка с фильтрацией HAVING

### Демонстрация PostgreSQL
- WHERE условия для фильтрации
- ORDER BY для сортировки
- GROUP BY для группировки
- HAVING для фильтрации групп
- Агрегатные функции (COUNT, AVG, MIN, MAX)
- CHECK ограничения для валидации
- UNIQUE ограничения
- Индексы для оптимизации
- Триггеры для автоматизации

## Структура проекта

```
├── include/
│   ├── database/
│   │   └── DatabaseManager.h
│   ├── models/
│   │   └── Hero.h
│   ├── services/
│   │   └── HeroService.h
│   └── ui/
│       └── ConsoleUI.h
├── src/
│   ├── database/
│   │   └── DatabaseManager.cpp
│   ├── services/
│   │   └── HeroService.cpp
│   ├── ui/
│   │   └── ConsoleUI.cpp
│   └── main.cpp
├── scripts/
│   └── setup_database.sql
└── Makefile
```

## Схема базы данных

Система использует единственную таблицу `heroes` с полным набором характеристик:

- Базовая информация: ID, имя, отображаемое имя, атрибут, сложность
- Характеристики: здоровье, мана, броня, урон, скорость атаки, скорость передвижения
- Атрибуты: базовые значения силы/ловкости/интеллекта и их приросты за уровень
- Метаданные: время создания и обновления записи

## Устранение неполадок

### Ошибка подключения к базе данных
- Убедитесь, что PostgreSQL запущен
- Проверьте настройки подключения в переменных окружения
- Убедитесь, что база данных `dota_heroes` создана

### Ошибки компиляции
- Убедитесь, что установлены все зависимости
- Проверьте пути к библиотекам в Makefile
- Используйте компилятор с поддержкой C++17