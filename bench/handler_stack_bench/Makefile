all: output/wide output/thin output/mid output/real output/massive output/local

include ../common.mk

output/%: %.c $(LIBSEFF_LIB) | output
	$(CC) $(CFLAGS_LIBSEFF) $< -o $@ $(LDFLAGS_LIBSEFF)
