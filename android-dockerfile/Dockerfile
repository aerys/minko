FROM lakoo/android-ndk:26-27.0.3-r17

# Intall make
RUN apt-get update && apt-get install make

# Install CMake
RUN cd / && \
   wget -q https://cmake.org/files/v3.9/cmake-3.9.6-Linux-x86_64.sh -O cmake.sh && \
   chmod +x ./cmake.sh && \
   ./cmake.sh --skip-license && \
   rm -rf cmake.sh

# Install the platform-tools
RUN cd /opt/android-sdk-linux/tools/bin && \
    ./sdkmanager "platforms;android-25" && ./sdkmanager --update

# Install rsync
RUN apt-get -y install rsync

# Install ant
RUN apt-get -y install ant

# Install zipalign
RUN apt-get -y install zipalign

# Install old tool dir
RUN wget -q https://dl.google.com/android/repository/tools_r25.2.5-linux.zip && \
    unzip tools_r25.2.5-linux.zip && \
    rm -rf android-sdk-linux/tools tools_r25.2.5-linux.zip && \
    mv tools android-sdk-linux