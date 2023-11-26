SRCDIR = src
STRUCTURE = $(shell cd $(SRCDIR) && find . -type d)

INCDIR = include
INCDIRNAME = homecontroller

CXX = g++
CXXFLAGS ?= -g -std=c++17 -fPIC -I$(INCDIR)

BINARYDIR = bin
OBJECTDIR = $(BINARYDIR)/obj

TARGETNAME = libhomecontroller.so
TARGET = $(BINARYDIR)/$(TARGETNAME)

LIBS += -lssl
LIBS += -lllhttp

LIBINSTALLDIR = /usr/local/lib
INCINSTALLDIR = /usr/local/include

# api/request
_OBJECTS += api/request/login_device_request.o
_HEADERS += api/request/login_device_request.h

_OBJECTS += api/request/request.o
_HEADERS += api/request/request.h

_OBJECTS += api/request/validate_request.o
_HEADERS += api/request/validate_request.h

# api/response
_OBJECTS += api/response/login_device_response.o
_HEADERS += api/response/login_device_response.h

_OBJECTS += api/response/response.o
_HEADERS += api/response/response.h

_OBJECTS += api/response/validate_device_response.o
_HEADERS += api/response/validate_device_response.h

_OBJECTS += api/response/validate_user_response.o
_HEADERS += api/response/validate_user_response.h

# api
_OBJECTS += api/client_packet.o
_HEADERS += api/client_packet.h

_OBJECTS += api/device.o
_HEADERS += api/device.h

_HEADERS += api/info.h

_OBJECTS += api/message.o
_HEADERS += api/message.h

_OBJECTS += api/request_maker.o
_HEADERS += api/request_maker.h

# http
_HEADERS += http/common.h

_OBJECTS += http/generic.o
_HEADERS += http/generic.h

_OBJECTS += http/parser.o
_HEADERS += http/parser.h

_OBJECTS += http/request.o
_HEADERS += http/request.h

_OBJECTS += http/response.o
_HEADERS += http/response.h

# json
_OBJECTS += json/document.o
_HEADERS += json/document.h

# net/ssl
_OBJECTS += net/ssl/client_connection.o
_HEADERS += net/ssl/client_connection.h

_OBJECTS += net/ssl/client.o
_HEADERS += net/ssl/client.h

_OBJECTS += net/ssl/connection.o
_HEADERS += net/ssl/connection.h

_OBJECTS += net/ssl/server_connection.o
_HEADERS += net/ssl/server_connection.h

_OBJECTS += net/ssl/server.o
_HEADERS += net/ssl/server.h

# net/ws
_OBJECTS += net/ws/client_wrapper.o
_HEADERS += net/ws/client_wrapper.h

_OBJECTS += net/ws/server_wrapper.o
_HEADERS += net/ws/server_wrapper.h

_OBJECTS += net/ws/wrapper.o
_HEADERS += net/ws/wrapper.h

# thread
_OBJECTS += thread/thread_pool.o
_HEADERS += thread/thread_pool.h

# util
_OBJECTS += util/bits.o
_HEADERS += util/bits.h

_HEADERS += util/base64.h

_OBJECTS += util/logger.o
_HEADERS += util/logger.h

_OBJECTS += util/str.o
_HEADERS += util/str.h

# root
_HEADERS += exception.h

OBJECTS = $(patsubst %,$(OBJECTDIR)/%,$(_OBJECTS))
HEADERS = $(patsubst %,$(INCDIR)/$(INCDIRNAME)/%,$(_HEADERS))

$(OBJECTDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJECTDIR)
	$(CXX) $(CPPFLAGS) -c -o $@ $< $(CXXFLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) -shared -o $@ $^ $(CXXFLAGS) $(LIBS)

$(OBJECTDIR):
	mkdir -p $(OBJECTDIR)
	mkdir -p $(addprefix $(OBJECTDIR)/,$(STRUCTURE))

install: $(TARGET)
	cp -r $(INCDIR)/$(INCDIRNAME) $(INCINSTALLDIR)
	cp $(TARGET) $(LIBINSTALLDIR)/$(TARGETNAME)

uninstall:
	rm -r $(INCINSTALLDIR)/$(INCDIRNAME)
	rm $(LIBINSTALLDIR)/$(TARGETNAME)

clean:
	rm -rf bin

.PHONY: clean install uninstall