FROM ubuntu:latest as builder

WORKDIR /binkd
#COPY . /binkd

RUN apt update && apt upgrade -y \
  && apt install -y git gcc make \
  && git clone https://github.com/pgul/binkd.git /binkd \
  && cp mkfls/unix/* . \
  && ./configure \
  && make -j$(getconf _NPROCESSORS_ONLN)

FROM ubuntu:latest
LABEL maintainer="Serg Podtynnyi <serg@podtynnyi.com>"

RUN apt update && apt upgrade -y 

WORKDIR /binkd
COPY --from=builder /binkd/binkd /binkd/

VOLUME 	/ftn
EXPOSE 24554

ENTRYPOINT ["/binkd/binkd"]
CMD ["-h"]
