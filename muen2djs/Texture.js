var Rectangle = require("muen/Rectangle");

/**
 * @constructor
 * @param {number} id
 * @param {number} width
 * @param {number} height
 * @param {number} mipmaps
 * @param {number} format
 */
function Texture(id, width, height, mipmaps, format) {
    if (id < 0) {
        throw TypeError("Texture id must be a unsigned integer");
    }

    /** @type number */
    this.id = assertInteger(id);

    /** @type number */
    this.width = assertInteger(width);

    /** @type number */
    this.height = assertInteger(height);

    /** @type number */
    this.mipmaps = assertInteger(mipmaps);

    /** @type number */
    this.format = assertInteger(format);

    /** @type {Rectangle | undefined} */
    this.$source = undefined;
}

/**
 * @param {string} path
 * @returns {Texture}
 */
Texture.load = function (path) {
    var t = __muenTextureLoad(path);
    return new Texture(t.id, t.width, t.height, t.mipmaps, t.format);
};

Texture.prototype.unload = function () {
    __muenTextureUnload(this);
};

Object.defineProperty(Texture.prototype, "valid", {
    get: function () {
        return __muenTextureIsValid(this);
    },
});

Object.defineProperty(Texture.prototype, "source", {
    get: function () {
        if (!this.$source) {
            this.$source = new Rectangle(0, 0, this.width, this.height);
        }

        return this.$source;
    },
});

/**
 * @param {number} n
 */
function assertInteger(n) {
    if ((n | 0) !== n) {
        throw TypeError("Texture param must be integer");
    }
    return n;
}

module.exports = Texture;
