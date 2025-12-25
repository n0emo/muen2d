function Color() {
    function parse_byte(s, i) {
        return parseInt(s.slice(i, i + 2), 16);
    }

    function assertByte(n) {
        if ((n | 0) !== n) {
            throw TypeError("Color byte must be integer");
        }

        if (n < 0 || n >= 256) {
            throw TypeError("Color byte must be in range [0; 255]");
        }

        return n;
    }

    if (arguments.length === 1) {
        var str = arguments[0];
        if (typeof str !== 'string' || (str.length != 7 && str.length != 9) || str[0] != '#') {
            throw TypeError("Invalid arguments for Color");
        }

        this.r = parse_byte(str, 1);
        this.g = parse_byte(str, 3);
        this.b = parse_byte(str, 5);
        this.a = str.length == 9 ? parse_byte(str, 7) : 255;
    } else if (arguments.length == 3 || arguments.length == 4) {
        this.r = assertByte(arguments[0]);
        this.g = assertByte(arguments[1]);
        this.b = assertByte(arguments[2]);
        this.a = arguments.length == 4 ? assertByte(arguments[3]) : 255;
    } else {
        throw TypeError("Invalid arguments for Color");
    }
}

function Vector2() {
    if (arguments.length === 0) {
        this.x = 0;
        this.y = 0;

    } else if (arguments.length === 2) {
        var x = arguments[0];
        var y = arguments[1];
        if (typeof x !== 'number' || typeof y !== 'number') {
            throw TypeError("Invalid arguments for Vector2");
        }
        this.x = x;
        this.y = y;
    } else {
        throw TypeError("Invalid arguments for Vector2")
    }
}

function Rectangle() {
    if (arguments.length === 0) {
        this.x = 0;
        this.y = 0;
        this.width = 0;
        this.height = 0;

    } else if (arguments.length === 4) {
        var x = arguments[0];
        var y = arguments[1];
        var width = arguments[2];
        var height = arguments[3];
        if (typeof x !== 'number' || typeof y !== 'number' || typeof width !== 'number' || typeof height !== 'number') {
            throw TypeError("Invalid arguments for Rectangle");
        }
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
    } else {
        throw TypeError("Invalid arguments for Rectangle")
    }
}


function require(name) {
    var cache = require.cache;
    if (name in cache) {
        return cache[name];
    }

    var module = {};
    fs.load(name + '.js')(module);
    cache[name] = module.exports;
    return module.exports;
}

require.cache = Object.create(null);
