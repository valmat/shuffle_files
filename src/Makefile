# CPP   = g++
# LD    = g++
CPP   = clang++
LD    = clang++

CPP_STD  = -std=c++20

OPT_FLAG =  -pthread -pedantic -Wall -Wextra -fno-rtti -Wsequence-point

INCLUDES   = -I.                            \
             -I"../deps/argagg/include"

# -fsanitize=address` включает AddressSanitizer, который помогает обнаруживать ошибки работы с памятью,
# 	такие как выход за границы массива и использование после освобождения.
# -fsanitize=undefined` включает UndefinedBehaviorSanitizer, который обнаруживает неопределенное поведение
# -fsanitize=leak` включает LeakSanitizer, который обнаруживает утечки памяти. Он автоматически включается 
# 	с AddressSanitizer в Clang.
# -fsanitize=thread` (ThreadSanitizer для обнаружения гонок данных) не может быть использован одновременно
# 	с `-fsanitize=address` из-за конфликта в реализации.

SAN_CPP_FLAGS  = -fsanitize=address,undefined,leak
# SAN_CPP_FLAGS  = -fsanitize=thread
SAN_LD_FLAGS  = ${SAN_CPP_FLAGS}

CPP_FLAGS  = ${CPP_STD} -O0 -g  ${OPT_FLAG} ${SAN_CPP_FLAGS} ${INCLUDES} -DDEBUG_BUILD -c
LD_FLAGS   = ${OPT_FLAG} ${SAN_LD_FLAGS}


STRIPBIN = true

version_color= \033[1;33m

ifeq ($(BUILD),release)   
    CPP   = g++
    LD    = g++
    CPP_STD  = -std=c++2a

	CPP_FLAGS  = ${CPP_STD} -O3 ${OPT_FLAG} ${INCLUDES} -flto=auto -DNDEBUG -c
	LD_FLAGS   = ${CPP_STD} ${OPT_FLAG} -Wl,--no-as-needed -flto=auto

	STRIPBIN = strip

    version_color = \033[1;32m
endif

############################################################
DEFAULT_VERSION=v0.0.0
version=$(shell git -C "." describe --tags --candidates=1 2>/dev/null || echo ${DEFAULT_VERSION})

.PHONY: clean release


SOURCES   := $(shell find -L . -mindepth 2 -type f -name '*.cpp')
OBJECTS   := $(patsubst ./%.cpp,oc/%.o,$(SOURCES))

EXECS_SRC  = $(shell find -L . -maxdepth 1 -type f -name '*.cpp')
EXECS_OBJ := $(patsubst ./%.cpp,oc/%.o,$(EXECS_SRC))
EXECS_BIN := $(patsubst oc/%.o,bin/%.bin,$(EXECS_OBJ))


all: $(EXECS_BIN)
	@echo
	@echo "\033[1;36mBuild version ${version_color}${version}\033[1;36m complite \033[0m"

# Compiling
ADD_FLAGS :=-DVER="\"${version}\""

${OBJECTS} : oc/%.o : ./%.cpp
	@mkdir -p $(dir $(@))
	${CPP} ${CPP_FLAGS} ${ADD_FLAGS} $< -o $@

${EXECS_OBJ} : oc/%.o : ./%.cpp
	@mkdir -p $(dir $(@))
	${CPP} ${CPP_FLAGS} ${ADD_FLAGS} $< -o $@

# Linking
LIBS := -lpthread

$(EXECS_BIN) : bin/%.bin : oc/%.o $(OBJECTS)
	@echo ""
	${LD} ${LD_FLAGS} $< $(OBJECTS) ${LIBS} -o $@
	@$(STRIPBIN) $@
	@echo ""

# make release version
release:
	make "BUILD=release" -j`nproc`

clean:
	find "oc"  -type f -name '*.o'   -delete
	find "bin" -type f -name '*.bin' -delete
	@echo "\033[0;33mClean complite \033[0m"
