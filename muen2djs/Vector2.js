/**
 * @class
 * Represents 2-dimensional vector {x,y}
 *
 * @constructor
 * @param {number} x Vector x component
 * @param {number} y Vector y component
 */
function Vector2(x, y) {
    if (typeof x !== "number" || typeof y !== "number") {
        throw TypeError("Invalid arguments for Vector2");
    }

    /**
     * Vector x component
     * @type {number}
     */
    this.x = x;

    /**
     * Vector y component
     * @type {number}
     */
    this.y = y;
}

/**
 * Creates zero initialized Vector2
 * @static
 * @returns {Vector2}
 */
Vector2.zero = function () {
    return new Vector2(0, 0);
};

/**
 * Clones vector creating new instance
 * @returns {Vector2}
 */
Vector2.prototype.clone = function () {
    return new Vector2(this.x, this.y);
};

module.exports = Vector2;
