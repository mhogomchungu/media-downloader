/*
 *
 *  Copyright (c) 2025
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "getsauce.h"
#include "../utility.h"

const char * getsauce::testData()
{
	return R"R(
 Site:      https://blablabla.com
 Title:     Weekly Pictures Of Cute Cats
 Type:      image
 Stream:

     [0]  -------------------
     Type:            image
     Quality:         unknown
     Parts:           4
     Size:            1234564 B
     # download with: get-sauce -s 0 ...


Downloading Weekly Pictures Of Cute Cats_0.png ...   0% |                                        |  [0s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...   1% |                                        | (344 kB/s) [0s:6s]


Downloading Weekly Pictures Of Cute Cats_0.png ...   2% |                                        | (324 kB/s) [0s:6s]


Downloading Weekly Pictures Of Cute Cats_0.png ...   3% |█                                       | (408 kB/s) [0s:5s]


Downloading Weekly Pictures Of Cute Cats_0.png ...   4% |█                                       | (435 kB/s) [0s:4s]


Downloading Weekly Pictures Of Cute Cats_0.png ...   5% |██                                      | (409 kB/s) [0s:5s]


Downloading Weekly Pictures Of Cute Cats_0.png ...   6% |██                                      | (413 kB/s) [0s:4s]


Downloading Weekly Pictures Of Cute Cats_0.png ...   7% |██                                      | (445 kB/s) [0s:4s]


Downloading Weekly Pictures Of Cute Cats_0.png ...   8% |███                                     | (485 kB/s) [0s:4s]


Downloading Weekly Pictures Of Cute Cats_0.png ...   9% |███                                     | (511 kB/s) [0s:3s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  10% |████                                    | (539 kB/s) [0s:3s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  11% |████                                    | (594 kB/s) [0s:3s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  12% |█████                                   | (611 kB/s) [0s:3s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  13% |█████                                   | (626 kB/s) [0s:3s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  14% |█████                                   | (639 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  15% |██████                                  | (695 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  16% |██████                                  | (704 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  17% |██████                                  | (746 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  18% |███████                                 | (746 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  19% |███████                                 | (746 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  20% |████████                                | (746 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  21% |████████                                | (746 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  22% |████████                                | (746 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  23% |█████████                               | (746 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  24% |█████████                               | (746 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  25% |██████████                              | (746 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  26% |██████████                              | (746 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  27% |██████████                              | (746 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  28% |███████████                             | (746 kB/s) [0s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  29% |███████████                             | (746 kB/s) [1s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  30% |████████████                            | (644 kB/s) [1s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  31% |████████████                            | (644 kB/s) [1s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  32% |████████████                            | (644 kB/s) [1s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  33% |█████████████                           | (644 kB/s) [1s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  34% |█████████████                           | (644 kB/s) [1s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  35% |██████████████                          | (644 kB/s) [1s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  36% |██████████████                          | (644 kB/s) [1s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  37% |██████████████                          | (644 kB/s) [1s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  38% |███████████████                         | (644 kB/s) [1s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  39% |███████████████                         | (644 kB/s) [1s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  40% |████████████████                        | (644 kB/s) [1s:2s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  41% |████████████████                        | (644 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  42% |████████████████                        | (644 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  43% |█████████████████                       | (644 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  44% |█████████████████                       | (644 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  45% |██████████████████                      | (644 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  46% |██████████████████                      | (644 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  47% |██████████████████                      | (644 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  48% |███████████████████                     | (644 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  49% |███████████████████                     | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  50% |████████████████████                    | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  51% |████████████████████                    | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  52% |█████████████████████                   | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  53% |█████████████████████                   | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  54% |█████████████████████                   | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  55% |██████████████████████                  | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  56% |██████████████████████                  | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  57% |██████████████████████                  | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  58% |███████████████████████                 | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  59% |███████████████████████                 | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  60% |████████████████████████                | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  61% |████████████████████████                | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  62% |████████████████████████                | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  63% |█████████████████████████               | (707 kB/s) [1s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  64% |█████████████████████████               | (707 kB/s) [2s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  65% |██████████████████████████              | (704 kB/s) [2s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  66% |██████████████████████████              | (704 kB/s) [2s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  67% |███████████████████████████             | (704 kB/s) [2s:1s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  68% |███████████████████████████             | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  69% |███████████████████████████             | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  70% |████████████████████████████            | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  71% |████████████████████████████            | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  72% |████████████████████████████            | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  73% |█████████████████████████████           | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  74% |█████████████████████████████           | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  75% |██████████████████████████████          | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  76% |██████████████████████████████          | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  77% |███████████████████████████████         | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  78% |███████████████████████████████         | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  79% |███████████████████████████████         | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  80% |████████████████████████████████        | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  81% |████████████████████████████████        | (704 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  82% |█████████████████████████████████       | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  83% |█████████████████████████████████       | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  84% |█████████████████████████████████       | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  85% |██████████████████████████████████      | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  86% |██████████████████████████████████      | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  87% |██████████████████████████████████      | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  88% |███████████████████████████████████     | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  89% |███████████████████████████████████     | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  90% |████████████████████████████████████    | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  91% |████████████████████████████████████    | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  92% |████████████████████████████████████    | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  93% |█████████████████████████████████████   | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  94% |█████████████████████████████████████   | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  95% |██████████████████████████████████████  | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  96% |██████████████████████████████████████  | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  97% |███████████████████████████████████████ | (709 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  98% |███████████████████████████████████████ | (709 kB/s) [3s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ...  99% |███████████████████████████████████████ | (709 kB/s) [3s:0s]


Downloading Weekly Pictures Of Cute Cats_0.png ... 100% |████████████████████████████████████████| (725 kB/s)
Downloading Weekly Pictures Of Cute Cats_1.png ...   0% |                                        |  [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...   1% |                                        | (3.4 MB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...   2% |                                        | (1.6 MB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...   3% |█                                       | (1.5 MB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...   4% |█                                       | (1.2 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...   5% |██                                      | (1.1 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...   6% |██                                      | (1.1 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...   7% |███                                     | (939 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...   8% |███                                     | (1.0 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...   9% |███                                     | (1.1 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  10% |████                                    | (1.1 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  12% |████                                    | (1.1 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  13% |█████                                   | (1.1 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  14% |█████                                   | (1.0 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  15% |██████                                  | (1.1 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  16% |██████                                  | (1.0 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  17% |██████                                  | (1.1 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  18% |███████                                 | (1.0 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  19% |███████                                 | (1.0 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  20% |████████                                | (992 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  21% |████████                                | (1.0 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  22% |█████████                               | (986 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  24% |█████████                               | (1.0 MB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  25% |██████████                              | (947 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  26% |██████████                              | (939 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  27% |██████████                              | (917 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  28% |███████████                             | (888 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  29% |███████████                             | (892 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  30% |████████████                            | (887 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  31% |████████████                            | (887 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  32% |█████████████                           | (887 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  33% |█████████████                           | (887 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  34% |█████████████                           | (887 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  36% |██████████████                          | (887 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  37% |██████████████                          | (887 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  38% |███████████████                         | (887 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  39% |███████████████                         | (887 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  40% |████████████████                        | (887 kB/s) [0s:1s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  41% |████████████████                        | (887 kB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  42% |█████████████████                       | (887 kB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  43% |█████████████████                       | (887 kB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  44% |█████████████████                       | (887 kB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  45% |██████████████████                      | (887 kB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  46% |██████████████████                      | (887 kB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  48% |███████████████████                     | (887 kB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  49% |███████████████████                     | (887 kB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  50% |████████████████████                    | (887 kB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  51% |████████████████████                    | (887 kB/s) [0s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  52% |████████████████████                    | (887 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  53% |█████████████████████                   | (887 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  54% |█████████████████████                   | (887 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  55% |██████████████████████                  | (887 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  56% |██████████████████████                  | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  57% |███████████████████████                 | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  58% |███████████████████████                 | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  60% |████████████████████████                | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  61% |████████████████████████                | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  62% |████████████████████████                | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  63% |█████████████████████████               | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  64% |█████████████████████████               | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  65% |██████████████████████████              | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  66% |██████████████████████████              | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  67% |███████████████████████████             | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  68% |███████████████████████████             | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  69% |███████████████████████████             | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  70% |████████████████████████████            | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  72% |████████████████████████████            | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  73% |█████████████████████████████           | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  74% |█████████████████████████████           | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  75% |██████████████████████████████          | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  76% |██████████████████████████████          | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  77% |██████████████████████████████          | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  78% |███████████████████████████████         | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  79% |███████████████████████████████         | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  80% |████████████████████████████████        | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  81% |████████████████████████████████        | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  82% |█████████████████████████████████       | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  84% |█████████████████████████████████       | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  85% |██████████████████████████████████      | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  86% |██████████████████████████████████      | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  87% |██████████████████████████████████      | (834 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  88% |███████████████████████████████████     | (857 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  89% |███████████████████████████████████     | (857 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  90% |████████████████████████████████████    | (857 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  91% |████████████████████████████████████    | (857 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  92% |█████████████████████████████████████   | (857 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  93% |█████████████████████████████████████   | (857 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  94% |█████████████████████████████████████   | (857 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  96% |██████████████████████████████████████  | (857 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  97% |██████████████████████████████████████  | (857 kB/s) [1s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  98% |███████████████████████████████████████ | (857 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ...  99% |███████████████████████████████████████ | (857 kB/s) [2s:0s]


Downloading Weekly Pictures Of Cute Cats_1.png ... 100% |████████████████████████████████████████| (740 kB/s))R" ;
}

getsauce::~getsauce()
{
}

getsauce::getsauce( const engines& engines,const engines::engine& engine,QJsonObject&,const QString& df ) :
	engines::engine::baseEngine( engines.Settings(),engine,engines.processEnvironment() ),
	m_engine( engine ),
	m_downloadFolder( df + "/" )
{
}

engines::engine::baseEngine::optionsEnvironment getsauce::setProxySetting( QStringList&,const QString& e )
{
	return { "HTTPS_PROXY",e } ;
}

QString getsauce::updateTextOnCompleteDownlod( const QString& uiText,
					       const QString& bkText,
					       const QString& dopts,
					       const QString& tabName,
					       const finishedState& f )
{
	using functions = engines::engine::baseEngine ;

	if( f.cancelled() ){

		return functions::updateTextOnCompleteDownlod( bkText,dopts,tabName,f ) ;

	}else if( f.success() ){

		return engines::engine::baseEngine::updateTextOnCompleteDownlod( uiText,dopts,tabName,f ) ;
	}else{
		auto m = engines::engine::baseEngine::updateTextOnCompleteDownlod( uiText,dopts,tabName,f ) ;

		return m + "\n" + bkText ;
	}
}

static bool _meetCondition( const engines::engine&,const QByteArray& e )
{
	return e.contains( "Downloading " ) && e.contains( "% |" ) ;
}

static bool _meetLocalCondition( const engines::engine&,const QByteArray& e )
{
	return e.contains( ") [" ) ;
}

using Output = engines::engine::baseEngine::filterOutPut ;

class getsauceFilter : public engines::engine::baseEngine::filterOutPut
{
public:
	getsauceFilter( const engines::engine& engine ) : m_engine( engine )
	{
	}
	Output::result formatOutput( const Output::args& args ) const override
	{
		auto data = args.data.toLine() + args.outPut ;

		auto m = data.indexOf( this->marker() ) ;

		if( m != -1 ){

			return this->formatOutput( args,data,m ) ;
		}else{
			return { args.outPut,m_engine,_meetLocalCondition } ;
		}
	}
	Output::result formatOutput( const Output::args& args,const QByteArray& allData,int ) const
	{
		auto size     = this->getSize( allData ) ;
		auto title    = this->getTitle( allData ) ;
		auto fileName = this->getFileName( allData ) ;

		if( fileName.isEmpty() ){

			if( title.isEmpty() ){

				return { args.outPut,m_engine,_meetLocalCondition } ;
			}else{
				m_tmp = title ;

				return { m_tmp,m_engine,_meetLocalCondition } ;
			}
		}

		auto mm = allData.lastIndexOf( "Downloading " ) ;

		if( mm == -1 ){

			if( title.isEmpty() ){

				return { args.outPut,m_engine,_meetLocalCondition } ;
			}else{
				m_tmp = title ;

				return { m_tmp,m_engine,_meetLocalCondition } ;
			}
		}

		const auto data = allData.mid( mm + 12 ) ;

		mm = data.indexOf( "..." ) ;

		if( mm == -1 ){

			if( title.isEmpty() ){

				return { args.outPut,m_engine,_meetLocalCondition } ;
			}else{
				m_tmp = title ;

				return { m_tmp,m_engine,_meetLocalCondition } ;
			}
		}

		auto percentage = data.mid( mm + 3 ) ;

		mm = percentage.indexOf( "%" ) ;

		if( mm == -1 ){

			if( title.isEmpty() ){

				return { args.outPut,m_engine,_meetLocalCondition } ;
			}else{
				m_tmp = title ;

				return { m_tmp,m_engine,_meetLocalCondition } ;
			}
		}

		percentage = percentage.mid( 0,mm + 1 ).trimmed() ;

		mm = data.indexOf( "(" ) ;

		if( mm == -1 ){

			if( title.isEmpty() ){

				return { args.outPut,m_engine,_meetLocalCondition } ;
			}else{
				m_tmp = title ;

				return { m_tmp,m_engine,_meetLocalCondition } ;
			}
		}

		auto speedAndETA = data.mid( mm ) ;

		mm = speedAndETA.indexOf( "]" ) ;

		if( mm == -1 ){

			if( title.isEmpty() ){

				return { args.outPut,m_engine,_meetLocalCondition } ;
			}else{
				m_tmp = title ;

				return { m_tmp,m_engine,_meetLocalCondition } ;
			}
		}

		speedAndETA = speedAndETA.mid( 0,mm + 1 ) ;

		QByteArray name ;

		if( fileName.isEmpty() ){

			if( !title.isEmpty() ){

				name = title ;
			}
		}else{
			name = fileName ;
		}

		if( size.isEmpty() ){

			m_tmp = name + "\n" + percentage + "   " + speedAndETA ;
		}else{
			m_tmp = name + "\n" + size + "   " + percentage + "   " + speedAndETA ;
		}

		return { m_tmp,m_engine,_meetLocalCondition } ;
	}
	bool meetCondition( const engines::engine::baseEngine::filterOutPut::args& args ) const override
	{
		return _meetCondition( m_engine,args.outPut ) ;
	}
	const engines::engine& engine() const override
	{
		return m_engine ;
	}
private:
	QByteArray marker() const
	{
		return "download with: " ;
	}
	QByteArray getTitle( const QByteArray& allData ) const
	{
		return this->getEntry( "Title:",allData ) ;
	}
	QByteArray getSize( const QByteArray& allData ) const
	{
		return this->getEntry( "Size:",allData ) ;
	}
	QByteArray getEntry( const QByteArray& start,const QByteArray& s ) const
	{
		std::array< const char *,5 > markers{ "Type:","Quality:","Parts:","Size:","#" } ;

		auto m = s.indexOf( start ) ;

		if( m != -1 ){

			auto e = s.mid( m + start.size() ) ;

			for( const auto it : markers ){

				m = e.indexOf( it ) ;

				if( m != -1 ){

					return e.mid( 0,m ).trimmed() ;
				}
			}
		}

		return {} ;
	}
	QByteArray getFileName( const QByteArray& allData ) const
	{
		auto mm = allData.lastIndexOf( "Downloading " ) ;

		if( mm == -1 ){

			return {} ;
		}

		const auto data = allData.mid( mm + 12 ) ;

		mm = data.indexOf( " ..." ) ;

		if( mm == -1 ){

			return {} ;
		}

		return data.mid( 0,mm ) ;
	}
	const engines::engine& m_engine ;
	mutable QByteArray m_tmp ;
} ;

engines::engine::baseEngine::DataFilter getsauce::Filter( int id )
{
	return { util::types::type_identity< getsauce::getsauce_dlFilter >(),m_engine,id,m_downloadFolder.toUtf8() } ;
}

engines::engine::baseEngine::FilterOutPut getsauce::filterOutput()
{
	const auto& engine = engines::engine::baseEngine::engine() ;

	return { util::types::type_identity< getsauceFilter >(),engine } ;
}

std::vector<engines::engine::baseEngine::mediaInfo> getsauce::mediaProperties( Logger& l,const QByteArray& e )
{
	QJsonParseError err ;

	auto json = QJsonDocument::fromJson( e,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		auto obj = json.object() ;

		auto streams = obj.value( "streams" ).toObject() ;
		auto site    = obj.value( "site" ).toString() ;
		auto title   = obj.value( "title" ).toString() ;
		auto type    = obj.value( "type" ).toString() ;

		QJsonArray arr ;

		QString id ;

		for( int i = 0 ; ; i++ ){

			id = QString::number( i ) ;

			auto oo = streams.value( id ).toObject() ;

			if( oo.isEmpty() ){

				break ;
			}else{
				oo.insert( "id",id ) ;
				oo.insert( "site",site ) ;
				oo.insert( "main_title",title ) ;
				oo.insert( "main_type",type ) ;

				arr.append( oo ) ;
			}
		}

		return this->mediaProperties( l,arr ) ;
	}else{
		utility::failedToParseJsonData( l,err ) ;

		return {} ;
	}
}

std::vector<engines::engine::baseEngine::mediaInfo> getsauce::mediaProperties( Logger&,const QJsonArray& arr )
{
	std::vector<engines::engine::baseEngine::mediaInfo> ent ;

	Logger::locale locale ;

	for( const auto& it : arr ){

		auto obj = it.toObject() ;

		auto resolution = obj.value( "quality" ).toString() ;
		auto id         = obj.value( "id" ).toString() ;
		auto sizeRaw    = obj.value( "size" ).toInt() ;
		auto size       = locale.formattedDataSize( sizeRaw ) ;
		auto notes      = obj.value( "info" ).toString() ;
		auto extension  = obj.value( "ext" ).toString() ;
		auto title      = obj.value( "title" ).toString() ;

		QString duration ;

		if( title.isEmpty() ){

			title = obj.value( "main_title" ).toString() ;
		}

		QStringList urls ;

		const auto parts = obj.value( "urls" ).toArray() ;

		int count = 0 ;

		for( const auto& it : parts ){

			count++ ;
			urls.append( it.toObject().value( "url" ).toString() ) ;
		}

		if( parts.size() == 1 && extension.isEmpty() ){

			extension = parts[ 0 ].toObject().value( "ext" ).toString() ;
		}

		if( !notes.isEmpty() ){

			notes += "\n" ;
		}

		notes += "Urls Count: " + QString::number( count ) ;

		auto sizeRawInt = QString::number( sizeRaw ) ;

		ent.emplace_back( urls,id,extension,resolution,size,sizeRawInt,notes,duration,title ) ;
	}

	return ent ;
}

bool getsauce::foundNetworkUrl( const QString& s )
{
	utility::CPU cpu ;

	if( utility::platformIsWindows() ){

		if( cpu.x86_64() ){

			return s.endsWith( "Windows_x86_64.zip" ) ;

		}else if( cpu.aarch64() ){

			return s.endsWith( "Windows_arm64.zip" ) ;

		}else if( cpu.x86_32() ){

			return s.endsWith( "Windows_i386.zip" ) ;
		}

	}else if( utility::platformIsLinux() ){

		if( cpu.x86_64() ){

			return s.endsWith( "Linux_x86_64.tar.gz" ) ;

		}else if( cpu.aarch64() ){

			return s.endsWith( "Linux_arm64.tar.gz" ) ;

		}else if( cpu.x86_32() ){

			return s.endsWith( "Linux_i386.tar.gz" ) ;
		}

	}else if( utility::platformIsOSX() ){

		if( cpu.x86_64() ){

			return s.endsWith( "Darwin_x86_64.tar.gz" ) ;

		}else if( cpu.aarch64() ){

			return s.endsWith( "Darwin_arm64.tar.gz" ) ;
		}
	}

	return false ;
}

getsauce::getsauce_dlFilter::getsauce_dlFilter( const engines::engine& engine,int id,QByteArray df ) :
	engines::engine::baseEngine::filter( engine,id ),
	m_banner( ".. " + QObject::tr( "This May Take A Very Long Time" ).toUtf8() + " .." ),
	m_downloadFolder( std::move( df ) )
{
}

const QByteArray& getsauce::getsauce_dlFilter::operator()( Logger::Data& e )
{
	if( e.doneDownloading() ){

		const auto& m = e.fileNames() ;

		if( m.size() == 1 ){

			m_tmp = m.back() ;

			return m_tmp ;

		}else if( m.size() > 1 ){

			auto a = QObject::tr( "%1 Already Downloaded" ) ;
			auto b = QString::number( m.size() - 1 ) ;

			m_tmp = a.arg( b ).toUtf8() + "\n" + m.back() ;

			return m_tmp ;
		}else{
			return e.lastText() ;
		}

	}else if( e.lastLineIsProgressLine() ){

		auto m = e.lastText().indexOf( "\n" ) ;

		if( m != -1 ){

			e.addFileName( e.lastText().mid( 0,m ) ) ;
		}

		const auto& mm = e.fileNames() ;

		if( mm.size() > 1 ){

			auto a = QObject::tr( "%1 Already Downloaded" ) ;
			auto b = QString::number( mm.size() - 1 ) ;

			m_tmp = a.arg( b ).toUtf8() + "\n" + e.lastText() ;

			return m_tmp ;
		}else{
			return e.lastText() ;
		}
	}else{
			return m_progress.text() ;
	}
}

getsauce::getsauce_dlFilter::~getsauce_dlFilter()
{
}

const QByteArray& getsauce::getsauce_dlFilter::doneDownloading( const QByteArray& )
{
	m_tmp.clear() ;

	return m_tmp ;
}
