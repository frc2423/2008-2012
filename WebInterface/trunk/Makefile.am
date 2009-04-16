

AUTOMAKE_OPTIONS = 1.6 foreign

bin_PROGRAMS = webface

webface_LDFLAGS = $(BOOST_SYSTEM_LDFLAGS) $(BOOST_THREAD_LDFLAGS)

webface_LDADD = $(BOOST_SYSTEM_LIBS) $(BOOST_THREAD_LIBS)

webface_SOURCES = \
	main.cpp \
	src/DataProxyInfo.h \
	src/StaticDeleter.h \
	src/VariableProxy.h \
	src/VariableProxyFlags.h \
	src/WebInterface.cpp \
	src/WebInterface.h \
	src/server/connection.cpp \
	src/server/connection.hpp \
	src/server/connection_manager.cpp \
	src/server/connection_manager.hpp \
	src/server/header.hpp \
	src/server/mime_types.cpp \
	src/server/mime_types.hpp \
	src/server/reply.cpp \
	src/server/reply.hpp \
	src/server/request.hpp \
	src/server/request_handler.cpp \
	src/server/request_handler.hpp \
	src/server/request_parser.cpp \
	src/server/request_parser.hpp \
	src/server/server.cpp \
	src/server/server.hpp
	

