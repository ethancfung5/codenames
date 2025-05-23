/**
 * @file user.cpp
 * @author Team 9 - UWO CS 3307
 * @brief User class to handle local log in and loading/storing json files
 * @version 0.1
 * @date 2025-03-30
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "user.h"

User* User::instance(QWidget* parent) {
  static User* _instance = nullptr;
  if (!_instance) {
    _instance = new User(parent);
  }

  return _instance;
}

User::User(QWidget* parent) : QWidget(parent) {
  this->setFixedSize(1000, 600);

  QVBoxLayout* layout = new QVBoxLayout(this);

  backButton = new QPushButton("Back to Main Menu", this);
  layout->addWidget(backButton);

  createAccountButton = new QPushButton("Create Account", this);
  layout->addWidget(createAccountButton);

  jsonContentLabel = new QLabel("Loading...", this);

  layout->addWidget(jsonContentLabel);

  // Create and populate the drop-down menu with usernames
  usernameComboBox = new QComboBox(this);
  layout->addWidget(usernameComboBox);

  loginButton = new QPushButton("Login", this);
  layout->addWidget(loginButton);

  setLayout(layout);

  connect(loginButton, &QPushButton::clicked, this, &User::handleLogin);

  // createAccountWindow = CreateAccountWindow::getInstance();
  connect(backButton, &QPushButton::clicked, this, &User::showMainMenu);

  createAccountWindow = CreateAccountWindow::getInstance();

  connect(createAccountButton, &QPushButton::clicked, this,
          &User::handleCreateAccount);

  connect(createAccountWindow, &CreateAccountWindow::accountCreated, this,
          &User::refreshUserDropdown);

  // Load usernames from the JSON file and populate the drop-down menu
  QJsonObject jsonObject = loadJsonFile();
  if (!jsonObject.isEmpty()) {
    populateUsernameComboBox(jsonObject);
  }
}

void User::show() {
  QJsonObject jsonData = loadJsonFile();  // Reload JSON to get latest data
  populateUsernameComboBox(jsonData);
  QWidget::show();
  qDebug() << "User shown";
}

void User::handleCreateAccount() {
  this->hide();
  createAccountWindow->setPreviousScreen(this);
  createAccountWindow->show();
}

void User::showMainMenu() {
  this->hide();
  emit backToMainMenu();
}

void User::refreshUserDropdown() {
  QJsonObject jsonData = loadJsonFile();  // Reload latest data
  populateUsernameComboBox(jsonData);     // Refresh the dropdown
}

void User::populateUsernameComboBox(const QJsonObject& jsonObject) {
  // Loop through the JSON object and add usernames to the combo box
  usernameComboBox->clear();
  QJsonObject::const_iterator it = jsonObject.constBegin();
  while (it != jsonObject.constEnd()) {
    QString username = it.key();          // Extract username
    usernameComboBox->addItem(username);  // Add to combo box
    ++it;
  }
}

User::~User() {}

QJsonObject User::loadJsonFile() {
  QFile file(jsonFilePath);
  QDir dir = QFileInfo(file).absoluteDir();
  QString absolutePath = dir.filePath(file.fileName());

  if (!file.exists() || file.size() == 0) {
    jsonContentLabel->setText("No profile found. Please sign up.");
    return QJsonObject();  // Return empty object if no profile exists
  }

  if (!file.open(QIODevice::ReadOnly)) {
    jsonContentLabel->setText("Error: Could not open profile.json");
    qDebug() << "Failed to open " << absolutePath;
    return QJsonObject();  // Return empty object on error
  }

  QByteArray jsonData = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  if (!doc.isObject()) {
    jsonContentLabel->setText("Error: Invalid JSON format");
    qDebug() << "Invalid JSON format";
    return QJsonObject();  // Return empty object on invalid JSON
  }

  QJsonObject jsonObject = doc.object();
  if (jsonObject.isEmpty()) {
    jsonContentLabel->setText("Profile is empty. Please sign up.");
    return QJsonObject();  // Return empty object if no users in profile
  }

  jsonContentLabel->setText("Profile found. Please log in.");
  return jsonObject;
}

void User::handleLogin() {
  // Get the selected username from the combo box
  QString selectedUsername = usernameComboBox->currentText().trimmed();

  // Check if a username is selected
  if (selectedUsername.isEmpty()) {
    jsonContentLabel->setText("Please select a username.");
    return;
  }

  QFile file(jsonFilePath);  // Use the relative path

  if (!file.open(QIODevice::ReadOnly)) {
    jsonContentLabel->setText("Error: Could not open profile.json");
    qDebug() << "Failed to open profile.json";
    return;
  }

  QByteArray jsonData = file.readAll();
  file.close();

  qDebug() << "Raw JSON data:"
           << jsonData;  // Print the raw content for debugging

  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  if (!doc.isObject()) {
    jsonContentLabel->setText("Error: Invalid JSON format");
    qDebug() << "Invalid JSON format. Document is not an object.";
    return;
  }

  QJsonObject jsonObject = doc.object();

  // Check if the selected username exists in the JSON object
  if (!jsonObject.contains(selectedUsername)) {
    jsonContentLabel->setText("Login failed. User not found.");
    qDebug() << "User not found: " << selectedUsername;
    return;
  }

  QJsonObject userObject = jsonObject[selectedUsername].toObject();
  QString storedUsername = userObject["user_name"].toString();

  qDebug() << "Stored Username: " << storedUsername;

  // Check if the selected username matches the stored username
  if (selectedUsername == storedUsername) {
    jsonContentLabel->setText("Login successful. Welcome, " + storedUsername +
                              "!");
    qDebug() << "User logged in: " << storedUsername;
  } else {
    jsonContentLabel->setText("Login failed. Check credentials.");
    qDebug() << "Login failed for user: " << selectedUsername;
  }
}

unsigned int User::getGamesPlayed(const QString& username) const {
  QFile file(jsonFilePath);
  QDir dir = QFileInfo(file).absoluteDir();
  QString absolutePath = dir.filePath(file.fileName());

  if (!file.exists()) {
    qDebug() << "Error: profile.json does not exist.";
    jsonContentLabel->setText("Error: No user data found.");
    return 0;  // Indicates error
  }

  // Read existing JSON data
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open" << absolutePath << " for reading.";
    jsonContentLabel->setText("Error: Could not read profile.json");
    return 0;
  }

  QByteArray jsonData = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  if (!doc.isObject()) {
    qDebug() << "Invalid JSON format.";
    jsonContentLabel->setText("Error: Invalid profile format.");
    return 0;
  }

  QJsonObject jsonObject = doc.object();

  // Check if user exists
  if (!jsonObject.contains(username)) {
    qDebug() << "User not found:" << username;
    jsonContentLabel->setText("Error: User does not exist.");
    return 0;
  }

  // Get user object
  QJsonObject userObject = jsonObject[username].toObject();

  // Check if "statistics" exists and contains "games_win"
  if (!userObject.contains("statistics") ||
      !userObject["statistics"].isObject()) {
    qDebug() << "Error: No statistics found for user:" << username;
    jsonContentLabel->setText("Error: User statistics missing.");
    return 0;
  }

  QJsonObject statisticsObject = userObject["statistics"].toObject();
  int gamesPlayed = statisticsObject.contains("games_played")
                        ? statisticsObject["games_played"].toInt()
                        : 0;

  // Ensure gamesPlayed is non-negative
  unsigned int finalGamesPlayed = std::max(gamesPlayed, 0);

  qDebug() << "Retrieved games played for user:" << username
           << "| Games played count:" << finalGamesPlayed;
  return finalGamesPlayed;
}

void User::updateGamesPlayed(const QString& username,
                             const unsigned int& newGamesPlayed) {
  QFile file(jsonFilePath);
  QDir dir = QFileInfo(file).absoluteDir();
  QString absolutePath = dir.filePath(file.fileName());

  if (!file.exists()) {
    qDebug() << "Error: profile.json does not exist.";
    jsonContentLabel->setText("Error: No user data found.");
    return;
  }

  // Read existing JSON data
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open" << absolutePath << " for reading.";
    jsonContentLabel->setText("Error: Could not read profile.json");
    return;
  }

  QByteArray jsonData = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  if (!doc.isObject()) {
    qDebug() << "Invalid JSON format.";
    jsonContentLabel->setText("Error: Invalid profile format.");
    return;
  }

  QJsonObject jsonObject = doc.object();

  // Check if user exists
  if (!jsonObject.contains(username)) {
    qDebug() << "User not found:" << username;
    jsonContentLabel->setText("Error: User does not exist.");
    return;
  }

  // Get user object
  QJsonObject userObject = jsonObject[username].toObject();

  // Ensure statistics object exists
  QJsonObject statisticsObject;
  if (userObject.contains("statistics") &&
      userObject["statistics"].isObject()) {
    statisticsObject = userObject["statistics"].toObject();
  } else {
    qDebug() << "Error: No statistics found for user:" << username;
    jsonContentLabel->setText("Error: User statistics missing.");
    return;
  }

  // Get the current wins count
  int currentWins = statisticsObject.contains("games_won")
                        ? statisticsObject["games_won"].toInt()
                        : 0;

  // Check if the new games played is smaller than the wins count
  if ((int)newGamesPlayed < currentWins) {
    qDebug() << "Error: New games played cannot be smaller than games won.";
    jsonContentLabel->setText(
        "Error: New games played count cannot be smaller than wins.");
    return;
  }

  // Update wins count
  statisticsObject["games_played"] = (int)newGamesPlayed;
  userObject["statistics"] = statisticsObject;
  jsonObject[username] = userObject;

  // Write updated JSON data
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Failed to write to" << absolutePath;
    jsonContentLabel->setText("Error: Could not write to profile.json");
    return;
  }

  QJsonDocument newDoc(jsonObject);
  file.write(newDoc.toJson(QJsonDocument::Indented));
  file.close();

  qDebug() << "Updated games played for user:" << username
           << "| New games played count:"
           << statisticsObject["games_played"].toInt();
  jsonContentLabel->setText("Games played count updated for " + username);
}

unsigned int User::getWins(const QString& username) const {
  QFile file(jsonFilePath);
  QDir dir = QFileInfo(file).absoluteDir();
  QString absolutePath = dir.filePath(file.fileName());

  if (!file.exists()) {
    qDebug() << "Error: profile.json does not exist.";
    jsonContentLabel->setText("Error: No user data found.");
    return 0;  // Indicates error
  }

  // Read existing JSON data
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open" << absolutePath << " for reading.";
    jsonContentLabel->setText("Error: Could not read profile.json");
    return 0;
  }

  QByteArray jsonData = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  if (!doc.isObject()) {
    qDebug() << "Invalid JSON format.";
    jsonContentLabel->setText("Error: Invalid profile format.");
    return 0;
  }

  QJsonObject jsonObject = doc.object();

  // Check if user exists
  if (!jsonObject.contains(username)) {
    qDebug() << "User not found:" << username;
    jsonContentLabel->setText("Error: User does not exist.");
    return 0;
  }

  // Get user object
  QJsonObject userObject = jsonObject[username].toObject();

  // Check if "statistics" exists and contains "games_win"
  if (!userObject.contains("statistics") ||
      !userObject["statistics"].isObject()) {
    qDebug() << "Error: No statistics found for user:" << username;
    jsonContentLabel->setText("Error: User statistics missing.");
    return 0;
  }

  QJsonObject statisticsObject = userObject["statistics"].toObject();
  int wins = statisticsObject.contains("games_win")
                 ? statisticsObject["games_win"].toInt()
                 : 0;

  // Ensure gamesPlayed is non-negative
  unsigned int finalWins = std::max(wins, 0);

  qDebug() << "Retrieved wins for user:" << username
           << "| Wins count:" << finalWins;
  return finalWins;
}

float User::getWinRate(const QString& username) const {
  unsigned int gamesPlayed = getGamesPlayed(username);
  unsigned int gamesWin = getWins(username);

  if (gamesPlayed == 0) {
    return 0;
  }

  float winRate = (float)gamesWin / (float)gamesPlayed;
  return winRate;
}

void User::updateWins(const QString& username, const unsigned int& newWins) {
  QFile file(jsonFilePath);
  QDir dir = QFileInfo(file).absoluteDir();
  QString absolutePath = dir.filePath(file.fileName());

  if (!file.exists()) {
    qDebug() << "Error: profile.json does not exist.";
    jsonContentLabel->setText("Error: No user data found.");
    return;
  }

  // Read existing JSON data
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open" << absolutePath << " for reading.";
    jsonContentLabel->setText("Error: Could not read profile.json");
    return;
  }

  QByteArray jsonData = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  if (!doc.isObject()) {
    qDebug() << "Invalid JSON format.";
    jsonContentLabel->setText("Error: Invalid profile format.");
    return;
  }

  QJsonObject jsonObject = doc.object();

  // Check if user exists
  if (!jsonObject.contains(username)) {
    qDebug() << "User not found:" << username;
    jsonContentLabel->setText("Error: User does not exist.");
    return;
  }

  // Get user object
  QJsonObject userObject = jsonObject[username].toObject();

  // Ensure statistics object exists
  QJsonObject statisticsObject;
  if (userObject.contains("statistics") &&
      userObject["statistics"].isObject()) {
    statisticsObject = userObject["statistics"].toObject();
  } else {
    qDebug() << "Error: No statistics found for user:" << username;
    jsonContentLabel->setText("Error: User statistics missing.");
    return;
  }

  // Get the current wins count
  int currentGamesPlayed = statisticsObject.contains("games_played")
                               ? statisticsObject["games_played"].toInt()
                               : 0;

  // Check if the new games played is smaller than the wins count
  if ((int)newWins > currentGamesPlayed) {
    qDebug() << "Error: New games won cannot be greater than games played.";
    jsonContentLabel->setText(
        "Error: New games win count cannot be greater than games played.");
    return;
  }

  // Update wins count
  statisticsObject["games_win"] = (int)newWins;
  userObject["statistics"] = statisticsObject;
  jsonObject[username] = userObject;

  // Write updated JSON data
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Failed to write to" << absolutePath;
    jsonContentLabel->setText("Error: Could not write to profile.json");
    return;
  }

  QJsonDocument newDoc(jsonObject);
  file.write(newDoc.toJson(QJsonDocument::Indented));
  file.close();

  qDebug() << "Updated wins for user:" << username
           << "| New wins count:" << statisticsObject["games_win"].toInt();
  jsonContentLabel->setText("Win count updated for " + username);
}

unsigned int User::getGuessTotal(const QString& username) const {
  QFile file(jsonFilePath);
  QDir dir = QFileInfo(file).absoluteDir();
  QString absolutePath = dir.filePath(file.fileName());

  if (!file.exists()) {
    qDebug() << "Error: profile.json does not exist.";
    jsonContentLabel->setText("Error: No user data found.");
    return 0;  // Indicates error
  }

  // Read existing JSON data
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open" << absolutePath << " for reading.";
    jsonContentLabel->setText("Error: Could not read profile.json");
    return 0;
  }

  QByteArray jsonData = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  if (!doc.isObject()) {
    qDebug() << "Invalid JSON format.";
    jsonContentLabel->setText("Error: Invalid profile format.");
    return 0;
  }

  QJsonObject jsonObject = doc.object();

  // Check if user exists
  if (!jsonObject.contains(username)) {
    qDebug() << "User not found:" << username;
    jsonContentLabel->setText("Error: User does not exist.");
    return 0;
  }

  // Get user object
  QJsonObject userObject = jsonObject[username].toObject();

  // Check if "statistics" exists and contains "games_win"
  if (!userObject.contains("statistics") ||
      !userObject["statistics"].isObject()) {
    qDebug() << "Error: No statistics found for user:" << username;
    jsonContentLabel->setText("Error: User statistics missing.");
    return 0;
  }

  QJsonObject statisticsObject = userObject["statistics"].toObject();
  int guessTotal = statisticsObject.contains("guess_total")
                       ? statisticsObject["guess_total"].toInt()
                       : 0;

  // Ensure gamesPlayed is non-negative
  unsigned int finalGuessTotal = std::max(guessTotal, 0);

  qDebug() << "Retrieved total number of guess for user:" << username
           << "| Guess total count:" << finalGuessTotal;
  return finalGuessTotal;
}

void User::updateGuessTotal(const QString& username,
                            const unsigned int& newGuessTotal) {
  QFile file(jsonFilePath);
  QDir dir = QFileInfo(file).absoluteDir();
  QString absolutePath = dir.filePath(file.fileName());

  if (!file.exists()) {
    qDebug() << "Error: profile.json does not exist.";
    jsonContentLabel->setText("Error: No user data found.");
    return;
  }

  // Read existing JSON data
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open" << absolutePath << " for reading.";
    jsonContentLabel->setText("Error: Could not read profile.json");
    return;
  }

  QByteArray jsonData = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  if (!doc.isObject()) {
    qDebug() << "Invalid JSON format.";
    jsonContentLabel->setText("Error: Invalid profile format.");
    return;
  }

  QJsonObject jsonObject = doc.object();

  // Check if user exists
  if (!jsonObject.contains(username)) {
    qDebug() << "User not found:" << username;
    jsonContentLabel->setText("Error: User does not exist.");
    return;
  }

  // Get user object
  QJsonObject userObject = jsonObject[username].toObject();

  // Ensure statistics object exists
  QJsonObject statisticsObject;
  if (userObject.contains("statistics") &&
      userObject["statistics"].isObject()) {
    statisticsObject = userObject["statistics"].toObject();
  } else {
    qDebug() << "Error: No statistics found for user:" << username;
    jsonContentLabel->setText("Error: User statistics missing.");
    return;
  }

  // Get the current guess hit count
  int currentGuessHit = statisticsObject.contains("guess_hit")
                            ? statisticsObject["guess_hit"].toInt()
                            : 0;

  // Check if the new guess total is smaller than the guess hit count
  if ((int)newGuessTotal < currentGuessHit) {
    qDebug() << "Error: New guess total cannot be less than guess hit.";
    jsonContentLabel->setText(
        "Error: New guess total count cannot be less than guess hit.");
    return;
  }

  // Update guess total count
  statisticsObject["guess_total"] = (int)newGuessTotal;
  userObject["statistics"] = statisticsObject;
  jsonObject[username] = userObject;

  // Write updated JSON data
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Failed to write to" << absolutePath;
    jsonContentLabel->setText("Error: Could not write to profile.json");
    return;
  }

  QJsonDocument newDoc(jsonObject);
  file.write(newDoc.toJson(QJsonDocument::Indented));
  file.close();

  qDebug() << "Updated guess total for user:" << username
           << "| New guess total count:"
           << statisticsObject["guess_total"].toInt();
  jsonContentLabel->setText("Guess total count updated for " + username);
}

unsigned int User::getGuessHit(const QString& username) const {
  QFile file(jsonFilePath);
  QDir dir = QFileInfo(file).absoluteDir();
  QString absolutePath = dir.filePath(file.fileName());

  if (!file.exists()) {
    qDebug() << "Error: profile.json does not exist.";
    jsonContentLabel->setText("Error: No user data found.");
    return 0;  // Indicates error
  }

  // Read existing JSON data
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open" << absolutePath << " for reading.";
    jsonContentLabel->setText("Error: Could not read profile.json");
    return 0;
  }

  QByteArray jsonData = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  if (!doc.isObject()) {
    qDebug() << "Invalid JSON format.";
    jsonContentLabel->setText("Error: Invalid profile format.");
    return 0;
  }

  QJsonObject jsonObject = doc.object();

  // Check if user exists
  if (!jsonObject.contains(username)) {
    qDebug() << "User not found:" << username;
    jsonContentLabel->setText("Error: User does not exist.");
    return 0;
  }

  // Get user object
  QJsonObject userObject = jsonObject[username].toObject();

  // Check if "statistics" exists and contains "games_win"
  if (!userObject.contains("statistics") ||
      !userObject["statistics"].isObject()) {
    qDebug() << "Error: No statistics found for user:" << username;
    jsonContentLabel->setText("Error: User statistics missing.");
    return 0;
  }

  QJsonObject statisticsObject = userObject["statistics"].toObject();
  int guessHit = statisticsObject.contains("guess_hit")
                     ? statisticsObject["guess_hit"].toInt()
                     : 0;

  // Ensure gamesPlayed is non-negative
  unsigned int finalGuessHit = std::max(guessHit, 0);

  qDebug() << "Retrieved guess hit for user:" << username
           << "| Guess hit count:" << finalGuessHit;
  return finalGuessHit;
}

float User::getHitRate(const QString& username) {
  unsigned int guessTotal = getGuessTotal(username);
  unsigned int guessHit = getGuessHit(username);

  if (guessTotal == 0) {
    return 0;
  }

  float hitRate = (float)guessHit / (float)guessTotal;
  return hitRate;
}

void User::updateGuessHit(const QString& username,
                          const unsigned int& newGuessHit) {
  QFile file(jsonFilePath);
  QDir dir = QFileInfo(file).absoluteDir();
  QString absolutePath = dir.filePath(file.fileName());

  if (!file.exists()) {
    qDebug() << "Error: profile.json does not exist.";
    jsonContentLabel->setText("Error: No user data found.");
    return;
  }

  // Read existing JSON data
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open" << absolutePath << " for reading.";
    jsonContentLabel->setText("Error: Could not read profile.json");
    return;
  }

  QByteArray jsonData = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  if (!doc.isObject()) {
    qDebug() << "Invalid JSON format.";
    jsonContentLabel->setText("Error: Invalid profile format.");
    return;
  }

  QJsonObject jsonObject = doc.object();

  // Check if user exists
  if (!jsonObject.contains(username)) {
    qDebug() << "User not found:" << username;
    jsonContentLabel->setText("Error: User does not exist.");
    return;
  }

  // Get user object
  QJsonObject userObject = jsonObject[username].toObject();

  // Ensure statistics object exists
  QJsonObject statisticsObject;
  if (userObject.contains("statistics") &&
      userObject["statistics"].isObject()) {
    statisticsObject = userObject["statistics"].toObject();
  } else {
    qDebug() << "Error: No statistics found for user:" << username;
    jsonContentLabel->setText("Error: User statistics missing.");
    return;
  }

  // Get the current guess hit count
  int currentGuessTotal = statisticsObject.contains("guess_total")
                              ? statisticsObject["guess_total"].toInt()
                              : 0;

  // Check if the new guess total is smaller than the guess hit count
  if ((int)newGuessHit > currentGuessTotal) {
    qDebug() << "Error: New guess hit cannot be greater than guess total.";
    jsonContentLabel->setText(
        "Error: New guess hit count cannot be greater than guess total.");
    return;
  }

  // Update wins count
  statisticsObject["guess_hit"] = (int)newGuessHit;
  userObject["statistics"] = statisticsObject;
  jsonObject[username] = userObject;

  // Write updated JSON data
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Failed to write to" << absolutePath;
    jsonContentLabel->setText("Error: Could not write to profile.json");
    return;
  }

  QJsonDocument newDoc(jsonObject);
  file.write(newDoc.toJson(QJsonDocument::Indented));
  file.close();

  qDebug() << "Updated guess hit for user:" << username
           << "| New guess hit count:" << statisticsObject["guess_hit"].toInt();
  jsonContentLabel->setText("Guess hit count updated for " + username);
}

void User::renameUser(const QString& oldUsername, const QString& newUsername) {
  QFile file(jsonFilePath);
  QDir dir = QFileInfo(file).absoluteDir();
  QString absolutePath = dir.filePath(file.fileName());

  if (!file.exists()) {
    qDebug() << "Error: profile.json does not exist.";
    jsonContentLabel->setText("Error: No user data found.");
    return;
  }

  // Read existing JSON data
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open" << absolutePath << " for reading.";
    jsonContentLabel->setText("Error: Could not read profile.json");
    return;
  }

  QByteArray jsonData = file.readAll();
  file.close();

  QJsonDocument doc = QJsonDocument::fromJson(jsonData);
  if (!doc.isObject()) {
    qDebug() << "Invalid JSON format.";
    jsonContentLabel->setText("Error: Invalid profile format.");
    return;
  }

  QJsonObject jsonObject = doc.object();

  // Check if the old username exists
  if (!jsonObject.contains(oldUsername)) {
    qDebug() << "User not found:" << oldUsername;
    jsonContentLabel->setText("Error: User does not exist.");
    return;
  }

  // Check if the new username already exists
  if (jsonObject.contains(newUsername)) {
    qDebug() << "New username already exists:" << newUsername;
    jsonContentLabel->setText("Error: Username already taken.");
    return;
  }

  // Rename the user: Move data from old username to new username
  QJsonObject userObject = jsonObject[oldUsername].toObject();

  // Ensure profile object exists
  if (userObject.contains("profile") && userObject["profile"].isObject()) {
    QJsonObject profileObject = userObject["profile"].toObject();
    profileObject["player_name"] = newUsername;  // Update stored player_name
    userObject["profile"] = profileObject;
  } else {
    qDebug() << "Error: No profile data found for user:" << oldUsername;
    jsonContentLabel->setText("Error: User profile missing.");
    return;
  }

  jsonObject.remove(oldUsername);        // Remove old entry
  jsonObject[newUsername] = userObject;  // Insert under new username

  // Write updated JSON data
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Failed to write to" << absolutePath;
    jsonContentLabel->setText("Error: Could not write to profile.json");
    return;
  }

  QJsonDocument newDoc(jsonObject);
  file.write(newDoc.toJson(QJsonDocument::Indented));
  file.close();

  qDebug() << "User renamed from" << oldUsername << "to" << newUsername;
  jsonContentLabel->setText("Username successfully changed.");
}

void User::won(const QString& username) {
  updateGamesPlayed(username, getGamesPlayed(username) + 1);
  updateWins(username, getWins(username) + 1);
}

void User::lost(const QString& username) {
  updateGamesPlayed(username, getGamesPlayed(username) + 1);
}

void User::hit(const QString& username) {
  updateGuessTotal(username, getGuessTotal(username) + 1);
  updateGuessHit(username, getGuessHit(username) + 1);
}

void User::miss(const QString& username) {
  updateGuessTotal(username, getGuessTotal(username) + 1);
}