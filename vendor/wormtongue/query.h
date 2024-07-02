#ifndef WORMTONGUE_QUERY_H
#define WORMTONGUE_QUERY_H

char *get_query_string(StringMap query_params);

char *get_query_param_value(StringMap query_params, char *key);

#endif
