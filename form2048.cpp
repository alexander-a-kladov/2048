#include "form2048.h"
#include <QPainter>
#include <QColor>

const QColor colors[NUM_DIGITS]={Qt::lightGray,
			Qt::darkGreen,Qt::darkBlue,Qt::darkRed,
			Qt::darkCyan,Qt::darkYellow,Qt::darkMagenta,
			Qt::cyan,Qt::green,Qt::magenta,Qt::blue,Qt::red,
			Qt::black,Qt::black,Qt::black,Qt::black,
			Qt::black,Qt::black};
int values[NUM_DIGITS]={0,2,4,8,16,32,64,128,256,512,1024,2048,
		4096,8192,16384,32768,65536,131072};

void Form2048::paintEvent(QPaintEvent *ev)
{
    QPainter p(this);
    p.setPen(Qt::yellow);
    for (int i=0;i<FIELD_SIZE;i++)
    for (int j=0;j<FIELD_SIZE;j++) {
	int value = values[field[i*FIELD_SIZE+j]];
	if (value>=131072) p.setFont(QFont("Arial",18));
	    else if (value>=16384) p.setFont(QFont("Arial",22));
	    else if (value>=1024) p.setFont(QFont("Arial",28));
	    else p.setFont(QFont("Arial",42));
	if (!pix_prz) p.setBrush(colors[field[i*FIELD_SIZE+j]]);
	p.drawRect(j*FIELD_WIDTH,i*FIELD_WIDTH,FIELD_WIDTH,FIELD_WIDTH);
	if (new_pos == (i*FIELD_SIZE+j)) {
	    p.drawRect(j*FIELD_WIDTH+5,i*FIELD_WIDTH+5,
			FIELD_WIDTH-10,FIELD_WIDTH-10);
	}
	if (field[i*FIELD_SIZE+j]) {
	    if (pix_prz) p.drawPixmap(j*FIELD_WIDTH+5,i*FIELD_WIDTH+5,
			    FIELD_WIDTH-10,FIELD_WIDTH-10,*pix_mas[field[i*FIELD_SIZE+j]]);
	    else p.drawText(j*FIELD_WIDTH,i*FIELD_WIDTH,
			    FIELD_WIDTH,FIELD_WIDTH,Qt::AlignCenter,
			    QString("%1").arg(values[field[i*FIELD_SIZE+j]]));
	}
	
    }
    for (int i=0;i<FIELD_SIZE;i++) {
	p.drawLine(i*FIELD_WIDTH,0,i*FIELD_WIDTH,width());
	p.drawLine(0,i*FIELD_WIDTH,height(),i*FIELD_WIDTH);
    }
    return;
}

void Form2048::keyPressEvent(QKeyEvent *ev)
{
    int dir;
    TMoveStack mstack_local;
    switch (ev->key())
    {
	case Qt::Key_Left:
	    dir = MOVE_LEFT;
	    break;
	case Qt::Key_Right:
	    dir = MOVE_RIGHT;
	    break;
	case Qt::Key_Up:
	    dir = MOVE_UP;
	    break;
	case Qt::Key_Down:
	    dir = MOVE_DOWN;
	    break;
	case Qt::Key_P:
	    if (pix_enable) {
		pix_prz = !pix_prz;
		if (pix_prz) 
		    setWindowTitle("Pix Switch Ok!");
		else 
		    setWindowTitle("Digits Switch Ok!");
	    } else {
		setWindowTitle("Pix Error!");
	    }
	    repaint();
	    return;
	    break;
	case Qt::Key_U:
	    if (move_stack.size()) {
		mstack_local = move_stack.pop();
		pop_mstack(mstack_local);
		setWindowTitle(QString("Undo OK! undo left(%1)").arg(move_stack.count()));
	    } else {
		setWindowTitle(QString("Undo Stack is Empty!"));
	    }
	    repaint();
	    return;
	    break;
	case Qt::Key_R:
	    initGame();
	    if (!fname.isEmpty()) {
		if (loadState()) setWindowTitle("Load OK!");
		else setWindowTitle("Load Fail!");
	    }
	    repaint();
	    return;
	    break;
	case Qt::Key_S:
	    if (saveState()) setWindowTitle("Save OK!");
	    else setWindowTitle("Save Fail!");
	    return;
	    break;
	default:
	    dir = DONT_MOVE;
    }
    if (dir!=DONT_MOVE) {
    push_mstack(&mstack_local);
    rotateLeft(dir);
    bool ismove;
    ismove = moveLeft();
    ismove |= summUpLeft();
    ismove |= moveLeft();
    rotateLeft(4-dir);
    if (ismove) {
	move_stack.push(mstack_local);
	fail_game = !setNewDigit();
    }
    }
    setWindowTitle(QString("2048: Score = %1 %2").arg(score).arg(fname));
    repaint();
    return;
}

void Form2048::rotateLeft(int counter)
{
    if (counter<0) return;
    counter %= 4;
    for (int i=0; i<counter; ++i) {
    int line[FIELD_SIZE*FIELD_SIZE];
    memcpy(line, field, sizeof(line));
    for (int j=0;j<FIELD_SIZE;++j) {
    for (int i=0;i<FIELD_SIZE;++i) {
        int tondex = (FIELD_SIZE-1-i)*FIELD_SIZE+j;
        int fromdex = j*FIELD_SIZE+i;
        field[tondex] = line[fromdex];
    }
    }
    }
}

bool Form2048::moveLeft() {
    int index;
    int pos;
    int line[FIELD_SIZE];
    bool ischange=false;
    for (int i=0;i<FIELD_SIZE;i++) {
	pos = 0;
	memset(line,0,sizeof(line));
	for (int j=0;j<FIELD_SIZE;j++) {
	    index = i*FIELD_SIZE+j;
	    if (field[index]) line[pos++] = field[index];
	}
	if (memcmp(&field[i*FIELD_SIZE],line,sizeof(line))!=0) ischange = true;
	memcpy(&field[i*FIELD_SIZE],line,sizeof(line));
    }
    return ischange;
}

bool Form2048::summUpLeft()
{
    bool ischange=false;
    for (int i=0;i<FIELD_SIZE;i++) {
	for (int j=0;j<FIELD_SIZE-1;j++) {
	    int index = i*FIELD_SIZE + j;
	    if (field[index] and field[index]==field[index+1]) {
	        field[index]+=1;
	        field[index+1]=0;
	        free_cells++;
	        ischange = true;
	    }
	}
    }
    return ischange;
}

void Form2048::push_mstack(TMoveStack *mstack)
{
    memcpy(mstack->field,field,sizeof(field));
    mstack->score = score;
    mstack->new_pos = new_pos;
    mstack->free_cells = free_cells;
    return;
}

void Form2048::pop_mstack(const TMoveStack &mstack)
{
    memcpy(field,mstack.field,sizeof(field));
    score = mstack.score;
    new_pos = mstack.new_pos;
    free_cells = mstack.free_cells;
    return;
}

void Form2048::closeEvent(QCloseEvent *ev)
{
//    saveState();
//    ev->accept();
    return;
}
