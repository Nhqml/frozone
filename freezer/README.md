# Frozone - freezer module

This folder contains all files related to the freezer module, a linux kernel module used to "freeze" certain components such as the creation of processes, new connections / sockets, new users, etc...

## Usage

To compile the module :
```bash
make
```

To load the module :
```bash
sudo insmod freezer_module.ko
```

To unload the module :
```bash
sudo rmmod freezer_module
```

Interaction with the module : WIP

## Development

To dev and test the kernel locally, Vagrant is recommended (automated VM setup).

Requirements:

- VM provider (Virtualbox recommended / VMWare Workstation)
- Vagrant (works on all OS)

Inside this folder, up the Vagrant box :
```bash
vagrant up
```

Then ssh inside with :
```bash
vagrant ssh
```

To see the output of the module :
```bash
dmesg -wH | grep "freezer"
```

The current folder is synced inside the vagrant box at `/vagrant`.
So you can just edit files from your host filesystem, and it will be synced inside.
