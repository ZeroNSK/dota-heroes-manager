#!/bin/bash

# Скрипт для автоматической настройки проекта

echo "=== Настройка системы управления героями Dota 2 ==="

# Проверка наличия PostgreSQL
if ! command -v psql &> /dev/null; then
    echo "PostgreSQL не установлен. Устанавливаем..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Linux
        sudo apt-get update
        sudo apt-get install -y postgresql postgresql-contrib libpqxx-dev build-essential
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        if command -v brew &> /dev/null; then
            brew install postgresql libpqxx
        else
            echo "Установите Homebrew и повторите попытку"
            exit 1
        fi
    else
        echo "Неподдерживаемая операционная система"
        exit 1
    fi
fi

# Запуск PostgreSQL
echo "Запуск PostgreSQL..."
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    sudo systemctl start postgresql
    sudo systemctl enable postgresql
elif [[ "$OSTYPE" == "darwin"* ]]; then
    brew services start postgresql
fi

# Ожидание запуска PostgreSQL
sleep 3

# Создание базы данных
echo "Создание базы данных..."
sudo -u postgres createdb dota_heroes 2>/dev/null || echo "База данных уже существует"

# Создание пользователя
echo "Создание пользователя базы данных..."
sudo -u postgres psql -c "CREATE USER dota_user WITH PASSWORD 'password';" 2>/dev/null || echo "Пользователь уже существует"
sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE dota_heroes TO dota_user;" 2>/dev/null

# Копирование файла конфигурации
if [ ! -f .env ]; then
    echo "Создание файла конфигурации..."
    cp .env.example .env
    echo "Отредактируйте файл .env при необходимости"
fi

# Сборка проекта
echo "Сборка проекта..."
make clean
make

echo "=== Настройка завершена! ==="
echo "Для запуска используйте: make run"
echo "Или: ./bin/dota_heroes_system"