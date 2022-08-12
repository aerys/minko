FROM ubuntu:20.04

ENV GPM_VERSION         0.18.0
ENV CMAKE_VERSION       3.21.3
ENV CMAKE_HOME          /opt/cmake
# LTS NDK: https://github.com/android/ndk/wiki#current-lts-release
# Older NDKs: https://github.com/android/ndk/wiki/Unsupported-Downloads
ENV ANDROID_NDK_VERSION r17c
ENV ANDROID_NDK_URL     https://dl.google.com/android/repository/android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip
ENV ANDROID_NDK_ROOT    /opt/android-ndk-linux
ENV ANDROID_SDK_ROOT    /opt/android-sdk-linux
ENV PATH                ${PATH}:${ANDROID_SDK_ROOT}/platform-tools:${ANDROID_SDK_ROOT}/cmdline-tools/latest/bin:${CMAKE_HOME}/bin
# Preserved for backward compatibility
ENV ANDROID_NDK         ${ANDROID_NDK_ROOT}
ENV ANDROID_NDK_HOME    ${ANDROID_NDK_ROOT}
ENV ANDROID_SDK_HOME    ${ANDROID_SDK_ROOT}
ENV ANDROID_HOME        ${ANDROID_SDK_ROOT}

RUN mkdir -p /root/.android && touch /root/.android/repositories.cfg

# Install OpenJDK
RUN apt-get update && apt-get install -y software-properties-common wget unzip
RUN add-apt-repository ppa:openjdk-r/ppa
RUN dpkg --add-architecture i386 && apt-get update

# Dependencies to execute Android builds
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y \
        openjdk-8-jdk \
        libc6:i386 \
        libstdc++6:i386 \
        libgcc1:i386 \
        libncurses5:i386 \
        libz1:i386 \
        net-tools


# Install Android Command line Tools
RUN cd /opt \
    && wget -q -O android-commandline-tools.zip https://dl.google.com/android/repository/commandlinetools-linux-6858069_latest.zip \
    && mkdir -p ${ANDROID_SDK_ROOT}/cmdline-tools \
    && unzip -q android-commandline-tools.zip -d /tmp/ \
    && mv /tmp/cmdline-tools/ ${ANDROID_SDK_ROOT}/cmdline-tools/latest \
    && rm android-commandline-tools.zip && ls -la ${ANDROID_SDK_ROOT}/cmdline-tools/latest/

# ------------------------------------------------------
# --- Install Android SDKs and other build packages

# Other tools and resources of Android SDK
#  you should only install the packages you need!
# To get a full list of available options you can use:
#  sdkmanager --list

# Accept licenses before installing components, no need to echo y for each component
# License is valid for all the standard components in versions installed from this file
# Non-standard components: MIPS system images, preview versions, GDK (Google Glass) and Android Google TV require separate licenses, not accepted there
RUN yes | sdkmanager --licenses

# SDKs
# Please keep these in descending order!
# The `yes` is for accepting all non-standard tool licenses.
RUN yes | sdkmanager --update --channel=0
# Please keep all sections in descending order!
RUN yes | sdkmanager \
        "build-tools;25.0.3" \
        "platform-tools" \
        "platforms;android-25"

# Install Android NDK
RUN wget -q -O android-ndk.zip ${ANDROID_NDK_URL} && \
	unzip android-ndk.zip && \
	rm -f android-ndk.zip && \
	mv android-ndk-${ANDROID_NDK_VERSION} ${ANDROID_NDK_HOME}

# Install "old" tools directory
RUN wget -q https://dl.google.com/android/repository/tools_r25.2.5-linux.zip && \
    unzip tools_r25.2.5-linux.zip && \
    rm -rf ${ANDROID_SDK_ROOT}/tools tools_r25.2.5-linux.zip && \
    mv tools ${ANDROID_SDK_ROOT}

# Install CMake
RUN mkdir -p ${CMAKE_HOME} && \
    cd ${CMAKE_HOME} && \
    wget -q -O cmake.tar.gz https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz && \
    tar xvzf cmake.tar.gz --strip 1 && \
    rm -rf cmake.tar.gz

RUN apt-get install -y \
        make \
        ant \
        zipalign \
        libfreetype6 \
        jq \
        rsync \
        curl \
        libncurses5

# Install GPM
RUN curl -Ls https://github.com/aerys/gpm-packages/raw/gpm-linux64/${GPM_VERSION}/gpm-linux64/gpm-linux64.tar.gz | tar xvz && \
    mv gpm /usr/bin && \
    chmod +x /usr/bin/gpm

CMD ["/bin/bash"]
