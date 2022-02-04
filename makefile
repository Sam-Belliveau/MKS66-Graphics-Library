# Directories for compiling results
BIN=./bin
OBJ=./obj
SRC=./src

# Output for binary
OUTPUT=$(BIN)/graphics_demo

# Get headers and cpp files
DEPS=$(shell find $(SRC) -name '*.hpp') $(shell find $(SRC) -name '*.h')
SRCS=$(shell find $(SRC) -name '*.cpp')
OBJS=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))

# Compiler / Compiler Settings
LINKS=-lm
FLAGS=-std=c++17 -O2
COMPILER=g++ $(FLAGS)

# Command to create directory
MKDIR=mkdir

# Compile Everything (relies on binary)
all: $(OUTPUT)

# Compile Binary (relies on object files)
$(OUTPUT): $(OBJS)
	$(MKDIR) -p $(BIN)
	$(COMPILER) $^ -o $@ $(LINKS)

# Compile Object Files (relies on sources / headers)
$(OBJ)/%.o: $(SRC)/%.cpp $(DEPS)
	$(MKDIR) -p $(@D)
	$(COMPILER) -c $< -o $@

# Run the Binary (relies on binary)
run: $(OUTPUT)
	$(OUTPUT)

# Clean Everything
clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)