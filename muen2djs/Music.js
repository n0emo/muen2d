/**
 * @constructor
 * @param {number} id
 */
function Music(id) {
    if (typeof id !== "number") {
        throw new TypeError("Invalid arguments for Music");
    }

    /** @type {number} */
    this.id = id;
}

Object.defineProperties(Music.prototype, {
    playing: {
        get: function () {
            return __muenMusicIsPlaying(this.id);
        },
    },

    looping: {
        get: function () {
            return __muenMusicGetLooping(this.id);
        },
        /** @param {boolean} value */
        set: function (value) {
            __muenMusicSetLooping(this.id, value);
        },
    },

    volume: {
        get: function () {
            return __muenMusicGetVolume(this.id);
        },
        /** @param {number} value */
        set: function (value) {
            __muenMusicSetVolume(this.id, value);
        },
    },

    pan: {
        get: function () {
            return __muenMusicGetPan(this.id);
        },
        /** @param {number} value */
        set: function (value) {
            __muenMusicSetPan(this.id, value);
        },
    },

    pitch: {
        get: function () {
            return __muenMusicGetPitch(this.id);
        },
        /** @param {number} value */
        set: function (value) {
            __muenMusicSetPitch(this.id, value);
        },
    },
});

/**
 * @param {string} path
 * @returns {Music}
 */
Music.load = function (path) {
    var id = __muenMusicLoad(path);
    return new Music(id);
};

Music.prototype.unload = function () {
    __muenMusicUnload(this.id);
};

Music.prototype.play = function () {
    __muenMusicPlay(this.id);
};

Music.prototype.stop = function () {
    __muenMusicStop(this.id);
};

Music.prototype.pause = function () {
    __muenMusicPause(this.id);
};

Music.prototype.resume = function () {
    __muenMusicResume(this.id);
};

/**
 * @param {number} cursor
 */
Music.prototype.seek = function (cursor) {
    __muenMusicSeek(this.id, cursor);
};

module.exports = Music;
