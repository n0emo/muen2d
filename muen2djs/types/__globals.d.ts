/**
 * @module
 * @hidden
 */

declare function __muenConsoleTrace(...args: unknown[]): void;
declare function __muenConsoleDebug(...args: unknown[]): void;
declare function __muenConsoleLog(...args: unknown[]): void;
declare function __muenConsoleWarn(...args: unknown[]): void;
declare function __muenConsoleError(...args: unknown[]): void;

declare function __muenGraphicsClear(color: Color): void;
declare function __muenGraphicsCircle(x: number, y: number, radius: number, color: Color): void;
declare function __muenGraphicsRectangle(x: number, y: number, width: number, height: number, color: Color): void;
declare function __muenGraphicsRectangleV(position: Vector2, size: Vector2, color: Color): void;
declare function __muenGraphicsRectangleRec(rec: Rectangle, color: Color): void;
declare function __muenGraphicsRectanglePro(rec: Rectangle, origin: Vector2, rotation: number, color: Color): void;
declare function __muenGraphicsBeginCameraMode(camera: Camera): void;
declare function __muenGraphicsEndCameraMode(): void;

declare function __muenScreenDt(): number;
declare function __muenScreenTime(): number;
declare function __muenScreenWidth(): number;
declare function __muenScreenHeight(): number;

declare function __muenSoundLoad(path: string): number;
declare function __muenSoundUnload(id: number): void;
declare function __muenSoundPlay(id: number): void;
declare function __muenSoundStop(id: number): void;
declare function __muenSoundIsPlaying(id: number): boolean;
declare function __muenSoundGetVolume(id: number): number;
declare function __muenSoundSetVolume(id: number, volume: number): void;
declare function __muenSoundGetPan(id: number): number;
declare function __muenSoundSetPan(id: number, pan: number): void;
declare function __muenSoundGetPitch(id: number): number;
declare function __muenSoundSetPitch(id: number, pitch: number): void;

declare function __muenMusicLoad(path: string): number;
declare function __muenMusicUnload(id: number): void;
declare function __muenMusicPlay(id: number): void;
declare function __muenMusicStop(id: number): void;
declare function __muenMusicPause(id: number): void;
declare function __muenMusicResume(id: number): void;
declare function __muenMusicSeek(id: number, cursor: number): void;
declare function __muenMusicIsPlaying(id: number): boolean;
declare function __muenMusicGetLooping(id: number): boolean;
declare function __muenMusicSetLooping(id: number, looping: boolean): void;
declare function __muenMusicGetVolume(id: number): number;
declare function __muenMusicSetVolume(id: number, volume: number): void;
declare function __muenMusicGetPan(id: number): number;
declare function __muenMusicSetPan(id: number, pan: number): void;
declare function __muenMusicGetPitch(id: number): number;
declare function __muenMusicSetPitch(id: number, pitch: number): void;
