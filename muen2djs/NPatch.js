/**
 * @constructor
 * @param {Rectangle} source
 * @param {number} left
 * @param {number} top
 * @param {number} right
 * @param {number} bottom
 * @param {"default" | "vertical" | "horizontal" | undefined} layout
 */
function NPatch(source, left, top, right, bottom, layout) {
    if (!layout) {
        layout = "default";
    }

    if (
        typeof source.x !== "number" ||
        typeof source.y !== "number" ||
        typeof source.width !== "number" ||
        typeof source.height !== "number" ||
        typeof left !== "number" ||
        typeof top !== "number" ||
        typeof right !== "number" ||
        typeof bottom !== "number" ||
        (layout !== "default" && layout !== "vertical" && layout !== "horizontal")
    ) {
        throw TypeError("Invalid arguments for NPatch");
    }

    this.x = source.x;
    this.y = source.y;
    this.width = source.width;
    this.height = source.height;
    this.left = left;
    this.top = top;
    this.right = right;
    this.bottom = bottom;
    this.layout = layout;
}

Object.defineProperties(NPatch, {
    default: {
        value: "default",
        writable: false,
    },
    vertical: {
        value: "vertical",
        writable: false,
    },
    horizontal: {
        value: "horizontal",
        writable: false,
    },
});

module.exports = NPatch;
