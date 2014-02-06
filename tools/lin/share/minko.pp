$deps = [
    "git",
    "cmake",
    "openjdk-6-jdk",
    "nodejs", 
    "libsdl2",
    "freeglut3-dev",
    "mesa-common-dev",
    "mesa-utils",
    "libosmesa6-dev",
    "xorg-dev",
    "libglu1-mesa-dev",
    "libgl1-mesa-swx11-dev",
    "x11proto-xf86vidmode-dev",
    "libxrandr-dev",
    "libxxf86vm-dev",
    "libxi-dev",
    "libudev-dev",
    "xvfb",
    "apache2",
    "php5",
    "libapache2-mod-php5",
    "gcc-4.8",
    "g++-4.8"
]

$clang_version = "3.2"
$clang_dir = "clang+llvm-${clang_version}-x86_64-linux-ubuntu-12.04"
$clang_filename = "${clang_dir}.tar.gz"
$clang_url = "http://llvm.org/releases/${clang_version}/${clang_filename}"
$clang_home = "/home/vagrant/src/clang+llvm"
$minko_home = "/home/vagrant/src/minko-cpp"
$emscripten_home = "/home/vagrant/src/emscripten"

Exec {
    user => "vagrant",
    cwd => "/vagrant",
    environment => ["PWD=/vagrant", "HOME=/home/vagrant"],
    logoutput => on_failure,
    path => [
        "/bin/",
        "/sbin/",
        "/usr/bin/",
        "/usr/sbin/",
        "${emscripten_home}",
        "${clang_home}/bin"
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

    file { $clang_home:
        ensure => "link",
        target => "/home/vagrant/src/${clang_dir}",
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

    exec { "add-apt-repository ppa:chris-lea/node.js":
        alias => "add-node-ppa",
        cwd => "/root",
        user => "root"
    }
    exec { "add-apt-repository ppa:zoogie/sdl2-snapshots":
        alias => "add-sdl2-ppa",
        cwd => "/root",
        user => "root"
    }

    exec { "apt-get update":
        alias => "apt-get-update",
        cwd => "/root",
        user => "root",
        require => Exec["add-gcc-ppa", "add-node-ppa", "add-sdl2-ppa"]
    }

    package {
      $deps:
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
        require => Package[$deps],
        creates => $emscripten_home
    }

    exec { "/usr/bin/git pull origin master":
        cwd => $emscripten_home,
        alias => "git-pull-emscripten",
        require => Exec["git-clone-emscripten"]
    }

    exec { "/bin/bash configure":
        cwd => "${emscripten_home}/third_party/lzma.js/lzip",
        alias => "emscripten-configure-lzma",
        require => Exec["git-pull-emscripten"]
    }

    exec { "/bin/bash doit.sh":
        cwd => "${emscripten_home}/third_party/lzma.js",
        alias => "emscripten-build-lzma",
        require => Exec["emscripten-configure-lzma"],
        creates =>"${emscripten_home}/third_party/lzma.js/lzma-native"
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

    exec { "/usr/bin/rsync --delete --ignore-errors -l -p -c -r --filter=':- .gitignore' /vagrant/ ${MINKO_HOME}":
        alias => "copy-repository",
        cwd => "/home/vagrant/src",
        environment => ["PWD=/home/vagrant/src", "HOME=/home/vagrant"]
    }

    file { "/var/www/sponza":
        alias => 'link-apache-sponza',
        ensure => 'link',
        target => "${minko_home}/examples/sponza/bin/release/",
        require => [
            Package[$deps],
            Exec["copy-repository"]
        ]
    }
    
    file { "/var/www/cube":
        alias => 'link-apache-cube',
        ensure => 'link',
        target => "${minko_home}/examples/cube/bin/release/",
        require => [
            Package[$deps],
            Exec["copy-repository"]
        ]
    }
    
    file { "/var/www/pulse":
        alias => 'link-apache-pulse',
        ensure => 'link',
        target => "${minko_home}/projects/ogilvy-babolat/pulse/www/",
        require => [
            Package[$deps],
            Exec["copy-repository"]
        ]
    }

    exec { "${minko_home}/tools/all/bin/premake5.sh gmake":
        alias => "configure",
        logoutput => true,
        cwd => $minko_home,
        environment => ["PWD=/home/vagrant/src", "HOME=/home/vagrant", "MINKO_HOME=${minko_home}", "EMSCRIPTEN_HOME=${emscripten_home}"],
        require => [
            File["link-apache-cube"],
            File["link-apache-pulse"],
            File["link-apache-sponza"],
            Exec["untar-clang-llvm"],
            Exec["git-pull-emscripten"],
            Exec["emscripten-build-lzma"],
            Exec["install-g++"],
            Exec["install-gcc"]
        ]
    }

    exec { "/usr/bin/make verbose=1 config=release":
        alias => "compile",
        logoutput => true,
        cwd => $minko_home,
        environment => ["PWD=/home/vagrant/src", "HOME=/home/vagrant", "MINKO_HOME=${minko_home}"],
        require => Exec["configure"]
    }
}

group { "puppet":
  ensure => "present",
}

include emscripten
