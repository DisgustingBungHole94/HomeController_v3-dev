SRCDIR = src
STRUCTURE = $(shell cd $(SRCDIR) && find . -type d)

DEPSDIR = thirdparty

CXX ?= g++
CXXFLAGS ?= -g $(INCLUDES)

BINARYDIR = bin
OBJECTDIR = $(BINARYDIR)/obj

TARGET = $(BINARYDIR)/test_device

LIBS += -lpthread
LIBS += -lssl
LIBS += -lhomecontroller

# root
_OBJECTS += main.o

OBJECTS = $(patsubst %,$(OBJECTDIR)/%,$(_OBJECTS))
HEADERS = $(patsubst %,$(SRCDIR)/%,$(_HEADERS))

$(OBJECTDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJECTDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(OBJECTDIR):
	mkdir -p $(OBJECTDIR)
	mkdir -p $(addprefix $(OBJECTDIR)/,$(STRUCTURE))

clean:
	rm -rf bin

.PHONY: clean