FROM ubuntu:24.04

COPY . /usr/src/project

WORKDIR /usr/src/project


RUN apt-get update \
 && apt-get install -y --no-install-recommends doxygen graphviz ca-certificates \
 && rm -rf /var/lib/apt/lists/*

CMD doxygen Doxyfile
