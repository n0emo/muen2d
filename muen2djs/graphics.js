/**
 * @namespace
 */
var graphics = {
    /**
     * @function
     * @param {Color} color
     * @returns {void}
     */ // @ts-ignore
    clear: __muenGraphicsClear,

    /**
     * @function
     * @param {number} x
     * @param {number} y
     * @param {number} radius
     * @param {Color} color
     * @returns {void}
     */ // @ts-ignore
    circle: __muenGraphicsCircle,

    /**
     * @function
     * @param {number} x
     * @param {number} y
     * @param {number} width
     * @param {number} height
     * @param {Color} color
     * @returns {void}
     */ // @ts-ignore
    rectangle: __muenGraphicsRectangle,

    /**
     * @function
     * @param {Vector2} position
     * @param {Vector2} size
     * @param {Color} color
     * @returns {void}
     */ // @ts-ignore
    rectangleV: __muenGraphicsRectangle,

    /**
     * @function
     * @param {Rectangle} rectangle
     * @param {Color} color
     * @returns {void}
     */ // @ts-ignore
    rectangleRec: __muenGraphicsRectangle,

    /**
     * @function
     * @param {Rectangle} rectangle
     * @param {Vector2} origin
     * @param {number} rotation
     * @param {Color} color
     * @returns {void}
     */ // @ts-ignore
    rectanglePro: __muenGraphicsRectangle,

    /**
     * @function
     * @param {Camera} camera
     * @returns {void}
     */ // @ts-ignore
    beginCameraMode: __muenGraphicsBeginCameraMode,

    /**
     * @function
     * @returns {void}
     */ // @ts-ignore
    endCameraMode: __muenGraphicsEndCameraMode,
};

module.exports = Object.freeze(graphics);
