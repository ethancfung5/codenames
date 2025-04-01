# Codenames Game

Codenames is a multiplayer word association game where two teams compete to find all their agents' codenames first. This implementation uses Qt for the user interface and WebSockets for real-time interaction between players.

## Requirements


#### Network Requirements for Online Play

The network you are using must allow websocket traffic. In our testing, the uwosecure-v2 and eduroam wifi's don't have it configured, but something like a cellular hotspot allows for it.

Another solution is to utilize tunnels (such as ngrok... etc) to allow external devices to connect to your server

#### Dependencies

To build and run this application, you will need the following dependencies:

- Qt Core
- Qt GUI
- Qt Widgets
- Qt WebSockets
- g++ (GNU Compiler Collection)
- make (Build Automation Tool)



To install the required dependencies, use the following command:

```bash
sudo apt update && sudo apt install -y qtbase5-dev libqt5websockets5-dev g++ make
```


## Building and Running the Project
### 1. Clone the Repository
Clone the repository or download the project files:

```bash
git clone https://github.com/AshtonF04/codenames.git
cd codenames
```

### 2. Build the application
Run the following commands in the project directory:

```bash
qmake
make clean
make
```

### 3. Run the Application
After successfully building the project, in the codenames directory, execute the application:
- On Linux, use the following command:
  ```bash
  ./bin/Codenames
  ```
- On Mac, use the following command:
  ```bash
  ./bin/Codenames.app/Contents/MacOS/Codenames
  ```


## Features
- Real-time multiplayer gameplay with WebSockets for seamless multiplayer experience.
- Real-time local gameplay for local play.
- Intuitive graphical interface built using Qt's GUI and widgets.
- Support for multiple platforms (Linux/macOS).
- A fun and challenging game where players guess the correct codenames based on clues.