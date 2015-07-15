# TRLEScriptModern
A modern (unofficial) edition of the Tomb Raider Level Editor Script Compiler

## Table of contents
0. Basic information
1. Usage
  1. Using the terminal/command promp
  2. Using the drag-and-drop functionality
2. Known bugs
3. New features
4. Syntax
  1. Gameflow scripts
    1. Supported commands for the PSXExtensions and PCExtensions chunks
    2. Supported commands for the Language chunk
    3. Supported commands for the Options chunk
    4. Supported commands for the Title chunk
    5. Supported commands for the Level chunk
5. Miscalleneous
6. Notes regarding compiling the source code of this program
7. Wiki
8. Disclaimer

## 0. Basic information
This is a tiny commandline tool which mimics the functionality of the DOS-only script compiler published as part of the Tomb Raider Level Editor (TRLE) in 2000. The term *modern* alludes to the fact that it is written in portable C++11 and is free software, therefore enabling anyone to use it as part of their own code as they wish, as long as they respect the **GNU GPL**. Also, it could easily be extended to compile a superset of the commands that are currently supported, e.g. the one used as part of the Tomb Raider Next Generation Level Editor (TRNG).
Moreover, some bugs present in the original script compiler were fixed and the syntax was, where it seemed possible, made more intuitive for the end user than it was originally (this was done to fix some bugs; the listing is below).
In addition, this tool supports batch processing of more than one script file.

## 1. Usage
This tool has to be ran either via a terminal/command prompt or simply by dragging all the files that need to be compiled into the compiled program binary file. This section discusses both usage scenarios.
Files that can be compiled by this tool are the same ones that can be compiled by the original, official script compiler i.e. the gameflow script files (usually named *script.txt*) and the language/strings script files (usually named *English.txt*, *Italian.txt* etc.). However, *only* the gameflow script files can be instructed to the tool to compile; it then automatically goes to detect the respective language script files and compiles them in sequence. This behaviour is inherited from the original script compiler as well.
Whatever the file name of the input script is, the compiled (output) script file will always be named *input_script_file_without_extension.dat*, where *input_script_file_without_extension* depends, of course, on the input file name.
The output will consist of the respective files with the *.dat* suffix, under the condition the entire compilation process went by without any errors. If any error occurs during the compilation process, the program will terminate with corresponding messages in the terminal/command prompt. If an error occurs which is not fatal to the program (also known as a warning), only the corresponding messages will be plotted to the terminal/command prompt to notify the user of what is wrong in their source file(s).

### 1.i Using the terminal/command prompt
Once in the terminal/command prompt, the user has to navigate to the directory in which they have stored both the gameflow and the language scripts that they wish to compile.
Once there, entering the program binary file name (most likely **TRLEScriptModern**) will present the user with basic usage instructions, which read:
```Usage: TRLEScriptModern <infile1> <infile2> <infile3>... <infileN>...```

E.g. if the files *script_new.txt* and *script_old.txt* have to be compiled, the following command can be used in the terminal/command prompt:
```TRLEScriptModern script_new.txt script_old.txt```

### 1.ii Using the drag-and-drop functionality
In most modern operating systems, users can drag-and-drop files into the binary executable file of a certain program. This program uses this functionality so that when the user drags-and-drops input script files into the binary executable file of the program (usually **TRLEScriptModern**), they will be compiled sequentially in the same way as they would in the previous subsection.

## 2. Known bugs
Unfortunately, this tools suffers from certain same issues as the one it inherits its funtionality from, the original script compiler. Some of them are also linked to the original *tomb4* binary, as it poses, with its hardcoded and non-open nature of code, various problems in how it interprets the compiled script files. On the other hand, this tool also managed to fix some of them, and those will be noted below. A subset of these bugs seems to be previously unaknowledged, so here is a complete listing of what I have been able to detect:

* Inability to use the comma character in names of puzzle, pickup, examine, key, puzzle-combo, pickup-combo and key-combo item names - **Fixed in this modern edition**; the aforementioned strings can now all contain commas
* Inability to use the colon character in any string - **Fixed in this modern edition** with a quirk (possibly also works with the original compiler): every string which contains a colon (regardless of what position is it on - it can be the beginning, the ending or any other position in the string) *has* to be written *in the language script files* in a way such that it contains a colon at the very beginning of the string i.e. the colon must be its first character. This does *not* apply to gameflow script files - the string there has to be written normally, *as it should be displayed in-game*!
E.g. If the string *Task: Pick up the small medipack* must be used as a legend string, it should be written in the gameflow script file as: `Legend=Task: Pick up the small medipack`, and in the language script file as: `:Task: Pick up the small medipack`

* Unexpected in-game behaviour when multiple levels share a single level file (in a certain order) - **This bug could not have been fixed in this modern edition** because the original *tomb4* binary is responsible for it. Details: When a gameflow script contains multiple levels which share the same level path (e.g. DATA\TEST) sequentially laid out, there appears to be nothing wrong and the original *tomb4* binary handles this case normally. However, problems start to arise when the levels which share the same level path are *not* sequentially laid out; in this case, the original *tomb4* binary will usually crash upon level transitions or in-game behaviour will not be as expected.

## 3. New features
* Batch processing. The user can now provide multiple input files for the tool to compile to corresponding output script files.

## 4. Syntax
The syntax used for TRLE scripts is fairly simple. The compiler is *not* case-sensitive, except of course for the user-defined language strings which must be written exactly as they appear in the corresponding primary language script file (the one with the index 0 in the Language chunk).
Everything that comes after a semicolon (;) character and before a newline is *considered a comment and will not be compiled*.
### 4.i Gameflow scripts
First, global options for the game must be provided, followed by the list of supported languages and their respective language script file names and, finally, the title and level listing.
All of the aforementioned parts are divided into sections or chunks, which are written like this: `[ChunkName]`.
The supported gameflow chunks are: *PSXExtensions*, *PCExtensions*, *Language*, *Options*, *Title* and *Level*.
#### 4.i.a Supported commands for the PSXExtensions and PCExtensions chunks
* `Level=  a 3-letter extension goes here; must begin with a dot character`
* `Cut=  a 3-letter extension goes here; must begin with a dot character`
* `FMV=  a 3-letter extension goes here; must begin with a dot character`

#### 4.i.b Supported commands for the Language chunk

* `File= indexNumber,*language_filename*`

#### 4.i.c Supported commands for the Options chunk

* `LoadSave= DISABLED/ENABLED`
* `Title=  DISABLED/ENABLED`
* `PlayAnyLevel= DISABLED/ENABLED`
* `InputTimeout= an integer`
* `FlyCheat= DISABLED/ENABLED`
* `Security= an integer` - this is unused
* `DemoDisc= DISABLED/ENABLED`

#### 4.i.d Supported commands for the Title chunk

* `LoadCamera= sourceX,sourceY,sourceZ,targetX,targetY,targetZ,room` - all arguments are integers
* `LensFlare=  Y,Z,X,red,green,blue` - all arguments are integers; X and Y must be positive; red,green,blue:[0,255]
* `AnimatingMIP= animatingObjectIndex,blockDistance` - all arguments are integers
* `UVrotate= speed` - integer [-128,127]
* `Layer1= red,green,blue,speed` - all arguments are integers [0,255] except speed: [-128,127]
* `Layer2= red,green,blue,speed` - all arguments are integers [0,255] except speed: [-128,127]
* `Mirror= room,xAxis` - all arguments are integers
* `Fog=  red,green,blue` - all arguments are integers [0,255]
* `ResidentCut=  indexOfCutsceneInTitle,indexOfCutsceneInCUTSEQ` - all arguments are integers; first argument: [1,4]
* `Starfield=  DISABLED/ENABLED`
* `Weather=  DISABLED/ENABLED`
* `Horizon=  DISABLED/ENABLED`
* `ColAddHorizon=  DISABLED/ENABLED`
* `Lightning=  DISABLED/ENABLED`
* `Train=  DISABLED/ENABLED`
* `Pulse=  DISABLED/ENABLED`
* `Level=  titlePath,audioTrack` - titlePath is a string; audioTrack is an integer

#### 4.i.e Supported commands for the Level chunk

* `LoadCamera= sourceX,sourceY,sourceZ,targetX,targetY,targetZ,room` - all arguments are integers
* `LensFlare=  Y,Z,X,red,green,blue` - all arguments are integers; X and Y must be positive; red,green,blue:[0,255]
* `AnimatingMIP= animatingObjectIndex,blockDistance` - all arguments are integers
* `UVrotate= speed` - integer [-128,127]
* `Layer1= red,green,blue,speed` - all arguments are integers [0,255] except speed: [-128,127]
* `Layer2= red,green,blue,speed` - all arguments are integers [0,255] except speed: [-128,127]
* `Mirror= room,xAxis` - all arguments are integers
* `Fog=  red,green,blue` - all arguments are integers [0,255]
* `Cut=  indexOfCutsceneInCUTSEQ` - integer; denotes a cutscene which plays at the beginning of the level
* `FMV=  indexOfFMV,FMVTriggerBitfield` - all arguments are integers; denotes a FMV which plays at the end of the level
* `ResidentCut=  indexOfCutsceneInLevel,indexOfCutsceneInCUTSEQ` - all arguments are integers; first argument: [1,4]
* `Legend= string` - denotes a legend string which must be present in the language script
* `ResetHUB= levelIndex` - integer
* `YoungLara=  DISABLED/ENABLED`
* `Starfield=  DISABLED/ENABLED`
* `Weather=  DISABLED/ENABLED`
* `Horizon=  DISABLED/ENABLED`
* `ColAddHorizon=  DISABLED/ENABLED`
* `Lightning=  DISABLED/ENABLED`
* `Train=  DISABLED/ENABLED`
* `Pulse=  DISABLED/ENABLED`
* `Timer=  DISABLED/ENABLED`
* `RemoveAmulet= DISABLED/ENABLED`
* `NoLevel=  DISABLED/ENABLED`
* `PuzzleCombo= firstIndex,secondIndex,name,height,size,yAngle,zAngle,xAngle,unknown` - integer,integer,string,integer,integer,integer,integer,integer,integer
* `PickupCombo=  firstIndex,secondIndex,name,height,size,yAngle,zAngle,xAngle,unknown` - integer,integer,string,integer,integer,integer,integer,integer,integer
* `KeyCombo= firstIndex,secondIndex,name,height,size,yAngle,zAngle,xAngle,unknown` - integer,integer,string,integer,integer,integer,integer,integer,integer
* `Puzzle= index,name,height,size,yAngle,zAngle,xAngle,unknown` - integer,string,integer,integer,integer,integer,integer,integer
* `Pickup= index,name,height,size,yAngle,zAngle,xAngle,unknown` - integer,string,integer,integer,integer,integer,integer,integer
* `Examine=  index,name,height,size,yAngle,zAngle,xAngle,unknown` - integer,string,integer,integer,integer,integer,integer,integer
* `Key=  index,name,height,size,yAngle,zAngle,xAngle,unknown` - integer,string,integer,integer,integer,integer,integer,integer
* `Level=  levelPath,audioTrack` - levelPath is a string; audioTrack is an integer

Further information on the meaning of these commands can be found [here](http://www.trsearch.org/Tutorial.php?action=gettut&id=6).

### 4.ii Language scripts

The syntax of language scripts is the same as the one for gameflow scripts, the only difference being that different chunks are used here and that the script file does not contain commands, but only strings written in the language in question.
The supported language chunks are: *Strings*, *PSXStrings* and *PCStrings*.
Every string written in a gameflow script file must also be present in the primary language script file (the one with the index 0 in the *Language* gameflow chunk), so that it can easily be translated into other language script files and so that the game engine can easily locate that translated string.

## 5. Miscalleneous
This tool can also be used to compile scripts for the game Tomb Raider: The Last Revelation.

## 6. Notes regarding compiling the source code of this program
When compiling with the GNU GCC compiler, the following compiler option may have to be specified: `-U__STRICT_ANSI__`

## 7. Wiki
This project's wiki contains (almost) all the information required to both compile to and read from the binary script files used in TRLE. It can be found [here](https://github.com/ActiveMath/TRLEScriptModern/wiki).

## 8. Disclaimer
This tool was not made and is not supported by Core Design Ltd, EIDOS Interactive and Square Enix.

