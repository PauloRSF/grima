#include <cjson/cJSON.h>

enum {
  CPINO_LOG_TRACE,
  CPINO_LOG_DEBUG,
  CPINO_LOG_INFO,
  CPINO_LOG_WARN,
  CPINO_LOG_ERROR,
  CPINO_LOG_FATAL
};

void cpino_log_with_context(unsigned short int level, cJSON *context,
                            const char *fmt, ...);

#define cpino_log_trace(...)                                                   \
  cpino_log_with_context(CPINO_LOG_TRACE, NULL, __VA_ARGS__)
#define cpino_log_debug(...)                                                   \
  cpino_log_with_context(CPINO_LOG_DEBUG, NULL, __VA_ARGS__)
#define cpino_log_info(...)                                                    \
  cpino_log_with_context(CPINO_LOG_INFO, NULL, __VA_ARGS__)
#define cpino_log_warn(...)                                                    \
  cpino_log_with_context(CPINO_LOG_WARN, NULL, __VA_ARGS__)
#define cpino_log_error(...)                                                   \
  cpino_log_with_context(CPINO_LOG_ERROR, NULL, __VA_ARGS__)
#define cpino_log_fatal(...)                                                   \
  cpino_log_with_context(CPINO_LOG_FATAL, NULL, __VA_ARGS__)

#define cpino_log_with_context_trace(...)                                      \
  cpino_log_with_context(CPINO_LOG_TRACE, __VA_ARGS__)
#define cpino_log_with_context_debug(...)                                      \
  cpino_log_with_context(CPINO_LOG_DEBUG, __VA_ARGS__)
#define cpino_log_with_context_info(...)                                       \
  cpino_log_with_context(CPINO_LOG_INFO, __VA_ARGS__)
#define cpino_log_with_context_warn(...)                                       \
  cpino_log_with_context(CPINO_LOG_WARN, __VA_ARGS__)
#define cpino_log_with_context_error(...)                                      \
  cpino_log_with_context(CPINO_LOG_ERROR, __VA_ARGS__)
#define cpino_log_with_context_fatal(...)                                      \
  cpino_log_with_context(CPINO_LOG_FATAL, __VA_ARGS__)
