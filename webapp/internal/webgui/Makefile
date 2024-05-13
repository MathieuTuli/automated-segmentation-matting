#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need GLFW (http://www.glfw.org):
# Linux:
#   apt-get install libglfw-dev
# Mac OS X:
#   brew install glfw
# MSYS2:
#   pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw
# >>> compiledb make

#CXX = g++
#CXX = clang++

EXE = main
SRC_DIRS = ./src
BUILD_DIR = ./build

LIBS_DIR = ./libs
IMGUI_DIR = ./include/imgui

# SOURCES = src/main.cpp
SOURCES := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
# OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
OBJS := $(SOURCES:%=$(BUILD_DIR)/%.o)
UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL

LIBS =
CXXFLAGS = -std=c++17 -g -Wall -Wformat
CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends

# NFD shit
CXXFLAGS += -I./include/nfd

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += $(LINUX_GL_LIBS) $(shell pkg-config --static --libs glfw3)
	LIBS += $(shell pkg-config --libs opencv4)
	LIBS += -L/usr/local/lib
	LIBS += ${LIBS_DIR}/nfd_linux/libnfd.a
	CXXFLAGS += $(shell pkg-config --cflags glfw3)
	CXXFLAGS += $(shell pkg-config --cflags opencv4)
	CXXFLAGS += $(shell pkg-config --cflags --libs gtk+-3.0)
	CXXFLAGS += -I/usr/local/include
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LIBS += -lglfw
	LIBS += ${LIBS_DIR}/nfd/libnfd.a
	LIBS += -L/usr/local/lib -L/opt/homebrew/lib 
	LIBS += $(shell pkg-config --libs opencv4)
	LIBS += -L /opt/homebrew/opt/ffmpeg/lib -lavformat -lpostproc -lavcodec -lswscale -lavfilter -lavutil -lswresample -lavdevice
	LIBS += -framework AppKit -framework UniformTypeIdentifiers
	#LIBS += -lglfw3
	CXXFLAGS += $(shell pkg-config --cflags opencv4)
	CXXFLAGS += -I/usr/local/include -I/opt/homebrew/include
	CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

# The final build step.
$(BUILD_DIR)/$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CXXFLAGS) $(CFLAGS) -c -o $@ $< 

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $< 

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

clean:
	rm -f $(EXE) $(OBJS)
