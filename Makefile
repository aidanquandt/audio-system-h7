.PHONY: all build flash rebuild clean build-and-flash configure codegen help host

all: build

# Serial port for host (override: make host PORT=COM3)
PORT ?= COM5

help:
	@echo "Available targets:"
	@echo "  build            Build both CM4 and CM7"
	@echo "  flash            Flash both cores"
	@echo "  build-and-flash  Build then flash"
	@echo "  rebuild          Clean, configure, and build"
	@echo "  clean            Remove build artifacts"
	@echo "  configure        Run CMake configuration"
	@echo "  codegen          Regenerate RPC code from proto/messages.yaml"
	@echo "  host             Run app host (web UI at http://127.0.0.1:5000, PORT=$(PORT))"

build:
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

codegen:
	./scripts/codegen.sh

host:
	python tools/host/app.py $(PORT)
