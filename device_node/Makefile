SRCDIR = src
STRUCTURE = $(shell cd $(SRCDIR) && find . -type d)

DEPSDIR = thirdparty

CXX ?= g++
CXXFLAGS ?= -g $(INCLUDES)

BINARYDIR = bin
OBJECTDIR = $(BINARYDIR)/obj

TARGET = $(BINARYDIR)/device_node

LIBS += -lpthread
LIBS += -lssl
LIBS += -lhomecontroller

# protocol_handler
_OBJECTS += protocol_handler/device_handler.o
_HEADERS += protocol_handler/device_handler.h

_OBJECTS += protocol_handler/protocol_handler.o
_HEADERS += protocol_handler/protocol_handler.h

_OBJECTS += protocol_handler/http_handler.o
_HEADERS += protocol_handler/http_handler.h

_OBJECTS += protocol_handler/ws_handler.o
_HEADERS += protocol_handler/ws_handler.h

# root
_OBJECTS += device_manager.o
_HEADERS += device_manager.h

_OBJECTS += device_node.o
_HEADERS += device_node.h

_OBJECTS += main.o

_HEADERS += state.h

#_OBJECTS += ticket_service.o
#_HEADERS += ticket_service.h

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