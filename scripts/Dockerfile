FROM ubuntu:22.04
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get -y dist-upgrade

RUN apt-get install -y build-essential
RUN apt-get install -y make cmake g++ gdb
RUN apt-get install -y autoconf libtool pkg-config git wget

RUN cd /home && wget http://downloads.sourceforge.net/project/boost/boost/1.79.0/boost_1_79_0.tar.gz \
  && tar xfz boost_1_79_0.tar.gz \
  && rm boost_1_79_0.tar.gz \
  && cd boost_1_79_0 \
  && ./bootstrap.sh --prefix=/usr/local \
  && ./b2 install \
  && cd /home \
  && rm -rf boost_1_79_0

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

# TODO: Refactor to download specific versions:
RUN apt-get install -y clang
RUN apt-get install -y clang-format
RUN apt-get install -y clang-tidy
RUN apt-get install -y valgrind
