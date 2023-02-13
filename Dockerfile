FROM debian:buster

# see https://wiki.winehq.org/Download
RUN dpkg --add-architecture i386  && \
        mkdir -pm755 /etc/apt/keyrings && \
        apt-get update && \
        apt-get install --no-install-recommends --assume-yes wget ca-certificates && \
        wget -O /etc/apt/keyrings/winehq-archive.key https://dl.winehq.org/wine-builds/winehq.key && \
        wget -NP /etc/apt/sources.list.d/ https://dl.winehq.org/wine-builds/debian/dists/buster/winehq-buster.sources && \
        apt-get update && \
        apt-get install --no-install-recommends --assume-yes wine-devel=6.23~buster-1 wine-devel-amd64=6.23~buster-1 wine-devel-i386=6.23~buster-1 wine-devel-dev=6.23~buster-1 cmake make libboost-dev g++-i686-linux-gnu libusb-1.0-0-dev:i386

# Build with
# cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/opt/wine-devel/bin/wineg++ -DCMAKE_CXX_FLAGS="--target i686-linux-gnu"
