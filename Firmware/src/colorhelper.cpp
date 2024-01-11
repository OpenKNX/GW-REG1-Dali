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
    
    x = getBytes(cx);
    y = getBytes(cy);

    y = y + 1;
    y = y - 1;
}

void ColorHelper::hsvToRGB(uint8_t in_h, uint8_t in_s, uint8_t in_v, uint8_t& r, uint8_t& g, uint8_t& b)
{
    float h = in_h / 255.0;
    float s = in_s / 255.0;
    float v = in_v / 255.0;

    double rt = 0;
    double gt = 0;
    double bt = 0;

    int i = int(h * 6);
    double f = h * 6 - i;
    double p = v * (1 - s);
    double q = v * (1 - f * s);
    double t = v * (1 - (1 - f) * s);

    switch(i % 6){
        case 0: rt = v, gt = t, bt = p; break;
        case 1: rt = q, gt = v, bt = p; break;
        case 2: rt = p, gt = v, bt = t; break;
        case 3: rt = p, gt = q, bt = v; break;
        case 4: rt = t, gt = p, bt = v; break;
        case 5: rt = v, gt = p, bt = q; break;
    }

	r = rt * 255;
	g = gt * 255;
	b = bt * 255;
}

void ColorHelper::kelvinToRGB(uint16_t kelvin, uint8_t& r, uint8_t& g, uint8_t& b)
{
    auto temp = kelvin / 100;

	if (temp <= 66)
	{
		r = 255;
		g = 99.4708025861 * log(temp) - 161.1195681661;

		if (temp <= 19)
			b = 0;
		else
			b = 138.5177312231 * log(temp - 10) - 305.0447927307;
	}
	else
	{
		r = 329.698727446 * pow(temp - 60, -0.1332047592);
		g = 288.1221695283 * pow(temp - 60, -0.0755148492);
		b = 255;
	}
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