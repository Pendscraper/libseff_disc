LIBSEFF     := ../../
LIBSEFF_H   := $(LIBSEFF)/src
LIBSEFF_LIB := $(LIBSEFF)/output/lib

BUILD = release

LIBSEFF_OLD     := ../../bench/libseff
LIBSEFF_OLD_H   := $(LIBSEFF_OLD)/src
LIBSEFF_OLD_LIB := $(LIBSEFF_OLD)/output/lib

LIBSEFF_GOAT := ../../bench/libseff_goat

NEW_BENCH_LOC := libseff_bench_from_internet
OLD_BENCH_LOC := libseff_bench_old

CC               := clang-16
CC_COMPILE_FLAGS := -std=gnu99 -O3 -I$(LIBSEFF_H) -fsplit-stack
CC_COMPILE_FLAGS_OLD := -std=gnu99 -O3 -I$(LIBSEFF_OLD_H) -fsplit-stack
CC_WARN_FLAGS    := -Wall -Wextra \
									  -Wformat=2 -Wno-unused-parameter -Wshadow \
										-Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
										-Wredundant-decls -Wnested-externs -Wmissing-include-dirs
CC_LINK_FLAGS    := -std=gnu99 -O3

LD         := $(shell which ld.gold)
LD_FLAGS   := -fuse-ld=$(LD) -l:libseff.a -l:libutils.a -L$(LIBSEFF_LIB)
LD_FLAGS_OLD   := -fuse-ld=$(LD) -l:libseff.a -l:libutils.a -L$(LIBSEFF_OLD_LIB)

compile_cmd = $(CC)	$(CC_COMPILE_FLAGS) $(CC_WARN_FLAGS) -o main.o -c main.c
link_cmd    = $(CC) $(CC_LINK_FLAGS) -o main main.o $(LD_FLAGS)
build_cmd   = cd $(NEW_BENCH_LOC)/$(1) ; $(call compile_cmd, $(1)); $(call link_cmd, $(1))
test_cmd    = cd $(NEW_BENCH_LOC)/$(1) ; ./main $(2) > actual ; echo $(3) > expected ; diff expected actual

compile_cmd_old = $(CC)	$(CC_COMPILE_FLAGS_OLD) $(CC_WARN_FLAGS) -o main.o -c main.c
link_cmd_old    = $(CC) $(CC_LINK_FLAGS) -o main main.o $(LD_FLAGS_OLD)
build_cmd_old   = cd $(OLD_BENCH_LOC)/$(1) ; $(call compile_cmd_old, $(1)); $(call link_cmd_old, $(1))
test_cmd_old    = cd $(OLD_BENCH_LOC)/$(1) ; ./main $(2) > actual ; echo $(3) > expected ; diff expected actual

all: build

bench:
	$(MAKE) BUILD=${BUILD} -C $(LIBSEFF) clean
	$(MAKE) BUILD=${BUILD} -C $(LIBSEFF) bench
	$(MAKE) BUILD=${BUILD} -C $(LIBSEFF_OLD) clean
	$(MAKE) BUILD=${BUILD} -C $(LIBSEFF_OLD) bench
	$(MAKE) BUILD=${BUILD} -C $(LIBSEFF_GOAT) clean
	$(MAKE) BUILD=${BUILD} -C $(LIBSEFF_GOAT) bench
	-rm $(LIBSEFF)/bench/*_bench/output/*.o
	-rm $(LIBSEFF_OLD)/bench/*_bench/output/*.o
	-rm $(LIBSEFF_GOAT)/bench/*_bench/output/*.o
	hyperfine --warmup 3 -N -m 20 --export-json resultsNew.json --export-csv resultsNew.csv \
					'$(LIBSEFF_GOAT)/bench'/*_bench/output/* 		\
					'$(LIBSEFF)/bench'/*_bench/output/* 			\
					'$(LIBSEFF_OLD)/bench'/*_bench/output/*
nuh:
	hyperfine -N -m 50 --warmup 5 --export-csv results67.csv \
					'$(NEW_BENCH_LOC)/countdown/main 200000000' \
					'$(NEW_BENCH_LOC)/product_early/main 100000' \
					'$(NEW_BENCH_LOC)/iterator/main 40000000' \
					'$(NEW_BENCH_LOC)/generator/main 25' \
					'$(NEW_BENCH_LOC)/parsing_dollars/main 20000' \
					'$(NEW_BENCH_LOC)/resume_nontail/main 20000' \
					'$(NEW_BENCH_LOC)/handler_sieve/main 60000'         \
					'$(OLD_BENCH_LOC)/countdown/main 200000000' \
					'$(OLD_BENCH_LOC)/product_early/main 100000' \
					'$(OLD_BENCH_LOC)/iterator/main 40000000' \
					'$(OLD_BENCH_LOC)/generator/main 25' \
					'$(OLD_BENCH_LOC)/parsing_dollars/main 20000' \
					'$(OLD_BENCH_LOC)/resume_nontail/main 20000' \
					'$(OLD_BENCH_LOC)/handler_sieve/main 60000'

build:
	$(call build_cmd,countdown)
	$(call build_cmd,fibonacci_recursive)
	$(call build_cmd,product_early)
	$(call build_cmd,iterator)
	$(call build_cmd,generator)
	$(call build_cmd,parsing_dollars)
	$(call build_cmd,resume_nontail)
	$(call build_cmd,handler_sieve)
	$(call build_cmd_old,countdown)
	$(call build_cmd_old,fibonacci_recursive)
	$(call build_cmd_old,product_early)
	$(call build_cmd_old,iterator)
	$(call build_cmd_old,generator)
	$(call build_cmd_old,parsing_dollars)
	$(call build_cmd_old,resume_nontail)
	$(call build_cmd_old,handler_sieve)
	
	

clean:
	-rm */main
	-rm */expected
	-rm */actual
	-rm */*.o
	-rm */*.out
