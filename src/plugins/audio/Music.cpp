#include <plugins/audio.hpp>

#include <array>
#include <cassert>
#include <gsl/gsl>

#include <spdlog/spdlog.h>

#include <engine/audio.hpp>
#include <engine.hpp>

namespace muen::js {

template<>
auto try_into<engine::audio::Music *>(const Value& val) noexcept -> JSResult<engine::audio::Music *> {
    const auto id = js::class_id<&plugins::audio::music_class::MUSIC>(val.ctx());
    auto ptr = static_cast<engine::audio::Music *>(JS_GetOpaque(val.cget(), id));
    if (ptr == nullptr) return Unexpected(JSError::type_error(val.ctx(), "Not an instance of Music"));
    return ptr;
}

} // namespace muen::js

namespace muen::plugins::audio::music_class {

using namespace gsl;

namespace audio = engine::audio;
namespace music = engine::audio::music;

static auto music_constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    auto& e = Engine::get(js);
    auto args = js::unpack_args<std::string>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [filename] = *args;
    auto music_result = audio::music::load(filename, e.file_store());
    if (!music_result)
        return JS_ThrowInternalError(
            js,
            "%s",
            fmt::format("Could not load music: {}", music_result.error()->msg()).c_str()
        );
    auto music = owner<Music *>(new Music {std::move(*music_result)});

    audio::get().musics.insert(music);

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&MUSIC>(js));
    JS_FreeValue(js, proto);

    JS_SetOpaque(obj, music);

    return obj;
}

static auto music_unload(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    auto ptr = owner<Music *>(*m);
    audio::get().musics.erase(ptr);
    delete ptr;
    return JS_UNDEFINED;
}

static auto music_play(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    music::play(**m);
    return JS_UNDEFINED;
}

static auto music_stop(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    music::stop(**m);
    return JS_UNDEFINED;
}

static auto music_pause(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    music::pause(**m);
    return JS_UNDEFINED;
}

static auto music_resume(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    music::resume(**m);
    return JS_UNDEFINED;
}

static auto music_seek(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    const auto args = js::unpack_args<float>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [cursor] = *args;
    music::seek(**m, cursor);
    return JS_UNDEFINED;
}

static auto music_get_playing(JSContext *js, JSValueConst this_val) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    return JS_NewBool(js, music::is_playing(**m));
}

static auto music_get_looping(JSContext *js, JSValueConst this_val) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    return JS_NewBool(js, music::get_looping(**m));
}

static auto music_get_volume(JSContext *js, JSValueConst this_val) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    return JS_NewFloat64(js, music::get_volume(**m));
}

static auto music_get_pan(JSContext *js, JSValueConst this_val) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    return JS_NewFloat64(js, music::get_pan(**m));
}

static auto music_get_pitch(JSContext *js, JSValueConst this_val) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    return JS_NewFloat64(js, music::get_pitch(**m));
}

static auto music_set_looping(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());

    const auto looping = js::try_into<bool>(js::borrow(js, val));
    if (!looping) return jsthrow(looping.error());
    music::set_looping(**m, *looping);
    return JS_UNDEFINED;
}

static auto music_set_volume(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    const auto volume = js::try_into<float>(js::borrow(js, val));
    if (!volume) return jsthrow(volume.error());
    music::set_volume(**m, *volume);
    return JS_UNDEFINED;
}

static auto music_set_pan(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    const auto pan = js::try_into<float>(js::borrow(js, val));
    if (!pan) return jsthrow(pan.error());
    music::set_pan(**m, *pan);
    return JS_UNDEFINED;
}

static auto music_set_pitch(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto m = try_into<audio::Music *>(js::borrow(js, this_val));
    if (!m) return jsthrow(m.error());
    const auto pitch = js::try_into<float>(js::borrow(js, val));
    if (!pitch) return jsthrow(pitch.error());
    music::set_pitch(**m, *pitch);
    return JS_UNDEFINED;
}

static const auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CFUNC_DEF("unload", 0, music_unload),
    JSCFunctionListEntry JS_CFUNC_DEF("play", 0, music_play),
    JSCFunctionListEntry JS_CFUNC_DEF("stop", 0, music_stop),
    JSCFunctionListEntry JS_CFUNC_DEF("pause", 0, music_pause),
    JSCFunctionListEntry JS_CFUNC_DEF("resume", 0, music_resume),
    JSCFunctionListEntry JS_CFUNC_DEF("seek", 1, music_seek),
    JSCFunctionListEntry JS_CGETSET_DEF("playing", music_get_playing, nullptr),
    JSCFunctionListEntry JS_CGETSET_DEF("looping", music_get_looping, music_set_looping),
    JSCFunctionListEntry JS_CGETSET_DEF("volume", music_get_volume, music_set_volume),
    JSCFunctionListEntry JS_CGETSET_DEF("pan", music_get_pan, music_set_pan),
    JSCFunctionListEntry JS_CGETSET_DEF("pitch", music_get_pitch, music_set_pitch),
};

extern const JSClassDef MUSIC = {
    .class_name = "Music",
    .finalizer = nullptr,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:music", [](auto js, auto m) -> int {
        JS_NewClass(JS_GetRuntime(js), js::class_id<&MUSIC>(js), &MUSIC);

        JSValue proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, js::class_id<&MUSIC>(js), proto);

        JSValue ctor = JS_NewCFunction2(js, music_constructor, "Music", 1, JS_CFUNC_constructor, 0);
        JS_SetConstructor(js, ctor, proto);

        JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::plugins::audio::music_class
