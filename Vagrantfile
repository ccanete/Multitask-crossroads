VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.box = "fedora18_x64_nocm"
  config.vm.box_url = "./fedora-18-x64-vbox4210-nocm.box"
  config.vm.synced_folder "./..", "/share/public/tp/tp-multitache", create: true
  config.vm.provision "shell" do |s|
    s.path = "provision/setup.sh"
  end
end
