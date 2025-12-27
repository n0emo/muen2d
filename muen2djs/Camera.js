var Vector2 = require("muen/Vector2");

/**
 * @class
 * Defines position/orientation in 2D space
 *
 * @constructor
 * @param {Vector2} offset Camera offset (displacement from target)
 * @param {Vector2} target Camera target (rotation and zoom origin)
 * @param {number} rotation Camera rotation in degrees
 * @param {number} zoom Camera zoom (scaling)
 */
function Camera(offset, target, rotation, zoom) {
    if (
        !(offset instanceof Vector2) ||
        !(target instanceof Vector2) ||
        typeof rotation !== "number" ||
        typeof zoom !== "number"
    ) {
        throw TypeError("Invalid arguments for Camera");
    }

    /**
     * Camera offset (displacement from target)
     * @type {Vector2}
     */
    this.offset = offset.clone();

    /**
     * Camera target (rotation and zoom origin)
     * @type {Vector2}
     */
    this.target = target.clone();

    /**
     * Camera rotation in degrees
     * @type {number}
     */
    this.rotation = rotation;

    /**
     * Camera zoom (scaling)
     *  @type {number}
     */
    this.zoom = zoom;
}

/**
 * Constructs camera with default parameters
 * @static
 * @returns {Camera}
 */
Camera.default = function () {
    return new Camera(Vector2.zero(), Vector2.zero(), 0, 1);
};

module.exports = Camera;
