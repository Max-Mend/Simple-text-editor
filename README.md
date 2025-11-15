# Simple Text Editor 

A minimalist, lightweight text editor built with Qt6, featuring a clean interface and essential editing functionality.

##  Screenshot

![Simple Text Editor](photo/1.png)

##  Features

### File Operations
- **New File** - Start with a blank document
- **Open File** - Open any text file from your system
- **Open Folder** - Browse and select folders
- **Save File** - Save your work to any location
- **File Status** - Visual feedback when files are saved

### Editor
- **Plain Text Editing** - Fast and responsive text editing
- **Monospace Font** - JetBrains Mono 14pt for comfortable coding/writing
- **Large Viewing Area** - Distraction-free editing experience

### Interface
- **Menu Bar** - Classic menu-driven interface
- **File Menu** - New, Open File, Open Folder
- **Edit Menu** - Run, Save File, Exit
- **Clean Design** - Minimalist UI focused on content
- **1200x800 Window** - Comfortable default size

##  Quick Start

### Prerequisites

**Ubuntu/Debian:**
```bash
sudo apt install qt6-base-dev
```

**Fedora:**
```bash
sudo dnf install qt6-qtbase-devel
```

**Arch Linux:**
```bash
sudo pacman -S qt6-base
```

### Build from Source

```bash
git clone https://github.com/Max-Mend/simple-text-editor.git
cd simple-text-editor
mkdir build && cd build
cmake ..
make
./text-editor
```

### Or compile directly:
```bash
g++ main.cpp -o text-editor $(pkg-config --cflags --libs Qt6Widgets)
./text-editor
```

##  Usage

### Opening Files
1. Click **File → Open File** or use keyboard shortcuts
2. Select your file from the dialog
3. Start editing

### Saving Files
1. Click **Edit → Save File**
2. Choose location and filename
3. Confirmation appears in status bar

### Keyboard Navigation
- Use standard text editing shortcuts (Ctrl+C, Ctrl+V, Ctrl+X)
- Menu shortcuts available with Alt key

##  Technical Details

### Built With
- **Qt6** - Cross-platform application framework
- **QPlainTextEdit** - Efficient plain text editing widget
- **C++17** - Modern C++ standards

### Font
- **JetBrains Mono** - Modern monospace font
- Editor: 14pt
- Menu: 11pt
- Fixed-pitch for alignment

### Architecture
- Single-file application for simplicity
- Lambda-based signal/slot connections
- File I/O using Qt's QFile API

##  Supported File Types

Opens and saves **all file types**:
- `.txt` - Plain text files
- `.cpp`, `.h`, `.c` - C/C++ source files
- `.py`, `.js`, `.html`, `.css` - Web and scripting files
- `.md`, `.json`, `.xml` - Markup and data files
- And any other text-based files

## Customization

### Change Font
Edit `main.cpp` line 18:
```cpp
QFont font("JetBrain Mono", 14);  // Change font name and size
```

### Change Window Size
Edit `main.cpp` line 69:
```cpp
mainWindow.resize(1200, 800);  // Change width and height
```

## Known Limitations

- No syntax highlighting
- No line numbers
- No find/replace functionality
- No undo/redo (uses Qt's default)
- "Run" menu item is not implemented
- "New File" clears current content without saving prompt

##  Contributing

Contributions are welcome! Feel free to:
- Report bugs via Issues
- Suggest new features
- Submit pull requests
- Improve documentation

##  License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

##  Requirements

- **Qt6** (Qt6Widgets module)
- **C++ Compiler** (GCC, Clang, or MSVC)
- **CMake** 3.16+ (for building)
- **JetBrains Mono font** (optional, falls back to system font)

## Use Cases

Perfect for:
- Quick text editing
- Viewing log files
- Editing configuration files
- Simple code editing
- Note-taking
- Markdown writing

##  Author

**Max-Mend**

##  Acknowledgments

- Built with Qt Framework
- JetBrains Mono font
- Inspired by minimalist text editors

---

*Simple, fast, and efficient text editing.* 
