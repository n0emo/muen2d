import screen from "muen:screen";
import graphics from "muen:graphics";

export class Ball {
    constructor(x, y, angle, color) {
        this.x = x;
        this.y = y;
        this.xVel = 400 * Math.cos((angle / 360) * Math.PI);
        this.yVel = 400 * Math.sin((angle / 360) * Math.PI);
        this.radius = 10;
        this.color = color;
    }

    update() {
        var oldX = this.x;
        var oldY = this.y;
        this.x += this.xVel * screen.dt;
        this.y += this.yVel * screen.dt;

        if (this.x < this.radius || this.x >= screen.width - this.radius) {
            this.x = oldX;
            this.xVel *= -1;
        }
        if (this.y < this.radius || this.y >= screen.height - this.radius) {
            this.y = oldY;
            this.yVel *= -1;
        }
    }

    draw() {
        graphics.circle(this.x, this.y, this.radius, this.color);
    }
}
