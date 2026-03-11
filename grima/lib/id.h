#ifndef GRIMA_LIB_ID_H
#define GRIMA_LIB_ID_H

typedef char* entity_id_t;

// TODO: create an id typedef?
entity_id_t generate_entity_id();
char* validate_entity_id(entity_id_t id);
void free_entity_id(entity_id_t id);

#endif
