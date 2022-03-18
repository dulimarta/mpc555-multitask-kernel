#include <qvbox.h>
#include <qbuttongroup.h>
#include <qtabwidget.h>
#include <qspinbox.h>

#include "CommandInt.h"

class Widget : public QWidget {
   Q_OBJECT
   public:
      Widget (CommandInt *, QWidget* parent = 0, const char*name=0);
   
   protected slots:
      void GoClicked();
	   void Suc1 (bool);
	   void Suc2 (bool);
	   void Val1 (bool);
	   void Val2 (bool);

   private:
      QPushButton *gobutton;
      QButtonGroup *b_grp, *f_grp, *j_grp, *c_grp;
      QTabWidget *tabWidget;
      QWidget *first, *second;
      QSpinBox *rotval, *turnval;

      CommandInt *interp;
};
