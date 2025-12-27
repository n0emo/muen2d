#pragma once

#include <cstdint>

extern "C" {
#include "js.h"
}

#include <mujs.h> // NOLINT

// NOLINTNEXTLINE
#define mujs_catch(j)                                                                                                  \
    if (auto& buf = *(static_cast<jmp_buf *>(js::savetry(j))); setjmp(buf)) {                                          \
        throw js::Exception(j);                                                                                        \
    }

#include <exception>
#include <string>

struct js_State;

namespace muen::js {

constexpr auto VERSION_MAJOR = JS_VERSION_MAJOR;
constexpr auto VERSION_MINOR = JS_VERSION_MINOR;
constexpr auto VERSION_PATCH = JS_VERSION_PATCH;
constexpr auto VERSION = JS_VERSION;

using State = js_State;
using Alloc = ::js_Alloc;
using Panic = ::js_Panic;
using CFunction = ::js_CFunction;
using Finalize = ::js_Finalize;
using HasProperty = ::js_HasProperty;
using Put = ::js_Put;
using Delete = ::js_Delete;
using Report = ::js_Report;

constexpr auto newstate = ::js_newstate;
constexpr auto setcontext = ::js_setcontext;
constexpr auto getcontext = ::js_getcontext;
constexpr auto setreport = ::js_setreport;
constexpr auto atpanic = ::js_atpanic;
constexpr auto freestate = ::js_freestate;
constexpr auto gc = ::js_gc;

constexpr auto dostring = ::js_dostring;
constexpr auto dofile = ::js_dofile;
constexpr auto ploadstring = ::js_ploadstring;
constexpr auto ploadfile = ::js_ploadfile;
constexpr auto pcall = ::js_pcall;
constexpr auto pconstruct = ::js_pconstruct;

constexpr auto savetry = ::js_savetry;
constexpr auto endtry = ::js_endtry;

constexpr auto STRICT = 1;

constexpr auto REGEXP_G = 1;
constexpr auto REGEXP_I = 2;
constexpr auto REGEXP_M = 4;

constexpr auto READONLY = 1;
constexpr auto DONTENUM = 2;
constexpr auto DONTCONF = 4;

enum class Type : std::uint8_t {
    Undefined = JS_ISUNDEFINED,
    Null = JS_ISNULL,
    Boolean = JS_ISBOOLEAN,
    Number = JS_ISNUMBER,
    String = JS_ISSTRING,
    Function = JS_ISFUNCTION,
    Object = JS_ISOBJECT
};

constexpr auto report = ::js_report;

constexpr auto newerror = ::js_newerror;
constexpr auto newevalerror = ::js_newevalerror;
constexpr auto newrangeerror = ::js_newrangeerror;
constexpr auto newreferenceerror = ::js_newreferenceerror;
constexpr auto newsyntaxerror = ::js_newsyntaxerror;
constexpr auto newtypeerror = ::js_newtypeerror;
constexpr auto newurierror = ::js_newurierror;

constexpr auto error = ::js_error;
constexpr auto evalerror = ::js_evalerror;
constexpr auto rangeerror = ::js_rangeerror;
constexpr auto referenceerror = ::js_referenceerror;
constexpr auto syntaxerror = ::js_syntaxerror;
constexpr auto typeerror = ::js_typeerror;
constexpr auto urierror = ::js_urierror;
constexpr auto jsthrow = ::js_throw;

constexpr auto loadstring = ::js_loadstring;
constexpr auto loadfile = ::js_loadfile;

constexpr auto eval = ::js_eval;
constexpr auto call = ::js_call;
constexpr auto construct = ::js_construct;

constexpr auto ref = ::js_ref;
constexpr auto unref = ::js_unref;

constexpr auto getregistry = ::js_getregistry;
constexpr auto setregistry = ::js_setregistry;
constexpr auto delregistry = ::js_delregistry;

constexpr auto getglobal = ::js_getglobal;
constexpr auto setglobal = ::js_setglobal;
constexpr auto defglobal = ::js_defglobal;
constexpr auto delglobal = ::js_delglobal;

constexpr auto hasproperty = ::js_hasproperty;
constexpr auto getproperty = ::js_getproperty;
constexpr auto setproperty = ::js_setproperty;
constexpr auto defproperty = ::js_defproperty;
constexpr auto delproperty = ::js_delproperty;
constexpr auto defaccessor = ::js_defaccessor;

constexpr auto getlength = ::js_getlength;
constexpr auto setlength = ::js_setlength;
constexpr auto hasindex = ::js_hasindex;
constexpr auto getindex = ::js_getindex;
constexpr auto setindex = ::js_setindex;
constexpr auto delindex = ::js_delindex;

constexpr auto currentfunction = ::js_currentfunction;
constexpr auto currentfunctiondata = ::js_currentfunctiondata;
constexpr auto pushglobal = ::js_pushglobal;
constexpr auto pushundefined = ::js_pushundefined;
constexpr auto pushnull = ::js_pushnull;
constexpr auto pushboolean = ::js_pushboolean;
constexpr auto pushnumber = ::js_pushnumber;
constexpr auto pushstring = ::js_pushstring;
constexpr auto pushlstring = ::js_pushlstring;
constexpr auto pushliteral = ::js_pushliteral;

constexpr auto newobjectx = ::js_newobjectx;
constexpr auto newobject = ::js_newobject;
constexpr auto newarray = ::js_newarray;
constexpr auto newboolean = ::js_newboolean;
constexpr auto newnumber = ::js_newnumber;
constexpr auto newstring = ::js_newstring;
constexpr auto newcfunction = ::js_newcfunction;
constexpr auto newcfunctionx = ::js_newcfunctionx;
constexpr auto newcconstructor = ::js_newcconstructor;
constexpr auto newuserdata = ::js_newuserdata;
constexpr auto newuserdatax = ::js_newuserdatax;
constexpr auto newregexp = ::js_newregexp;

constexpr auto pushiterator = ::js_pushiterator;
constexpr auto nextiterator = ::js_nextiterator;

constexpr auto isdefined = ::js_isdefined;
constexpr auto isundefined = ::js_isundefined;
constexpr auto isnull = ::js_isnull;
constexpr auto isboolean = ::js_isboolean;
constexpr auto isnumber = ::js_isnumber;
constexpr auto isstring = ::js_isstring;
constexpr auto isprimitive = ::js_isprimitive;
constexpr auto isobject = ::js_isobject;
constexpr auto isarray = ::js_isarray;
constexpr auto isregexp = ::js_isregexp;
constexpr auto iscoercible = ::js_iscoercible;
constexpr auto iscallable = ::js_iscallable;
constexpr auto isuserdata = ::js_isuserdata;
constexpr auto iserror = ::js_iserror;
constexpr auto isnumberobject = ::js_isnumberobject;
constexpr auto isstringobject = ::js_isstringobject;
constexpr auto isbooleanobject = ::js_isbooleanobject;
constexpr auto isdateobject = ::js_isdateobject;

constexpr auto toboolean = ::js_toboolean;
constexpr auto tonumber = ::js_tonumber;
constexpr auto tostring = ::js_tostring;
constexpr auto touserdata = ::js_touserdata;

constexpr auto trystring = ::js_trystring;
constexpr auto trynumber = ::js_trynumber;
constexpr auto tryinteger = ::js_tryinteger;
constexpr auto tryboolean = ::js_tryboolean;

constexpr auto tointeger = ::js_tointeger;
constexpr auto toint32 = ::js_toint32;
constexpr auto touint32 = ::js_touint32;
constexpr auto toint16 = ::js_toint16;
constexpr auto touint16 = ::js_touint16;

constexpr auto gettop = ::js_gettop;
constexpr auto pop = ::js_pop;
constexpr auto rot = ::js_rot;
constexpr auto copy = ::js_copy;
constexpr auto remove = ::js_remove;
constexpr auto insert = ::js_insert;
constexpr auto replace = ::js_replace;

constexpr auto dup = ::js_dup;
constexpr auto dup2 = ::js_dup2;
constexpr auto rot2 = ::js_rot2;
constexpr auto rot3 = ::js_rot3;
constexpr auto rot4 = ::js_rot4;
constexpr auto rot2pop1 = ::js_rot2pop1;
constexpr auto rot3pop2 = ::js_rot3pop2;

constexpr auto concat = ::js_concat;
constexpr auto compare = ::js_compare;
constexpr auto equal = ::js_equal;
constexpr auto strictequal = ::js_strictequal;
constexpr auto instanceof = ::js_instanceof;
constexpr auto typeof = ::js_typeof;
constexpr auto type = ::js_type;

constexpr auto repr = ::js_repr;
constexpr auto torepr = ::js_torepr;
constexpr auto tryrepr = ::js_tryrepr;

constexpr auto dump = ::js_dump;
constexpr auto create_function = ::js_create_function;
constexpr auto get_global_this = ::js_get_global_this;
constexpr auto pushobject = ::js_pushobject;

auto tofloat(State *j, int idx) -> float;
auto eval_file(State *j, const char *path) -> void;

struct Exception: public std::exception {
    std::string message;

    Exception(::js_State *j);
    [[nodiscard]] auto what() const noexcept -> const char * override;
};

} // namespace muen::js
