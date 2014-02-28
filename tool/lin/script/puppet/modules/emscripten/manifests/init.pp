class emscripten {
    $emscripten_home = "/opt/emscripten"

    $deps = [
        # Tool
        "git",

        # Web
        "apache2",
        "php5",
        "libapache2-mod-php5",

        # Compiler
        "nodejs",
        "llvm-3.2",
        "clang-3.2"
    ]

    Exec {
        user => "vagrant",
        cwd => "/vagrant",
        environment => ["PWD=/vagrant", "HOME=/home/vagrant", "MINKO_HOME=$minko::minko_home", "EMSCRIPTEN=$emscripten_home"],
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

    file { "$emscripten_home":
        owner => vagrant,
        group => vagrant,
        recurse => false,
        ensure => directory
    }

    file { "/home/vagrant/.emscripten":
        owner => vagrant,
        group => vagrant,
        mode => 664,
        source => "/vagrant/tool/lin/script/puppet/dot.emscripten" 
    }

    exec { "add-node-ppa":
        command => "add-apt-repository ppa:chris-lea/node.js",
        provider => "shell",
        cwd => "/root",
        user => "root",
        before => Exec["apt-get-update"],
        unless => "apt-cache show nodejs"
    }

    package { $deps:
        ensure => "present",
        require => Exec["apt-get-update"]
    }

    exec { "git-clone-emscripten":
        command => "git clone https://github.com/kripken/emscripten $emscripten_home",
        provider => "shell",
        require => Package["git"],
        creates => "${emscripten_home}/emcc"
    }

    exec { "git-pull-emscripten":
        command => "git pull origin master || true",
        provider => "shell",
        cwd => $emscripten_home,
        require => Exec["git-clone-emscripten"],
        before => Exec["configure"]
    }

    file { "/var/www/minko/":
        alias => 'link-web',
        ensure => 'link',
        target => "${minko_home}/examples/",
        require => [
            Package[$deps],
            Exec["copy-repository"]
        ]
    }
    
    service { "apache2":
        alias => "run-web",
        ensure => "running",
        require => Package["apache2"],
    }

    exec { "configure-html5":
        command => "tool/lin/script/premake5.sh gmake",
        provider => "shell",
        logoutput => true,
        cwd => $minko::minko_home,
        require => Exec["copy-repository"]
    }

    exec { "compile-html5":
        command => "make verbose=1 config=html5_release SHELL=/bin/bash",
        provider => "shell",
        logoutput => true,
        cwd => $minko::minko_home,
        require => [
            Exec["configure-html5"],
            Package["nodejs", "llvm-3.2", "clang-3.2"]
        ]
    }
}
