FROM ubuntu:24.04
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get clean
RUN apt-get update
RUN apt-get -y dist-upgrade

RUN apt-get install -y nodejs
RUN apt-get install -y npm
RUN npm install -g @angular/cli@19.0.7
RUN apt-get install -y curl nginx autotools-dev

# Install Chrome browser for unit tests
RUN apt-get install -y wget
RUN wget -q https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
RUN apt-get install -y ./google-chrome-stable_current_amd64.deb
