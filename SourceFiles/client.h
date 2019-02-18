#ifndef CLIENT_H
#define CLIENT_H

#include "drawable.h"
#include "pageturner.h"
#include "renderer.h"


class Client : public PageTurner
{
public:
    Client(Drawable *drawable);
	Client(Drawable *drawable, int argc ,char *argv[]);
    void nextPage();
	
private:
	Renderer *render;
    Drawable *drawable;
    void draw_rect(int x1, int y1, int x2, int y2, unsigned int color);
	void draw_panel();
};

#endif // CLIENT_H
