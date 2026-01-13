import graphics from "muen:graphics";
import Color from "muen:Color";
import Vector2 from "muen:Vector2";
import Rectangle from "muen:Rectangle";

export const config = { title: "Graphics" };

const bgColor = { r: 100, g: 100, b: 100, a: 255 };

export function draw() {
    graphics.clear(bgColor);
    graphics.rectangle(50, 100, 30, 30, Color.fromHex("#c49bffff"));
    graphics.rectangleV(new Vector2(100, 100), new Vector2(30, 30), Color.fromHex("#bf37bfff"));
    graphics.rectangleRec(new Rectangle(150, 100, 30, 30), Color.fromHex("#ff9ba5ff"));
    graphics.rectanglePro(new Rectangle(210, 100, 30, 30), new Vector2(10, 10), 45, Color.fromHex("#ffc787ff"));

    graphics.circle(200, 200, 10, Color.fromHex("#ff0000"));

    // TODO: this should give a runtime error actually
    // graphics.rectangle(50, 100, 30, 30, 40, Color.fromHex("#c49bffff"));
}
