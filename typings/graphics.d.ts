import Camera from "muen:Camera";
import { BasicColor } from "muen:Color";
import NPatch from "muen:NPatch";
import { BasicRectangle } from "muen:Rectangle";
import Texture from "muen:Texture";
import { BasicVector2 } from "muen:Vector2";

export interface Graphics {
    clear(color: BasicColor): Graphics;
    circle(x: number, y: number, radius: number, color: BasicColor): Graphics;
    rectangle(x: number, y: number, width: number, height: number, color: BasicColor): Graphics;
    rectangleV(position: BasicVector2, size: BasicVector2, color: BasicColor): Graphics;
    rectangleRec(rec: BasicRectangle, color: BasicColor): Graphics;
    rectanglePro(rec: BasicRectangle, origin: BasicVector2, rotation: number, color: BasicColor): Graphics;
    beginCameraMode(camera: Camera): Graphics;
    endCameraMode(): Graphics;
    texture(texture: Texture, x: number, y: number, tint: BasicColor): Graphics;
    textureV(texture: Texture, position: BasicVector2, tint: BasicColor): Graphics;
    textureEx(texture: Texture, position: BasicVector2, rotation: number, scale: number, tint: BasicColor): Graphics;
    textureRec(texture: Texture, source: BasicRectangle, position: BasicVector2, tint: BasicColor): Graphics;
    texturePro(
        texture: Texture,
        source: BasicRectangle,
        dest: BasicRectangle,
        origin: BasicVector2,
        rotation: number,
        tint: BasicColor,
    ): Graphics;
    textureNPatch(
        texture: Texture,
        nPatch: NPatch,
        dest: BasicRectangle,
        origin: BasicVector2,
        rotation: number,
        tint: BasicColor,
    ): Graphics;
}

export declare const graphics: Graphics;
export default graphics;
