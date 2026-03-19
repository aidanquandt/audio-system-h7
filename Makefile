.PHONY: all build codegen flash rebuild clean build-and-flash configure help

all: build

help:
	@echo "Available targets:"
	@echo "  codegen          Generate messaging_protocol.pb.c/.h from proto"
	@echo "  build            Build both CM4 and CM7"
	@echo "  flash            Flash both cores"
	@echo "  build-and-flash  Build then flash"
	@echo "  rebuild          Clean, configure, and build"
	@echo "  clean            Remove build artifacts"
	@echo "  configure        Run CMake configuration"

codegen:
	./scripts/gen-proto.sh

build: codegen
	./scripts/build.sh

flash:
	./scripts/flash.sh

rebuild:
	./scripts/rebuild.sh

clean:
	./scripts/clean.sh

build-and-flash:
	./scripts/build-and-flash.sh

configure:
	./scripts/configure.sh
