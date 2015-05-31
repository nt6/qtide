
#include <QApplication>

#include "wd.h"
#include "cmd.h"
#include "font.h"
#include "../base/bedit.h"
#include "../base/jsvr.h"
#include "../base/note.h"
#include "../base/state.h"
#include "../base/tedit.h"
#include "../base/term.h"

extern int rc;

static string smact(string);
static string smerror(string);
static string smfocus(string);
static string smfont(string);
static string smget(string);
static string smgetactive();
static string smgetscript(QString);
static string smgettabs();
static string smgetwin(string);
static string smgetwin1(Bedit *);
static string smgetxywh();
static string smgetxywh1(QWidget *);
static string smopen(string);
static string smopen(string);
static string smprompt(string);
static string smreplace(string);
static string smrun(string);
static string smsave(string);
static string smsaveactive();
static string smsaveall();
static string smset(string);
static string smsetselect(Bedit *,QStringList);
static string smsettext(Bedit *,QString);
static string smsetxywh(QWidget *,QStringList);
static string smtabindex(QString,string);

// ---------------------------------------------------------------------
// c is type, p is parameter
string sm(string c,string p)
{
  rc=0;
  if (c=="act")
    return smact(p);
  if (c=="active")
    return smtabindex("active",p);
  if (c=="close")
    return smtabindex("close",p);
  if (c=="focus")
    return smfocus(p);
  if (c=="font")
    return smfont(p);
  if (c=="get")
    return smget(p);
  if (c=="new")
    return smopen(p);
  if (c=="open")
    return smopen(p);
  if (c=="replace")
    return smreplace(p);
  if (c=="run")
    return smrun(p);
  if (c=="save")
    return smsave(p);
  if (c=="set")
    return smset(p);
  else if (c=="prompt")
    return smprompt(p);
  else
    return smerror("unrecognized sm command: " + c);
  return "";
}

// ---------------------------------------------------------------------
string smact(string p)
{
  Q_UNUSED(p);
  term->smact();
  return"";
}

// ---------------------------------------------------------------------
string smerror(string p)
{
  rc=1;
  return p;
}

// ---------------------------------------------------------------------
string smfocus(string p)
{
  if (p.size()==0)
    return smerror("sm focus needs additional parameters");
  if (p=="term")
    term->smact();
  else if (p=="edit") {
    if (note==0 || note->editIndex()==-1)
      return smerror("No active edit window");
    note->activateWindow();
    note->raise();
    note->repaint();
  } else
    return smerror("unrecognized sm command: focus " + p);
  return "";
}

// ---------------------------------------------------------------------
string smfont(string p)
{
  if (p.size()!=0) {
    Font *fnt = new Font(p);
    if (fnt->error) {
      delete fnt;
      return smerror("unrecognized sm command: font " + p);
    } else {
      config.Font=fnt->font;
      delete fnt;
    }
  }
  fontset(config.Font);
  return "";
}

// ---------------------------------------------------------------------
string smget(string p)
{
  if (p.size()==0)
    return smerror("sm get needs additional parameters");
  if (p=="active")
    return smgetactive();
  if (p=="tabs")
    return smgettabs();
  if (p=="term" || p=="edit" || p=="edit2")
    return smgetwin(p);
  if (p=="xywh")
    return smgetxywh();
  return smerror("unrecognized sm command: get " + p);
}

// ---------------------------------------------------------------------
string smgetactive()
{
  rc=-1;
  return (note && ActiveWindows.indexOf(note)<ActiveWindows.indexOf(term))
         ? "edit" : "term";
}

// ---------------------------------------------------------------------
string smgetscript(QString f)
{
  return dors(">{.getscripts_j_ '" + q2s(f) + "'");
}

// ---------------------------------------------------------------------
string smgettabs()
{
  rc=-2;
  return note->gettabstate();
}

// ---------------------------------------------------------------------
string smgetwin(string p)
{
  rc=-2;
  if (p=="term")
    return smgetwin1(tedit);
  if (p=="edit") {
    if (note==0)
      return smerror("No active edit window");
    if (note->editIndex()==-1)
      return smgetwin1((Bedit *)0);
    string r=smgetwin1((Bedit *)note->editPage());
    r+=spair("file",note->editFile());
    return r;
  }
  if (note2==0)
    return smerror("No active edit2 window");
  if (note2->editIndex()==-1)
    return smgetwin1((Bedit *)0);
  return smgetwin1((Bedit *)note2->editPage());
}

// ---------------------------------------------------------------------
string smgetwin1(Bedit *t)
{
  string r;

  if (t==0) {
    r+=spair("text",(string)"");
    r+=spair("select",(string)"");
  } else {
    QTextCursor c=t->textCursor();
    int b=c.selectionStart();
    int e=c.selectionEnd();
    r+=spair("text",t->toPlainText());
    r+=spair("select",QString::number(b)+" "+QString::number(e));
  }
  return r;
}

// ---------------------------------------------------------------------
string smgetxywh()
{
  rc=-2;
  string r;
  r+=spair("text",smgetxywh1(term));
  if (note)
    r+=spair("edit",smgetxywh1(note));
  if (note2)
    r+=spair("edit2",smgetxywh1(note2));
  return r;
}

// ---------------------------------------------------------------------
string smgetxywh1(QWidget *w)
{
  QPoint p=w->pos();
  QSize z=w->size();
  return q2s(QString::number(p.rx())+" "+QString::number(p.ry())+" "+
             QString::number(z.width())+" "+QString::number(z.height()));
}

// ---------------------------------------------------------------------
string smopen(string p)
{
  QStringList opt=qsplit(p);
  if (opt[0]!="tab")
    return smerror("unrecognized sm command: open " + p);
  term->vieweditor();
  if (opt.size()==1)
    note->newtemp();
  else {
    QString f=s2q(smgetscript(opt[1]));
    if (!cfexist(f))
      return smerror("file not found: " + q2s(f));
    note->fileopen(f);
  }
  rc=-1;
  return i2s(note->editIndex());
}

// ---------------------------------------------------------------------
string smprompt(string p)
{
  term->smprompt(s2q(p));
  return"";
}

// ---------------------------------------------------------------------
string smreplace(string p)
{
  if (note==0 || note->editIndex()<0)
    return smerror ("No active edit window");
  QStringList opt=qsplit(p);
  if (opt[0]!="edit")
    return smerror("unrecognized sm command: replace " + p);
  if (2!=opt.size())
    return smerror("replace needs 2 parameters: tab filename");
  QString f=s2q(smgetscript(opt[1]));
  if (!cfexist(f))
    return smerror("file not found: " + q2s(f));
  note->filereplace(f);
  return"";
}

// ---------------------------------------------------------------------
string smrun(string p)
{
  if (p!="edit")
    return smerror("unrecognized sm command: run " + p);
  if (note==0 || note->editIndex()<0)
    return smerror("No active edit window");
  note->runlines(true);
  return"";
}

// ---------------------------------------------------------------------
string smsave(string p)
{
  if (note==0)
    return smerror("No active edit window");
  if (p=="edit")
    return smsaveactive();
  if (p=="tabs")
    return smsaveall();
  return smerror("sm save parameter should be 'edit' or 'tabs': " + p);
}

// ---------------------------------------------------------------------
string smsaveactive()
{
  note->savecurrent();
  return "";
}

// ---------------------------------------------------------------------
string smsaveall()
{
  note->saveall();
  return "";
}

// ---------------------------------------------------------------------
string smset(string arg)
{
  QStringList opt=qsplit(arg);

  QString p,q;
  Bedit *e;
  QWidget *w;

  if (opt.size()==0)
    return smerror("sm set parameters not given");
  if (opt.size()==1)
    return smerror("sm set " + q2s(opt[0]) + " parameters not given");
  p=opt[0];
  q=opt[1];
  opt=opt.mid(2);

  if (p=="term") {
    e=tedit;
    w=term;
  } else if (p=="edit") {
    if (note==0)
      return smerror("No active edit window");
    e=(Bedit *)note->editPage();
    w=note;
  } else if (p=="edit2") {
    if (note2==0)
      return smerror("No active edit2 window");
    e=(Bedit *)note2->editPage();
    w=note2;
  } else
    return smerror("unrecognized sm command: set " + q2s(p));

  if (e==0 && (q=="select" || q=="text"))
    return smerror("no edit window for sm command: set " + q2s(q));

  if (q=="select")
    return smsetselect(e,opt);
  if (q=="text")
    return smsettext(e,opt[0]);
  if (q=="xywh")
    return smsetxywh(w,opt);

  return smerror("unrecognized sm command: set " + q2s(p) + " " + q2s(q));
}

// ---------------------------------------------------------------------
string smsetselect(Bedit *e,QStringList opt)
{
  QList<int> s=qsl2intlist(opt);

  if (s.size()!= 2)
    return smerror("sm set select should have begin and end parameters");
  int m=e->toPlainText().size();
  if (s[1]==-1) s[1]=m;
  s[1]=qMin(m,s[1]);
  s[0]=qMin(s[0],s[1]);
  e->setselect(s[0],s[1]-s[0]);
  return"";
}

// ---------------------------------------------------------------------
string smsettext(Bedit *e,QString s)
{
  if (e==tedit)
    e->setPlainText(s);
  else
    note->settext(s);
  return"";
}

// ---------------------------------------------------------------------
string smsetxywh(QWidget *w,QStringList opt)
{
  QList<int> s=qsl2intlist(opt);
  QPoint p=w->pos();
  QSize z=w->size();
  if (s[0]==-1) s[0]=p.rx();
  if (s[1]==-1) s[1]=p.ry();
  if (s[2]==-1) s[2]=z.width();
  if (s[3]==-1) s[3]=z.height();
  w->move(s[0],s[1]);
  w->resize(s[2],s[3]);
  return"";
}

// ---------------------------------------------------------------------
string smtabindex(QString cmd,string p)
{
  if (note==0 || note->editIndex()<0)
    return smerror ("No active edit window");
  QStringList opt=qsplit(p);
  if (opt[0]!="tab")
    return smerror ("unrecognized sm command parameters: " + p);
  int ndx=opt[1].toInt();
  if (ndx<0 || ndx>=note->count())
    return smerror ("invalid tab index: " + p);
  if (cmd=="active")
    note->setindex(ndx);
  else
    note->tabclose(ndx);
  return "";
}

