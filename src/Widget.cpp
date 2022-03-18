#include <qapplication.h>
#include <qbuttongroup.h>
#include <qfont.h>
#include <qgrid.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qstring.h>
#include <stdio.h>

#include "Widget.h"

Widget::Widget (CommandInt* ip, QWidget *parent, const char *name)
   : QWidget (parent, name)
{
   setCaption ("Robot Navigator");
   QBoxLayout *topLayout = new QVBoxLayout (this, 2);

   interp = ip;
   //--- setup menubar ---
   QMenuBar *mb = new QMenuBar (this);
   QPopupMenu* popup = new QPopupMenu();
   popup->insertItem ("&Quit", qApp, SLOT(quit()));
   mb->insertItem ("&File", popup);
   topLayout->setMenuBar (mb);

   tabWidget = new QTabWidget (this);

   //--- setup first tab ---
   first = new QWidget (tabWidget);

   //--- target group ---
   QHBox *hbox1 = new QHBox (first);
   j_grp = new QButtonGroup (3, QGroupBox::Vertical, 
         "Target", hbox1);
   j_grp->setExclusive (TRUE);
   QRadioButton *rb1 = new QRadioButton ("Joint-&1", j_grp);
   rb1->setChecked (TRUE);
   rb1->setMinimumSize (120, 0);
   //rb1->setMaximumSize (120, 30);
   (void) new QRadioButton ("Joint-&2", j_grp);
   (void) new QRadioButton ("Joint-&3", j_grp);

   //--- motion group ---
   c_grp = new QButtonGroup (3, QGroupBox::Vertical, 
         "Motion", hbox1);
   c_grp->setExclusive (TRUE);
   rb1 = new QRadioButton ("Rotate", c_grp);
   rb1->setChecked (TRUE);
   (void) new QRadioButton ("Recalibrate", c_grp);
   (void) new QRadioButton ("Reset Pos", c_grp);

   rotval = new QSpinBox (-180, 180, 1, c_grp);
   rotval->setValue (0);
   connect (rb1, SIGNAL(toggled(bool)), rotval, SLOT(setEnabled(bool)));

   tabWidget->addTab (first, "&Individual Joint");

   //--- setup first tab ---
   second = new QWidget (tabWidget);

   //--- body group ---
   QHBox *hbox2 = new QHBox (second);

   b_grp = new QButtonGroup (4, QGroupBox::Vertical, 
         "Body", hbox2);
   b_grp->setExclusive (TRUE);
   rb1 = new QRadioButton ("Forward", b_grp);
   rb1->setChecked (TRUE);
   (void) new QRadioButton ("Backward", b_grp);
   (void) new QRadioButton ("Stand Up", b_grp);
   (void) new QRadioButton ("Back Up", b_grp);
   rb1 = new QRadioButton ("Turn", b_grp);

   turnval = new QSpinBox (-180, 180, 1, b_grp);
   turnval->setValue (0);
   turnval->setEnabled (FALSE);
   connect (rb1, SIGNAL(toggled(bool)), turnval, SLOT(setEnabled(bool)));
   rb1 = new QRadioButton ("Feet", b_grp);

   //--- feet group ---
   f_grp = new QButtonGroup (4, QGroupBox::Vertical, 
         "Feet", hbox2);
   QPushButton *tb1 = new QPushButton ("Suction 1", f_grp);
   tb1->setToggleButton (TRUE);
	tb1->setEnabled (FALSE);
   connect (rb1, SIGNAL(toggled(bool)), tb1, SLOT(setEnabled(bool)));
   connect (tb1, SIGNAL(toggled(bool)), this, SLOT(Suc1(bool)));
   tb1 = new QPushButton ("Suction 2", f_grp);
   tb1->setToggleButton (TRUE);
	tb1->setEnabled (FALSE);
   connect (rb1, SIGNAL(toggled(bool)), tb1, SLOT(setEnabled(bool)));
   connect (tb1, SIGNAL(toggled(bool)), this, SLOT(Suc2(bool)));
   tb1 = new QPushButton ("Valve 1", f_grp);
   tb1->setToggleButton (TRUE);
	tb1->setEnabled (FALSE);
   connect (rb1, SIGNAL(toggled(bool)), tb1, SLOT(setEnabled(bool)));
   connect (tb1, SIGNAL(toggled(bool)), this, SLOT(Val1(bool)));
   tb1 = new QPushButton ("Valve 2", f_grp);
   tb1->setToggleButton (TRUE);
	tb1->setEnabled (FALSE);
   connect (rb1, SIGNAL(toggled(bool)), tb1, SLOT(setEnabled(bool)));
   connect (tb1, SIGNAL(toggled(bool)), this, SLOT(Val2(bool)));

   hbox1->resize (300, 110);
   hbox2->resize (300, 150);
   tabWidget->addTab (second, "&Move Commands");

   resize (310, 270);

   topLayout->addWidget (tabWidget);

   QPushButton *gobutton = new QPushButton ("&Go!", this, "gobutton");
   connect (gobutton, SIGNAL(clicked()), this, SLOT(GoClicked()));

   topLayout->addWidget (gobutton);
   topLayout->activate();
}

void Widget::GoClicked () {
   char cmd[40];

   if (first->isVisible()) {
      int motor;

      motor = j_grp->id (j_grp->selected()) + 1;
      switch (c_grp->id (c_grp->selected())) {
         case 0:
            printf ("Rotate joint %d relative %d degrees\n", 
			    motor, rotval->value());
            sprintf (cmd, "rotate %d %d", motor, rotval->value());
            break;
         case 1:
            printf ("Recalibrate joint %d\n", motor);
            sprintf (cmd, "recal %d ", motor);
            break;
         case 2:
            printf ("Reset Position joint %d\n", motor);
            sprintf (cmd, "resetpos %d ", motor);
            break;
      }
      interp->DoCmd (cmd);
   }
   else {
      switch (b_grp->id (b_grp->selected())) {
         case 0:
				printf ("Forward\n");
            interp->DoCmd ("forward");
				break;
         case 1:
				printf ("Backward\n");
            interp->DoCmd ("backward");
				break;
         case 2:
				printf ("Standup\n");
            interp->DoCmd ("standup");
				break;
         case 3:
				printf ("Backup\n");
            interp->DoCmd ("backup");
				break;
         case 4:
				printf ("Turn %d degrees\n", turnval->value());
            sprintf (cmd, "turn %d ", turnval->value());
            interp->DoCmd (cmd);
				break;
      }
   }
}

void Widget::Suc1 (bool b) { 
   char cmd[40];
	sprintf (cmd, "suction 1 %s", b ? "on":"off");
   cout << cmd << endl;
   interp->DoCmd (cmd);
}

void Widget::Suc2 (bool b) { 
   char cmd[40];
	sprintf (cmd, "suction 2 %s", b ? "on":"off");
   cout << cmd << endl;
   interp->DoCmd (cmd);
}

void Widget::Val1 (bool b) { 
   char cmd[40];
	sprintf (cmd, "valve 1 %s", b ? "on":"off");
   cout << cmd << endl;
   interp->DoCmd (cmd);
}

void Widget::Val2 (bool b) { 
   char cmd[40];
	sprintf (cmd, "valve 2 %s", b ? "on":"off");
   cout << cmd << endl;
   interp->DoCmd (cmd);
}
