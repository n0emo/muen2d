import Color from "glint:Color";
import { Font } from "glint:Font";
import graphics from "glint:graphics";
import screen from "glint:screen";
import Vector2 from "glint:Vector2";

let font;

export const config = {
    window: {
        title: "Text",
    },
};

export function load() {
    font = new Font({ path: "Blouse.ttf", name: "Blouse" });
    console.log(font);
}

export function draw() {
    graphics.text(
        "Hello from glint!",
        screen.width * 0.5 - 100,
        screen.height * 0.5 - 16,
        32,
        Color.fromHex("#eeeeee"),
    );

    graphics.textPro({
        text: "Pro move",
        fontSize: 32,
        position: new Vector2(screen.width * 0.5 - 80, screen.height * 0.5 + 48),
        color: Color.fromHex("#ee7777"),
        rotation: 45,
        origin: new Vector2(20, 20),
        spacing: 5,
        font: new Font({ name: "Blouse" }),
    });
}
