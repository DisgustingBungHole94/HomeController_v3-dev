# syntax=docker/dockerfile:1

FROM ubuntu:latest
LABEL Description="homecontroller development environment"

RUN apt -y update && apt install -y
RUN apt install -y g++ make cmake libssl-dev clang nodejs npm git wget unzip python-setuptools python3-setuptools

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

RUN wget https://github.com/joan2937/pigpio/archive/master.zip \
    && unzip master.zip \
    && rm master.zip \
    && cd pigpio-master \
    && make \
    && make install \

RUN echo "/usr/local/lib" > "/etc/ld.so.conf.d/999local.conf"
RUN ldconfig

RUN rm -r llhttp
RUN rm -r websocketpp
RUN rm -r rapidjson