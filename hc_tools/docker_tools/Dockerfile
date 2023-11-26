# syntax=docker/dockerfile:1

FROM ubuntu:latest
LABEL Description="homecontroller development environment"

RUN apt -y update && apt install -y
RUN apt install -y g++ make cmake libssl-dev clang nodejs npm git

RUN git clone https://github.com/nodejs/llhttp.git \
    && cd llhttp \
    && npm install \
    && make \
    && make install

RUN git clone https://github.com/zaphoyd/websocketpp.git \
    && cd websocketpp \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make install

RUN git clone https://github.com/Tencent/rapidjson.git \
    && cd rapidjson \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make install

RUN echo "/usr/local/lib" > "/etc/ld.so.conf.d/999local.conf"
RUN ldconfig

RUN rm -r llhttp
RUN rm -r websocketpp
RUN rm -r rapidjson