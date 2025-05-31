FROM ubuntu:24.04
ARG DEBIAN_FRONTEND=noninteractive

# Switch to a more stable mirror for apt-get: Cloudflare HTTPS mirror; also force IPv4
RUN sed -i 's|http://archive.ubuntu.com/ubuntu|https://mirror.cloudflare.com/ubuntu|g' /etc/apt/sources.list && \
    echo 'Acquire::ForceIPv4 "true";' > /etc/apt/apt.conf.d/99force-ipv4

RUN apt-get clean
RUN apt-get --allow-releaseinfo-change update
RUN apt-get -y dist-upgrade

RUN apt-get update && apt-get install -y --fix-missing --no-install-recommends \
    nodejs npm curl nginx autotools-dev wget

RUN npm install -g @angular/cli@19.0.7

# Install Chrome browser for unit tests
RUN apt-get install -y 
RUN wget -q https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
RUN apt-get install -y ./google-chrome-stable_current_amd64.deb

# Needed for Github Actions environment. Without the variable, the Chrome will try to write to ~/.local/share, which is not writable
ENV HOME=/tmp
