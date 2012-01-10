Empirical Mode Decomposition
============================

Fast and Adaptive Bidimensional Empirical Mode Decomposition Using Order-Statistics Filter Based Envelope Estimation.


### Usage

Compile it:

    make clean && make

Use it:

    ./emd <image>


### Results

    ./emd resources/lena.bmp
    Decomposing BEMC-1
    Variance: 2201.31
    Decomposing BEMC-2
    Variance: 1982.62
    Decomposing BEMC-3
    Variance: 1896.3
    ...

<table>
    <tr>
        <td>Original</td>
        <td>BEMC-1</td>
        <td>BEMC-2</td>
        <td>BEMC-3</td>
    </tr>
    <tr>
        <td><img src="https://github.com/willdurand/EMD/raw/master/resources/lena.bmp" /></td>
        <td><img src="https://github.com/willdurand/EMD/raw/master/resources/BEMC-1.bmp" /></td>
        <td><img src="https://github.com/willdurand/EMD/raw/master/resources/BEMC-2.bmp" /></td>
        <td><img src="https://github.com/willdurand/EMD/raw/master/resources/BEMC-3.bmp" /></td>
    </tr>
</table>


### License

    Copyright (c) 2011-2012 William Durand, Guillaume Bernard

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is furnished
    to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
