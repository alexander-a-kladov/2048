#include <QApplication>
#include "form2048.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Form2048 form;
    if (argc>1) 
	form.loadState(QString(argv[1]));
    else form.loadState("state.txt");
    form.setWindowTitle("Cursor,R-Restart/Load,S-Save,U-Undo,P-Pix");
    form.show();
    return app.exec();
}
