# Directories for compiling results
BIN=./bin
OBJ=./obj
SRC=./src

# Output for binary
OUTPUT=$(BIN)/graphics_demo
OUTPUT_PPM=./output_image.ppm
OUTPUT_PNG=./output_image.png

# Get headers and cpp files
DEPS=$(shell find $(SRC) -name '*.hpp') $(shell find $(SRC) -name '*.h')
SRCS=$(shell find $(SRC) -name '*.cpp')
OBJS=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))

# Compiler / Compiler Settings
LINKS=-lm
FLAGS=-std=c++20 -g
COMPILER=g++ $(FLAGS)

# Command to create directory
MKDIR=mkdir

# Display File (relies on png file)
all: $(OUTPUT_PNG)
	open $(OUTPUT_PNG)

# Compile Binary (relies on object files)
$(OUTPUT): $(OBJS)
	$(MKDIR) -p $(BIN)
	$(COMPILER) $^ -o $@ $(LINKS)

# Compile Object Files (relies on sources / headers)
$(OBJ)/%.o: $(SRC)/%.cpp $(DEPS)
	$(MKDIR) -p $(@D)
	$(COMPILER) -c $< -o $@

# Create PPM File (relies on binary)
$(OUTPUT_PPM): $(OUTPUT)
	$(OUTPUT)

# Create PNG File (relies on ppm file)
$(OUTPUT_PNG): $(OUTPUT_PPM)
	convert $(OUTPUT_PPM) $(OUTPUT_PNG)

# Clean Everything
clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)
	rm $(OUTPUT_PPM) $(OUTPUT_PNG)
	