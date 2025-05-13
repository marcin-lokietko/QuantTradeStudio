FROM ubuntu:24.04
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get clean
RUN apt-get update
RUN apt-get -y dist-upgrade

RUN apt-get install -y build-essential
RUN apt-get install -y make cmake g++ gdb
RUN apt-get install -y autoconf libtool pkg-config git wget

# TODO: Download specific versions:
RUN apt-get install -y libcurl4-openssl-dev
RUN apt-get install nlohmann-json3-dev
RUN apt-get install libssl-dev

RUN apt-get install -y libgoogle-glog-dev

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

# TODO: Download specific versions:
RUN apt-get install -y clang
RUN apt-get install -y clang-format
RUN apt-get install -y clang-tidy
RUN apt-get install -y valgrind

RUN apt-get install -y libasio-dev
RUN mkdir -p crow \
  && cd crow \
  && wget https://github.com/CrowCpp/Crow/releases/download/v1.2.0/Crow-1.2.0-Linux.deb \
  && dpkg -i Crow-1.2.0-Linux.deb

RUN apt-get install -y zlib1g-dev

RUN apt-get install -y python3 python3-pip
