CXX := g++
CXXFLAGS := -std=c++14 -Wall -pedantic -pthread
CXX_LDFLAGS := -lpthread -lboost_system
CXX_INCLUDE_DIRS := ./include /usr/local/include
CXX_INCLUDE_PARAMS = $(addprefix -I, $(CXX_INCLUDE_DIRS))
CXX_LIB_DIRS := /usr/local/lib
CXX_LIB_PARAMS = $(addprefix -L, $(CXX_LIB_DIRS))

TARGETS := http_server console.cgi

HTTP_SERVER_TARGETS := http_server server session request response
HTTP_SERVER_OBJECTS = $(patsubst %,%.o,$(HTTP_SERVER_TARGETS))

HTTP_CLIENT_TARGETS := console client
HTTP_CLIENT_OBJECTS = $(patsubst %,%.o,$(HTTP_CLIENT_TARGETS))

OBJECTS = $(HTTP_SERVER_OBJECTS) $(HTTP_CLIENT_OBJECTS)

all: $(TARGETS)

part1: $(TARGETS)

http_server: $(HTTP_SERVER_OBJECTS)
	$(CXX) $(CXX_LIB_PARAMS) $(CXX_LDFLAGS) $^ -o $@

console.cgi: $(HTTP_CLIENT_OBJECTS)
	$(CXX) $(CXX_LIB_PARAMS) $(CXX_LDFLAGS) $^ -o $@

$(OBJECTS): %.o: ./src/%.cpp
	$(CXX) -c $(CXXFLAGS) $(CXX_INCLUDE_PARAMS) $< -o $@

clean:
	rm -f $(TARGETS) *.o
