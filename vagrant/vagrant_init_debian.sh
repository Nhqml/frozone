#!/bin/sh

KERNEL_VERSION="5.10.0-12"

apt update
apt install -y "linux-image-$KERNEL_VERSION-amd64" "linux-headers-$KERNEL_VERSION-amd64" build-essential
