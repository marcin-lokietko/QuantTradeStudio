FROM ubuntu:24.04
ARG DEBIAN_FRONTEND=noninteractive

# Switch to a more stable mirror for apt-get: Cloudflare HTTPS mirror; also force IPv4
RUN sed -i 's|http://archive.ubuntu.com/ubuntu|https://mirror.cloudflare.com/ubuntu|g' /etc/apt/sources.list && \
    echo 'Acquire::ForceIPv4 "true";' > /etc/apt/apt.conf.d/99force-ipv4

RUN apt-get clean
RUN apt-get update
RUN apt-get -y dist-upgrade

RUN apt-get update && apt-get install -y --fix-missing --no-install-recommends\
    python3 python3-pip python3-behave python3-requests python3-flask
