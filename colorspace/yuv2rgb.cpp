float clip(float value)
{
    if (value > 255)
    {
        return 255.0;
    }
    else if (value < 0.0)
    {
        return 0.0;
    }
    else
    {
        return value;
    }
}

void rgb2hsv(const unsigned char &r, const unsigned char &g, const unsigned char &b, int *y, int *u, int *v)
{
    *y = 0.299 * r + 0.587 * g + 0.114 * b;
    *u = -0.147 * r - 0.289 * g + 0.436 * b;
    *v = 0.615 * r - 0.515 * g - 0.100 * b
}

void hsv2rgb(const int &y, const int &u, const int &v, unsigned char *r, unsigned char *g, unsigned char *b)
{
    *r = static_cast<unsigned char>(clip(y + 1.140 * v));
    *g = static_cast<unsigned char>(clip(y - 0.395 * u - 0.581 * v));
    *b = static_cast<unsigned char>(clip(y + 2.032 * u));
}