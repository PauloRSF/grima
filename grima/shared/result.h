#define RESULT_STRUCT(NAME, SUCCESS_TYPE, ERROR_TYPE) struct NAME { \
  bool success; \
  union { \
    SUCCESS_TYPE success; \
    ERROR_TYPE error; \
  } value; \
};
