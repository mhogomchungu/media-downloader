/*
 *
 *  Copyright (c) 2021
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

#include "yt-dlp.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "../networkAccess.h"
#include "../utility.h"

#include "aria2c.h"

const char * yt_dlp::testYtDlp()
{
	return R"R([youtube] Extracting URL: https://www.youtube.com/watch?v=tn2USd5KeVM
[youtube] Extracting URL: https://www.youtube.com/watch?v=tn2USd5KeVM
[youtube] tn2USd5KeVM: Downloading webpage
[youtube] tn2USd5KeVM: Downloading webpage
[youtube] tn2USd5KeVM: Downloading android player API JSON
[youtube] tn2USd5KeVM: Downloading android player API JSON
[info] tn2USd5KeVM: Downloading 1 format(s): 242+250
[info] tn2USd5KeVM: Downloading 1 format(s): 242+250
[dashsegments] Total fragments: 1
[download] Destination: For You, I Will-tn2USd5KeVM.f242.webm
[dashsegments] Total fragments: 1
[download] Destination: For You, I Will-tn2USd5KeVM.f242.webm
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"1024","ETA":"801","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"3748.8989290898444","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"1024","ETA":"801","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"3748.8989290898444","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3072","ETA":"270","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"11100.353549598756","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3072","ETA":"270","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"11100.353549598756","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"7168","ETA":"117","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"25553.916984198353","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"7168","ETA":"117","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"25553.916984198353","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"15360","ETA":"55","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"54191.706305212036","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"15360","ETA":"55","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"54191.706305212036","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"31744","ETA":"28","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"105082.03807274862","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"31744","ETA":"28","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"105082.03807274862","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"64512","ETA":"15","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"186530.45246344642","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"64512","ETA":"15","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"186530.45246344642","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"130048","ETA":"9","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"302596.7195118163","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"130048","ETA":"9","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"302596.7195118163","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"261120","ETA":"5","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"513690.6226527927","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"261120","ETA":"5","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"513690.6226527927","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"523264","ETA":"3","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"759468.0671639751","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"523264","ETA":"3","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"759468.0671639751","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"1047552","ETA":"2","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"944726.817633977","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"1047552","ETA":"2","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"944726.817633977","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"2096128","ETA":"0","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"1130690.3381772884","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"2096128","ETA":"0","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"1130690.3381772884","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3006209","ETA":"0","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"1050349.8707767602","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3006209","ETA":"0","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"1050349.8707767602","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3006209","ETA":"0","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"1048710.8298590486","fragment_index":"1","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3006209","ETA":"0","total_bytes_estimate":"3006209.0","total_bytes":"NA","speed":"1048710.8298590486","fragment_index":"1","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3006209","ETA":"NA","total_bytes_estimate":"NA","total_bytes":"3006209","speed":"1045432.6172785787","fragment_index":"NA","fragment_count":"NA"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f242.webm","downloaded_bytes":"3006209","ETA":"NA","total_bytes_estimate":"NA","total_bytes":"3006209","speed":"1045432.6172785787","fragment_index":"NA","fragment_count":"NA"}
[dashsegments] Total fragments: 1
[download] Destination: For You, I Will-tn2USd5KeVM.f250.webm
[dashsegments] Total fragments: 1
[download] Destination: For You, I Will-tn2USd5KeVM.f250.webm
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"1024","ETA":"198","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"4036.8055100230085","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"1024","ETA":"198","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"4036.8055100230085","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"3072","ETA":"66","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"11936.322350783255","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"3072","ETA":"66","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"11936.322350783255","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"7168","ETA":"28","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"27459.17039337285","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"7168","ETA":"28","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"27459.17039337285","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"15360","ETA":"13","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"57882.20012434548","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"15360","ETA":"13","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"57882.20012434548","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"31744","ETA":"6","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"112194.0950090543","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"31744","ETA":"6","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"112194.0950090543","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"64512","ETA":"3","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"193614.70508961147","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"64512","ETA":"3","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"193614.70508961147","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"130048","ETA":"2","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"324906.7034416374","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"130048","ETA":"2","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"324906.7034416374","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"261120","ETA":"1","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"469521.50453802437","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"261120","ETA":"1","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"469521.50453802437","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"523264","ETA":"0","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"679335.6383840272","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"523264","ETA":"0","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"679335.6383840272","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"801810","ETA":"0","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"798716.483346946","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"801810","ETA":"0","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"798716.483346946","fragment_index":"0","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"801810","ETA":"0","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"795310.8810879494","fragment_index":"1","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"801810","ETA":"0","total_bytes_estimate":"801810.0","total_bytes":"NA","speed":"795310.8810879494","fragment_index":"1","fragment_count":"1"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"801810","ETA":"NA","total_bytes_estimate":"NA","total_bytes":"801810","speed":"791098.3168199448","fragment_index":"NA","fragment_count":"NA"}
[download] {"filename":"For You, I Will-tn2USd5KeVM.f250.webm","downloaded_bytes":"801810","ETA":"NA","total_bytes_estimate":"NA","total_bytes":"801810","speed":"791098.3168199448","fragment_index":"NA","fragment_count":"NA"}
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
[Merger] Merging formats into "For You, I Will-tn2USd5KeVM.webm"
[Merger] Merging formats into "For You, I Will-tn2USd5KeVM.webm"
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
Deleting original file For You, I Will-tn2USd5KeVM.f242.webm (pass -k to keep)
Deleting original file For You, I Will-tn2USd5KeVM.f250.webm (pass -k to keep)
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
Deleting original file For You, I Will-tn2USd5KeVM.f242.webm (pass -k to keep)
Deleting original file For You, I Will-tn2USd5KeVM.f250.webm (pass -k to keep)
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"}
[postprocess] {"ETA":"","filename":"For You, I Will-tn2USd5KeVM.webm"})R" ;
}

const char * yt_dlp::testFfmpeg()
{
	return R"R([youtube] Extracting URL: https://www.youtube.com/watch?v=LOpES1ZlISk
[youtube] LOpES1ZlISk: Downloading webpage
[youtube] LOpES1ZlISk: Downloading ios player API JSON
[youtube] LOpES1ZlISk: Downloading android player API JSON
[youtube] LOpES1ZlISk: Downloading m3u8 information
[info] LOpES1ZlISk: Downloading 1 format(s): 242+250
[download] Destination: Bobby Marks on NBA free agency： Damian Lillard's future, A+ for the Lakers & more ｜ NBA on ESPN-LOpES1ZlISk.webm
Input #0, matroska,webm, from 'https://rr2---sn-8vq5jvhu1-q5ge.googlevideo.com/videoplayback?expire=1688404862&ei=Hq-iZJu0IMfIxwL-mLaQCQ&ip=197.250.197.106&id=o-AN-Mhp8xBo0FkzjLttQZTYA4rS_YvpOaxlWk1fVEvdeI&itag=242&source=youtube&requiressl=yes&mh=cM&mm=31%2C29&mn=sn-8vq5jvhu1-q5ge%2Csn-hgn7rnee&ms=au%2Crdu&mv=m&mvi=2&pl=18&initcwndbps=187500&vprv=1&svpuc=1&mime=video%2Fwebm&gir=yes&clen=9897290&dur=990.366&lmt=1688337165712349&mt=1688383058&fvip=1&keepalive=yes&fexp=24007246%2C51000012%2C51000024&beids=24350017&c=IOS&txp=5535434&sparams=expire%2Cei%2Cip%2Cid%2Citag%2Csource%2Crequiressl%2Cvprv%2Csvpuc%2Cmime%2Cgir%2Cclen%2Cdur%2Clmt&sig=AOq0QJ8wRQIhAKFiRG-SYmS_lgbtCtWeuEt80RrWW03pFxKBfpl90zbpAiBo74goFCLSHhlXu1TmepMystawvD9u91oXcTxxlE_Zpw%3D%3D&lsparams=mh%2Cmm%2Cmn%2Cms%2Cmv%2Cmvi%2Cpl%2Cinitcwndbps&lsig=AG3C_xAwRAIgEDiulbviVETAsGB8xZdRwUWvGvdjaqc5dX_8JmLs27sCID6Ysy7lQXN6gofN0OULiok1qwjLUSDRXfHkiKbmQw2G':
  Metadata:
    encoder         : google/video-file
  Duration: 00:16:30.37, start: 0.000000, bitrate: 79 kb/s
  Stream #0:0(eng): Video: vp9 (Profile 0), yuv420p(tv, bt709), 426x240, SAR 1:1 DAR 71:40, 30 fps, 30 tbr, 1k tbn (default)
Input #1, matroska,webm, from 'https://rr2---sn-8vq5jvhu1-q5ge.googlevideo.com/videoplayback?expire=1688404863&ei=H6-iZNPWL5WEWJHlpcAP&ip=197.250.197.106&id=o-AMrfanCUV2RL8GcaUhUIcOvDF8mBSvI4uKoL5tI8o6Kt&itag=250&source=youtube&requiressl=yes&mh=cM&mm=31%2C29&mn=sn-8vq5jvhu1-q5ge%2Csn-hgn7rnee&ms=au%2Crdu&mv=m&mvi=2&pl=18&initcwndbps=187500&spc=Ul2Sq81k0298IGzHwpxvOkPsaZQG910&vprv=1&svpuc=1&mime=audio%2Fwebm&gir=yes&clen=8067527&dur=990.401&lmt=1688330975379663&mt=1688382819&fvip=1&keepalive=yes&fexp=24007246&c=ANDROID&txp=5532434&sparams=expire%2Cei%2Cip%2Cid%2Citag%2Csource%2Crequiressl%2Cspc%2Cvprv%2Csvpuc%2Cmime%2Cgir%2Cclen%2Cdur%2Clmt&sig=AOq0QJ8wRgIhAMIMTZi-dS4vJWFNBY61qmGTiGyDzYDUsLx8yxbstjlkAiEAxs8cQNQ8oemMY-2q4GQF9iG64LbUtuG5Xebl1p0okWQ%3D&lsparams=mh%2Cmm%2Cmn%2Cms%2Cmv%2Cmvi%2Cpl%2Cinitcwndbps&lsig=AG3C_xAwRQIhAMfnOj8rkjCHX6bbkJ7KAPZvzOKwldrhtfw55zQxNyU7AiAYCXEerg-s936B6uuGEbJk_0BcvPstHGF4obpbe9ocaQ%3D%3D':
  Metadata:
    encoder         : google/video-file
  Duration: 00:16:30.40, start: -0.007000, bitrate: 65 kb/s
  Stream #1:0(eng): Audio: opus, 48000 Hz, stereo, fltp (default)
Output #0, webm, to 'file:Bobby Marks on NBA free agency： Damian Lillard's future, A+ for the Lakers & more ｜ NBA on ESPN-LOpES1ZlISk.webm.part':
  Metadata:
    encoder         : Lavf59.27.100
  Stream #0:0(eng): Video: vp9 (Profile 0), yuv420p(tv, bt709), 426x240 [SAR 1:1 DAR 71:40], q=2-31, 30 fps, 30 tbr, 1k tbn (default)
  Stream #0:1(eng): Audio: opus, 48000 Hz, stereo, fltp (default)
Stream mapping:
  Stream #0:0 -> #0:0 (copy)
  Stream #1:0 -> #0:1 (copy)
Press [q] to stop, [?] for help
frame=    1 fps=0.0 q=-1.0 size=       1kB time=00:00:00.00 bitrate=4768.0kbits/s speed=6.85x
frame=  871 fps=0.0 q=-1.0 size=     256kB time=00:00:29.00 bitrate=  72.3kbits/s speed=54.4x
frame= 1142 fps=1102 q=-1.0 size=     512kB time=00:00:38.03 bitrate= 110.3kbits/s speed=36.7x
frame= 1628 fps=1046 q=-1.0 size=     768kB time=00:00:54.23 bitrate= 116.0kbits/s speed=34.8x
frame= 2204 fps=1066 q=-1.0 size=    1280kB time=00:01:13.43 bitrate= 142.8kbits/s speed=35.5x
frame= 3151 fps=1226 q=-1.0 size=    1792kB time=00:01:45.00 bitrate= 139.8kbits/s speed=40.9x
frame= 4214 fps=1372 q=-1.0 size=    2304kB time=00:02:20.43 bitrate= 134.4kbits/s speed=45.7x
frame= 5128 fps=1434 q=-1.0 size=    2816kB time=00:02:50.90 bitrate= 135.0kbits/s speed=47.8x
frame= 6522 fps=1599 q=-1.0 size=    3584kB time=00:03:37.36 bitrate= 135.1kbits/s speed=53.3x
frame= 7621 fps=1653 q=-1.0 size=    4352kB time=00:04:14.00 bitrate= 140.4kbits/s speed=55.1x
frame= 9241 fps=1802 q=-1.0 size=    5376kB time=00:05:08.00 bitrate= 143.0kbits/s speed=60.1x
frame=11018 fps=1931 q=-1.0 size=    6400kB time=00:06:07.23 bitrate= 142.8kbits/s speed=64.4x
frame=11881 fps=1913 q=-1.0 size=    6912kB time=00:06:36.00 bitrate= 143.0kbits/s speed=63.8x
frame=13166 fps=1962 q=-1.0 size=    7680kB time=00:07:18.83 bitrate= 143.4kbits/s speed=65.4x
frame=14758 fps=2046 q=-1.0 size=    8448kB time=00:08:11.90 bitrate= 140.7kbits/s speed=68.2x
frame=16750 fps=2170 q=-1.0 size=    9728kB time=00:09:18.30 bitrate= 142.7kbits/s speed=72.3x
frame=17993 fps=2177 q=-1.0 size=   10496kB time=00:09:59.73 bitrate= 143.4kbits/s speed=72.6x
frame=20038 fps=2282 q=-1.0 size=   11520kB time=00:11:07.90 bitrate= 141.3kbits/s speed=76.1x
frame=21848 fps=2354 q=-1.0 size=   12544kB time=00:12:08.23 bitrate= 141.1kbits/s speed=78.5x
frame=24166 fps=2468 q=-1.0 size=   14080kB time=00:13:25.50 bitrate= 143.2kbits/s speed=82.3x
frame=26536 fps=2577 q=-1.0 size=   15360kB time=00:14:44.50 bitrate= 142.3kbits/s speed=85.9x
frame=28441 fps=2634 q=-1.0 size=   16640kB time=00:15:48.00 bitrate= 143.8kbits/s speed=87.8x
frame=29711 fps=2654 q=-1.0 Lsize=   17543kB time=00:16:30.38 bitrate= 145.1kbits/s speed=88.5x
video:9475kB audio:7554kB subtitle:0kB other streams:0kB global headers:0kB muxing overhead: 3.017891%
[download] 100% of   17.13MiB in 00:00:12 at 1.40MiB/s)R" ;
}

static const char * _jsonFullArguments()
{
	return R"R({"uploader":%(uploader)j,"id":%(id)j,"thumbnail":%(thumbnail)j,"duration":%(duration)j,"title":%(title)j,"upload_date":%(upload_date)j,"webpage_url":%(webpage_url)j,"formats":%(formats.:.{url,format_id,ext,resolution,filesize,tbr,vbr,abr,asr,container,protocol,vcodec,video_ext,acodec,audio_ext,format_note})j,"n_entries":%(n_entries)j,"playlist_id":%(playlist_id)j,"playlist_title":%(playlist_title)j,"playlist":%(playlist)j,"playlist_count":%(playlist_count)j,"playlist_uploader":%(playlist_uploader)j,"playlist_uploader_id":%(playlist_uploader_id)j})R" ;
}

QStringList yt_dlp::jsonNoFormatsArgumentList()
{
	auto a = R"R({"uploader":%(uploader)j,"id":%(id)j,"thumbnail":%(thumbnail)j,"duration":%(duration)j,"title":%(title)j,"upload_date":%(upload_date)j,"webpage_url":%(webpage_url)j,"n_entries":%(n_entries)j,"playlist_id":%(playlist_id)j,"playlist_title":%(playlist_title)j,"playlist":%(playlist)j,"playlist_count":%(playlist_count)j,"playlist_uploader":%(playlist_uploader)j,"playlist_uploader_id":%(playlist_uploader_id)j})R" ;

	return { "--newline","--print",a } ;
}

static QJsonObject _defaultControlStructure()
{
	QJsonObject obj ;

	obj.insert( "Connector","&&" ) ;

	obj.insert( "lhs",[](){

		QJsonObject obj ;

		obj.insert( "startsWith","[download]" ) ;

		return obj ;
	}() ) ;

	obj.insert( "rhs",[](){

		QJsonObject obj ;

		obj.insert( "contains","ETA" ) ;

		return obj ;
	}() ) ;

	return obj ;
}

QJsonObject yt_dlp::init( const QString& name,
			  const QString& configFileName,
			  Logger& logger,
			  const engines::enginePaths& enginePath )
{
	auto m = enginePath.enginePath( configFileName ) ;

	if( !QFile::exists( m ) ){

		QJsonObject mainObj ;

		if( name == "youtube-dl" ){

			mainObj.insert( "ShowListTableBoundary",[](){

				QJsonObject obj ;

				obj.insert( "ColumnNumber","0" ) ;
				obj.insert( "Comparator","equals" ) ;
				obj.insert( "String","format" ) ;

				return obj ;
			}() ) ;

			utility::addJsonCmd json( mainObj ) ;

			json.add( { { "Generic" },{ { "x86","youtube-dl",{ "youtube-dl" } },
						    { "amd64","youtube-dl",{ "youtube-dl" } } } } ) ;

			json.add( { { "Windows" },{ { "x86","youtube-dl.exe",{ "youtube-dl.exe" } },
						    { "amd64","youtube-dl.exe",{ "youtube-dl.exe" } } } } ) ;

			json.done() ;

			mainObj.insert( "DownloadUrl","https://api.github.com/repos/ytdl-org/youtube-dl/releases/latest" ) ;

			mainObj.insert( "DefaultListCmdOptions",[](){

				QJsonArray arr ;
				arr.append( "-F" ) ;

				return arr ;
			}() ) ;

			mainObj.insert( "DumptJsonArguments",[](){

				QJsonArray arr ;

				arr.append( "--dump-json" ) ;

				return arr ;
			}() ) ;
		}else{
			utility::addJsonCmd json( mainObj ) ;

			json.add( { { "Generic" },{ { "x86","yt-dlp",{ "yt-dlp" } },
						    { "amd64","yt-dlp",{ "yt-dlp" } } } } ) ;

			json.add( { { "Windows" },{ { "x86","yt-dlp_x86.exe",{ "yt-dlp_x86.exe" } },
						    { "amd64","yt-dlp.exe",{ "yt-dlp.exe" } } } } ) ;

			json.done() ;

			mainObj.insert( "DefaultListCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--print" ) ;
				arr.append( "%(formats)j" ) ;

				return arr ;
			}() ) ;

			mainObj.insert( "DumptJsonArguments",[](){

				QJsonArray arr ;

				arr.append( "--newline" ) ;
				arr.append( "--print" ) ;

				arr.append( _jsonFullArguments() ) ;

				return arr ;
			}() ) ;

			mainObj.insert( "DefaultCommentsCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--get-comments" ) ;
				arr.append( "--no-download" ) ;
				arr.append( "--print" ) ;
				arr.append( "{\"title\":%(title)j,\"comments\":%(comments)j}" ) ;

				return arr ;
			}() ) ;

			mainObj.insert( "DefaultSubstitlesCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--no-download" ) ;
				arr.append( "--print" ) ;
				arr.append( "{\"title\":%(title)j,\"automatic_captions\":%(automatic_captions)j,\"subtitles\":%(subtitles)j}" ) ;

				return arr ;
			}() ) ;

			mainObj.insert( "DefaultSubtitleDownloadOptions",[](){

				QJsonArray arr ;

				arr.append( "--embed-subs" ) ;

				return arr ;
			}() ) ;

			mainObj.insert( "DownloadUrl","https://api.github.com/repos/yt-dlp/yt-dlp/releases/latest" ) ;
		}

		mainObj.insert( "EncodingArgument","--encoding" ) ;

		mainObj.insert( "RequiredMinimumVersionOfMediaDownloader","2.2.0" ) ;

		mainObj.insert( "Name",name ) ;

		mainObj.insert( "CookieArgument","--cookies" ) ;

		mainObj.insert( "DefaultDownLoadCmdOptions",[](){

			QJsonArray arr ;

			arr.append( "--newline" ) ;
			arr.append( "--ignore-config" ) ;
			arr.append( "--no-playlist" ) ;
			arr.append( "-o" ) ;
			arr.append( "%(title).200s-%(id)s.%(ext)s" ) ;

			return arr ;
		}() ) ;

		mainObj.insert( "SkipLineWithText",[](){

			QJsonArray arr ;
			arr.append( "(pass -k to keep)" ) ;
			return arr ;
		}() ) ;

		mainObj.insert( "RemoveText",[](){

			QJsonArray arr ;

			return arr ;
		}() ) ;

		mainObj.insert( "SplitLinesBy",[](){

			QJsonArray arr ;

			arr.append( "\n" ) ;

			return arr ;
		}() ) ;

		mainObj.insert( "PlaylistItemsArgument","--playlist-items" ) ;

		mainObj.insert( "ControlJsonStructure",_defaultControlStructure() ) ;

		mainObj.insert( "VersionArgument","--version" ) ;

		mainObj.insert( "OptionsArgument","-f" ) ;

		mainObj.insert( "BackendPath",utility::stringConstants::defaultPath() ) ;

		mainObj.insert( "VersionStringLine",0 ) ;

		mainObj.insert( "VersionStringPosition",0 ) ;

		mainObj.insert( "BatchFileArgument","-a" ) ;

		mainObj.insert( "CanDownloadPlaylist",true ) ;

		mainObj.insert( "LikeYoutubeDl",true ) ;

		mainObj.insert( "ReplaceOutputWithProgressReport",false ) ;

		engines::file( m,logger ).write( mainObj ) ;

		return mainObj ;
	}else{
		return QJsonObject() ;
	}
}

yt_dlp::yt_dlp( const engines& engines,
		const engines::engine& engine,
		QJsonObject& obj,
		Logger& logger,
		const engines::enginePaths& enginePath,
		const util::version& version ) :
	engines::engine::functions( engines.Settings(),engine,engines.processEnvironment() ),
	m_engine( engine ),
	m_version( version ),
	m_likeYtdlp( m_engine.name() != "youtube-dl" )
{
	Q_UNUSED( m_version )

	auto name = obj.value( "Name" ).toString() ;

	if( name == "youtube-dl" || name == "yt-dlp" ){

		if( obj.value( "Cmd" ).isUndefined() ){

			auto configFileName = name + ".json" ;

			auto m = enginePath.enginePath( configFileName ) ;

			QFile::remove( m ) ;

			obj = yt_dlp::init( name,configFileName,logger,enginePath ) ;
		}
	}

	if( !obj.contains( "EncodingArgument" ) ){

		obj.insert( "EncodingArgument","--encoding" ) ;
	}

	if( name.contains( "yt-dlp" ) || name == "ytdl-patched" ){

		obj.insert( "DumptJsonArguments",[](){

			QJsonArray arr ;

			arr.append( "--newline" ) ;
			arr.append( "--print" ) ;

			arr.append( _jsonFullArguments() ) ;

			return arr ;
		}() ) ;

		if( !obj.contains( "DumptJsonArguments" ) ){

			obj.insert( "DefaultListCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--print" ) ;
				arr.append( "%(formats)j" ) ;

				return arr ;
			}() ) ;
		}

		if( !obj.contains( "DefaultCommentsCmdOptions" ) ){

			obj.insert( "DefaultCommentsCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--get-comments" ) ;
				arr.append( "--no-download" ) ;
				arr.append( "--print" ) ;
				arr.append( "{\"title\":%(title)j,\"comments\":%(comments)j}" ) ;

				return arr ;
			}() ) ;
		}

		if( !obj.contains( "DefaultSubstitlesCmdOptions" ) ){

			obj.insert( "DefaultSubstitlesCmdOptions",[](){

				QJsonArray arr ;

				arr.append( "--no-download" ) ;
				arr.append( "--print" ) ;
				arr.append( "{\"title\":%(title)j,\"automatic_captions\":%(automatic_captions)j,\"subtitles\":%(subtitles)j}" ) ;

				return arr ;
			}() ) ;
		}

		if( !obj.contains( "DefaultSubtitleDownloadOptions" ) ){

			obj.insert( "DefaultSubtitleDownloadOptions",[](){

				QJsonArray arr ;

				arr.append( "--embed-subs" ) ;

				return arr ;
			}() ) ;
		}
	}else{
		obj.insert( "CanDownloadPlaylist",false ) ;

		if( !obj.contains( "DumptJsonArguments" ) ){

			obj.insert( "DumptJsonArguments",[](){

				QJsonArray arr ;

				arr.append( "--dump-json" ) ;

				return arr ;
			}() ) ;
		}

		if( !obj.contains( "DefaultListCmdOptions" ) ){

			obj.insert( "DefaultListCmdOptions",[](){

				QJsonArray arr ;
				arr.append( "-F" ) ;

				return arr ;
			}() ) ;
		}
	}
}

#define COMPACTYEAR "2022"

void yt_dlp::appendCompatOption( QStringList& e )
{
	e.append( "--compat-options" ) ;
	e.append( COMPACTYEAR ) ;
}

const char * yt_dlp::youtube_dlFilter::compatYear()
{
	return "yt-dlp: error: wrong OPTS for --compat-options: " COMPACTYEAR ;
}

yt_dlp::~yt_dlp()
{
}

static bool _yt_dlp( const engines::engine&,const QByteArray& e )
{
	return utils::misc::startsWithAny( e,"[download]","[postprocess]" ) && e.contains( "ETA" ) ;
}

static bool _youtube_dl( const engines::engine&,const QByteArray& e )
{
	return e.startsWith( "[download]" ) && e.contains( "ETA" ) ;
}

static bool _fragment_output( const QByteArray& e )
{
	return utils::misc::startsWithAny( e,"[https @ ","[hls @ ","Opening '" ) ;
}

static bool _ffmpeg( const engines::engine&,const QByteArray& e )
{
	if( _fragment_output( e ) ){

		return true ;
	}else{
		return utils::misc::startsWithAny( e,"frame=","size=" ) ;
	}
}

static bool _aria2c( const engines::engine& s,const QByteArray& e )
{
	return aria2c::meetCondition( s,e ) ;
}

static bool _ffmpeg_internal( const engines::engine&,const QByteArray& e )
{
	return e.contains( " / ~" ) || e.startsWith( "Frame: " ) ;
}

static bool _shouldNotGetCalled( const engines::engine&,const QByteArray& )
{
	return false ;
}

class ytDlpFilter : public engines::engine::functions::filterOutPut
{
public:
	ytDlpFilter( const engines::engine& engine ) :
		m_engine( engine )
	{
	}
	engines::engine::functions::filterOutPut::result
	formatOutput( const engines::engine::functions::filterOutPut::args& args ) const override
	{
		if( m_function == _yt_dlp ){

			m_tmp = this->outPutFormat( args ) ;

			return { m_tmp,m_engine,m_function } ;

		}else if( m_function == _ffmpeg_internal ){

			m_tmp = this->outPutFfmpeg( args ) ;

			return { m_tmp,m_engine,m_function } ;
		}else{
			return { args.outPut,m_engine,m_function } ;
		}
	}
	bool meetCondition( const engines::engine::functions::filterOutPut::args& args ) const override
	{
		const auto& e = args.outPut ;

		if( _yt_dlp( m_engine,e ) ){

			if( m_engine.name() == "youtube-dl" ){

				m_function = _youtube_dl ;
			}else{
				m_function = _yt_dlp ;
			}

		}else if( _ffmpeg( m_engine,e ) ){

			m_function = _ffmpeg_internal ;

		}else if( _aria2c( m_engine,e ) ){

			m_function = _aria2c ;
		}else{
			m_function = _shouldNotGetCalled ;

			return false ;
		}

		return true ;
	}
	const engines::engine& engine() const override
	{
		return m_engine ;
	}
private:
	QByteArray outPutFormat( const engines::engine::functions::filterOutPut::args& args ) const
	{
		const auto& e = args.outPut ;
		const auto& locale = args.locale ;
		auto& s = args.data ;

		if( !s.mainLogger() && e.startsWith( "[postprocess]" ) ){

			auto obj = QJsonDocument::fromJson( e.mid( 14 ) ).object() ;

			s.ytDlpData().setFilePath( obj.value( "filename" ).toString().toUtf8() ) ;

			return {} ;
		}

		auto obj = QJsonDocument::fromJson( e.mid( 11 ) ).object() ;

		auto downloaded_str = obj.value( "downloaded_bytes" ).toString() ;
		auto totalbytesEstimate = obj.value( "total_bytes_estimate" ).toString() ;
		auto eta = obj.value( "ETA" ).toString() ;
		auto speed = obj.value( "speed" ).toString() ;
		auto totalBytes = obj.value( "total_bytes" ).toString() ;

		QString progress = e.mid( 0,11 ) ;

		if( downloaded_str == "NA" ){

			progress += "NA / " ;
		}else{
			auto m = qint64( downloaded_str.toDouble() ) ;

			progress += locale.formattedDataSize( m )  + " / " ;
		}

		double percentage = 0 ;

		if( totalBytes != "NA" ){

			auto mm = totalBytes.toDouble() ;

			if( mm != 0 ){

				percentage = downloaded_str.toDouble() * 100 / mm ;
			}

			progress += locale.formattedDataSize( qint64( mm ) ) ;

		}else if( totalbytesEstimate != "NA" ){

			auto mm = totalbytesEstimate.toDouble() ;

			if( mm != 0 ){

				percentage = downloaded_str.toDouble() * 100 / mm ;
			}

			progress += "~" + locale.formattedDataSize( qint64( mm ) ) ;
		}else{
			progress += "NA" ;
		}

		if( percentage < 100 ){

			progress += " (" + QString::number( percentage,'f',2 ) + "%)" ;
		}else{
			progress += " (100%)" ;
		}

		if( speed != "NA" ){

			auto mm = speed.toDouble() ;

			progress += " at " + locale.formattedDataSize( qint64( mm ) ) + "/s" ;
		}

		if( eta == "NA" ){

			progress += ", ETA NA" ;
		}else{
			progress += ", ETA " + locale.secondsToString( eta.toInt() ) ;
		}

		return progress.toUtf8() ;
	}
	double toSeconds( const QByteArray& s ) const
	{
		if( s.isEmpty() ){

			return 0 ;
		}else{
			auto mm = util::split( util::split( s,'.' )[ 0 ],':' ) ;

			if( mm.size() > 2 ){

				auto h  = mm[ 0 ].toDouble() ;
				auto m  = mm[ 1 ].toDouble() ;
				auto ss = mm[ 2 ].toDouble() ;

				ss += m * 60 ;
				ss += h * 60 * 60 ;

				return ss ;
			}else{
				return 0 ;
			}
		}
	}
	QByteArray duration( const QByteArray& e ) const
	{
		if( e.contains( "  Duration: " ) ){

			auto m = util::split( e,' ' ) ;

			for( int a = 0 ; a < m.size() ; a++ ){

				if( m[ a ] == "Duration:" && a + 1 < m.size() ){

					auto mm = m[ a + 1 ].replace( ",","" ) ;

					return mm ;
				}
			}

			return {} ;
		}else{
			return {} ;
		}
	}
	QString getOption( const QList< QByteArray >& m,const char * opt ) const
	{
		for( int i = 0 ; i < m.size() ; i++ ){

			const auto& s = m[ i ] ;

			if( s == opt ){

				if( i + 1 < m.size() ){

					return m[ i + 1 ] ;
				}

			}else if( s.startsWith( opt ) ){

				auto m = s.indexOf( '=' ) ;

				if( m == -1 ){

					return "NA" ;
				}else{
					return s.mid( m + 1 ) ;
				}
			}
		}

		return "NA" ;
	}
	qint64 size( QByteArray e ) const
	{
		return e.replace( "kB","" ).toLongLong() * 1024 ;
	}
	QByteArray outPutFfmpeg( const filterOutPut::args& args ) const
	{
		const auto& data   = args.outPut ;
		auto& s            = args.data ;
		const auto& locale = args.locale ;

		double totalTime = 0 ;

		if( _fragment_output( data ) ){

			return args.data.lastText() ;
		}

		QByteArray totalTimeString ;

		s.forEach( [ & ]( int,const QByteArray& e ){

			auto d = this->duration( e ) ;

			if( d.isEmpty() ){

				return false ;
			}else{
				totalTime = this->toSeconds( d ) ;

				totalTimeString = std::move( d ) ;

				return true ;
			}
		} ) ;

		auto m = util::split( data,' ' ) ;

		auto currentTimeString = this->getOption( m,"time=" ) ;
		auto size = this->getOption( m,"size=" ) ;

		if( size == "NA" ){

			size = this->getOption( m,"Lsize=" ) ;
		}

		auto currentTime = this->toSeconds( currentTimeString.toUtf8() ) ;

		auto currentSize = this->size( size.toUtf8() ) ;

		if( currentTime == 0 || totalTime == 0 || currentSize == 0 || size == "NA" ){

			auto frame   = this->getOption( m,"frame=" ) ;
			auto fps     = this->getOption( m,"fps=" ) ;
			auto bitrate = this->getOption( m,"bitrate=" ) ;
			auto speed   = this->getOption( m,"speed=" ) ;

			QString result = "Frame: %1, Fps: %2, Size: %3, Bitrate: %4, Speed: %5" ;

			return result.arg( frame,fps,size,bitrate,speed ).toUtf8() ;
		}else{
			auto r = currentTime * 100 / totalTime ;

			auto totalSize = totalTime * currentSize / currentTime ;

			auto totalSizeString = locale.formattedDataSize( totalSize ) ;
			auto currentSizeString = locale.formattedDataSize( currentSize ) ;

			auto completed = QString::number( r,'f',2 ) ;

			if( completed == "100.00" ){

				completed = "100" ;
			}

			auto frame   = this->getOption( m,"frame=" ) ;
			auto fps     = this->getOption( m,"fps=" ) ;
			auto bitrate = this->getOption( m,"bitrate=" ) ;
			auto speed   = this->getOption( m,"speed=" ) ;

			QString a1 = "%1 / ~%2 (%3%) at %4" ;
			auto a = a1.arg( currentSizeString,totalSizeString,completed,speed ).toUtf8() ;

			QString b1 = "Frame: %1, Fps: %2, Bitrate: %3" ;
			auto b = b1.arg( frame,fps,bitrate ).toUtf8() ;

			if( s.mainLogger() ){

				return a + ", " + b ;
			}else{
				return a + "\n" + b ;
			}
		}
	}

	mutable QByteArray m_tmp ;
	const engines::engine& m_engine ;
	mutable bool( *m_function )( const engines::engine&,const QByteArray& ) ;
} ;

engines::engine::functions::FilterOutPut yt_dlp::filterOutput()
{
	return { util::types::type_identity< ytDlpFilter >(),m_engine } ;
}

std::vector< engines::engine::functions::mediaInfo > yt_dlp::mediaProperties( Logger& l,const QByteArray& e )
{
	const auto& name = m_engine.name() ;

	if( name == "youtube-dl" ){

		return engines::engine::functions::mediaProperties( l,e ) ;
	}else{
		QJsonParseError err ;

		auto json = QJsonDocument::fromJson( e,&err ) ;

		if( err.error == QJsonParseError::NoError ){

			return this->mediaProperties( l,json.array() ) ;
		}else{
			utility::failedToParseJsonData( l,err ) ;

			return {} ;
		}
	}
}

std::vector< engines::engine::functions::mediaInfo > yt_dlp::mediaProperties( Logger& l,const QJsonArray& array )
{
	if( array.isEmpty() ){

		return {} ;
	}

	if( m_engine.name() == "youtube-dl" ){

		return engines::engine::functions::mediaProperties( l,array ) ;
	}

	std::vector< engines::engine::functions::mediaInfo > firstToShow ;
	std::vector< engines::engine::functions::mediaInfo > secondToShow ;
	std::vector< engines::engine::functions::mediaInfo > thirdtToShow ;

	Logger::locale s ;

	enum class mediaType{ audioOnly,videoOnly,audioVideo,unknown } ;

	auto _append = [ & ]( QString& s,const char * str,const QString& sstr,bool formatBitrate ){

		if( sstr == "none" || sstr.isEmpty() ){

			return ;
		}

		if( formatBitrate ){

			auto m = sstr.indexOf( '.' ) ;

			if( m == -1 ){

				s += str + sstr + "k, " ;
			}else{
				s += str + sstr.mid( 0,m ) + "k, " ;
			}
		}else{
			s += str + sstr + ", " ;
		}
	} ;

	for( const auto& it : array ){

		auto obj       = it.toObject() ;

		auto url       = obj.value( "url" ).toString() ;
		auto id        = obj.value( "format_id" ).toString() ;
		auto ext       = obj.value( "ext" ).toString() ;
		auto rsn       = obj.value( "resolution" ).toString() ;

		auto fileSize  = s.formattedDataSize( obj.value( "filesize" ).toInt() ) ;
		auto tbr       = QString::number( obj.value( "tbr" ).toDouble() ) ;
		auto vbr       = QString::number( obj.value( "vbr" ).toDouble() ) ;
		auto abr       = QString::number( obj.value( "abr" ).toDouble() ) ;
		auto asr       = QString::number( obj.value( "asr" ).toInt() ) ;

		auto container = obj.value( "container" ).toString() ;
		auto proto     = obj.value( "protocol" ).toString() ;
		auto vcodec    = obj.value( "vcodec" ).toString() ;
		//auto video_ext = obj.value( "video_ext" ).toString() ;
		auto acodec    = obj.value( "acodec" ).toString() ;
		//auto audio_ext = obj.value( "audio_ext" ).toString() ;
		auto fmtNotes  = obj.value( "format_note" ).toString() ;

		mediaType mt = mediaType::unknown ;

		if( rsn.isEmpty() ){

			rsn = fmtNotes ;
		}else{
			if( rsn == "audio only" ){

				mt = mediaType::audioOnly ;
			}else{
				bool hasVideo = vcodec != "none" ;
				bool hasAudio = acodec != "none" ;

				if( hasVideo && hasAudio ){

					rsn += "\naudio video" ;

					mt = mediaType::audioVideo ;

				}else if( hasVideo && !hasAudio ){

					rsn += "\nvideo only" ;

					mt = mediaType::videoOnly ;

				}else if( !hasVideo && hasAudio ){

					rsn += "\naudio only" ;

					mt = mediaType::audioOnly ;
				}
			}

			rsn += "\n" + fmtNotes ;
		}

		QString s ;

		if( container.isEmpty() ){

			s = QString( "Proto: %1, File Size: %2\n" ).arg( proto,fileSize ) ;
		}else{
			s = QString( "Proto: %1, File Size: %2\ncontainer: %3\n" ).arg( proto,fileSize,container ) ;
		}

		_append( s,"acodec: ",acodec,false ) ;
		_append( s,"vcodec: ",vcodec,false ) ;

		if( tbr != "0" ){

			_append( s,"tbr: ",tbr,true ) ;
		}

		if( asr != "0" ){

			_append( s,"asr: ",asr + "Hz",false ) ;
		}

		if( mt == mediaType::audioVideo ){

			_append( s,"vbr: ",vbr,true ) ;
			_append( s,"abr: ",abr,true ) ;

		}else if( mt == mediaType::audioOnly ){

			_append( s,"abr: ",abr,true ) ;

		}else if( mt == mediaType::videoOnly ){

			_append( s,"vbr: ",vbr,true ) ;
		}

		if( s.endsWith( ", " ) ){

			s.truncate( s.size() - 2 ) ;
		}

		QStringList arr{ url } ;

		if( ext == "mhtml" ){

			firstToShow.emplace_back( arr,id,ext,rsn,s ) ;

		}else if( rsn != "audio only" && !rsn.contains( "video only" ) ){

			thirdtToShow.emplace_back( arr,id,ext,rsn,s ) ;
		}else{
			secondToShow.emplace_back( arr,id,ext,rsn,s ) ;
		}
	}

	for( auto& it : secondToShow ){

		firstToShow.emplace_back( std::move( it ) ) ;
	}

	for( auto& it : thirdtToShow ){

		firstToShow.emplace_back( std::move( it ) ) ;
	}

	return firstToShow ;
}

bool yt_dlp::breakShowListIfContains( const QStringList& e )
{
	auto _match_found = []( const QJsonObject& obj,const QStringList& e ){

		auto a    = obj.value( "ColumnNumber" ).toString() ;
		auto cmp  = obj.value( "Comparator" ).toString() ;
		auto text = obj.value( "String" ).toString() ;

		if( !a.isEmpty() && !cmp.isEmpty() && !text.isEmpty() ){

			bool valid ;
			auto number = a.toInt( &valid ) ;

			if( valid && number < e.size() ){

				if( cmp == "equals" ){

					return text == e[ number ] ;

				}else if( cmp == "contains" ){

					return e[ number ].contains( text ) ;
				}
			}
		}

		return false ;
	} ;

	if( m_objs.size() == 0 ){

		if( e.size() > 1 ){

			return e.at( 0 ) == "format" || e.at( 2 ).contains( "-" ) ;
		}else{
			return false ;
		}
	}else{
		for( const auto& it : util::asConst( m_objs ) ){

			if( it.isObject() && _match_found( it.toObject(),e ) ){

				return true ;
			}
		}

		return false ;
	}
}

bool yt_dlp::supportsShowingComments()
{
	return this->likeYtdlp() ;
}

bool yt_dlp::updateVersionInfo()
{
	return false ;
}

bool yt_dlp::likeYtdlp()
{
	return m_likeYtdlp ;
}

void yt_dlp::updateLocalOptions( QStringList& opts )
{
	if( this->likeYtdlp() ){

		opts.prepend( "--break-on-reject" ) ;
		opts.prepend( "!playlist" ) ;
		opts.prepend( "--match-filter" ) ;
	}
}

void yt_dlp::setTextEncondig( const QString& args,QStringList& opts )
{
	const auto& e = engines::engine::functions::Settings().textEncoding() ;

	if( !e.isEmpty() && !args.isEmpty() ){

		opts.append( args ) ;
		opts.append( e ) ;
	}
}

engines::engine::functions::DataFilter yt_dlp::Filter( int id )
{
	return { util::types::type_identity< yt_dlp::youtube_dlFilter >(),id,m_engine,m_likeYtdlp } ;
}

void yt_dlp::runCommandOnDownloadedFile( const QString& e,const QString& )
{
	auto& settings = engines::engine::functions::Settings() ;
	auto a = settings.commandOnSuccessfulDownload() ;

	if( !a.isEmpty() && !e.isEmpty() ){

		auto b = settings.downloadFolder() + "/" + util::split( e,'\n',true ).at( 0 ) ;

		if( QFile::exists( b ) ){

			auto args = util::split( a,' ',true ) ;

			args.append( b ) ;

			auto exe = args.takeAt( 0 ) ;

			QProcess::startDetached( exe,args ) ;
		}
	}
}

QString yt_dlp::updateTextOnCompleteDownlod( const QString& uiText,
					     const QString& bkText,
					     const QString& dopts,
					     const engines::engine::functions::finishedState& f )
{
	using functions = engines::engine::functions ;

	if( f.cancelled() ){

		return functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;

	}else if( f.success() ){

		auto e = engines::engine::mediaAlreadInArchiveText() ;

		if( uiText.contains( e ) ){

			auto m = engines::engine::functions::updateTextOnCompleteDownlod( bkText,dopts,f ) ;

			return m + "\n" + e  ;
		}

		QStringList a ;

		for( const auto& it : util::split( uiText,'\n',true ) ){

			auto x = engines::engine::functions::postProcessing::processingText() ;
			auto y = engines::engine::functions::preProcessing::processingText() ;

			if( !it.contains( x ) && !it.contains( y ) ){

				a.append( it ) ;
			}
		}

		return engines::engine::functions::updateTextOnCompleteDownlod( a.join( "\n" ),dopts,f ) ;

	}else if( uiText == "EngineNeedUpdating" ){

		const auto& name = this->engine().name() ;
		auto version = "2023.01.06" ;

		return QObject::tr( "Please Update \"%1\" To Atleast Version \"%2\"" ).arg( name,version ) ;

	}else if( uiText.contains( "Requested format is not available" ) ){

		return functions::errorString( f,functions::errors::unknownFormat,bkText ) ;

	}else if( uiText.contains( "Temporary failure in name resolution" ) ){

		return functions::errorString( f,functions::errors::noNetwork,bkText ) ;

	}else if( uiText.contains( " is not a valid URL" ) ){

		return functions::errorString( f,functions::errors::unknownUrl,bkText ) ;

	}else if( uiText.contains( "ERROR: Unsupported URL:" ) ){

		return functions::errorString( f,functions::errors::notSupportedUrl,bkText ) ;
	}else{
		auto m = engines::engine::functions::updateTextOnCompleteDownlod( uiText,dopts,f ) ;
		return m + "\n" + bkText ;
	}
}

void yt_dlp::updateDownLoadCmdOptions( const engines::engine::functions::updateOpts& s )
{
	if( s.userOptions.contains( "--yes-playlist" ) ){

		s.ourOptions.removeAll( "--no-playlist" ) ;
	}

	if( !s.ourOptions.contains( "--newline" ) ){

		s.ourOptions.append( "--newline" ) ;
	}

	s.ourOptions.append( "--output-na-placeholder" ) ;
	s.ourOptions.append( "NA" ) ;

	auto _replace = [ this ]( QStringList& s,QString& txt,const QString& original,const QString& New ){

		if( m_likeYtdlp ){

			if( txt.contains( original ) ){

				s.append( "--parse-metadata" ) ;
				s.append( New + ":" + original ) ;
			}
		}else{
			txt.replace( original,New ) ;
		}
	} ;

	for( int m = 0 ; m < s.ourOptions.size() ; m++ ){

		if( s.ourOptions[ m ] == "-o" ){

			if( m + 1 < s.ourOptions.size() ){

				auto& e = s.ourOptions[ m + 1 ] ;

				auto w = s.uiIndex.toString( true,s.ourOptions ) ;
				auto ww = s.uiIndex.toString( false,s.ourOptions ) ;

				_replace( s.ourOptions,e,"%(autonumber)s",ww ) ;
				_replace( s.ourOptions,e,"%(playlist_index)s",w ) ;
				_replace( s.ourOptions,e,"%(playlist_autonumber)s",w ) ;
				_replace( s.ourOptions,e,"%(playlist_id)s",s.playlist_id ) ;
				_replace( s.ourOptions,e,"%(playlist_title)s",s.playlist_title ) ;
				_replace( s.ourOptions,e,"%(playlist)s",s.playlist ) ;
				_replace( s.ourOptions,e,"%(playlist_count)s",s.playlist_count ) ;
				_replace( s.ourOptions,e,"%(playlist_uploader)s",s.playlist_uploader ) ;
				_replace( s.ourOptions,e,"%(playlist_uploader_id)s",s.playlist_uploader_id ) ;
				_replace( s.ourOptions,e,"%(n_entries)s",s.n_entries ) ;

				m++ ;
			}
		}
	}

	engines::engine::functions::updateDownLoadCmdOptions( s ) ;

	if( m_likeYtdlp ){

		this->appendCompatOption( s.ourOptions ) ;

		if( m_engine.name() == "yt-dlp" || m_engine.name() == "ytdl-patched" ){

			while( s.ourOptions.contains( "--progress-template" ) ){

				utility::arguments( s.ourOptions ).removeOptionWithArgument( "--progress-template" ) ;
			}

			s.ourOptions.append( "--progress-template" ) ;
			s.ourOptions.append( R"R(download:[download] {"filename":"%(progress.filename)s","downloaded_bytes":"%(progress.downloaded_bytes)s","ETA":"%(progress.eta)s","total_bytes_estimate":"%(progress.total_bytes_estimate)s","total_bytes":"%(progress.total_bytes)s","speed":"%(progress.speed)s","fragment_index":"%(progress.fragment_index)s","fragment_count":"%(progress.fragment_count)s"})R" ) ;

			//s.ourOptions.append( "--progress-template" ) ;
			//s.ourOptions.append( R"R(postprocess:[postprocess] {"ETA":"","filename":"%(info.filepath)s"})R" ) ;
		}
	}
}

void yt_dlp::updateGetPlaylistCmdOptions( QStringList& e )
{
	if( this->likeYtdlp() ){

		e.append( "--lazy-playlist" ) ;

		this->appendCompatOption( e ) ;
	}

	e.append( "--output-na-placeholder" ) ;
	e.append( "\"NA\"" ) ;
}

void yt_dlp::updateCmdOptions( QStringList& e )
{
	if( this->likeYtdlp() ){

		this->appendCompatOption( e ) ;
	}

	e.append( "--output-na-placeholder" ) ;
	e.append( "\"NA\"" ) ;
}

yt_dlp::youtube_dlFilter::youtube_dlFilter( int processId,
					    const engines::engine& engine,
					    bool likeYtdlp ) :
	engines::engine::functions::filter( engine,processId ),
	m_engine( engine )
{
	Q_UNUSED( likeYtdlp )
}

const QByteArray& yt_dlp::youtube_dlFilter::operator()( const Logger::Data& s )
{
	if( s.lastLineIsProgressLine() ){

		const auto& m = s.lastText() ;

		if( m.startsWith( "[download] " ) ){

			m_tmp = m_fileName + "\n" + m.mid( 11 ) ;

		}else if( m_engine.name().contains( "aria2c" ) ){

			auto n = m.indexOf( ' ' ) ;

			if( n != -1 ){

				m_tmp = m_fileName + "\n" + m.mid( n + 1 ) ;
			}else{
				m_tmp = m_fileName + "\n" + m ;
			}

			aria2c::trimProgressLine( m_tmp ) ;
		}else{
			m_tmp = m_fileName + "\n" + m ;
		}

		return m_tmp ;
	}

	const auto m = s.toStringList() ;

	if( s.doneDownloading() ){

		if( utility::stringConstants::downloadFailed( s.lastText() ) ){

			for( auto it = m.rbegin() ; it != m.rend() ; it++ ){

				const QByteArray& e = *it ;

				if( e.startsWith( "ERROR: " ) ){

					m_tmp = e ;

					return m_tmp ;
				}
			}
		}

		if( m_fileName.isEmpty() ){

			/*
			 * Failed to find file name, try again.
			 */

			this->parseOutput( m ) ;

			if( m_fileName.isEmpty() ){

				const auto& m = s.ytDlpData().filePath() ;

				if( !m.isEmpty() ){

					return m ;
				}else{
					//????
					return m_tmp ;
				}
			}else{
				return m_fileName ;
			}
		}else{
			return m_fileName ;
		}
	}

	return this->parseOutput( m ) ;
}

yt_dlp::youtube_dlFilter::~youtube_dlFilter()
{
}

const QByteArray& yt_dlp::youtube_dlFilter::parseOutput( const Logger::Data::QByteArrayList& data )
{
	for( const auto& m : data ){

		const QByteArray& e = m ;

		if( e.contains( this->compatYear() ) ){

			m_tmp = "EngineNeedUpdating" ;

			return m_tmp ;
		}
		if( e.startsWith( "ERROR: " ) || e.startsWith( "yt-dlp: error:" ) ){

			m_tmp = e ;

			return m_tmp ;
		}
		if( e.startsWith( "[download] " ) && e.contains( " has already been downloaded" ) ){

			auto m = e.mid( e.indexOf( " " ) + 1 ) ;
			m.truncate( m.indexOf( " has already been downloaded" ) ) ;

			this->setFileName( m ) ;
		}
		if( e.contains( "] Destination: " ) ){

			this->setFileName( e.mid( e.indexOf( "] Destination: " ) + 15 ) ) ;
		}
		if( e.contains( " Merging formats into \"" ) ){

			auto m = e.mid( e.indexOf( '"' ) + 1 ) ;
			auto s = m.lastIndexOf( '"' ) ;

			if( s != -1 ){

				m.truncate( s ) ;
			}

			this->setFileName( m ) ;
		}
		if( e.contains( "has already been recorded" ) ){

			m_tmp = engines::engine::mediaAlreadInArchiveText().toUtf8() ;

			return m_tmp ;
		}
	}

	return m_preProcessing.text() ;
}

void yt_dlp::youtube_dlFilter::setFileName( const QByteArray& fileName )
{
	if( m_fileName != fileName ){

		m_fileName = fileName ;
	}
}
