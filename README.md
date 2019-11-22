# Mso C++ Library

This project provides parts of C++ library which has been
used extensively by Microsoft Office Apps. It can make it
easier to share robust cross-platform C++ code between teams
within Microsoft and outside.

Mso C++ Library currently contains following components:

* Mso::UnknownObject
* Mso::Future

## Development Requirements

### Windows

* Visual Studio 2019

### MacOS

* XCode

### Linux

* Visual Studio Code

## Build Environment Preparation Steps

These steps need to be performed only once.

1. (Windows) Open VS Developer Command Prompt for VS 2019

2. Create a "build" folder

```bash
mkdir build
cd build
```

3. Configure the build.

```bash
cmake ..
```

## Build

1. (Windows) Open VS Developer Command Prompt for VS 2019

2. Navigate to "build" folder

```bash
cd build
```

3. Build

```bash
cmake --build .
```

## Run Tests

1. (Windows) Open VS Developer Command Prompt for VS 2019

2. Navigate to "build" folder

```bash
cd build
```

3. Run Unit Tests

```bash
ctest -C Debug
```

## Code of Conduct

See [Code of Conduct](CODE_OF_CONDUCT.md)

## Contributing

See [Contributing](Contributing.md)
