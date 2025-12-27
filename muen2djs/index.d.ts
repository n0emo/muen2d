import Console = require("muen/Console");

declare global {
    function require(name: string): any;
    const console: Console;
}
