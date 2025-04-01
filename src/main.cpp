#include "mainwindow.h"
#include <QFile>
#include <QApplication>
#include <QLoggingCategory>
#include <QPalette>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv); // Initialize the Qt application
    app.setWindowIcon(QIcon(":/images/logo.png"));  // Use a resource path or absolute path
    // Force dark mode by setting the application's palette to dark
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));      // Dark background
    darkPalette.setColor(QPalette::WindowText, QColor(255, 255, 255)); // Light text color
    darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));        // Dark input field background
    darkPalette.setColor(QPalette::Text, QColor(255, 255, 255));     // Light text in input fields
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));      // Dark buttons
    darkPalette.setColor(QPalette::ButtonText, QColor(255, 255, 255)); // Button text color

    app.setPalette(darkPalette); // Apply the dark palette to the application

    MainWindow game; // Create an instance of the application
    game.setWindowTitle("Codenames"); // Set the window title

    game.setFixedSize(1000, 800);

    game.show(); // Display the game window

    return app.exec(); // Enter the Qt event loop
}