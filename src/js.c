#include "js.h"

#include <jsi.h>

static void js_dumpstack(js_State *J);
static void js_dumpvalue(js_State *J, js_Value v);

void js_dump(js_State *j) {
    js_dumpstack(j);
}

void js_dumpstack(js_State *J) {
    printf("stack {\n");
    for (int i = 0; i < J->top; ++i) {
        putchar(i == J->bot ? '>' : ' ');
        printf("%4d: ", i);
        js_dumpvalue(J, J->stack[i]);
        putchar('\n');
    }
    printf("}\n");
}

void js_create_function(js_State *J, const char *source) {
    int top = js_gettop(J);
    js_Buffer *sb = NULL;
    const char *body = NULL;
    js_Ast *parse = NULL;
    js_Function *fun = NULL;

    if (js_try(J)) {
        js_free(J, sb);
        jsP_freeparse(J);
        js_throw(J);
    }

    /* p1, p2, ..., pn */
    if (top > 2) {
        for (int i = 1; i < top - 1; ++i) {
            if (i > 1)
                js_putc(J, &sb, ',');
            js_puts(J, &sb, js_tostring(J, i));
        }
        js_putc(J, &sb, ')');
        js_putc(J, &sb, 0);
    }

    /* body */
    body = js_isdefined(J, top - 1) ? js_tostring(J, top - 1) : "";

    parse = jsP_parsefunction(J, source, sb ? sb->s : NULL, body);
    fun = jsC_compilefunction(J, parse);

    js_endtry(J);
    js_free(J, sb);
    jsP_freeparse(J);

    js_newfunction(J, fun, J->E);
}

js_Object *js_get_global_this(js_State *j) {
    return j->G;
}

static void js_dumpvalue(js_State *J, js_Value v) {
    switch (v.t.type) {
        case JS_TUNDEFINED:
            printf("undefined");
            break;
        case JS_TNULL:
            printf("null");
            break;
        case JS_TBOOLEAN:
            printf(v.u.boolean ? "true" : "false");
            break;
        case JS_TNUMBER:
            printf("%.9g", v.u.number);
            break;
        case JS_TSHRSTR:
            printf("'%s'", v.u.shrstr);
            break;
        case JS_TLITSTR:
            printf("'%s'", v.u.litstr);
            break;
        case JS_TMEMSTR:
            printf("'%s'", v.u.memstr->p);
            break;
        case JS_TOBJECT:
            if (v.u.object == J->G) {
                printf("[Global]");
                break;
            }
            switch (v.u.object->type) {
                case JS_COBJECT:
                    printf("[Object %p]", (void *)v.u.object);
                    break;
                case JS_CARRAY:
                    printf("[Array %p]", (void *)v.u.object);
                    break;
                case JS_CFUNCTION:
                    printf(
                        "[Function %p, %s, %s:%d]",
                        (void *)v.u.object,
                        v.u.object->u.f.function->name,
                        v.u.object->u.f.function->filename,
                        v.u.object->u.f.function->line
                    );
                    break;
                case JS_CSCRIPT:
                    printf("[Script %s]", v.u.object->u.f.function->filename);
                    break;
                case JS_CCFUNCTION:
                    printf("[CFunction %s]", v.u.object->u.c.name);
                    break;
                case JS_CBOOLEAN:
                    printf("[Boolean %d]", v.u.object->u.boolean);
                    break;
                case JS_CNUMBER:
                    printf("[Number %g]", v.u.object->u.number);
                    break;
                case JS_CSTRING:
                    printf("[String'%s']", v.u.object->u.s.string);
                    break;
                case JS_CERROR:
                    printf("[Error]");
                    break;
                case JS_CARGUMENTS:
                    printf("[Arguments %p]", (void *)v.u.object);
                    break;
                case JS_CITERATOR:
                    printf("[Iterator %p]", (void *)v.u.object);
                    break;
                case JS_CUSERDATA:
                    printf("[Userdata %s %p]", v.u.object->u.user.tag, v.u.object->u.user.data);
                    break;
                default:
                    printf("[Object %p]", (void *)v.u.object);
                    break;
            }
            break;
        default:
            printf("[Object %p]", (void *)v.u.object);
            break;
    }
}
