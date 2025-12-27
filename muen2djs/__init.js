/**
 * @global
 * @function
 * @param {string} name
 * @returns {any}
 */
var require;

Object.defineProperty(this, "require", {
    value: (function () {
        var require = Function(
            "name",
            "var cache = require.cache;" +
                "if (name in cache) {" +
                "   return cache[name];" +
                "}" +
                "var module = {};" +
                "__muenFsLoad(name + '.js')(module);" +
                "cache[name] = module.exports;" +
                "return module.exports;"
        );

        Object.defineProperty(require, "cache", {
            value: Object.create(null),
            writable: false,
            configurable: false,
            enumerable: false,
        });

        return require;
    })(),
    writable: false,
    configurable: false,
    enumerable: false,
});

var Console = require("muen/Console");

/**
 * @global
 * @constant
 * @type {Console}
 */
// biome-ignore lint/correctness/noUnusedVariables: variable for JSDoc
var console;

Object.defineProperty(this, "console", {
    value: new Console(),
    writable: false,
    configurable: false,
    enumerable: true,
});

// @ts-ignore
Console = undefined;
