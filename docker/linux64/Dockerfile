FROM amd64/gcc:9.4.0

RUN apt-get update && apt-get install -y \
    curl \
    jq \
    libasound2-dev \
    libfreetype6-dev \
    libgl1-mesa-dev \
    libglew-dev \
    libglu1-mesa-dev \
    libgtk-3-dev \
    libgtk2.0-dev \
    libgtkglext1-dev \
    libnss3-dev \
    libosmesa6-dev \
    libpulse-dev \
    libxss-dev \
    mesa-common-dev

RUN mkdir -p /opt/cmake && \
    curl -L -o /tmp/cmake.tar.gz https://github.com/Kitware/CMake/releases/download/v3.24.3/cmake-3.24.3-linux-x86_64.tar.gz && \
    tar xvzf /tmp/cmake.tar.gz --strip 1 -C /opt/cmake && \
    rm -rf /tmp/cmake.tar.gz

ENV PATH="$PATH:/opt/cmake/bin"
