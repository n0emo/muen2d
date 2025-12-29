var graphics = require("muen/graphics");
var Color = require("muen/Color");
var Texture = require("muen/Texture");

function Game() {
    this.config = { title: "Graphics" };
    this.bgColor = Color.fromHex("#181818");
    this.t = undefined;
}

Game.prototype.load = function () {
    this.t = Texture.load("cat.jpg");
    console.log(this.t.id);
    console.log(this.t.valid);
};

Game.prototype.update = function () {};

Game.prototype.draw = function () {};

module.exports = Game;
