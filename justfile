output_file := `mktemp`
source_files := `find . -name '*.c' -type f | grep -v './misc/' | xargs`
compilation_flags := "-g -O3 -I/usr/include/postgresql -L/usr/include/postgresql/libpq -Ilib -Ilib/wormtongue -Ivendor -Igrima -Igrima/lib -Igrima/shared -Iinclude -DDEV -D__USE_XOPEN -D_GNU_SOURCE"
link_flags := "-lpq -luuid -luring -largon2 -lsodium -lcjson"

dev-check:
  @gcc {{compilation_flags}} {{source_files}} {{link_flags}} -o {{output_file}}

dev:
  @gcc {{compilation_flags}} {{source_files}} {{link_flags}} -o {{output_file}}
  @sh -c 'source /root/.bashrc && {{output_file}} | npx pino-pretty -c'

valgrind:
  @gcc {{compilation_flags}} {{source_files}} {{link_flags}} -o {{output_file}}
  @valgrind --leak-check=full --track-origins=yes {{output_file}}

gdb:
  @gcc {{compilation_flags}} {{source_files}} {{link_flags}} -o {{output_file}}
  @gdb {{output_file}}

format:
  @clang-format -i {{source_files}}

docker-format:
  @docker compose run -it app bash -c "clang-format -i {{source_files}}"

app-shell:
  docker compose run -it app /bin/bash || :

db-shell:
  docker compose run -it postgres psql -U postgres || :

db-migrate:
  docker compose exec postgres bash -c "find /sql -type f -exec psql -U postgres -d grima_development -f {} \;"

db-reset:
  docker compose exec postgres psql -U postgres -c 'DROP DATABASE grima_development'
  docker compose exec postgres psql -U postgres -c 'CREATE DATABASE grima_development'
  just db-migrate
