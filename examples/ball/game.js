function Game() {
    console.log("Creating new game");

    this.bgColor = new Color("#181818");
    this.ball = new Ball();

    this.config = {
        fps: 165,
        width: 800,
        height: 600,
    };

    this.update = function() {
        this.ball.update();
    }

    this.draw = function() {
        graphics.clear(this.bgColor);
        this.ball.draw();
    }
}

function Ball() {
    this.x = 11;
    this.y = 11;
    this.xVel = 400;
    this.yVel = 400;
    this.radius = 10;
    this.color = new Color("#a040a0")

    this.update = function() {
        this.x += this.xVel * screen.dt;
        this.y += this.yVel * screen.dt;

        if (this.x < this.radius || this.x >= screen.width - this.radius) {
            this.xVel *= -1;
        }
        if (this.y < this.radius || this.y >= screen.height - this.radius) {
            this.yVel *= -1;
        }
    }

    this.draw = function() {
        graphics.circle(this.x, this.y, this.radius, this.color);
    }
}
