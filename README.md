# Grima

A pure C implementation of the [RealWorld API](https://github.com/gothinkster/realworld).

## How to run

### With Docker

```sh
docker compose up --build
```

### Without Docker

You need to install:

- [PostgreSQL](https://www.postgresql.org/download)
- [just](https://github.com/casey/just)

After installing those, create a `.env` file. You can use the example one as a starting point:

```sh
cp .env.example .env
```

Change the `.env` file variables with the values that suit your setup. Then, start the app with:

```sh
just dev
```

After the app finishes booting, you can hit `http://localhost:<PORT>` (with `PORT` being the one you set in the .env file)
