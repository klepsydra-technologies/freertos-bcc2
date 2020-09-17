FREERTOS=FreeRTOSv10.3.1/FreeRTOS
#FREERTOS=FreeRTOS
#FREERTOS=FreeRTOSv7.0.2
RTOS_SOURCE_DIR=$(CURDIR)/$(FREERTOS)/Source
RTOS_PORT_DIR  =$(RTOS_SOURCE_DIR)/portable/GCC/sparc-leon
RTOS_DEMO_DIR  =$(CURDIR)/$(FREERTOS)/Demo/Common
VPATH=$(RTOS_SOURCE_DIR):$(RTOS_SOURCE_DIR)/portable/MemMang:$(RTOS_PORT_DIR):$(RTOS_DEMO_DIR)/Minimal
MAKEMYSELF=Makefile
PREFIX?=/opt/FreeRTOSv10.3.1-bcc2

BCC_BIN_DIR=/opt/bcc-2.1.1-gcc/bin

CC       = ${BCC_BIN_DIR}/sparc-gaisler-elf-gcc
CFLAGS  += -g -I . -I $(RTOS_SOURCE_DIR)/include -I $(RTOS_PORT_DIR) -I $(RTOS_DEMO_DIR)/include \
		-mcpu=leon3 -qbsp=leon3 -pedantic -Wall -O3
#	  -D PACK_STRUCT_END=__attribute\(\(packed\)\) -D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) \
#	  -D__leonbare__

LDFLAGS = -L. -lfreertos -ldrv

INSTALLDIR=$(PREFIX)/lib/$(DESTDIR)/
LIBOBJ=libfreertos.a

all:	$(LIBOBJ)

$(LIBOBJ):  list.o   \
		queue.o  \
		tasks.o  \
		timers.o  \
		event_groups.o  \
		port.o   \
		portA.o  \
		heap_2.o \
		death.o
	${BCC_BIN_DIR}/sparc-gaisler-elf-ar cr $@ $^

ex1.exe: libfreertos.a
	${CC} $(CFLAGS) ex1.c -g $(LDFLAGS) -o ex1.exe ; ${BCC_BIN_DIR}/sparc-gaisler-elf-objdump -d -l -S ex1.exe  >  ex1.exe.dis

ex2.exe: libfreertos.a
	${CC} $(CFLAGS) ex2.c -g $(LDFLAGS) -o ex2.exe ; ${BCC_BIN_DIR}/sparc-gaisler-elf-objdump -d -l -S ex2.exe  >  ex2.exe.dis

clean:
	-rm *.o *.a *.exe *.dis

#examples: ex1.exe ex2.exe
examples: ex1.exe

multi-do:
	compiler="${CC}"; \
	  for i in `$${compiler} --print-multi-lib 2>/dev/null`; do \
	    dir=`echo $$i | sed -e 's/;.*$$//'`; \
	    if [ "$${dir}" = "." ]; then \
	      true; \
	    else \
	        flags=`echo $$i | sed -e 's/^[^;]*;//' -e 's/@/ -/g'`; \
		$(MAKE) -f $(MAKEMYSELF) $(FLAGS_TO_PASS) \
				CFLAGS="$(CFLAGS) $${flags}" \
				ASFLAGS="$${flags}" \
				FCFLAGS="$(FCFLAGS) $${flags}" \
				FFLAGS="$(FFLAGS) $${flags}" \
				ADAFLAGS="$(ADAFLAGS) $${flags}" \
				prefix="$(prefix)" \
				PREFIX="$(PREFIX)" \
				exec_prefix="$(exec_prefix)" \
				GCJFLAGS="$(GCJFLAGS) $${flags}" \
				CXXFLAGS="$(CXXFLAGS) $${flags}" \
				LIBCFLAGS="$(LIBCFLAGS) $${flags}" \
				LIBCXXFLAGS="$(LIBCXXFLAGS) $${flags}" \
				LDFLAGS="$(LDFLAGS) $${flags}" \
				MULTIFLAGS="$${flags}" \
				DESTDIR="$${dir}" \
				INSTALL="$(INSTALL)" \
				INSTALL_DATA="$(INSTALL_DATA)" \
				INSTALL_PROGRAM="$(INSTALL_PROGRAM)" \
				INSTALL_SCRIPT="$(INSTALL_SCRIPT)" \
				$(DO)  || exit 1;\
	    fi; \
	  done; \

multi-all: examples

multi-install:
	mkdir -p $(INSTALLDIR)
	install $(LIBOBJ) $(INSTALLDIR)

multido-compile:
	make -f $(MAKEMYSELF) PREFIX=$(PREFIX) multi-do DO="clean multi-all multi-install"
	make -f $(MAKEMYSELF) PREFIX=$(PREFIX) clean multi-all multi-install

install-headers:
	-mkdir -p $(PREFIX)/include/freertos
	for f in `cd $(FREERTOS)/Source/include; ls`; do \
		cp $(FREERTOS)/Source/include/$$f $(PREFIX)/include/freertos/; \
	done
	cp FreeRTOSConfig.h $(PREFIX)/include/freertos/;
	cp $(FREERTOS)/Source/portable/GCC/sparc-leon/portmacro.h $(PREFIX)/include/freertos/;
	cp $(FREERTOS)/Source/portable/GCC/sparc-leon/leonstack.h $(PREFIX)/include/freertos/;

recompile:
	make PREFIX=$(PREFIX) install-headers
	make PREFIX=$(PREFIX) multido-compile
