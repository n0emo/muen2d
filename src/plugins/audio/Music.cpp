#include <plugins/audio.hpp>

#include <array>
#include <cassert>

#include <spdlog/spdlog.h>

#include <engine/audio.hpp>
#include <engine.hpp>

namespace muen::plugins::audio::music_class {

namespace audio = engine::audio;
namespace music = engine::audio::music;

auto class_id(JSContext *js) -> ::JSClassID {
    static const auto id = [](auto js) -> ::JSClassID {
        auto id = ::JSClassID {};
        ::JS_NewClassID(JS_GetRuntime(js), &id);
        return id;
    }(js);

    return id;
}

auto from_value_unsafe(JSContext *js, JSValueConst val) -> audio::Music * {
    auto music = static_cast<audio::Music *>(JS_GetOpaque(val, class_id(js)));
    return music;
}

static auto music_constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    if (argc != 1) {
        JS_ThrowTypeError(js, "Music constructor expects 1 argument, but %d were given", argc);
    }
    auto e = engine::from_js(js);
    const char *filename = ::JS_ToCString(js, argv[0]);
    if (!filename) {
        return ::JS_ThrowTypeError(js, "Invalid filename");
    }

    const auto path = engine::resolve_path(e, filename);
    const auto result = audio::music::load(path);
    ::JS_FreeCString(js, filename);
    if (!result.has_value()) {
        return JS_ThrowInternalError(js, "%s", fmt::format("Could not load music: {}", result.error()->msg()).c_str());
    }
    audio::get().musics.insert(*result);

    auto proto = ::JS_GetPropertyStr(js, new_target, "prototype");
    auto obj = ::JS_NewObjectProtoClass(js, proto, class_id(js));
    ::JS_FreeValue(js, proto);

    ::JS_SetOpaque(obj, *result);

    return obj;
}

static auto music_unload(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *) -> ::JSValue {
    if (argc != 0) {
        JS_ThrowTypeError(js, "Music.unload expects no arguments, but %d were given", argc);
    }

    auto m = from_value_unsafe(js, this_val);
    audio::get().musics.erase(m);
    music::unload(m);
    return ::JS_UNDEFINED;
}

static auto music_play(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *) -> ::JSValue {
    if (argc != 0) {
        JS_ThrowTypeError(js, "Music.play expects no arguments, but %d were given", argc);
    }

    auto m = from_value_unsafe(js, this_val);
    music::play(*m);
    return ::JS_UNDEFINED;
}

static auto music_stop(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *) -> ::JSValue {
    if (argc != 0) {
        JS_ThrowTypeError(js, "Music.stop expects no arguments, but %d were given", argc);
    }

    auto m = from_value_unsafe(js, this_val);
    music::stop(*m);
    return ::JS_UNDEFINED;
}

static auto music_pause(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *) -> ::JSValue {
    if (argc != 0) {
        JS_ThrowTypeError(js, "Music.pause expects no arguments, but %d were given", argc);
    }

    auto m = from_value_unsafe(js, this_val);
    music::pause(*m);
    return ::JS_UNDEFINED;
}

static auto music_resume(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *) -> ::JSValue {
    if (argc != 0) {
        JS_ThrowTypeError(js, "Music.resume expects no arguments, but %d were given", argc);
    }

    auto m = from_value_unsafe(js, this_val);
    music::resume(*m);
    return ::JS_UNDEFINED;
}

static auto music_seek(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    if (argc != 1) {
        JS_ThrowTypeError(js, "Music.resume expects 1 argument, but %d were given", argc);
    }

    double cursor = 0;
    ::JS_ToFloat64(js, &cursor, argv[0]);

    auto m = from_value_unsafe(js, this_val);
    music::seek(*m, static_cast<float>(cursor));

    return ::JS_UNDEFINED;
}

static auto music_get_playing(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto m = from_value_unsafe(js, this_val);
    return ::JS_NewBool(js, music::is_playing(*m));
}

static auto music_get_looping(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto m = from_value_unsafe(js, this_val);
    return ::JS_NewBool(js, music::get_looping(*m));
}

static auto music_get_volume(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto m = from_value_unsafe(js, this_val);
    return ::JS_NewFloat64(js, music::get_volume(*m));
}

static auto music_get_pan(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto m = from_value_unsafe(js, this_val);
    return ::JS_NewFloat64(js, music::get_pan(*m));
}

static auto music_get_pitch(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto m = from_value_unsafe(js, this_val);
    return ::JS_NewFloat64(js, music::get_pitch(*m));
}

static auto music_set_looping(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    const auto looping = ::JS_ToBool(js, val);
    auto m = from_value_unsafe(js, this_val);
    music::set_looping(*m, looping);
    return ::JS_UNDEFINED;
}

static auto music_set_volume(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    double volume = 0;
    ::JS_ToFloat64(js, &volume, val);

    auto m = from_value_unsafe(js, this_val);
    music::set_volume(*m, static_cast<float>(volume));
    return ::JS_UNDEFINED;
}

static auto music_set_pan(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    double pan = 0;
    ::JS_ToFloat64(js, &pan, val);

    auto m = from_value_unsafe(js, this_val);
    music::set_pan(*m, static_cast<float>(pan));
    return ::JS_UNDEFINED;
}

static auto music_set_pitch(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    double pitch = 0;
    ::JS_ToFloat64(js, &pitch, val);
    auto m = from_value_unsafe(js, this_val);
    music::set_pitch(*m, static_cast<float>(pitch));
    return ::JS_UNDEFINED;
}

static const auto PROTO_FUNCS = std::array {
    ::JSCFunctionListEntry JS_CFUNC_DEF("unload", 0, music_unload),
    ::JSCFunctionListEntry JS_CFUNC_DEF("play", 0, music_play),
    ::JSCFunctionListEntry JS_CFUNC_DEF("stop", 0, music_stop),
    ::JSCFunctionListEntry JS_CFUNC_DEF("pause", 0, music_pause),
    ::JSCFunctionListEntry JS_CFUNC_DEF("resume", 0, music_resume),
    ::JSCFunctionListEntry JS_CFUNC_DEF("seek", 1, music_seek),
    ::JSCFunctionListEntry JS_CGETSET_DEF("playing", music_get_playing, nullptr),
    ::JSCFunctionListEntry JS_CGETSET_DEF("looping", music_get_looping, music_set_looping),
    ::JSCFunctionListEntry JS_CGETSET_DEF("volume", music_get_volume, music_set_volume),
    ::JSCFunctionListEntry JS_CGETSET_DEF("pan", music_get_pan, music_set_pan),
    ::JSCFunctionListEntry JS_CGETSET_DEF("pitch", music_get_pitch, music_set_pitch),
};

static const auto MUSIC_CLASS = ::JSClassDef {
    .class_name = "Music",
    .finalizer = nullptr,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(::JSContext *js) -> ::JSModuleDef * {
    auto m = ::JS_NewCModule(js, "muen:music", [](auto js, auto m) -> int {
        ::JS_NewClass(::JS_GetRuntime(js), class_id(js), &MUSIC_CLASS);

        ::JSValue proto = ::JS_NewObject(js);
        ::JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        ::JS_SetClassProto(js, class_id(js), proto);

        ::JSValue ctor = ::JS_NewCFunction2(js, music_constructor, "Music", 1, ::JS_CFUNC_constructor, 0);
        ::JS_SetConstructor(js, ctor, proto);

        ::JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    ::JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::plugins::audio::music_class
