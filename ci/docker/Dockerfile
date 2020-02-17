FROM debian:buster

RUN apt update
RUN apt -y install build-essential pkg-config autopoint bison curl flex gettext git gperf help2man m4 mercurial ninja-build python3-mako python3-protobuf python3-yaml texinfo unzip wget xsltproc xz-utils libexpat1-dev rsync python3-pip doxygen

RUN pip3 install meson
RUN pip3 install xbstrap

ADD gitconfig /etc/gitconfig

CMD ["bash"]
