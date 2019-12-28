# TODO ...

# calling directory without trailing /
export PROJECT_DIR := $(patsubst %/,%, $(dir $(realpath ${MAKEFILE_LIST})))
export PROJECT_NAME:= $(patsubst $(dir ${PROJECT_DIR})%,%, ${PROJECT_DIR})
export MAIN_SOURCE := ${PROJECT_NAME}.ino

ifneq "" "$(wildcard ${PROJECT_DIR}/sketch.json))"
	export BOARD ?= $(shell jq -r .cpu.fqbn < ${PROJECT_DIR}/sketch.json)
endif
ifeq "" "${BOARD}"
	$(error "BOARD variable must be defined"
endif
export BOARD_DOT = $(subst :,.,${BOARD})

export PORT ?= $(shell arduino-cli board list --format json | jq -r \
  "map({ a:.address, f: .boards | map(.FQBN)[] | select(.==\"${BOARD}\") }) | map(.a)[]" )

$(info "${PROJECT_DIR} / ${PROJECT_NAME} / ${MAIN_SOURCE} / ${BOARD} / ${BOARD_DOT} / ${PORT}")


BIN=${PROJECT_NAME}.${BOARD_DOT}.elf

all: upload

upload : ${BIN}
	arduino-cli upload -b ${BOARD} -p ${PORT} ${PROJECT_DIR}

compile : ${BIN}

# TODO : how to depend to library changes or other .h/.c files ?
# => gcc -option_to_deduce_dependencies
${BIN} : ${MAIN_SOURCE}
	arduino-cli compile -b ${BOARD} ${PROJECT_DIR}

console:
	light version of arddude ???
	or rewrite arddude to call arduino-cli to get ports and launch uploads ?
	