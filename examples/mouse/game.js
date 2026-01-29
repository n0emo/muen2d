import Color from "glint:Color";
import graphics from "glint:graphics";
import mouse from "glint:mouse";

export const config = {
    window: {
        title: "Mouse",
    },
};

const black = Color.fromHex("#000000");
const blue = Color.fromHex("#4e3affff");
const red = Color.fromHex("#ff6161ff");
const purple = Color.fromHex("#773bcbff");

export function load() {
    mouse.enabled = false;
}

export function draw() {
    graphics.clear(black);
    if (mouse.isButtonDown("left") && mouse.isButtonDown("right")) {
        graphics.clear(purple);
    } else if (mouse.isButtonDown("left")) {
        graphics.clear(red);
    } else if (mouse.isButtonDown("right")) {
        graphics.clear(blue);
    }
}
