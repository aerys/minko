$apt_deps = [
    "git",
    "cmake",
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
    "xvfb",
    "gcc-4.8",
    "g++-4.8"
]

Exec {
    user => "vagrant",
    cwd => "/vagrant",
    logoutput => on_failure,
    environment => ["PWD=/vagrant", "HOME=/home/vagrant"],
    path => [
        "/bin/",
        "/sbin/",
        "/usr/bin/",
        "/usr/sbin/"
    ],
    timeout => 0
}

class software {

    package {
      $apt_deps:
        ensure => "latest",
        require => Exec["apt-get-update"];
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

    exec { "/usr/bin/rsync -r --filter=':- .gitignore' /vagrant/ /home/vagrant/src/minko-cpp":
        alias => "copy-repository",
        cwd => "/home/vagrant/src",
        environment => ["PWD=/home/vagrant/src", "HOME=/home/vagrant"]
    }

    exec { "/home/vagrant/src/minko-cpp/tools/lin/bin/premake4 --no-glsl-optimizer gmake":
        alias => "configure",
        logoutput => true,
        cwd => "/home/vagrant/src/minko-cpp",
        environment => ["PWD=/home/vagrant/src", "HOME=/home/vagrant"],
        require => [
            Exec["copy-repository"],
            Exec["install-g++"],
            Exec["install-gcc"]
        ]
    }

    exec { "/usr/bin/make verbose=1 config=debug":
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

include software
