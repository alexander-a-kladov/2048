#include <QWidget>
#include <QKeyEvent>
#include <QFile>
#include <QTextStream>
#include <QPixmap>
#include <QDir>
#include <QStack>
#include <time.h>
#include <iostream>

#ifndef __FORM_2048_H__
#define __FORM_2048_H__

enum EDIRECTION {
    DONT_MOVE,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_DOWN,
    MOVE_UP
};

const int FIELD_SIZE=4;
const int FIELD_WIDTH=100;
const int NUM_DIGITS=18;
const int CELL_NUMBER=(FIELD_SIZE*FIELD_SIZE);

typedef struct {
    int field[CELL_NUMBER];
    int score, new_pos;
    int free_cells;
} TMoveStack;




class Form2048 : public QWidget {
    Q_OBJECT
    public:
	Form2048(QWidget *parent=0) : QWidget(parent)
	{
	    setFixedSize(FIELD_WIDTH*FIELD_SIZE,FIELD_WIDTH*FIELD_SIZE);
	    loadImages();
	    
	    initGame();
	}
	
	void loadImages()
	{
	    const char *icons_path = "/usr/share/icons/lol/48x48/apps/";
	    const char *templ_str = "*.png";

	    int i,j;
	    pix_prz = false;
	    pix_enable = false;
	    QDir dir(icons_path, templ_str);
	//    cout << dir.entryList().count() << endl;
	    if (dir.entryList().count()<40) return;
	    for (i=0;i<NUM_DIGITS;i++) {
		pix_mas[i] = new QPixmap(icons_path+dir.entryList()[i+20]);
	    }
	    pix_enable = true;
	    return;
	}
	
	void initGame()
	{
	    int i=0;
	    srand(time(NULL));
	    free_cells = CELL_NUMBER;
	    for (i=0;i<CELL_NUMBER;i++)
		field[i]=0;
	    score = 0;
	    fail_game = false;
	    setNewDigit();
	    setNewDigit();
	    return;
	}
	
	bool saveState(QString name)
	{
	    QFile file;
	    QString str;
	    bool res = false;
	    for (int i=0;i<FIELD_SIZE;i++) {
	    for (int j=0;j<FIELD_SIZE;j++) 
		str+=QString("%1").arg(field[i*FIELD_SIZE+j],1,NUM_DIGITS);
	    }
	    file.setFileName(name);
	    if (file.open(QIODevice::WriteOnly)) {
		QTextStream out(&file);
		out << str;
		res = true;
		file.close();
	    }
	    return res;
	}
	
	bool loadState(QString name)
	{
	    QFile file;
	    QString str;
	    bool ok,res=false;
	    file.setFileName(name);
	    if (file.open(QIODevice::ReadOnly)) {
		fname = name;
		QTextStream in(&file);
		in >> str;
		res = true;
		file.close();
	    }
	    free_cells = 0;
	    //cout << "str.length() = " << str.length() << endl;
	    if (str.length()<CELL_NUMBER) {initGame();return res;}
	    for (int i=0;i<FIELD_SIZE;i++)
	    for (int j=0;j<FIELD_SIZE;j++) {
		field[i*FIELD_SIZE+j]=QString(str.at(i*FIELD_SIZE+j)).toUInt(&ok,NUM_DIGITS);
		if (!field[i*FIELD_SIZE+j]) free_cells++;
	    }
	    move_stack.clear();
	    return res;
	}
	
	bool setNewDigit()
	{
	    int pos;
	    int i=0,j;
	    //static int counter=1;
	    if (!free_cells) return false;
	    pos = rand()%free_cells;
	//    cout << "pos = " << pos << endl;
	    j=0;
	    while (field[j]) {j++;}
	    while (i<pos) {
		if (!field[j]) {i++;j++;}
		while (field[j]) {j++;}
	    }
	    new_pos = j;
	    field[j] = ((rand()%10)==9)?2:1;
	    free_cells--;
	    //counter %= 12;
	    //if (!counter) counter = 1;
	    return true;
	}
	
	bool moveAll(int dir);
	bool summUp(int dir);
	bool summUpElement(int i, int j, int *check_value, int *pos);
	bool fail_game;
    protected:
	void paintEvent(QPaintEvent *ev);
	void keyPressEvent(QKeyEvent *ev);
	void closeEvent(QCloseEvent *ev);
	QStack<TMoveStack> move_stack;
	void push_mstack(TMoveStack *mstack);
	void pop_mstack(const TMoveStack &mstack);
	int score;
	QPixmap *pix_mas[NUM_DIGITS];
	QString fname;
	bool pix_prz, pix_enable;
	int new_pos;
	int field[FIELD_SIZE*FIELD_SIZE];
	int free_cells;
};

#endif
