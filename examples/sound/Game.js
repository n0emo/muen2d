import Sound from "muen:Sound";
import Color from "muen:Color";
import screen from "muen:screen";
import graphics from "muen:graphics";

class Game {
    constructor() {
        this.bg = Color.fromHex("#181818");
        this.fg = Color.fromHex("#902020");

        this.x = 20;
        this.y = 20;
        this.velX = 500;
        this.velY = 500;
        this.radius = 10;
    }

    load() {
        this.hitsound = new Sound("hit.mp3");
    }

    update() {
        var newX = this.x + this.velX * screen.dt;
        var newY = this.y + this.velY * screen.dt;
        var collided = false;

        if (newX >= this.radius && newX < screen.width - this.radius) {
            this.x = newX;
        } else {
            this.velX *= -1;
            collided = true;
        }

        if (newY >= this.radius && newY < screen.height - this.radius) {
            this.y = newY;
        } else {
            this.velY *= -1;
            collided = true;
        }

        if (collided) {
            this.hitsound.play();
        }
    }

    draw() {
        graphics.clear(this.bg);
        graphics.circle(this.x, this.y, this.radius, this.fg);
    }
}

let game = new Game();

export const config = {
    title: "Ball sounds",
};

export function load() {
    game.load();
}

export function update() {
    game.update();
}

export function draw() {
    game.draw();
}
