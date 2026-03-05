CPPFLAGS += -g -Wall -O0 -std=c++11
LDFLAGS += -fsanitize=address

PROGS = format-xml

all: $(PROGS)

format-xml: CPPFLAGS += `pkg-config --cflags libxml-2.0`
format-xml: CPPFLAGS += `pkg-config --libs libxml-2.0`
format-xml: format-xml.o
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

clean :
	$(RM) $(PROGS) *.o
