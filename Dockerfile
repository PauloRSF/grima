FROM debian:latest

WORKDIR /app

RUN apt update
RUN apt install -y git curl wget build-essential python3 pipx libpq5 libpq-dev uuid-dev valgrind liburing2 liburing-dev libargon2-1 libargon2-dev libsodium23 libsodium-dev libcjson1 libcjson-dev entr clang-format

RUN curl --proto '=https' --tlsv1.2 -sSf -o rust.sh https://sh.rustup.rs
RUN sh rust.sh -y

ENV PATH="${PATH}:/root/.cargo/bin"

RUN cargo install just

ENV NVM_DIR="/root/.nvm"

RUN curl -o nvm.sh https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh
RUN bash ./nvm.sh

RUN rm /usr/bin/sh && ln -s /usr/bin/bash /usr/bin/sh

RUN source /root/.bashrc && nvm install 20 && npm i -g pino-pretty

RUN pipx install gdbgui

CMD ["just", "dev"]
