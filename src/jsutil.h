#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct js_State js_State;

void js_dump(js_State *j);

void js_create_function(js_State *J, const char *source);

#ifdef __cplusplus
}
#endif
