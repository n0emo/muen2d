import console from "glint:console";

Object.defineProperty(globalThis, "console", {
    value: console,
    writable: false,
    configurable: false,
    enumerable: true,
});
