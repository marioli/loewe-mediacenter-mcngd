## MAKEFILE MCNGD

#GEN_CFLAGS=-W -Wall -O2
GEN_CFLAGS=-W -Wall -Wextra -O0 -g

BUILD_NUMBER_LDFLAGS = -Xlinker --defsym -Xlinker __BUILD_DATE=$$(date +'%Y%m%d')
## legacy git
BUILD_NUMBER_LDFLAGS+= -Xlinker --defsym -Xlinker __BUILD_HASH=$$(git log -1 --pretty=format:'0x%h')
## later git versions
#BUILD_NUMBER_LDFLAGS+= -Xlinker --defsym -Xlinker __BUILD_HASH=$$(git log -1 --format='0x%h')

SSL_LIBS = `pkg-config --libs libssl`

JANSSON_LIBS = `pkg-config --libs jansson`
JANSSON_CFLAGS =  `pkg-config --cflags jansson`

XML_LIBS = `xml2-config --libs`
XML_CFLAGS =  `xml2-config --cflags`

#CURL_LIBS = `pkg-config --libs libcurl`
#CURL_CFLAGS = `pkg-config --cflags libcurl`

MB5_LIBS = `pkg-config --libs libmusicbrainz5`
MB5_CFLAGS = `pkg-config --cflags libmusicbrainz5`

DISCID_LIBS = `pkg-config --libs libdiscid`
DISCID_CFLAGS = `pkg-config --cflags libdiscid`

MCNGD_OBJ = mcng-buffer.o  mcng-config.o  mcng-log.o  mcng-musicid.o  mcng-xml.o  mcng.o  mcngd.o mcng-event.o
MCNGD_LIBS = $(XML_LIBS) $(JANSSON_LIBS) $(SSL_LIBS) $(MB5_LIBS) $(DISCID_LIBS)

MCNGC_OBJ = mcng.o
MCNGC_LIBS = 

CFLAGS = $(GEN_CFLAGS) $(XML_CFLAGS) $(JANSSON_CFLAGS) $(MB5_CFLAGS) $(DISCID_CFLAGS)

all: mcngd mcng

mcngd: $(MCNGD_OBJ)
	gcc -o mcngd $(MCNGD_OBJ) $(MCNGD_LIBS) $(BUILD_NUMBER_LDFLAGS)

mcng: $(MCNGC_OBJ)
	gcc -o mcng $(MCNGC_OBJ) $(MCNGC_LIBS) $(BUILD_NUMBER_LDFLAGS)

clean:
	rm -f *.o
	rm -f mcngd
	rm -f mcng

install: all
	install -m 755 mcngd /usr/sbin
	install -m 755 mcng /usr/bin


deb: all
	sudo ./mk_pkg.sh

