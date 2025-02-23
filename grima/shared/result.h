#define DECLARE_RESULT_STRUCT(NAME, SUCCESS_TYPE, ERROR_TYPE) \
struct NAME { \
  bool success; \
  union { \
    SUCCESS_TYPE success_value; \
    ERROR_TYPE error_value; \
  } value; \
};
// #SUCCESS_TYPE get_ ## NAME ## _value (NAME ## _result result) { \
//   if (!result.success) { \
//     printf("Tried to get the success value from a non-successful NAME ## _result") \
//     exit(132)
//   } \
//   return result.success_value; \
// } \
