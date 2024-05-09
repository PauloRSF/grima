FROM debian:12.1

WORKDIR /app

RUN apt update
RUN apt install -y git curl wget build-essential libpq5 libpq-dev uuid-dev valgrind liburing2 liburing-dev entr

# RUN wget -qO - 'https://proget.makedeb.org/debian-feeds/prebuilt-mpr.pub' | gpg --dearmor | tee /usr/share/keyrings/prebuilt-mpr-archive-keyring.gpg 1>/dev/null
# RUN echo "deb [arch=all,amd64 signed-by=/usr/share/keyrings/prebuilt-mpr-archive-keyring.gpg] https://proget.makedeb.org prebuilt-mpr bookworm" | tee /etc/apt/sources.list.d/prebuilt-mpr.list
# RUN apt update -y
# RUN apt install makedeb
# RUN git clone https://mpr.makedeb.org/just
# RUN cd just
# RUN makedeb -si
# RUN cd ..
RUN curl --proto '=https' --tlsv1.2 -sSf -o rust.sh https://sh.rustup.rs
RUN sh rust.sh -y

ENV PATH="${PATH}:/root/.cargo/bin"

RUN cargo install just

ENV NVM_DIR="/root/.nvm"

RUN curl -o nvm.sh https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh
RUN bash ./nvm.sh

RUN rm /usr/bin/sh && ln -s /usr/bin/bash /usr/bin/sh

RUN source /root/.bashrc && nvm install 20 && npm i -g pino-pretty

CMD ["just", "dev"]
