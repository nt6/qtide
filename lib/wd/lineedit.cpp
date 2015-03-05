
#include "edit.h"
#include "lineedit.h"
#include "form.h"

// ---------------------------------------------------------------------
LineEdit::LineEdit(Child *c, QWidget *parent) : QLineEdit(parent)
{
  Q_UNUSED(parent);
  pchild = c;
}

// ---------------------------------------------------------------------
void LineEdit::keyPressEvent(QKeyEvent *event)
{
  // sysmodifiers = shift+2*control
  int key1=0;
  int key=event->key();
#ifdef QT_OS_ANDROID
  if (key==Qt::Key_Back) {
    QLineEdit::keyPressEvent(event);
    return;
  }
#endif
  if (!(event->modifiers() & Qt::CTRL) && !(event->modifiers() & Qt::SHIFT)) {
    switch (key) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Escape:
      QLineEdit::keyPressEvent(event);
      return;
    default:
      break;
    }
  }
  if ((key>0x10000ff)||((key>=Qt::Key_F1)&&(key<=Qt::Key_F35))) {
    QLineEdit::keyPressEvent(event);
    return;
  } else
    key1=translateqkey(key);
  char sysmodifiers[20];
  sprintf(sysmodifiers , "%d", (2*(!!(event->modifiers() & Qt::CTRL))) + (!!(event->modifiers() & Qt::SHIFT)));
  char sysdata[20];
  if (key==key1)
    sprintf(sysdata , "%s", event->text().toUtf8().constData());
  else sprintf(sysdata , "%s", QString(QChar(key1)).toUtf8().constData());

  pchild->event=string("char");
  pchild->sysmodifiers=string(sysmodifiers);
  pchild->sysdata=string(sysdata);
  pchild->pform->signalevent(pchild);
  QLineEdit::keyPressEvent(event);
}