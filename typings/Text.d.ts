import { BasicColor } from "muen:Color";
import { Font } from "muen:Font";
import { BasicVector2 } from "muen:Vector2";

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
