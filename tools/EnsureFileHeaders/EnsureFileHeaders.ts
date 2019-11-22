// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

import * as fs from 'fs'; // File Manipulation
const readdirp = require('readdirp'); // Read Directory Recursively
const argv = require('yargs').argv; // Args Parsing

// Globals
let DirectoryArg:string;

// Note: \r\n works only on Windows OS. Need to make it cross platform
const HeaderContent:string = "// Copyright (c) Microsoft Corporation.\r\n// Licensed under the MIT license.\r\n\r\n";

// Note: Adding header into *.js has a conflict with tsc compiler
// Disable .js for now
const HeaderConfigs = [
    {
        'fileFilter': ['*.cpp', '*.h', '*.cs', '*.ts', '*.mm', '*.hpp'],
        'headerContent': "// Copyright (c) Microsoft Corporation.\r\n// Licensed under the MIT license.\r\n\r\n"
    },
    {
        'fileFilter': ['CMakeLists.txt', '*.cmake'],
        'headerContent': "# Copyright (c) Microsoft Corporation.\r\n# Licensed under the MIT license.\r\n\r\n"
    }];

function PrintHelp()
{
    console.log("Usage:");
    console.log("node " + __filename + " -d directoryPathToScan");
    return;
}

// Processes Command Line Arguments
// Returns true if Arguments were processed successfully 
// and app should continue execution.
// Otherwise returns false
function ProcessArgs():boolean
{
    const helpArg = argv.h;
    if (helpArg)
    {
        PrintHelp();
        return false;
    }

    DirectoryArg = argv.d;
    if (!DirectoryArg)
    {
        PrintHelp();
        return false;
    }

    return true;
}

// Adds a header to the given file if it's not there
function EnsureFileHeader(fileName:string, headerContent:string)
{
    console.log('EnsureOpenSourceHeader()');
    console.log('fileName: ' + fileName);

    const fileContent:string = fs.readFileSync(fileName, 'utf8');

    if (!fileContent.startsWith(headerContent))
    {
        let fileContentWithHeader = headerContent + fileContent;
        fs.writeFileSync(fileName, fileContentWithHeader);

        console.log("File Header Updated");
    }
    else
    {
        console.log("File Header Update Not Needed");
    }
}

async function EnsureFileHeaders()
{
    for (const headerConfig of HeaderConfigs)
    {
        const sourceFiles = await readdirp.promise(DirectoryArg, {
            fileFilter: headerConfig.fileFilter,
            directoryFilter: ['!.git', '!node_modules']
        });

        for (const sourceFile of sourceFiles)
        {
            EnsureFileHeader(sourceFile.fullPath, headerConfig.headerContent);
        }
    }
}

async function Main()
{
    if (!ProcessArgs())
    {
        return;
    }

    await EnsureFileHeaders();
}

Main();
