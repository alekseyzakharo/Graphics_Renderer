#include "SourceFiles\Header Files\client.h"

using namespace std;

Client::Client(Drawable *drawable)
{
    this->drawable = drawable;
}

Client::Client(Drawable *drawable,int argc, char *argv[])
{
	this->drawable = drawable;
	render = new Renderer(drawable,argc, argv);
}

void Client::nextPage() {
	
	draw_panel();

	(*render).render_scene();

	drawable->updateScreen();
}

void Client::draw_rect(int x1, int y1, int x2, int y2, unsigned int color) {
    for(int x = x1; x<x2; x++) {
        for(int y=y1; y<y2; y++) {
            drawable->setPixel(x, y, color);
        }
    }
}

void Client::draw_panel()
{
	draw_rect(0, 0, 750, 750, 0xffffffff);
	draw_rect(50, 50, 700, 700, 0xff000000);
}
