# Grima

C REST API made (kinda) from scratch just for shits and giggles.

Just kidding, the point of doing this is to:

- Learn more about Linux syscalls and io_uring
- Learn more about app architectures and Domain Driven Design
- Have fun :p

## How to run it

The app is dockerized, so, there's two ways you can run it:

### With Docker

First, build the image from the Dockerfile with

```sh
docker build -t grima .
```

Then, run

```sh
docker compose up
```

### Without Docker

First, [you need to run PostgreSQL](https://www.postgresql.org/download) and add the connection URI to `.env`. See the `.env.example` file for reference.

Then, you just (pun intended) need to install [just](https://github.com/casey/just). After that, you can run the `dev` script:

```sh
just dev
```
