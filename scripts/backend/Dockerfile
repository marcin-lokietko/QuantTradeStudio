FROM ubuntu:24.04
ARG DEBIAN_FRONTEND=noninteractive

# Switch to a more stable mirror for apt-get: Cloudflare HTTPS mirror; also force IPv4
RUN sed -i 's|http://archive.ubuntu.com/ubuntu|https://mirror.cloudflare.com/ubuntu|g' /etc/apt/sources.list && \
  echo 'Acquire::ForceIPv4 "true";' > /etc/apt/apt.conf.d/99force-ipv4

RUN apt-get clean
RUN apt-get --allow-releaseinfo-change update
RUN apt-get -y dist-upgrade

RUN apt-get update && apt-get install -y --fix-missing --no-install-recommends \
  build-essential make cmake g++ gdb autoconf libtool pkg-config git wget \
  libcurl4-openssl-dev nlohmann-json3-dev libssl-dev libspdlog-dev libfmt-dev \
  clang clang-format clang-tidy valgrind libasio-dev zlib1g-dev \
  python3 python3-pip

RUN mkdir -p gtest \
  && cd gtest \
  && git clone https://github.com/google/googletest/ \
  && cd googletest \
  && git checkout v1.14.0 \
  && mkdir build \
  && cd build \
  && cmake .. \
  && make \
  && make install

RUN mkdir -p crow \
  && cd crow \
  && wget https://github.com/CrowCpp/Crow/releases/download/v1.2.0/Crow-1.2.0-Linux.deb \
  && dpkg -i Crow-1.2.0-Linux.deb
