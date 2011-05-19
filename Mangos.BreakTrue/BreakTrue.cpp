/*
 * The MIT License
 *
 * BreakTrue, Copyright (c) 2011 MaNGOS, based on code from the Encore project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "BreakTrue.h"

PatternScanner::PatternScanner(byte *data, int len) : _data(data), _dataLen(len) {}

bool PatternScanner::CompareSequences(const byte *pattern, int patternSz, int offset)
{
    for (int i=0; i<patternSz; ++i)
    {
        if (pattern[i]!=null && pattern[i]!=_data[offset+i])
            return false;
    }
    return true;
}

int PatternScanner::Find(const byte *pattern, int patternSz)
{
    for (int i=0; i<_dataLen-patternSz; ++i)
    {
        if (CompareSequences(pattern, patternSz, i))
            return i;
    }
    return null;
}

ClientPatcher::ClientPatcher(std::string fileName)
{
    static byte connIndexPattern[] =
    {
        0xE8, null, null, null, null, 0x8B, 0xC8, 0x83, 0xC4, 0x04,
        0x83, 0xE1, 0x01, 0x80, 0xBC, 0x31, null, null, 0x00, 0x00
    };
    _connectionIndexPattern = connIndexPattern;
    _connectionIndexPatternLen = sizeof(connIndexPattern);

    static byte connIndexCheckPattern[] =
    {
        0x0F, 0x85, null, null, null, null, 0x8B, 0x4D, 0x10, 0x8B,
        0x55, 0x08, 0x53, 0x51, 0x8B, 0x8F, null, null, null, null
    };
    _connectionIndexCheckPattern = connIndexCheckPattern;
    _connectionIndexCheckPatternLen = sizeof(connIndexCheckPattern);

    static byte emailCheckPattern[] =
    {
        0x74, null, 0xBE, 0x01, 0x00, 0x00, 0x00, 0x8B, 0x0D, null,
        null, null, null, 0x85, 0xC9, 0x74, null, 0x8B, 0x01, 0x8B,
    };
    _emailCheckPattern = emailCheckPattern;
    _emailCheckPatternLen = sizeof(emailCheckPattern);

    _fileName = fileName;

    std::ifstream::pos_type size;
    byte *data;

    std::ifstream file(_fileName.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        data = new byte[size];
        file.seekg(0, std::ios::beg);
        file.read((char*)data, size);
        file.close();
        _scanner = new PatternScanner(data, size);
    }
    else
    {
        std::cout << "Could not open " << fileName << std::endl;
    }
}

bool ClientPatcher::Patch(std::string patchName, const byte *pattern, int patternSz, const byte *replacementBytes, int replacementBytesSz)
{
    if (!_scanner)
        return false;

    int offset = _scanner->Find(pattern, patternSz);

    if (offset == null)
    {
        std::cout << patchName << ": Offset not found." << std::endl;
        return false;
    }

    std::cout << patchName << ": Offset found at: 0x" << std::hex << offset << std::endl;

    std::fstream file(_fileName.c_str(), std::ios::in|std::ios::out|std::ios::binary);
    if (!file.is_open())
        return false;

    file.seekp(offset);
    file.write((const char*)replacementBytes, replacementBytesSz);
    file.close();

    return true;
}

bool ClientPatcher::Patch()
{
    {
        const byte replace[] = { 0xB8, 0x00, 0x00, 0x00, 0x00 };
        if (!Patch("Connection index selection", _connectionIndexPattern, _connectionIndexPatternLen, replace, sizeof(replace)))
            return false;
    }
    {
        const byte replace[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
        if (!Patch("Connection index check", _connectionIndexCheckPattern, _connectionIndexCheckPatternLen, replace, sizeof(replace)))
            return false;
    }
    {
        const byte replace[] = { 0xEB };
        if (!Patch("Grunt/Battle.net selection", _emailCheckPattern, _emailCheckPatternLen, replace, sizeof(replace)))
            return false;
    }
    return true;
}

extern int main(int argc, char **argv)
{
    std::string fileName = argc>1 ? argv[1] : "Wow.exe";
    ClientPatcher* patcher = new ClientPatcher(fileName);
    bool result = patcher->Patch();
    std::cout << "Patching " << (result ? "succeeded" : "failed") << std::endl;
}
