all: output/wide output/thin output_old/wide output_old/thin

include ../common.mk

output_old:
	mkdir $@

output/wide: wide.c $(LIBSEFF_LIB) | output
	$(CC) $(CFLAGS_LIBSEFF) $< -o $@ $(LDFLAGS_LIBSEFF)

output_old/wide: wide_old.c $(LIBSEFF_OG_LIB) | output_old
	$(CC) $(CFLAGS_LIBSEFF_OG) $< -o $@ $(LDFLAGS_LIBSEFF_OG)

output/thin: thin.c $(LIBSEFF_LIB) | output
	$(CC) $(CFLAGS_LIBSEFF) $< -o $@ $(LDFLAGS_LIBSEFF)

output_old/thin: thin_old.c $(LIBSEFF_OG_LIB) | output_old
	$(CC) $(CFLAGS_LIBSEFF_OG) $< -o $@ $(LDFLAGS_LIBSEFF_OG)
