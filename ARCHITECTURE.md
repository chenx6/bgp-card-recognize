# Architecture

## File structure

```plaintext
.
├── ARCHITECTURE.md
├── build
├── CMakeLists.txt
├── include
│   ├── encode.h
│   ├── find_cards.h
│   └── hash.h
├── README.md
└── src
    ├── encode.cpp     # Encode card information
    ├── find_cards.cpp # Card finding algorithm
    ├── hash.cpp       # Image hashing algorithm(imitated bestdori)
    └── main.cpp       # Program entry
```

## Program logic

```plaintext
main.cpp: Get image -> Turn image into Gray Image -> threshould
find_cards.cpp: Get the left top card -> Calculate blank line's position between cards -> Get all cards
hash.cpp: include some algorithm that was imitated bestdori(the algorithm is FFT?)
encode.cpp: encode card information to bestdori's card data
```
