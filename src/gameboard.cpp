#include "gameboard.h"

GameBoard::GameBoard(const QString& redSpyMaster, const QString& redOperative,
                    const QString& blueSpyMaster, const QString& blueOperative,
                    QWidget* parent)
    : QWidget(parent),
      redSpyMasterName(redSpyMaster),
      redOperativeName(redOperative),
      blueSpyMasterName(blueSpyMaster),
      blueOperativeName(blueOperative), 
      
      redCardsRemaining(0),
      blueCardsRemaining(0),
      maxGuesses(0),
      currentGuesses(0)
{
    
    // Set the window title and fixed size
    setWindowTitle("Codenames - Game Board");
    setFixedSize(1200, 800);
    
    // Load words from the file and generate the game grid
    loadWordsFromFile();
    generateGameGrid();
    setupUI();
}

// Destructor for the GameBoard class
GameBoard::~GameBoard() {}

void GameBoard::show() {
    // 
    resetGame();
    QWidget::show();
    qDebug() << "GameBoard shown";
}

void GameBoard::setRedSpyMasterName(const QString& name) {
    redSpyMasterName = name;
}

void GameBoard::setRedOperativeName(const QString& name) {
    redOperativeName = name;
}

void GameBoard::setBlueSpyMasterName(const QString& name) {
    blueSpyMasterName = name;
}

void GameBoard::setBlueOperativeName(const QString& name) {
    blueOperativeName = name;
}

void GameBoard::updateTeamLabels() {
 redTeamLabel->setText(
    "<b>Red Team</b><br>"
    "<b>Spymaster:</b> <span style='color: white;'>" + redSpyMasterName + "</span><br>"
    "<b>Operative:</b> <span style='color: white;'>" + redOperativeName + "</span>"
);

blueTeamLabel->setText(
    "<b>Blue Team</b><br>"
    "<b>Spymaster:</b> <span style='color: white;'>" + blueSpyMasterName + "</span><br>"
    "<b>Operative:</b> <span style='color: white;'>" + blueOperativeName + "</span>"
);

}

void GameBoard::loadWordsFromFile() {
    // Load words from file
    QFile file(":/resources/wordlist-eng.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "failed to open wordlist-eng.text file, using fallback";
        // Fallback word list if file not found
        wordList = QStringList() << "apple" << "banana" << "cat" << "dog" << "elephant"
                                << "fish" << "goat" << "horse" << "iguana" << "jelly"
                                << "kangaroo" << "lion" << "monkey" << "nest" << "owl"
                                << "penguin" << "queen" << "rabbit" << "snake" << "tiger"
                                << "umbrella" << "violin" << "whale" << "xray" << "zebra";
        return;
    }

    // Read words from file
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            wordList.append(line);
        }
    }
    file.close();
}

void GameBoard::generateGameGrid() {
    // Ensure we have enough words
    if (wordList.size() < 25) {
        qDebug() << "Not enough words to generate a game grid" << wordList.size();
        return;
    }

    // Shuffle words
    for (int i = wordList.size() - 1; i > 0; --i) {
        int j = QRandomGenerator::global()->bounded(i + 1);
        wordList.swapItemsAt(i, j);
    }

    // Assign card types (Codenames rules: 9 for starting team, 8 for other team, 1 assassin, 7 neutral)
    int redCards = 9;
    int blueCards = 8;
    int assassinCards = 1;
    int neutralCards = 7;

    // Set remaining card counts
    redCardsRemaining = redCards;
    blueCardsRemaining = blueCards;

    // Assign card types
    QList<CardType> cardTypes;
    for (int i = 0; i < redCards; ++i) cardTypes.append(RED_TEAM);
    for (int i = 0; i < blueCards; ++i) cardTypes.append(BLUE_TEAM);
    for (int i = 0; i < neutralCards; ++i) cardTypes.append(NEUTRAL);
    for (int i = 0; i < assassinCards; ++i) cardTypes.append(ASSASSIN);

    // Shuffle card types
    for (int i = cardTypes.size() - 1; i > 0; --i) {
        int j = QRandomGenerator::global()->bounded(i + 1);
        cardTypes.swapItemsAt(i, j);
    }

    // Fill the grid
    int wordIndex = 0;
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            gameGrid[i][j].word = wordList[wordIndex];
            gameGrid[i][j].type = cardTypes[wordIndex];
            gameGrid[i][j].revealed = false;
            ++wordIndex;
        }
    }
}

void GameBoard::setupUI() {
    // Main horizontal layout that will hold grid area and chat box
    QHBoxLayout* mainHorizontalLayout = new QHBoxLayout(this);
    
    // Create vertical layout for the game elements (left side)
    QVBoxLayout* gameVerticalLayout = new QVBoxLayout();
    
    // Team info
    redTeamLabel = new QLabel(
        "<b>Red Team</b><br>"
        "<b>Spymaster:</b> " + redSpyMasterName + "<br>"
        "<b>Operative:</b> " + redOperativeName
    );

    blueTeamLabel = new QLabel(
        "<b>Blue Team</b><br>"
        "<b>Spymaster:</b> " + blueSpyMasterName + "<br>"
        "<b>Operative:</b> " + blueOperativeName
    );
    currentTurnLabel = new QLabel("Current Turn: " + redSpyMasterName);


    redTeamLabel->setStyleSheet("color: #ff9999;  font-size: 16px;");
    blueTeamLabel->setStyleSheet("color: #9999ff;  font-size: 16px;");
    currentTurnLabel->setStyleSheet("color: white;  font-size: 20px; font-weight: bold;");
        
    redScoreLabel = new QLabel("Red Cards Remaining: " + QString::number(redCardsRemaining));
    redScoreLabel->setStyleSheet("color: #ff9999; font-weight: bold; font-size: 16px;");

    blueScoreLabel = new QLabel("Blue Cards remaining: " + QString::number(blueCardsRemaining));
    blueScoreLabel->setStyleSheet("color: #9999ff; font-weight: bold; font-size: 16px;");


    // Implement score layout
    QHBoxLayout* scoreLayout = new QHBoxLayout();
    scoreLayout->addWidget(redScoreLabel);
    scoreLayout->addStretch();
    scoreLayout->addWidget(blueScoreLabel);
    gameVerticalLayout->addLayout(scoreLayout);

    // Implement team layout
    QHBoxLayout* teamLayout = new QHBoxLayout();
    teamLayout->addWidget(redTeamLabel);
    teamLayout->addStretch();
    teamLayout->addWidget(blueTeamLabel);
    gameVerticalLayout->addLayout(teamLayout);

    gameVerticalLayout->addWidget(currentTurnLabel); // Add current turn label

    // Implement current hint label
    currentHint = new QLabel("Current hint: ");
    currentHint->setAlignment(Qt::AlignCenter);
    currentHint->setStyleSheet("font-weight: bold; font-size: 20px; color: white; ");
    gameVerticalLayout->insertWidget(2, currentHint);

    // Grid setup
    gridLayout = new QGridLayout();

    // Create cards and add them to the grid
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            cards[i][j] = new QPushButton(gameGrid[i][j].word);
            cards[i][j]->setFixedSize(120, 80);
            gridLayout->addWidget(cards[i][j], i, j);

            // Set card styles
            switch (gameGrid[i][j].type) {
                case RED_TEAM:
                    cards[i][j]->setStyleSheet("background-color: #ff9999; color: black");
                    break;
                case BLUE_TEAM:
                    cards[i][j]->setStyleSheet("background-color: #9999ff; color: black");
                    break;
                case NEUTRAL:
                    cards[i][j]->setStyleSheet("background-color: #f0f0f0; color: black");
                    break;
                case ASSASSIN:
                    cards[i][j]->setStyleSheet("background-color: #333333; color: white;");
                    break;
            }
            cards[i][j]->setEnabled(false);
            // Connect the button's clicked signal to a lambda or a slot
            connect(cards[i][j], &QPushButton::clicked, this, [=]() {
                onCardClicked(i, j);
            });
        }
    }
    gameVerticalLayout->addLayout(gridLayout);

    // Implement transition widget
    transition = new Transition(this);
    transition->setFixedSize(400, 200);
    transition->hide();
    gameVerticalLayout->addWidget(transition);
    connect(transition, &Transition::continueClicked, this, &GameBoard::onContinueClicked);
    
    // Implement spymaster hint widget
    spymasterHint = new SpymasterHint(this);
    gameVerticalLayout->addWidget(spymasterHint);
    connect(spymasterHint, &SpymasterHint::hintSubmitted, this, &GameBoard::displayHint);

    // Implement operator guess widget
    operatorGuess = new OperatorGuess(this);
    gameVerticalLayout->addWidget(operatorGuess);
    operatorGuess->setVisible(false);
    connect(operatorGuess, &OperatorGuess::guessSubmitted, this, &GameBoard::displayGuess);

    // Add the game vertical layout to the main horizontal layout
    mainHorizontalLayout->addLayout(gameVerticalLayout);

    
    // Determine the team based on the current turn
    ChatBox::Team team = (currentTurn == RED_SPY || currentTurn == RED_OP) ? ChatBox::RED_TEAM : ChatBox::BLUE_TEAM;

    // Create and add chat box to the right
    chatBox = new ChatBox(redSpyMasterName, team, this);
    mainHorizontalLayout->addWidget(chatBox);

    // Set spacing and stretch factors
    mainHorizontalLayout->setStretch(0, 3);  
    mainHorizontalLayout->setStretch(1, 1); 
    mainHorizontalLayout->setSpacing(20);    

    setLayout(mainHorizontalLayout);
}

void GameBoard::displayGuess() {
    nextTurn();
    showTransition();
}

void GameBoard::onCardClicked(int row, int col) {
    if (gameGrid[row][col].revealed) {
        return;
    }
    // Increment the guess count for this turn
    currentGuesses++;

    // Mark the card as revealed and disable it
    gameGrid[row][col].revealed = true;
    cards[row][col]->setText("");  // Clear the text to show the card is revealed
    cards[row][col]->setEnabled(false);

    users = User::instance();

    // Always reveal the card's true color, regardless of whether it's correct
    switch (gameGrid[row][col].type) {
        case RED_TEAM:
            if (currentTurn == RED_OP) {
                users->hit(redOperativeName);
            } else if (currentTurn == BLUE_OP) {
                users->miss(blueOperativeName);
            }
            cards[row][col]->setStyleSheet("background-color: #ff9999; color: black");
            break;
        case BLUE_TEAM:
            if (currentTurn == BLUE_OP) {
                users->hit(blueOperativeName);
            } else if (currentTurn == RED_OP) {
                users->miss(redOperativeName);
            }
            cards[row][col]->setStyleSheet("background-color: #9999ff; color: black");
            break;
        case NEUTRAL:
            if (currentTurn == RED_OP) {
                users->miss(redOperativeName);
            } else if (currentTurn == BLUE_OP) {
                users->miss(blueOperativeName);
            }
            cards[row][col]->setStyleSheet("background-color: #f0f0f0; color: black");
            break;
        case ASSASSIN:
            if (currentTurn == RED_OP) {
                users->miss(redOperativeName);
            } else if (currentTurn == BLUE_OP) {
                users->miss(blueOperativeName);
            }
            cards[row][col]->setStyleSheet("background-color: #333333; color: white");
            break;
    }

    // Update scores 
    if (gameGrid[row][col].type == RED_TEAM) {
        redCardsRemaining--;
        updateScores();
        qDebug() << "Red team card selected. Red cards remaining:" << redCardsRemaining;
    } 
    else if (gameGrid[row][col].type == BLUE_TEAM) {
        blueCardsRemaining--;
        updateScores();
        qDebug() << "Blue team card selected. Blue cards remaining:" << blueCardsRemaining;
    }

    // Add the guess to the chat box
    QString currOperativeName = (currentTurn == RED_OP) ? redOperativeName : blueOperativeName;
    QString teamColor = (currentTurn == RED_OP) ? "Red" : "Blue";
    QString hintMessage = currOperativeName + " taps " + gameGrid[row][col].word;
    chatBox->addSystemMessage(hintMessage, (currentTurn == RED_OP) ? ChatBox::RED_TEAM : ChatBox::BLUE_TEAM);

    bool correctCard = (currentTurn == RED_OP && gameGrid[row][col].type == RED_TEAM) || 
                       (currentTurn == BLUE_OP && gameGrid[row][col].type == BLUE_TEAM);

    checkGameEnd();

    // If maxGuesses is 0 (unlimited), skip this check
    if (maxGuesses > 0) {
        // Allow a bonus guess if the operative has guessed correctly 'number' times
        int correctGuesses = maxGuesses - 1;  // maxGuesses includes the bonus guess
        int effectiveMax = correctCard ? maxGuesses : (maxGuesses - 1);
        if (currentGuesses >= effectiveMax) {
            qDebug() << "Maximum guesses reached (" << currentGuesses << "/" << effectiveMax << ")";
            for (int i = 0; i < GRID_SIZE; ++i) {
                for (int j = 0; j < GRID_SIZE; ++j) {
                    cards[i][j]->setEnabled(false);
                }
            }
            chatBox->limitReachedMessage();
            operatorGuess->setVisible(false);
            nextTurn();
            showTransition();
            return;
        }
    }

    // Show the transition widget
    if (!correctCard) {
        qDebug() << "Wrong card selected by" << (currentTurn == RED_OP ? "Red team" : "Blue team")
                 << "- Card type:" << gameGrid[row][col].type;
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                cards[i][j]->setEnabled(false);
            }
        }
        operatorGuess->setVisible(false);

        nextTurn();
        showTransition();  
    }
}

void GameBoard::displayHint(const QString& hint, int number) {
    qDebug() << "Received hint:" << hint << "for" << number << "corresponding words";
    // Update the coreesponding number for the hint, if it is 0, display "∞"
    if (number == 0) {
        correspondingNumber = "∞";
        maxGuesses=0;
    } 
    else {
        correspondingNumber = QString::number(number);
        maxGuesses = number + 1;
    }
    currentHint->setText("Current hint: " + hint + " (" + correspondingNumber + ")"); // Update the hint

    // Add the hint to the chat box
    QString currSpymasterName = (currentTurn == RED_SPY) ? redSpyMasterName : blueSpyMasterName;
    QString teamColor = (currentTurn == RED_SPY) ? "Red" : "Blue";
    QString chatNumber = (number == 0) ? "∞" : QString::number(number);
    QString hintMessage = currSpymasterName + " gives clue " + hint + " " + chatNumber;
    chatBox->addSystemMessage(hintMessage, (currentTurn == RED_SPY) ? ChatBox::RED_TEAM : ChatBox::BLUE_TEAM);

    nextTurn();
}

void GameBoard::nextTurn() {
    // Switch to the next player's turn
    currentTurn = (currentTurn + 1) % 4;

    // Hide Board for next player
    if (currentTurn == RED_OP || currentTurn == BLUE_OP) {
        // Reset guess count at start of operative turn
        currentGuesses=0;
        
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                if (!gameGrid[i][j].revealed) {
                    cards[i][j]->setEnabled(true);
                    cards[i][j]->setStyleSheet("background-color: #f0f0f0; color: black");
                } 
                else {
                    cards[i][j]->setEnabled(false);  // Ensure revealed cards stay disabled
                }
            }
        }
        // Remove spymaster widget
        spymasterHint->hide();
        operatorGuess->setVisible(true);
        operatorGuess->setEnabled(true);
        operatorGuess->reset();
    }

    // Reveal the board for spymaster
    if (currentTurn == RED_SPY || currentTurn == BLUE_SPY) {
        currentHint->setText("Current hint: "); // Clear the hint
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                cards[i][j]->setEnabled(false);
                switch (gameGrid[i][j].type) {
                    case RED_TEAM:
                        cards[i][j]->setStyleSheet("background-color: #ff9999; color: black");
                        break;
                    case BLUE_TEAM:
                        cards[i][j]->setStyleSheet("background-color: #9999ff; color: black");
                        break;
                    case NEUTRAL:
                        cards[i][j]->setStyleSheet("background-color: #f0f0f0; color: black");
                        break;
                    case ASSASSIN:
                        cards[i][j]->setStyleSheet("background-color: #333333; color: white;");
                        break;
                }            
            }
        }
        // Show spymaster widget
        spymasterHint->show();
        operatorGuess->setVisible(false);
    }

    // Update the team labels
    if(currentTurn == RED_SPY) {
        currentTurnLabel->setText("Current Turn: " + redSpyMasterName);
        currentPlayerName = redSpyMasterName;
    }
    else if(currentTurn == RED_OP) {
        currentTurnLabel->setText("Current Turn: " + redOperativeName);
        currentPlayerName = redOperativeName;
    }
    else if(currentTurn == BLUE_SPY) {
        currentTurnLabel->setText("Current Turn: " + blueSpyMasterName);
        currentPlayerName = blueSpyMasterName;
    }
    else if(currentTurn == BLUE_OP) {
        currentTurnLabel->setText("Current Turn: " + blueOperativeName);
        currentPlayerName = blueOperativeName;
    }
    chatBox->setPlayerName(currentPlayerName); // Update the chat box with the current player's name
}

void GameBoard::onContinueClicked() {
    transition->hide();

    // Reveal the board for spymaster
    if (currentTurn == RED_SPY || currentTurn == BLUE_SPY) {
        currentHint->setText("Current hint: ");
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                cards[i][j]->setEnabled(false);
                switch (gameGrid[i][j].type) {
                    case RED_TEAM:
                        cards[i][j]->setStyleSheet("background-color: #ff9999; color: black");
                        break;
                    case BLUE_TEAM:
                        cards[i][j]->setStyleSheet("background-color: #9999ff; color: black");
                        break;
                    case NEUTRAL:
                        cards[i][j]->setStyleSheet("background-color: #f0f0f0; color: black");
                        break;
                    case ASSASSIN:
                        cards[i][j]->setStyleSheet("background-color: #333333; color: white");
                        break;
                }            
            }
        }
        // Show spymaster widget
        spymasterHint->setEnabled(true);
        spymasterHint->show();
        operatorGuess->setEnabled(false);
        operatorGuess->setVisible(false);
        qDebug() << "Spymaster turn - Cards disabled, showing hint widget";
    }

    // Update the current turn label
    if (currentTurn == RED_SPY) {
        currentTurnLabel->setText("Current Turn: " + redSpyMasterName);
    } else if (currentTurn == BLUE_SPY) {
        currentTurnLabel->setText("Current Turn: " + blueSpyMasterName);
    }

    qDebug() << "Continue clicked, spymaster turn set up. Current turn:" << currentTurn;
}

void GameBoard::showTransition() {
    QString nextSpymasterName;

    // Determine the name of the next spymaster
    if (currentTurn == RED_SPY) {
        nextSpymasterName = redSpyMasterName;
    } 
    else if (currentTurn == BLUE_SPY) {
        nextSpymasterName = blueSpyMasterName;
    } 
    else {
        qDebug() << "Error: showTransition called when currentTurn is not a spymaster turn:" << currentTurn;
        return;
    }

    // Disable the board
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            cards[i][j]->setEnabled(false);
            if(gameGrid[i][j].revealed == false) {
                cards[i][j]->setStyleSheet("background-color: #f0f0f0; color: black");
            }
        }
    }

    // Hide the spymaster widget and show the operator guess widget
    spymasterHint->setEnabled(false);
    operatorGuess->setEnabled(false);
    spymasterHint->setVisible(false);
    operatorGuess->setVisible(false);

    transition->setMessage("Please pass the device to " + nextSpymasterName);
    transition->show();
}

void GameBoard::updateScores() {
    redScoreLabel->setText("Red Cards Remaining: " + QString::number(redCardsRemaining));
    blueScoreLabel->setText("Blue Cards Remaining: " + QString::number(blueCardsRemaining));
}

void GameBoard::checkGameEnd() {
    // Check if the game has ended
    users = User::instance();

    // Red team wins
    if (redCardsRemaining == 0) {
        users->won(redSpyMasterName);
        users->won(redOperativeName);
        users->lost(blueSpyMasterName);
        users->lost(blueOperativeName);
        endGame("Red Team Wins!");

        return;
    }
    // Blue team wins
    if (blueCardsRemaining == 0) {
        users->won(blueSpyMasterName);
        users->won(blueOperativeName);
        users->lost(redSpyMasterName);
        users->lost(redOperativeName);
        endGame("Blue Team Wins!");
        
        return;
    }

    // Check if the assassin card has been revealed
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (gameGrid[i][j].revealed && gameGrid[i][j].type == ASSASSIN) {
                if (currentTurn == RED_OP) {
                    users->won(blueSpyMasterName);
                    users->won(blueOperativeName);
                    users->lost(redSpyMasterName);
                    users->lost(redOperativeName);

                    endGame("Blue Team Wins! Red Team hit the Assassin card.");
                } else if (currentTurn == BLUE_OP) {
                    users->won(redSpyMasterName);
                    users->won(redOperativeName);
                    users->lost(blueSpyMasterName);
                    users->lost(blueOperativeName);

                    endGame("Red Team Wins! Blue Team hit the Assassin card.");
                }
                return;
            }
        }
    }
}

void GameBoard::endGame(const QString& message) {
    // Disable all elements
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            cards[i][j]->setEnabled(false);
        }
    }
    spymasterHint->setEnabled(false);
    operatorGuess->setEnabled(false);
    transition->hide();

    // Show a pop-up with the game result and an "OK" button
    QMessageBox endGameBox;
    endGameBox.setWindowTitle("Game Over");
    endGameBox.setText(message);
    endGameBox.setStandardButtons(QMessageBox::Ok);
    endGameBox.setDefaultButton(QMessageBox::Ok);
    endGameBox.exec();

    // Emit the gameEnded signal to notify PreGame to return to the main menu
    emit gameEnded();

    this->close();
}

void GameBoard::resetGame() {

    // Load words from file
    loadWordsFromFile();
    generateGameGrid();

    // Reset the UI elements
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            cards[i][j]->setText(gameGrid[i][j].word);
            cards[i][j]->setEnabled(false);
            switch (gameGrid[i][j].type) {
                case RED_TEAM:
                    cards[i][j]->setStyleSheet("background-color: #ff9999; color: black");
                    break;
                case BLUE_TEAM:
                    cards[i][j]->setStyleSheet("background-color: #9999ff; color: black");
                    break;
                case NEUTRAL:
                    cards[i][j]->setStyleSheet("background-color: #f0f0f0; color: black");
                    break;
                case ASSASSIN:
                    cards[i][j]->setStyleSheet("background-color: #333333; color: white;");
                    break;
            }
        }
    }

    // Reset scores
    redCardsRemaining = 0;
    blueCardsRemaining = 0;

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (gameGrid[i][j].type == RED_TEAM) redCardsRemaining++;
            if (gameGrid[i][j].type == BLUE_TEAM) blueCardsRemaining++;
        }
    }
    updateScores();

    // Reset turn and labels
    currentTurn = RED_SPY;
    currentTurnLabel->setText("Current Turn: " + redSpyMasterName);
    currentHint->setText("Current hint: ");

    // Reset guess tracking variables
    maxGuesses=0;
    currentGuesses=0;

    // Reset widget states
    spymasterHint->setEnabled(true); 
    spymasterHint->show();

    spymasterHint->reset();

    operatorGuess->setEnabled(true);
    operatorGuess->setVisible(false);

    operatorGuess->reset();

    transition->hide();
    chatBox->clearChat(); // Clear the chat box
    chatBox->setPlayerName(redSpyMasterName); // Reset the player name in the chat box
}