class minko {
    $minko_home = "/opt/minko"

    $deps = [
        # "git",
        # "cmake",
        # "openjdk-6-jdk",
        # "nodejs", 
        # "freeglut3-dev",
        # "mesa-common-dev",
        # "mesa-utils",
        # "libosmesa6-dev",
        # "xorg-dev",
        # "libglu1-mesa-dev",
        # "libgl1-mesa-swx11-dev",
        # "x11proto-xf86vidmode-dev",
        # "libxrandr-dev",
        # "libxxf86vm-dev",
        # "libxi-dev",
        # "libudev-dev",
        # "xvfb",
        # "apache2",
        # "php5",
        # "libapache2-mod-php5",

        # HTTP
        "libcurl4-openssl-dev",
        
        # Oculus
        "libudev-dev",
        "libxinerama-dev",

        # Compilers
        "gcc-4.8",
        "g++-4.8",

        # OpenGL
        "libgl1-mesa-dev",
        "libglu1-mesa-dev",
        "libglew-dev",
        "libsdl2-dev"
    ]

    Exec {
        user => "vagrant",
        cwd => "/vagrant",
        environment => ["PWD=/vagrant", "HOME=/home/vagrant"],
        logoutput => on_failure,
        path => [
            "/bin/",
            "/sbin/",
            "/usr/bin/",
            "/usr/sbin/"
        ],
        timeout => 0
    }

    file { "$minko_home":
        owner => vagrant,
        group => vagrant,
        recurse => false,
        ensure => directory
    }

    exec { "add-gcc-ppa":
        command => "add-apt-repository ppa:ubuntu-toolchain-r/test",
        provider => "shell",
        cwd => "/root",
        user => "root",
        before => Exec["apt-get-update"],
        unless => "apt-cache show gcc-4.8"
    }

    exec { "add-sdl2-ppa":
        command => "add-apt-repository ppa:zoogie/sdl2-snapshots",
        provider => "shell",
        cwd => "/root",
        user => "root",
        before => Exec["apt-get-update"],
        unless => "apt-cache show libsdl2"
    }

    exec { "apt-get-update":
        command => "apt-get update",
        provider => "shell",
        cwd => "/root",
        user => "root"
    }

    package { $deps:
        ensure => "present",
        require => Exec["apt-get-update"]
    }

    exec { "install-gcc":
        command => "update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 20 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 20",
        provider => "shell",
        user => "root",
        require => Package["gcc-4.8", "g++-4.8"],
        unless => "gcc --version | grep '4\\.8'",
        notify => Exec["compile-32", "compile-64"]
    }

    exec { "copy-repository":
        command => "rsync --delete --ignore-errors -l -p -c -r --filter=':- .gitignore' /vagrant/ .",
        provider => "shell",
        cwd => $minko_home,
        environment => ["PWD=/home/vagrant/src", "HOME=/home/vagrant"]
    }

    exec { "configure":
        command => "tool/lin/script/premake5.sh gmake",
        provider => "shell",
        logoutput => true,
        cwd => $minko_home,
        require => Exec["copy-repository"]
    }

    exec { "compile-64":
        command => "make verbose=1 config=linux64_release",
        provider => "shell",
        logoutput => true,
        cwd => $minko_home,
        require => Exec["configure"],
        unless => "uname -m | grep -v x86_64"
    }

    exec { "compile-32":
        command => "make verbose=1 config=linux32_release SHELL=/bin/bash",
        provider => "shell",
        logoutput => true,
        cwd => $minko_home,
        require => Exec["configure"],
        unless => "uname -m | grep -v i686"
    }
}
