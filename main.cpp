#include "Widget.h"

#include <Qt>
#include <QApplication>


int main(int argc, char *argv[]) {

  QCoreApplication::addLibraryPath("../lib");

  QApplication app(argc,argv);
  Sonar sonar;
  sonar.show();
  app.exec();

  return 0;
}
