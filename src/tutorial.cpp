#include "tutorial.h"

Tutorial::Tutorial(QWidget *parent)
    : QMainWindow(parent), clickCount(0) {
    setWindowTitle("Tutorial");
    setFixedSize(1000, 800);

    // Center the window on the screen
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - this->width()) / 2;
        int y = (screenGeometry.height() - this->height()) / 2;
        this->move(x, y);
    }

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Background image setup
    QLabel* backgroundLabel = new QLabel(centralWidget);
    QPixmap background(":/images/Tutorial-1.png");
    background = background.scaled(1000, 600, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    backgroundLabel->setPixmap(background);
    backgroundLabel->setGeometry(0, 0, width(), height());
    backgroundLabel->setAlignment(Qt::AlignCenter);

    // Title label setup
    titleLabel = new QLabel("TUTORIAL", centralWidget);
    titleLabel->setFixedSize(300, 30);
    titleLabel->setStyleSheet(
        "font-weight: 800;"
        "font-size: 32px;"
        "color: black;"
        "background: transparent;"
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->move((width() - titleLabel->width()) / 2, 20);
    titleLabel->raise();

    // Create the text box
    textBox = new QLabel(centralWidget);  // Initialize class member
    textBox->setText("Welcome to the tutorial, here we will teach you how to play codenames!");
    textBox->setFixedSize(400, 200);
    textBox->setStyleSheet(
        "QLabel {"
        "   background-color: rgba(255, 255, 255, 0.95);"
        "   color: black;"
        "   border-radius: 10px;"
        "   padding: 10px;"
        "   font-size: 16px;"
        "}"
    );
    textBox->setAlignment(Qt::AlignCenter);
    textBox->setWordWrap(true);
    textBox->move((width() - textBox->width()) / 2, 
                  225);

    // Create and style continue button
    continueButton = new QPushButton("Continue", centralWidget);  // Initialize class member
    continueButton->setFixedSize(80, 25);
    
    QString buttonStyle = 
        "QPushButton {"
        "   background-color: #412AD5;"
        "   color: white;"
        "   border-radius: 3px;"
        "   border: 1px solid #412AD5;"
        "   padding: 3px;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgb(54, 35, 177);"
        "}"
        "QPushButton:pressed {"
        "   background-color: rgb(54, 35, 177);"
        "}";
    
    continueButton->setStyleSheet(buttonStyle);
    
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect;
    shadowEffect->setBlurRadius(3);
    shadowEffect->setOffset(0, 2);
    shadowEffect->setColor(Qt::black);
    continueButton->setGraphicsEffect(shadowEffect);

    // Position button relative to textbox
    continueButton->move(
        textBox->x() + textBox->width() - continueButton->width() - 10,
        textBox->y() + textBox->height() - continueButton->height() - 10
    );
    
    textBox->raise();
    continueButton->raise();

    // Connect the button click
    connect(continueButton, &QPushButton::clicked, this, &Tutorial::onContinueClicked);


}

void Tutorial::updateContinueButtonPosition() {
    if (textBox && continueButton) {
        continueButton->move(
            textBox->x() + textBox->width() - continueButton->width() - 10,
            textBox->y() + textBox->height() - continueButton->height() - 10
        );
    }
}

void Tutorial::onContinueClicked() {
    clickCount++;
    qDebug() << "Continue clicked:" << clickCount << "times";
    
    QLabel* backgroundLabel = nullptr;
    
    switch(clickCount){
        case 1:
            textBox->setText("Above the main game board you can see how many words each team has left to guess, what players are on what teams, whose turn it is, and the current hint.");
            break;

        case 2:
            textBox->setText("In the main portion of the screen we can see the game board with all the words being used for this game.");
            textBox->move((width() - textBox->width()) / 2, 50);
            updateContinueButtonPosition();
            break;       
        case 3:
            textBox->setText("The game begins with the spymasters turn, they are able to see which team all words belong to. The red spymasters job is to use the textbox at the bottom to give single word hints to help their teammate guess which words on the board are red.");
            break; 
        case 4:
            textBox->move((width() - textBox->width()) / 2, 225);
            updateContinueButtonPosition();
            textBox->setText("In this example the spymaster has given the hint 'SUN 2' indicating there are 2 words related to the word sun for their teammate to guess. Notice how the board is now all white as it is the operatives turn. Their goal is to use the hint to guess which words are their teams color without seeing the colors directly.");
            // Update background image
            backgroundLabel = centralWidget->findChild<QLabel*>();  // Get the first QLabel (background)
            if (backgroundLabel) {
                QPixmap background(":/images/Tutorial-2.png");
                background = background.scaled(1000, 600, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                backgroundLabel->setPixmap(background);
            }
            break;
        case 5:
            textBox->setText("The operative now has to use the hint to click the words which they think correlate to the hint given, in this example dawn and day are the intended words. Notice how the cards have turned red after being clicked to indicate they guessed correctly. If they were incorrect they would have turned blue, or if they clicked the black card they would have lost instantly!");
            // Update background image
            backgroundLabel = centralWidget->findChild<QLabel*>();  // Get the first QLabel (background)
            if (backgroundLabel) {
                QPixmap background(":/images/Tutorial-3.png");
                background = background.scaled(1000, 600, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                backgroundLabel->setPixmap(background);
            }
            break;
        case 6:
            textBox->setText("After clicking finish guessing, they are shown a new transtion screen, here they pass the computer to the next teams spymaster who then clicks continue, to ensure the operative does not accidently see the spymaster board revealing which words belong to which team.");
            // Update background image
            backgroundLabel = centralWidget->findChild<QLabel*>();  // Get the first QLabel (background)
            if (backgroundLabel) {
                QPixmap background(":/images/Tutorial-4.png");
                background = background.scaled(1000, 600, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                backgroundLabel->setPixmap(background);
            }
            textBox->move((width() - textBox->width()) / 2, 50);
            updateContinueButtonPosition();
            break;
        case 7:
            textBox->setText("Now that the next teams spymaster has the device and has clicked continue we have returned to the spymaster board. From this point forward the game continues exactly as described previously with the device being passed around player to player, giving hints and guessing words.");
            // Update background image
            backgroundLabel = centralWidget->findChild<QLabel*>();  // Get the first QLabel (background)
            if (backgroundLabel) {
                QPixmap background(":/images/Tutorial-5.png");
                background = background.scaled(1000, 600, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                backgroundLabel->setPixmap(background);
            }

            break;
        case 8:
            textBox->setText("You now understand the main loop of the game, the goal is to guess all of your teams words before the other team does, without accidently clicking the black death card for an instant loss! You can now return to the main menu and begin playing!");
            continueButton->setText("Menu");
            break;

        default:  // When they click after seeing "Menu"
            this->close();  // This will trigger closeEvent which emits tutorialClosed
            return;  // Exit the function after closing
    }
}

Tutorial::~Tutorial() {
    // Qt will handle cleanup of child widgets
}

void Tutorial::resetTutorial() {
    clickCount = 0;
    textBox->setText("Welcome to the tutorial, here we will teach you how to play codenames!");
    continueButton->setText("Continue");  // Reset button text
    
    // Reset background to Tutorial-1
    QLabel* backgroundLabel = centralWidget->findChild<QLabel*>();
    if (backgroundLabel) {
        QPixmap background(":/images/Tutorial-1.png");
        background = background.scaled(1000, 600, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        backgroundLabel->setPixmap(background);
    }
    
    // Reset textbox position to original position
    textBox->move((width() - textBox->width()) / 2, 225);
    updateContinueButtonPosition();
}

void Tutorial::closeEvent(QCloseEvent* event) {
    resetTutorial();  // Reset everything before closing
    emit tutorialClosed();
    event->accept();
}
