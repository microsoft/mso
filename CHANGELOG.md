# Change Log - @microsoft/mso

This log was last generated on Tue, 03 Jan 2023 19:45:22 GMT and should not be manually modified.

<!-- Start content -->

## 0.0.23

Tue, 03 Jan 2023 19:45:22 GMT

### Patches

- Apply compliance fixes (dannyvv@microsoft.com)

## 0.0.22

Wed, 11 May 2022 17:17:50 GMT

### Patches

- Bump clang-format to 1.8 to address CVE in async (dannyvv@microsoft.com)

## 0.0.21

Fri, 11 Feb 2022 23:07:07 GMT

### Patches

- Fix build break related to fetching GTest sources (vmorozov@microsoft.com)
- Add private package.json to EnsureFileHeaders tool (vmorozov@microsoft.com)
- Use origin/main branch to publish (vmorozov@microsoft.com)

## 0.0.21

Fri, 11 Feb 2022 22:59:00 GMT

### Patches

- Fix build break related to fetching GTest sources (vmorozov@microsoft.com)
- Use origin/main branch to publish (vmorozov@microsoft.com)

## 0.0.20
Tue, 09 Mar 2021 03:06:31 GMT

### Patches

- Bump lodash to 4.17.21 (dannyvv@microsoft.com)
## 0.0.19
Tue, 07 Apr 2020 23:51:46 GMT

### Patches

- Added future liblet from the RNW Mso fork (vmorozov@microsoft.com)
## 0.0.18
Mon, 06 Apr 2020 21:31:49 GMT

### Patches

- Added activeObject liblet from RNW Mso fork (vmorozov@microsoft.com)
## 0.0.17
Mon, 06 Apr 2020 06:21:31 GMT

### Patches

- Added dispatchQueue and span liblets from RNW Mso fork (vmorozov@microsoft.com)
## 0.0.16
Mon, 06 Apr 2020 00:13:16 GMT

### Patches

- Added errorCode liblet from RNW Mso fork (vmorozov@microsoft.com)
## 0.0.15
Sun, 05 Apr 2020 23:15:22 GMT

### Patches

- Updated mortifCpp and guid liblets from RNW Mso fork (vmorozov@microsoft.com)
## 0.0.14
Sun, 05 Apr 2020 03:22:13 GMT

### Patches

- Updated remaining liblets from RNW Mso fork (vmorozov@microsoft.com)
## 0.0.13
Sat, 04 Apr 2020 05:08:55 GMT

### Patches

- Updated a set of liblets from RNW Mso fork (vmorozov@microsoft.com)
## 0.0.12
Fri, 03 Apr 2020 18:15:55 GMT

### Patches

- Updated compilerAdapters liblet from RNW Mso clone (vmorozov@microsoft.com)
## 0.0.11
Fri, 03 Apr 2020 07:57:59 GMT

### Patches

- Moved and renamed files as in the RNW repo (vmorozov@microsoft.com)
## 0.0.10
Fri, 03 Apr 2020 06:08:58 GMT

### Patches

- Turn on strict GCC warnings and compiler settings. Fix the code so it compiles. (adam@thefoxmans.net)
## 0.0.9
Fri, 03 Apr 2020 01:44:22 GMT

### Patches

- Minor fixes to make the GCC build work. (adam@thefoxmans.net)
## 0.0.8
Thu, 02 Apr 2020 23:54:29 GMT

### Patches

- Add strict warnings and compilation flags for Clang builds. Fix code to conform to strict clang settings. Only ignore warnings for identifiers with names containing a dollar sign, and for upcoming changes to name mangling in C++17. (adam@thefoxmans.net)
## 0.0.7
Thu, 02 Apr 2020 21:39:53 GMT

### Patches

- Add warning-suppression for GCC. (adam@thefoxmans.net)
## 0.0.6
Thu, 02 Apr 2020 07:43:26 GMT

### Patches

- Replace all warning pragmas with macros, making them more expressive and cross-platform. (adam@thefoxmans.net)
## 0.0.5
Thu, 02 Apr 2020 04:14:39 GMT

### Patches

- Turn on level 4 warnings, use non-permissive mode, and make all warnings show up as errors. Fix all impacted code. (adam@thefoxmans.net)
## 0.0.4
Wed, 01 Apr 2020 14:58:01 GMT

### Patches

- Formatted code with ClangFormat (vmorozov@microsoft.com)
## 0.0.3
Tue, 31 Mar 2020 23:36:20 GMT

### Patches

- Removed the .clang-format until we create a better one (vmorozov@microsoft.com)
- Make iOS and macOS builds work. Verified debug and release. (afoxman@microsoft.com)
## 0.0.3
Tue, 31 Mar 2020 23:31:27 GMT

### Patches

- Removed the .clang-format until we create a better one (vmorozov@microsoft.com)
- Make iOS and macOS builds work. Verified debug and release. (afoxman@microsoft.com)
## 0.0.2
Tue, 31 Mar 2020 18:07:08 GMT

### Patches

- Make package.json public for beachball to work. Remove workspaces since JS coding is not desired in this repo. Merge child workspace package.json files into top-level package.json. (adam@thefoxmans.net)
- Add a PR CI loop definition which builds and tests all windows platforms/architectures. (adam@thefoxmans.net)
## 0.0.1
Tue, 31 Mar 2020 17:24:58 GMT

### Patches

- Add a PR CI loop definition which builds and tests all windows platforms/architectures. (adam@thefoxmans.net)
## 0.0.1
Tue, 31 Mar 2020 16:41:24 GMT

### Patches

- Add a PR CI loop definition which builds and tests all windows platforms/architectures. (adam@thefoxmans.net)
