## Process this file with automake to produce Makefile.in


AM_CPPFLAGS = \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\"

AM_CFLAGS =\
	-Wall -Wextra\
	-fdiagnostics-show-option\
	-g\
	-pthread -I ./col3-bib/
 
bin_PROGRAMS = col3_client_etu

col3_client_etu_SOURCES = \
	main.c

col3_client_etu_LDADD =  \
	-lncurses \
	-lrt \
	-L ./col3-bib/ -lcol3
