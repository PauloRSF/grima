output_file := `mktemp`

dev port:
  @gcc -g -I/usr/include/postgresql -L/usr/include/postgresql/libpq -Ilib -DLOG_USE_COLOR main.c lib/*.c -lpq -o {{output_file}}
  @{{output_file}} {{port}}

valgrind port:
  @gcc -g -I/usr/include/postgresql -L/usr/include/postgresql/libpq -Ilib -DLOG_USE_COLOR main.c lib/*.c -lpq -o {{output_file}}
  valgrind {{output_file}} {{port}}

create-development-db:
  docker compose cp ./sql/init.development.sql postgres:/opt
  docker compose exec postgres psql -U postgres -f /opt/init.development.sql

reset-development-db:
  docker compose exec postgres psql -U postgres -c 'DROP DATABASE grima_development'
  just create-development-db
