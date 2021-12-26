# Makefile from:
# https://stackoverflow.com/questions/30573481/how-to-write-a-makefile-with-separate-source-and-header-directories/30602701#30602701

CC := g++

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

EXE := $(BIN_DIR)/therminal
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# TODO: Add single pugixml .o-file manually here
OBJ := $(OBJ) obj/pugixml.o

$(info $$SRC is [${SRC}])

$(info $$OBJ is [${OBJ}])

CPPFLAGS := -Iinclude -MMD -MP
CFLAGS   := -Wall
LDFLAGS  := -Llib
LDLIBS   := -lm -lstdc++fs -lwiringPi -lpthread

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

#Added manually compilation of pugixml   
obj/pugixml.o: ../pugixml/src/pugixml.cpp ../pugixml/src/pugixml.hpp
	$(CC) -I../pugixml/src -MMD -MP -Wall -c ../pugixml/src/pugixml.cpp -o obj/pugixml.o

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)