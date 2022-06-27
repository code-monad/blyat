from debian:bullseye-slim as build
RUN sed -i 's/deb.debian.org/mirrors.ustc.edu.cn/g' /etc/apt/sources.list
RUN sed -i 's|security.debian.org/debian-security|mirrors.ustc.edu.cn/debian-security|g' /etc/apt/sources.list
RUN TZ=Asia/Shanghai DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y tzdata
RUN apt-get install build-essential cmake software-properties-common libboost-dev zlib1g-dev -y
RUN apt-get clean
ADD . /src
WORKDIR /src
RUN mkdir build && cd build && cmake .. && make -j

from debian:bullseye-slim as fin
ENV ADDR="0.0.0.0"
ENV PORT=65472
ENV THREADS=4
RUN sed -i 's/deb.debian.org/mirrors.ustc.edu.cn/g' /etc/apt/sources.list
RUN sed -i 's|security.debian.org/debian-security|mirrors.ustc.edu.cn/debian-security|g' /etc/apt/sources.list
RUN TZ=Asia/Shanghai DEBIAN_FRONTEND=noninteractive apt-get update && apt-get install -y tzdata && apt-get clean
WORKDIR /app/
COPY --from=build /src/build/blyat .
COPY --from=build /src/config/blyat.toml ./config/blyat.toml
COPY --from=build /src/docker/scripts.sh ./run.sh
EXPOSE ${PORT}
ENTRYPOINT ["/app/run.sh"]
