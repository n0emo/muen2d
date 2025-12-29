/**
 * @module muen
 */

/**
 * Allows to draw something on the screen
 * @namespace
 */
declare interface graphics {
    clear(color: Color): graphics;
    circle(x: number, y: number, radius: number, color: Color): graphics;
    rectangle(x: number, y: number, width: number, height: number, color: Color): graphics;
    rectangleV(position: Vector2, size: Vector2, color: Color): graphics;
    rectangleRec(rec: Rectangle, color: Color): graphics;
    rectanglePro(rec: Rectangle, origin: Vector2, rotation: number, color: Color): graphics;
    beginCameraMode(camera: Camera): graphics;
    endCameraMode(): graphics;
}

/**
 * @module
 * @hidden
 */
declare module "muen/graphics" {
    const graphics: graphics;
    export = graphics;
}
