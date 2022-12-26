CXX := g++
CXXFLAGS := -std=c++14 -Wall -pedantic -pthread -lboost_system
CXX_INCLUDE_DIRS := ./include /usr/local/include
CXX_INCLUDE_PARAMS = $(addprefix -I, $(CXX_INCLUDE_DIRS))
CXX_LIB_DIRS := /usr/local/lib
CXX_LIB_PARAMS = $(addprefix -L, $(CXX_LIB_DIRS))

TARGETS := http_server

HTTP_SERVER_TARGETS := http_server server session request response
HTTP_SERVER_OBJECTS = $(patsubst %,%.o,$(HTTP_SERVER_TARGETS))

all: $(TARGETS)

http_server: $(HTTP_SERVER_OBJECTS)
	$(CXX) $^ -o $@

$(HTTP_SERVER_OBJECTS): %.o: ./src/%.cpp
	$(CXX) -c $(CXXFLAGS) $(CXX_INCLUDE_PARAMS) $(CXX_LIB_PARAMS) $< -o $@

clean:
	rm -f $(TARGETS) *.o
