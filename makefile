# Directories for compiling results
BIN=./bin
OBJ=./obj
SRC=./src

LEGACY=$(SRC)/legacy

# Output for binary
OUTPUT=$(BIN)/graphics_demo
OUTPUT_PPM=./output_image.ppm
OUTPUT_PNG=./output_image.png

# Get headers and cpp files
DEPS=$(shell find $(SRC) -name '*.hpp') $(shell find $(SRC) -name '*.h')
SRCS=$(shell find $(SRC) -name '*.cpp') $(shell find $(SRC) -name '*.c')
OBJS_UNFILTERED=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS)) $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
OBJS=$(filter $(OBJ)/%.o, $(OBJS_UNFILTERED))

YTAB=$(LEGACY)/y.tab.c $(LEGACY)/y.tab.h
YLEX=$(LEGACY)/lex.yy.c

MDLL=$(LEGACY)/mdl.l
MDLY=$(LEGACY)/mdl.y

# Compiler / Compiler Settings
LINKS=-lm
FLAGS=-std=c++20 -O2
COMPILER=g++ $(FLAGS)

# Command to create directory
MKDIR=mkdir

# Test File To Run With
TEST=./face.mdl

# Display File (relies on png file)
all: $(OUTPUT)
	$(OUTPUT) $(TEST)

# Compile Binary (relies on object files)
$(OUTPUT): $(YLEX) $(OBJS)
	$(MKDIR) -p $(BIN)
	$(COMPILER) $(OBJS) -o $@ $(LINKS)

# Compile Object Files (relies on sources / headers)
$(OBJ)/%.o: $(SRC)/%.cpp $(DEPS)
	$(MKDIR) -p $(@D)
	$(COMPILER) -c $< -o $@

# Compile Object Files (relies on sources / headers)
$(OBJ)/%.o: $(SRC)/%.c $(DEPS)
	$(MKDIR) -p $(@D)
	$(COMPILER) -c $< -o $@

$(YLEX): $(MDLL) $(YTAB)
	flex -I $(MDLL)
	mv ./lex.yy.c $(LEGACY)/

$(YTAB): $(MDLY) $(DEPS)
	bison -d -y $(MDLY)
	mv ./y.tab.h $(LEGACY)/
	mv ./y.tab.c $(LEGACY)/

# Clean Everything
clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)
	rm $(LEGACY)/y.tab.h
	rm $(LEGACY)/y.tab.c
	rm $(LEGACY)/lex.yy.c
	rm $(OUTPUT_PPM) $(OUTPUT_PNG)
	