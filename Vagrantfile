# -*- mode: ruby -*-
# vi: set ft=ruby :

ENV["VAGRANT_EXPERIMENTAL"]="disks"

Vagrant.configure("2") do |config|

  config.vm.define :ubuntu do |ubuntu|
    ubuntu.vm.box = "generic/ubuntu2004"
    ubuntu.vm.provision "shell", path: "vagrant/vagrant_init_ubuntu.sh"
  end

  config.vm.define :debian do |debian|
    debian.vm.box = "generic/debian11"
    debian.vm.provision "shell", path: "vagrant/vagrant_init_debian.sh"
    debian.vm.provision :reload
  end

  # Default disk size for vmware is 128GB
  config.vm.provider "vmware_workstation" do |v|
    v.memory = "4096"
  end

  # Works only for virtualbox
  config.vm.disk :disk, size: "20GB", primary: true
  config.vm.provider "virtualbox" do |v|
    v.memory = "4096"
    v.cpus = "2"
  end

  # Default disk size for libvirt is 128GB
  config.vm.provider "libvirt" do |v|
    v.memory = "4096"
    v.cpus = "2"
    v.vm.synced_folder ".", "/vagrant", type: "nfs", nfs_udp: false, nfs_version: 4
  end

  config.vm.synced_folder ".", "/vagrant"
end
