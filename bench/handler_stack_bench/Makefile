all: output/tall

include ../common.mk

output/%: %.c $(LIBSEFF_LIB) | output
	$(CC) $(CFLAGS_LIBSEFF) $< -o $@ $(LDFLAGS_LIBSEFF)
