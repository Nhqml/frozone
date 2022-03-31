.PHONY: build echo

REGISTY_URL := registry.cri.epita.fr
REGISTRY_USERNAME := $$(grep "username" .env | cut -d':' -f2)
REGISTRY_PASSWORD := $$(grep "password" .env | cut -d':' -f2)
REGISTRY_BASE_IMAGE := $(REGISTY_URL)/kenji.gaillac/frozone

# Append more OSes here
OS_LIST := ubuntu ubuntu2104

login:
	@docker login -u ${REGISTRY_USERNAME} -p ${REGISTRY_PASSWORD} ${REGISTY_URL}

build: login
	@for os in ${OS_LIST}; do \
		ENV_BASE_IMG=${REGISTRY_BASE_IMAGE}/$$os/gcc; \
		docker build -f Dockerfile_"$$os"_gcc --cache-from $$ENV_BASE_IMG -t $$ENV_BASE_IMG:latest .; \
		docker push $$ENV_BASE_IMG; \
		done
