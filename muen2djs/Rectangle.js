var Vector2 = require("muen/Vector2");

/**
 * @class
 * Rectangle, 4 components
 *
 * @constructor
 * @param {number} x
 * @param {number} y
 * @param {number} width
 * @param {number} height
 */
function Rectangle(x, y, width, height) {
    if (typeof x !== "number" || typeof y !== "number" || typeof width !== "number" || typeof height !== "number") {
        throw TypeError("Invalid arguments for Rectangle");
    }

    /**
     * Rectangle top-left corner position x
     * @type {number}
     */
    this.x = x;

    /**
     * Rectangle top-left corner position y
     * @type {number}
     */
    this.y = y;

    /**
     * Rectangle width
     * @type {number}
     */
    this.width = width;

    /**
     * Rectangle height
     * @type {number}
     */
    this.height = height;
}

/**
 * Creates zero initialized rectangle
 * @returns {Rectangle}
 */
Rectangle.zero = function () {
    return new Rectangle(0, 0, 0, 0);
};

/**
 * Creates vector with top-left corner set to `position` and size set to `size`
 * @param {Vector2} position
 * @param {Vector2} size
 */
Rectangle.fromVectors = function (position, size) {
    if (!(position instanceof Vector2) || !(size instanceof Vector2)) {
        throw new TypeError("Invalid arguments for Rectangle.fromVectors");
    }

    return new Rectangle(position.x, position.y, size.x, size.y);
};

module.exports = Rectangle;
