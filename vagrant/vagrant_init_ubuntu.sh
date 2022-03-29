#!/bin/sh

apt update
apt install -y linux-headers-$(uname -r) build-essential
