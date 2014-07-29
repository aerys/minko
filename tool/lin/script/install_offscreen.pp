class minko {
    $minko_home = "/vagrant"

    $deps = [

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
        # "libudev-dev",
        # "libxinerama-dev",

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

    exec { "apt-get-update":
        command => "apt-get update",
        provider => "shell",
        user => "root"
    }

    package { $deps:
        ensure => "present",
        require => Exec["apt-get-update"]
    }

    exec { "configure":
        command => "tool/lin/script/premake5.sh gmake",
        provider => "shell",
        logoutput => true,
        cwd => $minko_home,
        require => Exec["copy-repository"]
    }

    exec { "compile":
        command => "make verbose=1 config=linux64_release",
        provider => "shell",
        logoutput => true,
        cwd => $minko_home,
        require => Exec["configure"],
    }

    exec { "test":
        command => "test/bin/linux64/release/test",
        provider => "shell",
        logoutput => true,
        cwd => $minko_home,
        require => Exec["compile"]
    }
}
