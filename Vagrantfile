#!/usr/bin/env ruby

Vagrant.configure("2") do |config|
  # Ubuntu 13.10 64-bit: http://cloud-images.ubuntu.com/vagrant/saucy/current/saucy-server-cloudimg-amd64-vagrant-disk1.box
  # Ubuntu 13.10 32-bit: http://cloud-images.ubuntu.com/vagrant/saucy/current/saucy-server-cloudimg-i386-vagrant-disk1.box
  # Ubuntu 12.04 64-bit: http://cloud-images.ubuntu.com/vagrant/precise/current/precise-server-cloudimg-amd64-vagrant-disk1.box
  # Ubuntu 12.04 32-bit: http://cloud-images.ubuntu.com/vagrant/precise/current/precise-server-cloudimg-i386-vagrant-disk1.box

  config.vm.box = "ubuntu-1310-amd64"
  config.vm.box_url = "http://cloud-images.ubuntu.com/vagrant/saucy/current/saucy-server-cloudimg-amd64-vagrant-disk1.box"

  config.vm.network :forwarded_port, guest: 80, host: 8080
  config.vm.hostname = "vagrant.minko.io"

  # Boot with a GUI so you can see the screen. (Default is headless)
  # config.vm.provider "virtualbox" do |v|
  #   v.gui = true
  # end

  config.vm.provider "virtualbox" do |v|
    v.memory = 2048
  end

  config.vm.provision :puppet do |puppet|
    puppet.manifests_path = "tools/lin/scripts/puppet/manifests"
    puppet.module_path = "tools/lin/scripts/puppet/modules" 
  end
end