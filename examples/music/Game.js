var Music = require("muen/Music");

function Game() {}

Game.prototype.load = function () {
    this.music = Music.load("GloriousMorning.mp3");
    this.music.looping = true;
    this.music.volume = 0.5;
    this.music.pitch = 1.5;
    this.music.play();
};

Game.prototype.update = function () {};
Game.prototype.draw = function () {};

module.exports = Game;
