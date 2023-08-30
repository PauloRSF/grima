output_file := `mktemp`
source_files := `find -name '*.c' -not -path "./app/modules/persons/interface/*" -not -path "./app/modules/persons/application/*" -type f -printf '%p '`
compilation_flags := "-g -I/usr/include/postgresql -L/usr/include/postgresql/libpq -Ilib -Iapp -DLOG_USE_COLOR"
link_flags := "-lpq -luuid"

dev port:
  @gcc {{compilation_flags}} {{source_files}} {{link_flags}} -o {{output_file}}
  @{{output_file}} {{port}}

valgrind port:
  @gcc {{compilation_flags}} {{source_files}} {{link_flags}} -o {{output_file}}
  valgrind --leak-check=full --track-origins=yes {{output_file}} {{port}}

create-development-db:
  docker compose cp ./sql/init.development.sql postgres:/opt
  docker compose exec postgres psql -U postgres -f /opt/init.development.sql

reset-development-db:
  docker compose exec postgres psql -U postgres -c 'DROP DATABASE grima_development'
  just create-development-db
