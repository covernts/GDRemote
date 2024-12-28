# GD Remote
GD Remote is a mod for Geometry Dash that starts a web panel for controlling various modifiers in the game. The panel allows you to toggle different hacks and features dynamically through a web interface.

## Usage
1. Start Geometry Dash with the mod installed.
2. Open your web browser and navigate to `http://localhost:3000/` to access the modifier panel.
3. Use the checkboxes and buttons to toggle different hacks and features.

## Build
To build this mod, make sure you have the following tools installed:
- **CMake**
- **MSVC / Clang**
- **Git**
- **Geode CLI**
- **Geode SDK**

### Steps:
```bash
git clone https://github.com/covernts/GDRemote
cd GDRemote
geode build
```

## Libraries Used
- [Geode](https://github.com/geode-sdk/geode) - A modding framework for Geometry Dash.
- [Crow](https://github.com/CrowCpp/Crow) - A HTTP and Websocket framework.
- [Asio](https://github.com/chriskohlhoff/asio) - Basic networking library.