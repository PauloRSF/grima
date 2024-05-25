output_file := `mktemp`
source_files := `find -name '*.c' -type f -printf '%p '`
compilation_flags := "-g -O3 -I/usr/include/postgresql -L/usr/include/postgresql/libpq -Ilib -Ivendor -Igrima -Iinclude"
link_flags := "-lpq -luuid -luring"

dev:
  @gcc {{compilation_flags}} {{source_files}} {{link_flags}} -o {{output_file}}
  @sh -c 'source /root/.bashrc && {{output_file}} | npx pino-pretty -c'

valgrind:
  @gcc {{compilation_flags}} {{source_files}} {{link_flags}} -o {{output_file}}
  @valgrind --leak-check=full --track-origins=yes {{output_file}}

create-development-db:
  docker compose cp ./sql/init.development.sql postgres:/opt
  docker compose exec postgres psql -U postgres -f /opt/init.development.sql

reset-development-db:
  docker compose exec postgres psql -U postgres -c 'DROP DATABASE grima_development'
  just create-development-db
