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

#include <fstream>
#include <string>
#include <iostream>
#include <vector>

typedef unsigned char byte;
#define null 0xFF

class PatternScanner
{
    public:
        PatternScanner(byte *data, int len);
        bool CompareSequences(const byte *pattern, int patternSz, int offset);
        int Find(const byte *pattern, int patternSz);
    private:
        const byte *_data;
        int _dataLen;
};

class ClientPatcher
{
    public:
        ClientPatcher(std::string fileName);
        bool Patch();
    private:
        bool Patch(std::string patchName, const byte *pattern, int patternSz, const byte *replacementBytes, int replacementBytesSz);
    protected:
        std::string _fileName;
        const byte *_connectionIndexPattern;
        int _connectionIndexPatternLen;
        const byte *_connectionIndexCheckPattern;
        int _connectionIndexCheckPatternLen;
        const byte *_emailCheckPattern;
        int _emailCheckPatternLen;
        PatternScanner *_scanner;
};
