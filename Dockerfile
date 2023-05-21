FROM alpine:3.14.1 as builder
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.ustc.edu.cn/g' /etc/apk/repositories
RUN apk update && apk add --no-cache make cmake g++ sqlite-dev
WORKDIR /app
COPY . /app/
RUN cd orbitTools/core && make 
RUN cd orbitTools/orbit && make 
RUN cd calPath && make 

FROM alpine:3.14.1
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.ustc.edu.cn/g' /etc/apk/repositories
RUN apk update && apk add --no-cache libstdc++ sqlite-dev 
COPY --from=builder /app/calPath/calpath /calpath
ADD data /data
CMD ["/calpath", "/data"]