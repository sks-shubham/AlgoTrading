CXX = g++
CXXFLAGS = -g -O2 -std=c++17
INCLUDES = -I/usr/include
LIBS = -L/usr/lib -lcurl -lboost_thread -lboost_system -lboost_chrono -lssl -lcrypto -lcpprest

# Rule for building .o files from .cpp files
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

# Rule for building the executable
TARGET = my_program
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)
