$emscripten_deps = ["git", "openjdk-6-jdk", "nodejs"]
$clang_version = "3.2"
$clang_dir = "clang+llvm-${clang_version}-x86-linux-ubuntu-12.04.tar.gz"
$clang_filename = "${clang_dir}"
$clang_url = "http://llvm.org/releases/${clang_version}/${clang_filename}"

Exec {
    user => "vagrant",
    cwd => "/home/vagrant/src/emscripten",
    logoutput => on_failure,
    environment => ["PWD=/home/vagrant/src/emscripten", "HOME=/home/vagrant"],
    timeout => 0
}

class emscripten {
    file { "/home/vagrant/.emscripten":
        owner => vagrant,
        group => vagrant,
        mode => 664,
        source => "/vagrant/tools/lin/share/dot.emscripten"
    }

    file { "/home/vagrant/src":
        owner => vagrant,
        group => vagrant,
        mode  => 775,
        recurse=> false,
        ensure => directory
    }

    exec { "/usr/bin/apt-get update":
        alias => "apt-get-update",
        cwd => "/root",
        user => "root",
    }

    package {
      $emscripten_deps:
        ensure => "latest",
        require => Exec["apt-get-update"];

      "python-software-properties":
        ensure => "latest";
    }


    exec { "/usr/bin/git clone https://github.com/kripken/emscripten/":
        alias => "git-clone-emscripten",
        cwd => "/home/vagrant/src",
        require => Package[$emscripten_deps],
        creates => "/home/vagrant/src/emscripten"
    }

    exec { "/usr/bin/git pull origin master":
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
}

group { "puppet":
  ensure => "present",
}

include emscripten
