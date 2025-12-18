#!/bin/bash

echo "=== Запуск системы управления героями Dota 2 ==="

# Проверяем, что PostgreSQL запущен
if ! pgrep -x "postgres" > /dev/null; then
    echo "Запускаем PostgreSQL..."
    if command -v brew &> /dev/null; then
        # macOS с Homebrew
        brew services start postgresql
    else
        # Linux
        sudo systemctl start postgresql
    fi
    sleep 2
fi

# Устанавливаем переменные окружения по умолчанию
export DB_HOST=${DB_HOST:-localhost}
export DB_PORT=${DB_PORT:-5432}
export DB_NAME=${DB_NAME:-dota_heroes}
export DB_USER=${DB_USER:-ivan}
export DB_PASSWORD=${DB_PASSWORD:-}

echo "Настройки подключения:"
echo "  Host: $DB_HOST"
echo "  Port: $DB_PORT"
echo "  Database: $DB_NAME"
echo "  User: $DB_USER"

# Компилируем проект
echo "Компиляция проекта..."
make clean
make

if [ $? -eq 0 ]; then
    echo "Компиляция успешна! Запускаем приложение..."
    echo ""
    ./bin/dota_heroes_system
else
    echo "Ошибка компиляции!"
    exit 1
fi