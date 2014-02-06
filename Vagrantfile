#!/usr/bin/env ruby

Vagrant.configure("2") do |config|
  # config.vm.box = "ubuntu-1204-cloud-i386"
  # config.vm.box_url = "http://cloud-images.ubuntu.com/precise/current/precise-server-cloudimg-vagrant-i386-disk1.box"
  config.vm.box = "ubuntu-1204-cloud-amd64"
  config.vm.box_url = "http://cloud-images.ubuntu.com/vagrant/precise/current/precise-server-cloudimg-amd64-vagrant-disk1.box"
  config.vm.network :forwarded_port, guest: 80, host: 8080
  config.vm.hostname = "vagrant.minko.io"

  # Boot with a GUI so you can see the screen. (Default is headless)
  # config.vm.provider "virtualbox" do |v|
  #   v.gui = true
  # end

  config.vm.provision :puppet do |puppet|
    puppet.manifests_path = "tools/lin/share"
    puppet.manifest_file = "minko.pp"
  end
end