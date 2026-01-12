#include <plugins/audio.hpp>

#include <array>
#include <cassert>

#include <spdlog/spdlog.h>

#include <engine.hpp>

namespace muen::plugins::audio::sound_class {

namespace audio = engine::audio;
namespace sound = engine::audio::sound;

auto sound_class_id(::JSContext *js) -> ::JSClassID {
    static const auto id = [](auto js) -> ::JSClassID {
        auto id = ::JSClassID {};
        ::JS_NewClassID(JS_GetRuntime(js), &id);
        return id;
    }(js);

    return id;
}

auto from_value_unsafe(::JSContext *js, ::JSValueConst val) -> audio::Sound * {
    auto sound = static_cast<audio::Sound *>(JS_GetOpaque(val, sound_class_id(js)));
    return sound;
}

static auto sound_constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    if (argc != 1) {
        return JS_ThrowTypeError(js, "Sound constructor expects 1 argument, but %d were given", argc);
    }

    auto e = engine::from_js(js);
    const char *filename = ::JS_ToCString(js, argv[0]);
    if (!filename) {
        return ::JS_ThrowTypeError(js, "Invalid filename");
    }

    const auto path = engine::resolve_path(e, filename);
    const auto result = sound::load(path);
    ::JS_FreeCString(js, filename);
    if (!result.has_value()) {
        return ::JS_ThrowInternalError(js, "Could not load sound: %s", result.error().c_str());
    }
    audio::get().sounds.insert(*result);

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    auto obj = JS_NewObjectProtoClass(js, proto, sound_class_id(js));
    JS_FreeValue(js, proto);

    JS_SetOpaque(obj, *result);

    return obj;
}

static auto sound_unload(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *) -> ::JSValue {
    if (argc != 0) {
        return JS_ThrowTypeError(js, "Sound.unload expects no arguments, but %d were given", argc);
    }

    auto s = from_value_unsafe(js, this_val);
    audio::get().sounds.erase(s);
    sound::unload(s);
    return ::JS_UNDEFINED;
}

static auto sound_play(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *) -> ::JSValue {
    if (argc != 0) {
        return JS_ThrowTypeError(js, "Sound.play expects no arguments, but %d were given", argc);
    }

    auto s = from_value_unsafe(js, this_val);
    sound::play(*s);
    return ::JS_UNDEFINED;
}

static auto sound_stop(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *) -> ::JSValue {
    if (argc != 0) {
        return JS_ThrowTypeError(js, "Sound.stop expects no arguments, but %d were given", argc);
    }

    auto s = from_value_unsafe(js, this_val);
    sound::stop(*s);
    return ::JS_UNDEFINED;
}

static auto sound_get_playing(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto s = from_value_unsafe(js, this_val);
    return ::JS_NewBool(js, sound::is_playing(*s));
}

static auto sound_get_volume(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto s = from_value_unsafe(js, this_val);
    return ::JS_NewFloat64(js, sound::get_volume(*s));
}

static auto sound_get_pan(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto s = from_value_unsafe(js, this_val);
    return ::JS_NewFloat64(js, sound::get_pan(*s));
}

static auto sound_get_pitch(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto s = from_value_unsafe(js, this_val);
    return ::JS_NewFloat64(js, sound::get_pitch(*s));
}

// Setters
static auto sound_set_volume(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    double volume = 0;
    ::JS_ToFloat64(js, &volume, val);
    auto s = from_value_unsafe(js, this_val);
    sound::set_volume(*s, static_cast<float>(volume));
    return ::JS_UNDEFINED;
}

static auto sound_set_pan(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    double pan = 0;
    ::JS_ToFloat64(js, &pan, val);
    auto s = from_value_unsafe(js, this_val);
    sound::set_pan(*s, static_cast<float>(pan));
    return ::JS_UNDEFINED;
}

static auto sound_set_pitch(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    double pitch = 0;
    ::JS_ToFloat64(js, &pitch, val);
    auto s = from_value_unsafe(js, this_val);
    sound::set_pitch(*s, static_cast<float>(pitch));
    return ::JS_UNDEFINED;
}

const static auto PROTO_FUNCS = ::std::array {
    ::JSCFunctionListEntry JS_CFUNC_DEF("unload", 0, sound_unload),
    ::JSCFunctionListEntry JS_CFUNC_DEF("play", 0, sound_play),
    ::JSCFunctionListEntry JS_CFUNC_DEF("stop", 0, sound_stop),
    ::JSCFunctionListEntry JS_CGETSET_DEF("playing", sound_get_playing, nullptr),
    ::JSCFunctionListEntry JS_CGETSET_DEF("volume", sound_get_volume, sound_set_volume),
    ::JSCFunctionListEntry JS_CGETSET_DEF("pan", sound_get_pan, sound_set_pan),
    ::JSCFunctionListEntry JS_CGETSET_DEF("pitch", sound_get_pitch, sound_set_pitch),
};

static const auto SOUND_CLASS = ::JSClassDef {
    .class_name = "Sound",
    .finalizer = nullptr,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(::JSContext *js) -> ::JSModuleDef * {
    auto m = ::JS_NewCModule(js, "muen:sound", [](auto js, auto m) -> int {
        ::JS_NewClass(::JS_GetRuntime(js), sound_class_id(js), &SOUND_CLASS);

        ::JSValue proto = ::JS_NewObject(js);
        ::JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int{PROTO_FUNCS.size()});
        ::JS_SetClassProto(js, sound_class_id(js), proto);

        ::JSValue ctor = ::JS_NewCFunction2(js, sound_constructor, "Sound", 1, ::JS_CFUNC_constructor, 0);
        ::JS_SetConstructor(js, ctor, proto);

        ::JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    ::JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::plugins::audio::SoundJS
