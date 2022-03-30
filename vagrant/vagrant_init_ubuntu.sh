#!/bin/sh

apt update
apt install -y linux-headers-$(uname -r) build-essential python3 python3-pip pkg-config libcunit1 libcunit1-doc libcunit1-dev

pip3 install meson && pip3 install ninja