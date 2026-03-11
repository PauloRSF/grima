#include <stdlib.h>

#include <uuid/uuid.h>

entity_id_t generate_entity_id() {
  char *id = malloc(37);
  uuid_t uuid;
  uuid_generate(uuid);
  uuid_unparse(uuid, id);

  return id;
}

char *validate_entity_id(entity_id_t id) {
  if (id == NULL)
    return "is required";

  uuid_t uuid;

  if (uuid_parse(id, uuid) != 0)
    return "must be a valid UUID";

  return NULL;
}

void free_entity_id(entity_id_t id) {
  free(id);
}
