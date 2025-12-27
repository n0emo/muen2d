/**
 * @namespace screen
 * @property {number} dt Time since last frame in seconds
 * @property {number} width Width of the game screen
 * @property {number} height Height of the game screen
 */
var screen = {};

Object.defineProperty(screen, "dt", {
    enumerable: false,
    configurable: false,
    get: function () {
        // @ts-ignore
        return __muenScreenDt();
    },
});

Object.defineProperty(screen, "width", {
    get: function () {
        // @ts-ignore
        return __muenScreenWidth();
    },
    enumerable: false,
    configurable: false,
});

Object.defineProperty(screen, "height", {
    get: function () {
        // @ts-ignore
        return __muenScreenHeight();
    },
    enumerable: false,
    configurable: false,
});

module.exports = Object.freeze(screen);
