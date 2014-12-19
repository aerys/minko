#!/usr/bin/env ruby

Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu-trusty-64-current"
  config.vm.box_url = "https://cloud-images.ubuntu.com/vagrant/trusty/current/trusty-server-cloudimg-amd64-vagrant-disk1.box"

  config.vm.hostname = "vagrant.minko.io"

  config.vm.provider "virtualbox" do |v|
    v.memory = 2048
  end

  # Windows users: if you encounter permission/symlink issues, please uncomment the following line:
  # config.vm.synced_folder ".", "/vagrant", type: "rsync", rsync__args: ["--progress", "--perms", "--archive", "-z", "--filter=:- .gitignore"], rsync__excludes: [".git/"]

  config.vm.provision :puppet do |puppet|
    puppet.manifests_path = "tool/lin/script"
    puppet.manifest_file = "install_offscreen.pp"
  end
end
