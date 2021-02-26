FROM alpine:3.13 as builder

WORKDIR /binkd
#COPY . /binkd

RUN apk add --no-cache --virtual .build-deps \
  build-base git make gcc binutils abuild \
  && git clone https://github.com/pgul/binkd.git /binkd \
  && cp mkfls/unix/* . \
  && ./configure \
  && make -j$(getconf _NPROCESSORS_ONLN)


FROM alpine:3.13
LABEL maintainer="Serg Podtynnyi <serg@podtynnyi.com>"

RUN apk add --no-cache curl \
  && apk update \
  && apk upgrade --no-cache

WORKDIR /binkd
COPY --from=builder /binkd/binkd /binkd/

VOLUME 	/ftn
EXPOSE 24554

ENTRYPOINT ["/binkd/binkd"]
CMD ["-h"]
