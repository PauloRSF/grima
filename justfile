output_file := `mktemp`
source_files := `find -not -path './misc/*' -name '*.c' -type f -printf '%p '`
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
  @~/.local/bin/gdbgui --host 0.0.0.0 --port 5001 {{output_file}}

create-development-db:
  docker compose cp ./sql/init.development.sql postgres:/opt
  docker compose exec postgres psql -U postgres -f /opt/init.development.sql

reset-development-db:
  docker compose exec postgres psql -U postgres -c 'DROP DATABASE grima_development'
  just create-development-db
