/*
#############################################################################
# TIE-02201 Ohjelmointi 2: Perusteet, Kesä 2020                             #
# Project4: Hanoin torni / Tower of Hanoi                                   #
# File: main.cpp                                                            #
# Description: Gets our QApplication up and running. Main passes control    #
#              to QT.                                                       #
#############################################################################
*/

/* Student: Mikael Uuro
 * id: 201454
 * email: mikael.uuro@tuni.fi
 * git: https://course-gitlab.tuni.fi/tie-02201-ohj2_2020-KESA/uuro.git */
#include "mainwindow.hh"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
