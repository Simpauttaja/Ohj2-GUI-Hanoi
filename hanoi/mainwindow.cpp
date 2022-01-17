/*
#############################################################################
# TIE-02201 Ohjelmointi 2: Perusteet, Kesä 2020                             #
# Project4: Hanoin torni / Tower of Hanoi                                   #
# File: mainwindow.cpp                                                      #
# Description: * Holds definitons for all functions used in program         #
#                (presented in hh-file)                                     #
#              * forms commands, pins                                       #
#              * manages when game is over                                  #
#############################################################################
*/

/* Student: Mikael Uuro
 * id: 201454
 * email: mikael.uuro@tuni.fi
 * git: https://course-gitlab.tuni.fi/tie-02201-ohj2_2020-KESA/uuro.git */

#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QKeyEvent>
#include <algorithm>
#include <cmath>
#include <iostream>

// Setup scene and put placeholders for lineEdits, statusLabels
// and LCD-displays. Disable buttons for now.
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    int left_margin = 10; // x coordinate
    int top_margin = 270; // y coordinate

    scene_ = new QGraphicsScene(this);
    ui_->graphicsView->setGeometry(left_margin, top_margin,
                                   BORDER_RIGHT + 2, BORDER_DOWN + 2);

    ui_->graphicsView->setScene(scene_);    
    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

    ui_->diskLineEdit->setPlaceholderText("1-12");
    draw_pins();

    // Assign a validator for the disk number input box. After this
    // lineEdit only accepts values between 1-MAX_DISKS (12 by default).
    // QIntValidator* ival = new QIntValidator(1, MAX_DISKS, this);
    // ui_->diskLineEdit->setValidator(ival);

    // By applying a mask the command input / moveLineEdit only takes
    // an input with 2 characters between A-Z (or a-z).
    ui_->moveLineEdit->setInputMask("AA");

    ui_->moveLineEdit->setPlaceholderText("AB, Ab, ab...");
    ui_->moveLineEdit->setCursorPosition(0);
    ui_->moveLineEdit->setDisabled(true);

    ui_->moveCountLcdNumber->setSegmentStyle(QLCDNumber::Flat);
    ui_->distanceCountLcdNumber->setSegmentStyle(QLCDNumber::Flat);
    form_commands(commands, pins);
    disable_pushbuttons();

    ui_->statusLabel_2->setText("");


}


MainWindow::~MainWindow()
{
    delete ui_;
}


void MainWindow::form_commands(QVector<command>& commands, QVector<Pin>& pins)
{
    commands = {{"AB", a_disks, b_disks, ui_->aToBPushButton},
               {"AC", a_disks, c_disks, ui_->aToCPushButton},
               {"BA", b_disks, a_disks, ui_->bToAPushButton},
               {"BC", b_disks, c_disks, ui_->bToCPushButton},
               {"CA", c_disks, a_disks, ui_->cToAPushButton},
               {"CB", c_disks, b_disks, ui_->cToBPushButton}};

    // Each pin will have different color for discs.
    QBrush redBrush(Qt::red);
    QBrush greenBrush(Qt::green);
    QBrush blueBrush(Qt::blue);

    pins = {{a_pin, a_disks, redBrush}, {b_pin, b_disks, greenBrush},
            {c_pin, c_disks, blueBrush}};
}


void MainWindow::on_diskLineEdit_returnPressed()
{
    if ( ui_->diskLineEdit != 0) {
        // Upon receiving eligible disc number, disable the input field.
        // and start the game.
        total_disks = ui_->diskLineEdit->text().toInt();
        ui_->diskLineEdit->setDisabled(true);

        // All discs go into A-pin at the beginning.
        for ( int i = 0; i < total_disks; ++i) {
            Disk disk;
            disk.x_coordinate = A_PIN_X;

            // As mentioned, if there are more than 12 discs, we need
            // to adjust height and x-step size between consecutive discs.
            if ( total_disks > 12 ) {
                alt_step = (BORDER_DOWN - STEP) / total_disks;
                disk.y_coordinate = BORDER_DOWN - alt_step - ( alt_step * i );
                disk.diameter = FIRST_DIAMETER - alt_step * i / 2;

            } else {
                disk.y_coordinate = BORDER_DOWN - STEP - ( STEP * i );
                disk.diameter = FIRST_DIAMETER - STEP * i / 2;
            }
            a_disks.push_back(disk);
        }
    }
    // Additional feature: display move count and the differenctial
    // to minimum required moves, which can be calculated as follows.
    minimum_moves = pow(2, total_disks) - 1;
    ui_->distanceCountLcdNumber->display(move_count - minimum_moves);

    ui_->moveLineEdit->setEnabled(true);
    ui_->statusLabel_2->setText("Click buttons OR give text command and press "
                                "ENTER to move discs.");

    print_status(move_count, disc_height);
}


void MainWindow::print_status(int& move_count, int& height)
{
    // Update the vectors in commands after every move.
    form_commands(commands, pins);

    disable_pushbuttons();
    //scene_->clear();
    //draw_pins();


    move_count += 1;
    ui_->moveCountLcdNumber->display(move_count);
    ui_->distanceCountLcdNumber->display(move_count - minimum_moves);

    QPen blackPen(Qt::black);

    if (total_disks > 12) {
        height = alt_step;
    } else {
        height = STEP;
    }

    // Go through each pin's vector and print each disc with corresponding
    // color.

    for (int i = 0; i < pins.size(); ++i) {        
        if (!pins[i].pin_discs.isEmpty()) {
            QVector<Disk> discs_to_print = pins[i].pin_discs;

            for (int j = 0; j < discs_to_print.size(); ++j) {
                Disk disk = discs_to_print.at(j);
                int x_offset = 0;
                x_offset = disk.diameter / 2 - PIN_WIDTH / 2;
                pins[i].pin_rect = scene_->addRect(disk.x_coordinate - x_offset,
                                                   disk.y_coordinate,
                                                   disk.diameter, height,
                                                   blackPen, pins[i].color);

            }
        }
    }

    // Update available moves.
    enable_moves(commands);

    // If game is over, game ends here with statustext and all buttons disabled
    // except for reset.
    if (game_over()) {        
        ui_->statusLabel_2->setText("Game over, GGWP!");
        disable_pushbuttons();
        ui_->moveLineEdit->setDisabled(true);
        return;
    }
}

void MainWindow::draw_pins()
{
    QPen blackPen(Qt::black);
    blackPen.setWidth(2);
    QBrush blackBrush(Qt::black);

    a_pin = scene_->addRect(A_PIN_X, STEP, PIN_WIDTH, BORDER_DOWN - STEP,
                            blackPen, blackBrush);

    b_pin = scene_->addRect(B_PIN_X, STEP, PIN_WIDTH, BORDER_DOWN - STEP,
                            blackPen, blackBrush);

    c_pin = scene_->addRect(C_PIN_X, STEP, PIN_WIDTH, BORDER_DOWN - STEP,
                            blackPen, blackBrush);
}

void MainWindow::move_disk(QVector<Disk>& from_pin, QVector<Disk>& to_pin,
                           const int target_x)
{
    Disk target_last;    
    target_last.diameter = -1;

    // If there is no disc to move...
    if (from_pin.isEmpty()) {
        return;

    } else {
        // ...else compare diameters and move only, if the target pin's last
        // disc has bigger diamater. Or if the target pin is empty, we can
        // also make the move.
        Disk from_last = from_pin.back();
        if (!to_pin.isEmpty()) {
            target_last = to_pin.back();
        }

        if (from_last.diameter > target_last.diameter and !to_pin.isEmpty()) {
            return;

        } else {
            int n = to_pin.size();
            from_last.x_coordinate = target_x;

            if (total_disks <= 12) {
                from_last.y_coordinate = BORDER_DOWN - STEP - ( STEP * n );
            } else {
                from_last.y_coordinate = BORDER_DOWN - alt_step -
                        ( alt_step * n );
            }

            // Remove the last disc from from_pin and add it to the back
            // of to_pin vector.





            to_pin.push_back(from_last);
            from_pin.pop_back();
        }
    }
}

// All of the below xToYPushButton_clicked -functions
// call move_disc with appropiate vectors and coordinates
// representing from_pin, target/to_pin and the new x-coordinates for the disc.
void MainWindow::on_aToBPushButton_clicked()
{
    ui_->statusLabel_2->clear();
    move_disk(a_disks, b_disks, B_PIN_X);
    print_status(move_count, disc_height);
}


void MainWindow::on_aToCPushButton_clicked()
{
    ui_->statusLabel_2->clear();
    move_disk(a_disks, c_disks, C_PIN_X);
    print_status(move_count, disc_height);
}


void MainWindow::on_bToCPushButton_clicked()
{
    ui_->statusLabel_2->clear();
    move_disk(b_disks, c_disks, C_PIN_X);
    print_status(move_count, disc_height);
}


void MainWindow::on_cToBPushButton_clicked()
{
    ui_->statusLabel_2->clear();
    move_disk(c_disks, b_disks, B_PIN_X);
    print_status(move_count, disc_height);
}


void MainWindow::on_bToAPushButton_clicked()
{
    ui_->statusLabel_2->clear();
    move_disk(b_disks, a_disks, A_PIN_X);
    print_status(move_count, disc_height);
}


void MainWindow::on_cToAPushButton_clicked()
{
    ui_->statusLabel_2->clear();
    move_disk(c_disks, a_disks, A_PIN_X);
    print_status(move_count, disc_height);
}


void MainWindow::on_resetPushButton_clicked()
{
    // Reset and clear everything and return to
    // initial state.
    scene_->clear();
    draw_pins();

    a_disks.clear();
    b_disks.clear();
    c_disks.clear();

    ui_->diskLineEdit->setEnabled(true);
    ui_->diskLineEdit->clear();

    disable_pushbuttons();
    move_count = -1;

    ui_->statusLabel_2->clear();
    ui_->moveCountLcdNumber->display(0);
    ui_->distanceCountLcdNumber->display(0);
    ui_->moveLineEdit->setDisabled(true);
}


void MainWindow::disable_pushbuttons()
{
    for (int i = 0; i < commands.size(); ++i) {
        commands[i].btn->setDisabled(true);
    }
}


bool MainWindow::game_over()
{
    if (b_disks.size()== total_disks or c_disks.size() == total_disks) {
        return true;
    } else {
        return false;
    }
}


void MainWindow::enable_moves(QVector<command>& commands)
{
    ui_->moveLineEdit->setEnabled(true);

    // Go through all possible commands and see where the
    // diameter requirement is met and enable those buttons.
    for (int i = 0; i < commands.size(); ++i) {

        if (commands[i].from_pin.isEmpty()) {
            continue;

        } else if (commands[i].target_pin.isEmpty()) {
            commands[i].btn->setEnabled(true);

        } else {
            Disk from_last = commands[i].from_pin.back();
            Disk target_last = commands[i].target_pin.back();

            if(from_last.diameter < target_last.diameter) {
                commands[i].btn->setEnabled(true);
            }
        }
    }
}


void MainWindow::on_moveLineEdit_returnPressed()
{
    ui_->statusLabel_2->clear();
    QString cmd = ui_->moveLineEdit->text().toUpper();
    ui_->moveLineEdit->clear();

    int found = 0;


    for (int i = 0; i < commands.size(); ++i) {
        if (commands[i].str == cmd) {
            if (commands[i].btn->isEnabled()) {
                commands[i].btn->animateClick();

            } else {
                ui_->statusLabel_2->setText("Forbidden move, try again!");
            }
            found += 1;
            return;
        }
    }
    if (found == 0) {
        ui_->statusLabel_2->setText("Error: Pins are A, B and C. Enter 2 different pins.");
    }
    ui_->moveLineEdit->setPlaceholderText("AB, Ab, ab...");
    ui_->moveLineEdit->setCursorPosition(0);
}
