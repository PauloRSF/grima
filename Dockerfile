FROM debian:12.1

WORKDIR /app

RUN apt update
RUN apt install -y wget build-essential libpq5 libpq-dev uuid-dev valgrind liburing2 liburing-dev

RUN wget -qO - 'https://proget.makedeb.org/debian-feeds/prebuilt-mpr.pub' | gpg --dearmor | tee /usr/share/keyrings/prebuilt-mpr-archive-keyring.gpg 1>/dev/null
RUN echo "deb [arch=all,amd64 signed-by=/usr/share/keyrings/prebuilt-mpr-archive-keyring.gpg] https://proget.makedeb.org prebuilt-mpr bookworm" | tee /etc/apt/sources.list.d/prebuilt-mpr.list
RUN apt update

RUN apt install -y just

CMD ["just", "dev"]
