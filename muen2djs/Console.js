/**
 * @class
 * @constructor
 */
function Console() {}

/**
 * @function
 * @param {...unknown} args
 * @returns {void}
 */ // @ts-ignore
Console.prototype.trace = __muenConsoleTrace;

/**
 * @function
 * @param {...unknown} args
 * @returns {void}
 */ // @ts-ignore
Console.prototype.debug = __muenConsoleDebug;

/**
 * @function
 * @param {...unknown} args
 * @returns {void}
 */ // @ts-ignore
Console.prototype.log = __muenConsoleLog;

/**
 * @function
 * @param {...unknown} args
 * @returns {void}
 */ // @ts-ignore
Console.prototype.warn = __muenConsoleWarn;

/**
 * @function
 * @param {...unknown} args
 * @returns {void}
 */ // @ts-ignore
Console.prototype.error = __muenConsoleError;

module.exports = Console;
