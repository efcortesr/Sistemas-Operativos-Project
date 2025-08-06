CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -Iinclude
TARGET = procplanner
SRCDIR = src
INCDIR = include
SOURCES = $(SRCDIR)/main.cpp

# Crear el directorio de salida si no existe
$(shell mkdir -p bin)

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o bin/$(TARGET) $(SOURCES)

clean:
	rm -rf bin/$(TARGET)

install: $(TARGET)
	cp bin/$(TARGET) /usr/local/bin/

.PHONY: all clean install

# Reglas de ayuda
help:
	@echo "Uso del Makefile:"
	@echo "  make          - Compila el programa"
	@echo "  make clean    - Limpia archivos compilados"
	@echo "  make install  - Instala el programa en el sistema"
	@echo "  make help     - Muestra esta ayuda"
