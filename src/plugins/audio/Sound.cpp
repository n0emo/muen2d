#include <plugins/audio.hpp>

#include <array>
#include <cassert>
#include <gsl/gsl>

#include <spdlog/spdlog.h>

#include <engine.hpp>

namespace muen::js {

template<>
auto try_into<engine::audio::Sound *>(const Value& val) noexcept -> JSResult<engine::audio::Sound *> {
    const auto id = class_id<&plugins::audio::sound_class::SOUND>(val.ctx());
    auto ptr = static_cast<engine::audio::Sound *>(JS_GetOpaque(val.cget(), id));
    if (ptr == nullptr) return Unexpected(JSError::type_error(val.ctx(), "Not an instance of Sound"));
    return ptr;
}

} // namespace muen::js

namespace muen::plugins::audio::sound_class {

using namespace gsl;

namespace audio = engine::audio;
namespace sound = engine::audio::sound;

static auto sound_constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    auto& e = Engine::get(js);
    auto args = js::unpack_args<std::string>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [filename] = *args;
    auto sound_result = audio::sound::load(filename, e.file_store());
    if (!sound_result)
        return JS_ThrowInternalError(
            js,
            "%s",
            fmt::format("Could not load sound: {}", sound_result.error()->msg()).c_str()
        );
    auto sound = owner<Sound *>(new Sound {std::move(*sound_result)});
    audio::get().sounds.insert(sound);

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&SOUND>(js));
    JS_FreeValue(js, proto);

    JS_SetOpaque(obj, sound);

    return obj;
}

static auto sound_unload(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto s = js::try_into<audio::Sound *>(js::borrow(js, this_val));
    if (!s) return jsthrow(s.error());
    auto ptr = owner<Sound *>(*s);
    audio::get().sounds.erase(ptr);
    delete ptr;
    return JS_UNDEFINED;
}

static auto sound_play(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto s = js::try_into<audio::Sound *>(js::borrow(js, this_val));
    if (!s) return jsthrow(s.error());
    sound::play(**s);
    return JS_UNDEFINED;
}

static auto sound_stop(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto s = js::try_into<audio::Sound *>(js::borrow(js, this_val));
    if (!s) return jsthrow(s.error());
    sound::stop(**s);
    return JS_UNDEFINED;
}

static auto sound_get_playing(JSContext *js, JSValueConst this_val) -> JSValue {
    auto s = js::try_into<audio::Sound *>(js::borrow(js, this_val));
    if (!s) return jsthrow(s.error());
    return JS_NewBool(js, sound::is_playing(**s));
}

static auto sound_get_volume(JSContext *js, JSValueConst this_val) -> JSValue {
    auto s = js::try_into<audio::Sound *>(js::borrow(js, this_val));
    if (!s) return jsthrow(s.error());
    return JS_NewFloat64(js, sound::get_volume(**s));
}

static auto sound_get_pan(JSContext *js, JSValueConst this_val) -> JSValue {
    auto s = js::try_into<audio::Sound *>(js::borrow(js, this_val));
    if (!s) return jsthrow(s.error());
    return JS_NewFloat64(js, sound::get_pan(**s));
}

static auto sound_get_pitch(JSContext *js, JSValueConst this_val) -> JSValue {
    auto s = js::try_into<audio::Sound *>(js::borrow(js, this_val));
    if (!s) return jsthrow(s.error());
    return JS_NewFloat64(js, sound::get_pitch(**s));
}

static auto sound_set_volume(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto s = js::try_into<audio::Sound *>(js::borrow(js, this_val));
    if (!s) return jsthrow(s.error());
    auto volume = js::try_into<float>(js::borrow(js, val));
    if (!volume) return jsthrow(volume.error());
    sound::set_volume(**s, *volume);
    return JS_UNDEFINED;
}

static auto sound_set_pan(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto s = js::try_into<audio::Sound *>(js::borrow(js, this_val));
    if (!s) return jsthrow(s.error());
    auto pan = js::try_into<float>(js::borrow(js, val));
    if (!pan) return jsthrow(pan.error());
    sound::set_pan(**s, *pan);
    return JS_UNDEFINED;
}

static auto sound_set_pitch(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto s = js::try_into<audio::Sound *>(js::borrow(js, this_val));
    if (!s) return jsthrow(s.error());
    auto pitch = js::try_into<float>(js::borrow(js, val));
    if (!pitch) return jsthrow(pitch.error());
    sound::set_pitch(**s, *pitch);
    return JS_UNDEFINED;
}

const static auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CFUNC_DEF("unload", 0, sound_unload),
    JSCFunctionListEntry JS_CFUNC_DEF("play", 0, sound_play),
    JSCFunctionListEntry JS_CFUNC_DEF("stop", 0, sound_stop),
    JSCFunctionListEntry JS_CGETSET_DEF("playing", sound_get_playing, nullptr),
    JSCFunctionListEntry JS_CGETSET_DEF("volume", sound_get_volume, sound_set_volume),
    JSCFunctionListEntry JS_CGETSET_DEF("pan", sound_get_pan, sound_set_pan),
    JSCFunctionListEntry JS_CGETSET_DEF("pitch", sound_get_pitch, sound_set_pitch),
};

static const auto SOUND_CLASS = JSClassDef {
    .class_name = "Sound",
    .finalizer = nullptr,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:sound", [](auto js, auto m) -> int {
        const auto id = js::class_id<&SOUND>(js);
        JS_NewClass(JS_GetRuntime(js), id, &SOUND_CLASS);

        JSValue proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, id, proto);

        JSValue ctor = JS_NewCFunction2(js, sound_constructor, "Sound", 1, JS_CFUNC_constructor, 0);
        JS_SetConstructor(js, ctor, proto);

        JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::plugins::audio::sound_class
