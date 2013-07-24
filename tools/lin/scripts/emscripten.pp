$emscripten_deps = ["git", "openjdk-6-jdk", "nodejs", "freeglut3-dev", "mesa-common-dev", "apache2"] #, "xorg-dev", "libglu1-mesa-dev"]
$clang_version = "3.2"
$clang_dir = "clang+llvm-${clang_version}-x86-linux-ubuntu-12.04"
$clang_filename = "${clang_dir}.tar.gz"
$clang_url = "http://llvm.org/releases/${clang_version}/${clang_filename}"

Exec {
    user => "vagrant",
    cwd => "/vagrant",
    logoutput => on_failure,
    environment => ["PWD=/vagrant", "HOME=/home/vagrant"],
    path => [
        "/bin/",
        "/sbin/",
        "/usr/bin/",
        "/usr/sbin/",
        "/home/vagrant/src/emscripten",
        "/home/vagrant/src/clang+llvm-3.2-x86-linux-ubuntu-12.04/bin"
    ],
    timeout => 0
}

class emscripten {
    file { "/home/vagrant/.emscripten":
        owner => vagrant,
        group => vagrant,
        mode => 664,
        source => "/vagrant/tools/lin/share/dot.emscripten"
    }

    file { "/home/vagrant/.bash_aliases":
        owner => vagrant,
        group => vagrant,
        mode => 664,
        source => "/vagrant/tools/lin/share/dot.bashrc"
    }

    file { "/home/vagrant/src":
        owner => vagrant,
        group => vagrant,
        mode  => 775,
        recurse=> false,
        ensure => directory
    }

    exec { "add-apt-repository ppa:ubuntu-toolchain-r/test":
        alias => "add-gcc-ppa",
        cwd => "/root",
        user => "root"
    }

    exec { "/usr/bin/apt-get update":
        alias => "apt-get-update",
        cwd => "/root",
        user => "root",
        require => Exec["add-gcc-ppa"]
    }

    package {
      $emscripten_deps:
        ensure => "latest",
        require => Exec["apt-get-update"];

      "python-software-properties":
        ensure => "latest",
        require => Exec["apt-get-update"];

      ["gcc-4.8", "g++-4.8"]:
        ensure => "latest",
        require => Exec["apt-get-update"]
    }

    exec { "update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 20":
        alias => "install-gcc",
        cwd => "/root",
        user => "root",
        require => Package["gcc-4.8"],
        unless => "gcc --version | grep '4\\.8'"
    }

    exec { "update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 20":
        alias => "install-g++",
        cwd => "/root",
        user => "root",
        require => Package["g++-4.8"],
        unless => "g++ --version | grep '4\\.8'"
    }

    exec { "/usr/bin/git clone https://github.com/kripken/emscripten/":
        alias => "git-clone-emscripten",
        cwd => "/home/vagrant/src",
        require => Package[$emscripten_deps],
        creates => "/home/vagrant/src/emscripten"
    }

    exec { "/usr/bin/git pull origin master":
        cwd => "/home/vagrant/src/emscripten",
        alias => "git-pull-emscripten",
        require => Exec["git-clone-emscripten"]
    }

    exec { "/usr/bin/wget ${clang_url}":
        alias => "wget-clang-llvm",
        cwd => "/home/vagrant/src",
        creates => "/home/vagrant/src/${clang_filename}",
        environment => ["PWD=/home/vagrant/src", "HOME=/home/vagrant"],
    }

    exec { "/bin/tar -zxf ${clang_filename}":
        alias => "untar-clang-llvm",
        cwd => "/home/vagrant/src",
        environment => ["PWD=/home/vagrant/src", "HOME=/home/vagrant"],
        creates => "/home/vagrant/src/${clang_dir}",
        require => Exec["wget-clang-llvm"]
    }

    exec { "/usr/bin/rsync -r --filter=':- .gitignore' /vagrant/ /home/vagrant/src/minko-cpp":
        alias => "copy-repository",
        cwd => "/home/vagrant/src",
        environment => ["PWD=/home/vagrant/src", "HOME=/home/vagrant"]
    }

    file { "/var/www/sponza":
        alias => "link-apache",
        ensure => 'link',
        target => '/home/vagrant/src/minko-cpp/examples/sponza/bin/release/',
        require => [
            Package[$emscripten_deps],
            Exec["copy-repository"]
        ]
    }

    exec { "/home/vagrant/src/minko-cpp/tools/lin/bin/premake4 --platform=emscripten gmake":
        alias => "configure",
        logoutput => true,
        cwd => "/home/vagrant/src/minko-cpp",
        environment => ["PWD=/home/vagrant/src", "HOME=/home/vagrant"],
        require => [
            File["link-apache"],
            Exec["untar-clang-llvm"],
            Exec["git-pull-emscripten"],
            Exec["install-g++"],
            Exec["install-gcc"]
        ]
    }

    exec { "/usr/bin/make verbose=1 config=release":
        alias => "compile",
        logoutput => true,
        cwd => "/home/vagrant/src/minko-cpp",
        environment => ["PWD=/home/vagrant/src", "HOME=/home/vagrant"],
        require => Exec["configure"]
    }
}

group { "puppet":
  ensure => "present",
}

include emscripten
