FROM ubuntu:jammy

RUN apt-get update -y && apt-get upgrade -y
RUN apt-get install -y build-essential cmake gcc-12 g++-12 gdb git ninja-build

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 10
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 20

RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 10
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 20

RUN mkdir /src && \
    cd /src && \
    git clone -b 9.1.0 https://github.com/fmtlib/fmt.git && \
    cd fmt && \
    mkdir build && \
    cd build && \
    cmake -G Ninja -DFMT_DOC=Off -DFMT_TEST=Off .. && \
    ninja -j $(nproc) && \
    ninja install
