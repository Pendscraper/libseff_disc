all: output/tall

include ../common.mk

output/tall: output/new/tall output/orig/tall

output/new: | output
	mkdir $@

output/orig: | output
	mkdir $@

output/new/tall: tall.c $(LIBSEFF_LIB) | output/new
	$(CC) $(CFLAGS_LIBSEFF) $< -o $@ $(LDFLAGS_LIBSEFF)

output/orig/tall: tall_old.c $(LIBSEFF_OG_LIB) | output/orig
	$(CC) $(CFLAGS_LIBSEFF_OG) $< -o $@ $(LDFLAGS_LIBSEFF_OG)
	
