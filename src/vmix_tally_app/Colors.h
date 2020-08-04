#ifndef COLORS_H
#define COLORS_H

struct Colors {
    Colors(unsigned short fore, unsigned short back) : foreColor(fore), backColor(back) {}
    
    unsigned short foreColor;
    unsigned short backColor;
};

#endif