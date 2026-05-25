#ifndef HTTP_H
#define HTTP_H

typedef enum
{
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_PATCH,
    HTTP_DELETE,
    HTTP_UNKNOWN
} HttpMethod;

HttpMethod http_method_from_string(const char *method);
const char *http_method_to_string(HttpMethod method);

#endif
