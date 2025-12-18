# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# Определение операционной системы и настройка путей
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS с Homebrew
    HOMEBREW_PREFIX := $(shell brew --prefix)
    PQXX_PREFIX := $(shell brew --prefix libpqxx)
    PQ_PREFIX := $(shell brew --prefix libpq)
    CXXFLAGS += -I$(HOMEBREW_PREFIX)/include -I$(PQXX_PREFIX)/include -I$(PQ_PREFIX)/include
    LDFLAGS = -L$(HOMEBREW_PREFIX)/lib -L$(PQXX_PREFIX)/lib -L$(PQ_PREFIX)/lib -lpqxx -lpq
else
    # Linux
    LDFLAGS = -lpqxx -lpq
endif

# Директории
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Исполняемый файл
TARGET = $(BINDIR)/dota_heroes_system

# Исходные файлы
SOURCES = $(wildcard $(SRCDIR)/*.cpp) \
          $(wildcard $(SRCDIR)/*/*.cpp) \
          $(wildcard $(SRCDIR)/*/*/*.cpp)

# Объектные файлы
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Правило по умолчанию
all: $(TARGET)

# Создание исполняемого файла
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Компиляция объектных файлов
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Создание директорий
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Очистка
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Установка зависимостей (для Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y libpqxx-dev postgresql postgresql-contrib

# Запуск
run: $(TARGET)
	./$(TARGET)

# Тесты
test-ability: test_ability_service.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) test_ability_service.cpp $(filter-out obj/main.o,$(OBJECTS)) -o bin/test_ability_service $(LDFLAGS)

test-roles: test_roles_attributes.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) test_roles_attributes.cpp $(filter-out obj/main.o,$(OBJECTS)) -o bin/test_roles_attributes $(LDFLAGS)

test-roles-simple: test_roles_attributes_simple.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) test_roles_attributes_simple.cpp $(filter-out obj/main.o obj/ui/ConsoleUI.o,$(OBJECTS)) -o bin/test_roles_attributes_simple $(LDFLAGS)

# Отладочная сборка
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

.PHONY: all clean install-deps run test-ability debug