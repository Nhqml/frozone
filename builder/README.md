# pfa-freezer-builder

## Description

A repo that contains scripts to build docker images used by the [frozone](https://gitlab.cri.epita.fr/kenji.gaillac/frozone)'s project pipeline.


Scripts are used to build docker images locally, then pushed to cri's gitlab registry, at registry.cri.epita.fr

## Why

Shared Docker runners from EPITA's gitlab do not have access to the internet, which does not allow to `apt update` nor `apt install`, thus removing the ability to build docker images using the `docker-in-docker` image from the CI/CD.

So we have to build them locally, then push them on the registry.

## Build

- Fill the `.env` file with your own docker credentials (username, and the password is a personal access token).

- Build your docker images with `make build`

## Using the images

Images are then pushed to `registry.cri.epita.fr/kenji.gaillac/frozone/<os>/gcc`.
You can use these images from the .gitlab-ci.yml for your jobs.

## Add more OSes to the list

- Add another Dockerfile named `Dockerfile_<os>_gcc`
- Add the os in the Makefile in the variable `OS_LIST`
