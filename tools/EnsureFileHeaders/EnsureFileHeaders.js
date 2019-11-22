"use strict";
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (_) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
exports.__esModule = true;
var fs = require("fs"); // File Manipulation
var readdirp = require('readdirp'); // Read Directory Recursively
var argv = require('yargs').argv; // Args Parsing
// Globals
var DirectoryArg;
// Note: \r\n works only on Windows OS. Need to make it cross platform
var HeaderContent = "// Copyright (c) Microsoft Corporation.\r\n// Licensed under the MIT license.\r\n\r\n";
// Note: Adding header into *.js has a conflict with tsc compiler
// Disable .js for now
var HeaderConfigs = [
    {
        'fileFilter': ['*.cpp', '*.h', '*.cs', '*.ts', '*.mm', '*.hpp'],
        'headerContent': "// Copyright (c) Microsoft Corporation.\r\n// Licensed under the MIT license.\r\n\r\n"
    },
    {
        'fileFilter': ['CMakeLists.txt', '*.cmake'],
        'headerContent': "# Copyright (c) Microsoft Corporation.\r\n# Licensed under the MIT license.\r\n\r\n"
    }
];
function PrintHelp() {
    console.log("Usage:");
    console.log("node " + __filename + " -d directoryPathToScan");
    return;
}
// Processes Command Line Arguments
// Returns true if Arguments were processed successfully 
// and app should continue execution.
// Otherwise returns false
function ProcessArgs() {
    var helpArg = argv.h;
    if (helpArg) {
        PrintHelp();
        return false;
    }
    DirectoryArg = argv.d;
    if (!DirectoryArg) {
        PrintHelp();
        return false;
    }
    return true;
}
// Adds a header to the given file if it's not there
function EnsureFileHeader(fileName, headerContent) {
    console.log('EnsureOpenSourceHeader()');
    console.log('fileName: ' + fileName);
    var fileContent = fs.readFileSync(fileName, 'utf8');
    if (!fileContent.startsWith(headerContent)) {
        var fileContentWithHeader = headerContent + fileContent;
        fs.writeFileSync(fileName, fileContentWithHeader);
        console.log("File Header Updated");
    }
    else {
        console.log("File Header Update Not Needed");
    }
}
function EnsureFileHeaders() {
    return __awaiter(this, void 0, void 0, function () {
        var _i, HeaderConfigs_1, headerConfig, sourceFiles, _a, sourceFiles_1, sourceFile;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    _i = 0, HeaderConfigs_1 = HeaderConfigs;
                    _b.label = 1;
                case 1:
                    if (!(_i < HeaderConfigs_1.length)) return [3 /*break*/, 4];
                    headerConfig = HeaderConfigs_1[_i];
                    return [4 /*yield*/, readdirp.promise(DirectoryArg, {
                            fileFilter: headerConfig.fileFilter,
                            directoryFilter: ['!.git', '!node_modules']
                        })];
                case 2:
                    sourceFiles = _b.sent();
                    for (_a = 0, sourceFiles_1 = sourceFiles; _a < sourceFiles_1.length; _a++) {
                        sourceFile = sourceFiles_1[_a];
                        EnsureFileHeader(sourceFile.fullPath, headerConfig.headerContent);
                    }
                    _b.label = 3;
                case 3:
                    _i++;
                    return [3 /*break*/, 1];
                case 4: return [2 /*return*/];
            }
        });
    });
}
function Main() {
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    if (!ProcessArgs()) {
                        return [2 /*return*/];
                    }
                    return [4 /*yield*/, EnsureFileHeaders()];
                case 1:
                    _a.sent();
                    return [2 /*return*/];
            }
        });
    });
}
Main();
