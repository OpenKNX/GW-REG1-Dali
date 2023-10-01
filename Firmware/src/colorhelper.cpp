#include "colorhelper.h"

void ColorHelper::rgbToXY(uint8_t in_r, uint8_t in_g, uint8_t in_b, uint16_t& x, uint16_t& y)
{
    float r = in_r / 255.0;
    float g = in_g / 255.0;
    float b = in_b / 255.0;
    r = (r   > 0.04045) ? pow((r   + 0.055) / (1.0 + 0.055), 2.4) : (r   / 12.92);
    g = (g > 0.04045) ? pow((g + 0.055) / (1.0 + 0.055), 2.4) : (g / 12.92);
    b = (b  > 0.04045) ? pow((b  + 0.055) / (1.0 + 0.055), 2.4) : (b  / 12.92);
    
	float X = r * 0.4124 + g * 0.3576 + b * 0.1805;
	float Y = r * 0.2126 + g * 0.7152 + b * 0.0722;
	float Z = r * 0.0193 + g * 0.1192 + b * 0.9505;


    float cx = X / (X + Y + Z);
    float cy = Y / (X + Y + Z);
    
    Serial.print("HUE: ");
	Serial.print(cx);
	Serial.print("/");
	Serial.println(cy);

    x = getBytes(cx);
    y = getBytes(cy);

    y = y + 1;
    y = y - 1;
}

void ColorHelper::hslToRGB(uint8_t in_h, uint8_t in_s, uint8_t in_l, uint8_t& r, uint8_t& g, uint8_t& b)
{
    float h = in_h / 255.0;
    float s = in_s / 255.0;
    float l = in_l / 255.0;

    if (in_s == 0)
	{
		r = g = b = in_l; // achromatic
	}
	else
	{
		auto q = in_l < 0.5 ? in_l * (1 + in_s) : in_l + in_s - in_l * in_s;
		auto p = 2 * in_l - q;
		r = hue2rgb(p, q, in_h + 1 / 3.0);
		g = hue2rgb(p, q, in_h);
		b = hue2rgb(p, q, in_h - 1 / 3.0);
	}

	r = static_cast<uint8_t>(r * 255);
	g = static_cast<uint8_t>(g * 255);
	b = static_cast<uint8_t>(b * 255);
}

uint16_t ColorHelper::getBytes(float input)
{
	return max(min(round(input * 65536), 65534), 0);
}

double ColorHelper::hue2rgb(double p, double q, double t)
{
	if (t < 0) t += 1;
	if (t > 1) t -= 1;
	if (t < 1 / 6.0) return p + (q - p) * 6 * t;
	if (t < 1 / 2.0) return q;
	if (t < 2 / 3.0) return p + (q - p) * (2 / 3.0 - t) * 6;
	return p;
}