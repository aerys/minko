class offscreen {
    $deps = [
        # "cmake",
        # "openjdk-6-jdk",
        # "libsdl2",
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
    ]

    package { $deps:
        ensure => "present",
        require => Exec["apt-get-update"]
    }
}
