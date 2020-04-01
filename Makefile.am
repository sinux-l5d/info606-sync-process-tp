## Process this file with automake to produce Makefile.in

SUBDIRS = col3-bibtp1

AM_CPPFLAGS = \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\"

AM_CFLAGS =\
	-Wall -Wextra\
	-fdiagnostics-show-option\
	-g\
	-pthread -Icol3-bibtp1
 
bin_PROGRAMS = col3_client_etu

col3_client_etu_SOURCES = \
	main.c \
	clientCOL3.c \
	clientCOL3.h

col3_client_etu_LDADD =  \
	-lncurses \
	-lrt \
	col3-bibtp1/libcol3tp1.a
