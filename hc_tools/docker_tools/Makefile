DOCKER_REPO = homecontroller
DOCKER_IMAGE = libhomecontroller_build
DOCKER_IMAGE_VERSION = latest

DOCKER_CONTAINER = ${DOCKER_IMAGE}

LOCAL_SOURCE_PATH ?= ${CURDIR}/../..
DOCKER_SRC_PATH = /app

DOCKER_SHELL = bash
DOCKER_PORTS = 4434:4434

DOCKER_RUN_PARAMS = -d -it --init --rm \
					-p ${DOCKER_PORTS} \
					--name="${DOCKER_IMAGE}" \
					--workdir="${DOCKER_SRC_PATH}" \
					--mount type=bind,source=${LOCAL_SOURCE_PATH},target=${DOCKER_SRC_PATH} \
					${DOCKER_REPO}/${DOCKER_IMAGE}:${DOCKER_IMAGE_VERSION}

DOCKER_CONTAINER_RUNNING = $(shell docker container inspect -f '{{.State.Running}}' ${DOCKER_CONTAINER} 2>/dev/null)

build-image:
	docker build -t ${DOCKER_REPO}/${DOCKER_IMAGE}:latest .

run-image:
	@if [ "${DOCKER_CONTAINER_RUNNING}" != "true" ]; then \
		docker run ${DOCKER_RUN_PARAMS} \
		${DOCKER_SHELL}; \
	else \
		docker exec -it ${DOCKER_CONTAINER} \
		${DOCKER_SHELL}; \
	fi

.PHONY: build-image run-image
