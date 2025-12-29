/**
 * @module muen
 */

/**
 * Provides access to the debugging console.
 * @namespace
 *
 * @example
 * ```js
 * // Logs "[info]: 2 + 2 = 4 as expected"
 * console.log("2 + 2 =", 4, "as expected");
 * ```
 */
declare interface console {
    /**
     * Outputs a message to the console with the trace log level.
     */
    trace(...args: unknown[]): void;

    /**
     * Outputs a message to the console with the debug log level.
     */
    debug(...args: unknown[]): void;

    /**
     * Outputs a message to the console with the info log level.
     */
    log(...args: unknown[]): void;

    /**
     * Outputs a message to the console with the warn log level.
     */
    warn(...args: unknown[]): void;

    /**
     * Outputs a message to the console with the error log level.
     */
    error(...args: unknown[]): void;
}

/**
 * @module
 * @hidden
 */
declare module "muen/console" {
    const console: console;
    export = console;
}
