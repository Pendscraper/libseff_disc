all: output/tall

include ../common.mk

output/tall: output/new/tall output/orig/tall

output/new:
	mkdir $@

output/orig:
	mkdir $@

output/new/tall: tall.c $(LIBSEFF_LIB) | output output/new
	$(CC) $(CFLAGS_LIBSEFF) $< -o $@ $(LDFLAGS_LIBSEFF)

output/orig/tall: tall.c $(LIBSEFF_OG_LIB) | output output/orig
	$(CC) $(CFLAGS_LIBSEFF) $< -o $@ $(LDFLAGS_LIBSEFF)
	
