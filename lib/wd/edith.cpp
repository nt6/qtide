
#include <QTextEdit>
#include <QScrollBar>

#include "../base/state.h"
#include "wd.h"
#include "edith.h"
#include "form.h"
#include "pane.h"
#include "cmd.h"

// ---------------------------------------------------------------------
Edith::Edith(string n, string s, Form *f, Pane *p) : Child(n,s,f,p)
{
  type="edith";
  QTextEdit *w=new QTextEdit;
#ifdef QT_OS_ANDROID
  w->setStyleSheet(scrollbarstyle(config.ScrollBarSize*DM_density));
#endif
  widget=(QWidget*) w;
  QString qn=s2q(n);
  QStringList opt=qsplit(s);
  w->setObjectName(qn);
  w->setReadOnly(true);
}

// ---------------------------------------------------------------------
void Edith::cmd(string p,string v)
{
  QTextEdit *w=(QTextEdit*) widget;
  QStringList opt=qsplit(v);
  if (p=="print") {
#ifndef QT_NO_PRINTER
#ifdef QT50
    w->print((QPagedPaintDevice *)config.Printer);
#else
    w->print(config.Printer);
#endif
#else
    Q_UNUSED(w);
#endif
  } else Child::set(p,v);
}

// ---------------------------------------------------------------------
void Edith::set(string p,string v)
{
  QTextEdit *w=(QTextEdit*) widget;
  QStringList opt=qsplit(v);
  QScrollBar *sb;

  int bgn,end,pos=0;

  if (p=="edit") {
    int s;
    if (opt.isEmpty())
      s=1;
    else
      s=c_strtoi(q2s(opt.at(0)));
    if (0==s) {
      if (!w->isReadOnly())  {
        QString t=w->toPlainText();
        w->setHtml(t);
        w->setReadOnly(1);
      }
    } else {
      if (w->isReadOnly()) {
        QString t=w->toHtml();
        w->setPlainText(t);
        w->setReadOnly(0);
      }
    }
  } else if (p=="text") {
    w->setHtml(s2q(v));
    w->setReadOnly(1);
  } else if (p=="select") {
    if (opt.isEmpty())
      w->selectAll();
    else {
      bgn=end=c_strtoi(q2s(opt.at(0)));
      if (opt.size()>1)
        end=c_strtoi(q2s(opt.at(1)));
      setselect(w,bgn,end);
    }
  } else if (p=="scroll") {
    if (opt.size()) {
      sb=w->verticalScrollBar();
      if (opt.at(0)=="min")
        pos=sb->minimum();
      else if (opt.at(0)=="max")
        pos=sb->maximum();
      else
        pos=c_strtoi(q2s(opt.at(0)));
      sb->setValue(pos);
    } else {
      error("set scroll requires additional parameters: " + p);
      return;
    }
  } else if (p=="wrap") {
    w->setLineWrapMode((remquotes(v)!="0")?QTextEdit::WidgetWidth:QTextEdit::NoWrap);
  } else if (p=="find") {
      w->find(opt.at(0));
  } else Child::set(p,v);
}

// ---------------------------------------------------------------------
void Edith::setselect(QTextEdit *w, int bgn, int end)
{
  QTextCursor c = w->textCursor();
  c.setPosition(end,QTextCursor::MoveAnchor);
  c.setPosition(bgn,QTextCursor::KeepAnchor);
  w->setTextCursor(c);
}

// ---------------------------------------------------------------------
string Edith::state()
{
  QTextEdit *w=(QTextEdit*) widget;
  QTextCursor c=w->textCursor();
  int b,e;
  b=c.selectionStart();
  e=c.selectionEnd();
  QScrollBar *v=w->verticalScrollBar();
  string r;
  r+=spair(id,q2s(w->toHtml()));
  r+=spair(id+"_select",i2s(b)+" "+i2s(e));
  r+=spair(id+"_scroll",i2s(v->value()));
  return r;
}
