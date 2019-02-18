#include "window361.h"
#include "client.h"
#include <QApplication>


// you can rewrite main(.cpp/.h) and client(.cpp/.h) and add more files.
// Leave renderarea(.cpp/.h) and window(.cpp/.h) unchanged.
// Change Drawable and PageTurner if you like, but know that renderarea
// (which you cannot change) IS_A Drawable, and window361 depends on
// PageTurner. If you need a different interface to Drawable, it will be
// better to make a separate DrawableTwo (or whatever) interface.

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);   // because it's a Qt application

    Window361 window;               // make and show the window--size is already correct
    window.show();

    Drawable *sheet = window.getDrawable();

    Client client(sheet, argc, argv);           // the client (your program) gets a (Drawable *)
    window.setPageTurner(&client);  // the window must be given a (PageTurner *)
                                    // I made the client a PageTurner, but it doesn't have to
                                    // be that way.
    return app.exec();
}

