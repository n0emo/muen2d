import Vector2, { BasicVector2 } from "glint:Vector2";

export type ButtonType = "left" | "right" | "middle" | "side" | "extra" | "forward" | "back";

export type CursorType =
    | "default"
    | "arrow"
    | "ibeam"
    | "crosshair"
    | "pointingHand"
    | "resizeEw"
    | "resizeNs"
    | "resizeNwse"
    | "resizeNesw"
    | "resizeAll"
    | "notAllowed";

export interface Mouse {
    isButtonPressed(button: ButtonType): boolean;
    isButtonDown(button: ButtonType): boolean;
    isButtonReleased(button: ButtonType): boolean;
    isButtonUp(button: ButtonType): boolean;

    get x(): number;
    set x(value: number);
    get y(): number;
    set y(value: number);
    get position(): Vector2;
    set position(value: BasicVector2);
    get delta(): Vector2;
    get cursor(): CursorType;
    set cursor(value: CursorType);
    get visible(): boolean;
    set visible(value: boolean);
    get enabled(): boolean;
    set enabled(value: boolean);
    get isOnScreen(): boolean;
}

export declare const mouse: Mouse;
export default mouse;
