OS := $(shell uname)

CXX ?= g++

EXECUTABLE_CLIENT := client.exe
EXECUTABLE_SERVER := server.exe

INCLUDE := -isystem ./cppzmq \
           -isystem ./readerwriterqueue \
           -isystem /usr/local/include/breakpad \
           -I ./lib/

# This can be useful to build with sanitizers, for example:
# CXX=clang++ EXTRA_LDFLAGS="-fsanitize=memory -fno-omit-frame-pointer -g" make re
EXTRA_LDFLAGS ?=
LDFLAGS := -L/usr/local/lib -lzmq -lpthread -lfmt -luuid $(EXTRA_LDFLAGS)
ifeq ($(OS),Linux)
LDFLAGS += -lbreakpad_client
endif

CLIENT := apps/client.cpp
SERVER := apps/server.cpp

CLIENT_OBJECT := $(CLIENT:.cpp=.o)
SERVER_OBJECT := $(SERVER:.cpp=.o)

CLIENT_BUILD := $(CLIENT_OBJECT)
SERVER_BUILD := $(SERVER_OBJECT)

# Can be useful to compil with clang/libc++, for example:
# CXX=clang++ EXTRA_CXXFLAGS="-stdlib=libc++" EXTRA_LDFLAGS="-lc++" make re
EXTRA_CXXFLAGS ?= 
CXXFLAGS := -std=c++14 \
            -Wall \
            -Wextra \
            -Wshadow \
            -Wnon-virtual-dtor \
            -pedantic \
            -Wold-style-cast \
            -Wcast-align \
            -Wunused \
            -Woverloaded-virtual \
            -Weffc++ \
            -Wconversion \
            -Werror \
            -O3 \
            -march=native \
            -g \
            $(EXTRA_CXXFLAGS)

all: print_build_config $(EXECUTABLE_CLIENT) $(EXECUTABLE_SERVER)

print_build_config:
	@echo "[ Compiler    ] `$(CXX) --version | head -n 1`"
	@echo "[ Flags       ] $(CXXFLAGS)"
	@echo "[ Linker      ] $(LDFLAGS)"
	@echo "[ Directories ] $(INCLUDE)"

$(EXECUTABLE_CLIENT): $(CLIENT_BUILD)
	@echo "[ Link        ] $@"
	@$(CXX) $(CLIENT_BUILD) -o $@ $(LDFLAGS)

$(EXECUTABLE_SERVER): $(SERVER_BUILD)
	@echo "[ Link        ] $@"
	@$(CXX) $(SERVER_BUILD) -o $@ $(LDFLAGS)

.cpp.o:
	@$(CXX) $(CXXFLAGS) $(INCLUDE) $< -c -o $@
	@echo "[ Build       ] $@"

clean:
	@rm -f $(CLIENT_OBJECT) $(SERVER_OBJECT) 

fclean: clean
	@rm -f $(EXECUTABLE_CLIENT) $(EXECUTABLE_SERVER)

re: fclean all

style:
	@for src in "$(CLIENT)" "$(SERVER)" "lib/lib.hpp" ; do \
		echo "Formatting $$src..." ; \
		clang-format -i "$$src" ; \
		clang-tidy -header-filter=".*" -checks='-*,readability-identifier-naming' \
		    -config="{CheckOptions: [ \
		    { key: readability-identifier-naming.NamespaceCase, value: lower_case },\
		    { key: readability-identifier-naming.ClassCase, value: camelBack  },\
		    { key: readability-identifier-naming.StructCase, value: camelBack  },\
		    { key: readability-identifier-naming.FunctionCase, value: lower_case },\
		    { key: readability-identifier-naming.VariableCase, value: lower_case },\
		    { key: readability-identifier-naming.GlobalConstantCase, value: UPPER_CASE }\
		    ]}" "$$src" ; \
	done
	@echo "Done"

.PHONY: all print_build_config $(EXECUTABLE_CLIENT) $(EXECUTABLE_SERVER) clean re get-deps style
