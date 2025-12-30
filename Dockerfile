FROM debian:bullseye

# see https://wiki.winehq.org/Download
RUN dpkg --add-architecture i386  && \
        mkdir -pm755 /etc/apt/keyrings && \
        apt-get update && \
        apt-get install --no-install-recommends --assume-yes wget ca-certificates && \
        wget -O /etc/apt/keyrings/winehq-archive.key https://dl.winehq.org/wine-builds/winehq.key && \
        wget -NP /etc/apt/sources.list.d/ https://dl.winehq.org/wine-builds/debian/dists/bullseye/winehq-bullseye.sources && \
        apt-get update && \
        apt-get install --no-install-recommends --assume-yes wine-devel=9.1~bullseye-1 wine-devel-amd64=9.1~bullseye-1 wine-devel-i386=9.1~bullseye-1 wine-devel-dev=9.1~bullseye-1 make libboost-dev libusb-1.0-0-dev libusb-1.0-0-dev:i386 clang-16 lld-16 llvm-16 libc6-dev-i386-cross libgcc-10-dev-i386-cross && \
        ln -s clang-16 /usr/bin/clang && \
        ln -s clang++-16 /usr/bin/clang++ && \
        ln -s ld.lld-16 /usr/bin/ld.lld && \
        rm -rf /var/lib/apt/lists/*

# Build with: make
