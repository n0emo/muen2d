import { BasicColor } from "glint:Color";
import { Font } from "glint:Font";
import { BasicVector2 } from "glint:Vector2";

interface BaseText {
    fontSize: number;
    position: BasicVector2;
    color: BasicColor;

    origin?: BasicVector2;
    rotation?: number;
    spacing?: number;
    font?: Font;
}

export interface StringText extends BaseText {
    text: string;
}

export interface CodepointText extends BaseText {
    codepoint: number;
}

export interface CodepointsText extends BaseText {
    codepoints: number[];
}

export type Text = StringText | CodepointText | CodepointsText;
