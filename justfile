output_file := `mktemp`

dev port:
  @gcc -g -Ilib -DLOG_USE_COLOR main.c lib/*.c -o {{output_file}}
  @{{output_file}} {{port}}

valgrind port:
  @gcc -g -Ilib -DLOG_USE_COLOR main.c lib/*.c -o {{output_file}}
  valgrind {{output_file}} {{port}}
