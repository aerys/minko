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

# Install ant
RUN apt-get -y install ant

# Install zipalign
RUN apt-get -y install zipalign

# Install old tool dir
RUN wget -q https://dl.google.com/android/repository/tools_r25.2.5-linux.zip && \
    unzip tools_r25.2.5-linux.zip && \
    rm -rf android-sdk-linux/tools tools_r25.2.5-linux.zip && \
    mv tools android-sdk-linux

# Make keystore
RUN keytool -genkey -alias myalias \
    -keyalg RSA -keystore my-release-key.keystore \
    -dname "CN=Name, OU=Unit, O=Organization, L=City, S=State, C=Coutry" \
    -storepass password -keypass password

# Move to root
RUN mv my-release-key.keystore /root/
