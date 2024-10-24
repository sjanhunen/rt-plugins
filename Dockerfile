FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y build-essential -y cmake
RUN apt-get install -y ladspa-sdk