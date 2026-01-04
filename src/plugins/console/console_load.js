import console from "muen:console";

Object.defineProperty(globalThis, "console", {
    value: console,
    writable: false,
    configurable: false,
    enumerable: true,
});
