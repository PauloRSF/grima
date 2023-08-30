# Grima

C REST API made (kinda) from scratch just for shits and giggles.

Just kidding, the point of doing this is to:

- Learn more about Linux syscalls and, hopefully, io_uring
- Learn more about app architectures and Domain Driven Design
- Have fun :p

## How to run it

### Development

First, you need to run PostgreSQL and add the connection URI to `.env`. See the `.env.example` file for reference.

Then, you just (pun intended) need to install [just](https://github.com/casey/just). After that, you can run the `dev` script:

```sh
just dev <port>
```
