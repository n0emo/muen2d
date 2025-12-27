#pragma once

typedef struct js_State js_State;
typedef struct js_Object js_Object;

void js_dump(js_State *j);
void js_create_function(js_State *J, const char *source);
js_Object *js_get_global_this(js_State *j);
void js_pushobject(js_State *j, js_Object *o);
