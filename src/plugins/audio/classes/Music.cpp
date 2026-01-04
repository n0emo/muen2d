#include "./Music.hpp"

#include <array>
#include <cassert>

#include <spdlog/spdlog.h>

#include <plugins/audio/audio.hpp>
#include <engine.hpp>

namespace muen::plugins::audio {

auto music_class_id(JSContext *js) -> ::JSClassID {
    static const auto id = [](auto js) -> ::JSClassID {
        auto id = ::JSClassID {};
        ::JS_NewClassID(JS_GetRuntime(js), &id);
        return id;
    }(js);

    return id;
}

} // namespace muen::plugins::audio

namespace muen::plugins::audio::music_class {

static inline auto get_music(JSContext *js, JSValueConst val) -> Music * {
    auto music = static_cast<Music *>(JS_GetOpaque(val, music_class_id(js)));
    assert(music);
    return music;
}

static auto music_constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    auto e = engine::from_js(js);
    const char *filename = ::JS_ToCString(js, argv[0]);
    if (!filename) {
        return ::JS_ThrowTypeError(js, "Invalid filename");
    }

    const auto path = engine::resolve_path(e, filename);
    const auto result = music::load(path);
    ::JS_FreeCString(js, filename);
    if (!result.has_value()) {
        return ::JS_ThrowInternalError(js, "Could not load music: %s", result.error().c_str());
    }
    audio::get().musics.insert(*result);

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    auto obj = JS_NewObjectProtoClass(js, proto, music_class_id(js));
    JS_FreeValue(js, proto);

    JS_SetOpaque(obj, *result);

    return obj;
}

// Instance methods
static auto music_unload(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    auto m = get_music(js, this_val);
    audio::get().musics.erase(m);
    music::unload(m);
    return ::JS_UNDEFINED;
}

static auto music_play(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    auto m = get_music(js, this_val);
    music::play(*m);
    return ::JS_UNDEFINED;
}

static auto music_stop(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    auto m = get_music(js, this_val);
    music::stop(*m);
    return ::JS_UNDEFINED;
}

static auto music_pause(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    auto m = get_music(js, this_val);
    music::pause(*m);
    return ::JS_UNDEFINED;
}

static auto music_resume(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    auto m = get_music(js, this_val);
    music::resume(*m);
    return ::JS_UNDEFINED;
}

static auto music_seek(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    double cursor = 0;
    ::JS_ToFloat64(js, &cursor, argv[0]);

    auto m = get_music(js, this_val);
    music::seek(*m, static_cast<float>(cursor));

    return ::JS_UNDEFINED;
}

static auto music_get_playing(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto m = get_music(js, this_val);
    return ::JS_NewBool(js, music::is_playing(*m));
}

static auto music_get_looping(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto m = get_music(js, this_val);
    return ::JS_NewBool(js, music::get_looping(*m));
}

static auto music_get_volume(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto m = get_music(js, this_val);
    return ::JS_NewFloat64(js, music::get_volume(*m));
}

static auto music_get_pan(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto m = get_music(js, this_val);
    return ::JS_NewFloat64(js, music::get_pan(*m));
}

static auto music_get_pitch(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto m = get_music(js, this_val);
    return ::JS_NewFloat64(js, music::get_pitch(*m));
}

static auto music_set_looping(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    const auto looping = ::JS_ToBool(js, val);
    auto m = get_music(js, this_val);
    music::set_looping(*m, looping);
    return ::JS_UNDEFINED;
}

static auto music_set_volume(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    double volume = 0;
    ::JS_ToFloat64(js, &volume, val);

    auto m = get_music(js, this_val);
    music::set_volume(*m, static_cast<float>(volume));
    return ::JS_UNDEFINED;
}

static auto music_set_pan(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    double pan = 0;
    ::JS_ToFloat64(js, &pan, val);

    auto m = get_music(js, this_val);
    music::set_pan(*m, static_cast<float>(pan));
    return ::JS_UNDEFINED;
}

static auto music_set_pitch(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    double pitch = 0;
    ::JS_ToFloat64(js, &pitch, val);
    auto m = get_music(js, this_val);
    music::set_pitch(*m, static_cast<float>(pitch));
    return ::JS_UNDEFINED;
}

static const auto proto_funcs = std::array {
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

static ::JSClassDef music_class = {
    .class_name = "Music",
    .finalizer = nullptr,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(::JSContext *js) -> ::JSModuleDef * {
    auto m = ::JS_NewCModule(js, "muen:music", [](auto js, auto m) -> int {
        ::JS_NewClass(::JS_GetRuntime(js), music_class_id(js), &music_class);

        ::JSValue proto = ::JS_NewObject(js);
        ::JS_SetPropertyFunctionList(js, proto, proto_funcs.data(), proto_funcs.size());
        ::JS_SetClassProto(js, music_class_id(js), proto);

        ::JSValue ctor = ::JS_NewCFunction2(js, music_constructor, "Music", 1, ::JS_CFUNC_constructor, 0);
        ::JS_SetConstructor(js, ctor, proto);

        ::JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    ::JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::plugins::audio::music_class
