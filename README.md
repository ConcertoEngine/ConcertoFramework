# ConcertoCore

Concerto Core is a C++ library that provides a set of common functionality used by all of the components in the Concerto suite of software. It is designed to be lightweight, flexible, and portable, and is suitable for a wide range of software applications.

It is essentially developed for learning purposes.

## Build Status
| Platform | Status                                                                                                                                                                                          |
|----------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Windows  | [![Windows build status](https://github.com/ConcertoEngine/ConcertoCore/actions/workflows/windows.yml/badge.svg)](https://github.com/ConcertoEngine/ConcertoCore/actions/workflows/windows.yml) |
| Linux    | [![Linux build status](https://github.com/ConcertoEngine/ConcertoCore/actions/workflows/ubuntu24.yml/badge.svg)](https://github.com/ConcertoEngine/ConcertoCore/actions/workflows/ubuntu22.yml) |
| MacOS    | [![macOS build status](https://github.com/ConcertoEngine/ConcertoCore/actions/workflows/macos.yml/badge.svg)](https://github.com/ConcertoEngine/ConcertoCore/actions/workflows/macos.yml)       |

## Installation

Install [xmake](https://xmake.io/#/)

Clone this repo 

```bash
git clone git@github.com:ArthurVasseur/ConcertoGraphics.git
```

## Build

Build the project with `xmake`.

```bash
xmake
```
The compiled shared library will be located in the `$(buildir)/$(plat)/$(arch)/$(mode)` directory. 

Creating cmake project files.

```bash
xmake project -k cmakelists
```

Creating Visual Studio project files.

```bash
xmake project -k vsxmake
```

## Run

You can run unit tests : 
```bash
xmake run xmake run ConcertoCoreTests
```
