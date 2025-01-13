FROM ubuntu:24.04
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get clean
RUN apt-get update
RUN apt-get -y dist-upgrade

RUN apt install -y nodejs
RUN apt install -y npm
RUN npm install -g @angular/cli@19.0.7
RUN apt-get install -y curl nginx autotools-dev
