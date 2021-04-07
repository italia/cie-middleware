# Istruzioni per compilare la libreria di firma

## Requisiti
- CMake
- Visual Studio Code 2019

## Dipendendenze
- libcurl
- bzip
- cryptopp
- freetype
- libiconv
- libpng
- libxml2
- openssl
- podofo
- zlib

## Step
- Creare una cartella con il nome *Dependencies* nella root del progetto che conterrà tutte le dipendenze precedentemente elencate. Per ogni dipendenza va creata la cartella con il nome e che conterrà sia la libreria statica, sia i relativi header files. 

    Ad esempio: 

    ```
    Dependencies
    │
    └───libcurl
    │   │   libcurl.lib
    │   └───include
    │       └───curl
    │       │   headerfile1.h
    │       │   headerfile2.h
    │       │   ...
    ```
- Creare una cartella con il nome *build* nella root del progetto che conterrò tutti i file di build. 
- Aprire il terminale, spostarsi nella cartella *build* ed eseguire il seguente comando:
    
    *cmake ..*
- Eseguire il comando:

    *cmake --build .*

- Al termine della build eseguire il comando:

    *cmake --install .*  
