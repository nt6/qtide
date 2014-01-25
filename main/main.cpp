#include <QApplication>
#include <QDebug>

using namespace std;

#ifdef QT_OS_ANDROID
#include <jni.h>
extern "C" int state_run (int,char **,QString);
extern "C" void javaOnLoad(JavaVM * vm, JNIEnv * env);

#else
#include <QLibrary>

typedef int (*Run)(int,char **,QString);
#endif

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

#ifdef QT_OS_ANDROID
  return state_run(argc, argv, QCoreApplication::applicationFilePath());
#else
#ifdef _WIN32
#ifndef FHS
  QString s=QCoreApplication::applicationDirPath() + "/jqt";
#else
  QString s= "jqt";
#endif
#else
#ifndef FHS
  QString s=QCoreApplication::applicationDirPath() + "/libjqt";
#else
  QString s= "libjqt";
#endif
#endif
  QLibrary *lib=new QLibrary(s);
  Run state_run=(Run) lib->resolve("state_run");
  if (state_run)
    return state_run(argc, argv, lib->fileName());

  qDebug() << lib->fileName();
  qDebug() << "could not resolve: state_run:\n\n" + lib->errorString();

  return -1;
#endif
}

#ifdef QT_OS_ANDROID
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*)
{
  JNIEnv *jnienv;
  if (vm->GetEnv(reinterpret_cast<void**>(&jnienv), JNI_VERSION_1_6) != JNI_OK) {
    qCritical() << "JNI_OnLoad GetEnv Failed";
    return -1;
  }
  javaOnLoad(vm, jnienv);

  return JNI_VERSION_1_6;
}
#endif
