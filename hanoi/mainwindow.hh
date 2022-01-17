/*
#############################################################################
# TIE-02201 Ohjelmointi 2: Perusteet, Kesä 2020                             #
# Project4: Hanoin torni / Tower of Hanoi                                   #
# File: mainwindow.hh                                                       #
# Description: MainWindow-class datastructure                               #
#              * Populates with structs for discs and pins                  #
#              * provides functions used in making moves and running        #
#                the game                                                   #
#############################################################################
*/

/* Student: Mikael Uuro
 * id: 201454
 * email: mikael.uuro@tuni.fi
 * git: https://course-gitlab.tuni.fi/tie-02201-ohj2_2020-KESA/uuro.git */

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH
#include <QGraphicsScene>
#include <QMainWindow>
#include <map>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Every disk will contain information of their coordinates and diameter.
    // Discs will change location by altering coordinates and re-drawing them.
    struct Disk {
        int x_coordinate = 0;
        int y_coordinate = 0;
        int diameter = 0;
        QGraphicsRectItem *disk_rect;
    };

    // A, B and C pins can contain disks, and these will be represented
    // by 3 vectors.
    QVector<Disk> a_disks;
    QVector<Disk> b_disks;
    QVector<Disk> c_disks;

    // QVector command will contain information of command and which pin is
    // the "from pin" aka. pin to give away a disc and which pin is the
    // "target pin" aka. pin to receive a disc. Will also store the
    // related QPushbutton's pointer.
    struct command {
        QString str;
        QVector<Disk> from_pin;
        QVector<Disk> target_pin;
        QPushButton *btn;

    };
    QVector<command> commands;

    // Each pin A, B and C will have separate vectors representing the
    // discs located in these pins, along with separate color for discs
    // in different pins.
    struct Pin {
        QGraphicsRectItem* pin_rect;
        QVector<Disk> pin_discs;
        QBrush color;        
    };

    QVector<Pin> pins;

    // Form vectors for commands and pins, so that actions can be performed in
    // a slightly handier way.
    void form_commands(QVector<command>& commands, QVector<Pin>& pins);


private slots:

    // Takes the number of discs upon pressing ENTER (well, return...).
    // Initializes the discs into vectors and starts the game by enabling
    // required buttons etc.
    void on_diskLineEdit_returnPressed();

    // Moves a disc from A to B, if move is allowed.
    void on_aToBPushButton_clicked();

    // Moves a disc from B to C, if move is allowed.
    void on_bToCPushButton_clicked();

    // See above...
    void on_cToBPushButton_clicked();

    void on_aToCPushButton_clicked();

    void on_bToAPushButton_clicked();

    void on_cToAPushButton_clicked();

    // Resets the game to its initial state by removing discs,
    // emptying vectors, disabling buttons and so forth.
    void on_resetPushButton_clicked();

    // Alternate way of moving discs. Enter for example "AB"
    // into the lineEdit field and it will find the corresponding
    // button from commands and activate it, if possible.
    void on_moveLineEdit_returnPressed();

private:
    // Our ui and game area, aka. scene
    Ui::MainWindow *ui_;
    QGraphicsScene* scene_;

    // Pins will be modeled using rectangular shapes
    QGraphicsRectItem* a_pin;
    QGraphicsRectItem* b_pin;
    QGraphicsRectItem* c_pin;

    QVector<QGraphicsRectItem*> disc_rects;


    // With max discs of 12, pin width is 10px and basic step
    // in size between different discs is 20. Pin width is 10
    // and the smallest disc is designed always to be at least 20.
    const int MAX_DISKS = 12;
    const int PIN_WIDTH = 10;
    const int STEP = 20;

    // For more than 12 discs, we need to recalculate suitable
    // step size and disc height, so that all discs visually fit
    // to the pins.
    int alt_step = 0;
    int disc_height = 0;

    // Initialize scene_
    const int BORDER_UP = 0;
    const int BORDER_DOWN = 260;
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = 680;

    // Pin locations, to be located at 1/4, 2/4 and 3/4 of the scene_
    // width.
    const int A_PIN_X = BORDER_RIGHT / 4;
    const int B_PIN_X = BORDER_RIGHT / 2;
    const int C_PIN_X = BORDER_RIGHT * 3/4;

    // First disc has this diameter. Rest will be smaller in diameter by
    // 1 STEP (or alt_step) for each consecutive disc.
    const int FIRST_DIAMETER = BORDER_RIGHT / 5;
    int total_disks = 0;
    int move_count = -1;
    int minimum_moves = 0;

    // Print_status is called after every move to update game situation.
    // It modifies the move count and makes sure we are drawing discs with
    // appropiate disc_height for the amount of discs given. Also calls
    // other functions to disable and re-enable buttons based on
    // allowed moves available.
    void print_status(int& move_count, int& disc_height);

    // Draws the QGrapchisRectItems for pins.
    void draw_pins();

    // Takes a disc from the end of from_pin-vector and moves it to_pin-vector
    // if move is allowed. Target_x is the new x-coordinate for the moved disc.
    void move_disk(QVector<Disk>& from_pin, QVector<Disk>& to_pin,
                   const int target_x);

    // Disable all of the pushbuttons.
    void disable_pushbuttons();

    // Go through vector "commands" and enable the buttons representing allowed
    // moves.
    void enable_moves(QVector<command>& commands);

    // Check if either B or C pin has all disks and determine whether game is
    // over or not.
    bool game_over();

};

#endif // MAINWINDOW_HH
