# Directories for compiling results
BIN=./bin
OBJ=./obj
SRC=./src

# Output for binary
OUTPUT=$(BIN)/graphics_demo

# Get headers and c files
SUBDIRS:=$(wildcard $(SRC)/*/.)
DEPS=$(wildcard $(SUBDIRS)/*.hpp)
SRCS=$(wildcard $(SRC)/*.cpp)
OBJS=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))

# Compiler / Compiler Settings
LINKS=-lm
FLAGS=-O2
COMPILER=g++ $(FLAGS)

# Command to create directory
MKDIR=mkdir

# Compile the Binary
all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(MKDIR) -p $(BIN)
	$(COMPILER) $^ -o $@ $(LINKS)

# Compile Every Object
$(OBJ)/%.o: $(SRC)/%.cpp $(DEPS)
	$(MKDIR) -p $(@D)
	$(COMPILER) -c $< -o $@

# Run the binary
run: $(OUTPUT)
	$(OUTPUT)

# Clean make output
clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)