/*
 *
 *  Copyright (c) 2022
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

#include "svtplay-dl.h"
#include "../settings.h"
#include "../util.hpp"
#include "../utils/miscellaneous.hpp"
#include "../utility.h"

const char * svtplay_dl::testData()
{
	return R"R(DEBUG [1689598027.9425933] svtplay-dl/svtplay_dl/utils/getmedia.py/get_media: version: 4.24
DEBUG [1689598027.9450247] svtplay-dl/svtplay_dl/service/__init__.py/__init__: service: svtplay
DEBUG [1689598027.9451392] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://www.svtplay.se/klipp/KZxpGB6/kriget-i-ukraina-har-besoker-svt-en-skyttegrav-i-zaporizjzja-regionen?id=KZxpGB6'
DEBUG [1689598027.9475543] $urllib3/connectionpool.py/_new_conn: Starting new HTTPS connection (1): www.svtplay.se:443
DEBUG [1689598031.6283054] $urllib3/connectionpool.py/_make_request: https://www.svtplay.se:443 "GET /klipp/KZxpGB6/kriget-i-ukraina-har-besoker-svt-en-skyttegrav-i-zaporizjzja-regionen?id=KZxpGB6 HTTP/1.1" 200 None
DEBUG [1689598034.2711911] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://api.svt.se/video/KZxpGB6'
DEBUG [1689598034.2727182] $urllib3/connectionpool.py/_new_conn: Starting new HTTPS connection (1): api.svt.se:443
DEBUG [1689598043.7094605] $urllib3/connectionpool.py/_make_request: https://api.svt.se:443 "GET /video/KZxpGB6 HTTP/1.1" 200 1224
DEBUG [1689598043.7114809] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/hls-ts-lb-full.m3u8'
DEBUG [1689598043.713569] $urllib3/connectionpool.py/_new_conn: Starting new HTTPS connection (1): svt-vod-7c.akamaized.net:443
DEBUG [1689598049.906357] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/hls-ts-lb-full.m3u8 HTTP/1.1" 200 590
DEBUG [1689598049.9126453] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/hls-ts-full.m3u8'
DEBUG [1689598050.0216253] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/hls-ts-full.m3u8 HTTP/1.1" 200 704
DEBUG [1689598050.0271435] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/hls-ts-avc.m3u8'
DEBUG [1689598050.1202781] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/hls-ts-avc.m3u8 HTTP/1.1" 200 704
DEBUG [1689598050.1256115] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/dash-full.mpd'
DEBUG [1689598050.217629] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/dash-full.mpd HTTP/1.1" 200 918
DEBUG [1689598050.234446] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/hls-cmaf-lb-full.m3u8'
DEBUG [1689598050.3231468] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/hls-cmaf-lb-full.m3u8 HTTP/1.1" 200 593
DEBUG [1689598050.3272824] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/dash-lb-full.mpd'
DEBUG [1689598050.418111] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/dash-lb-full.mpd HTTP/1.1" 200 827
DEBUG [1689598050.428062] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/dash-hbbtv-avc.mpd'
DEBUG [1689598050.5233016] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/dash-hbbtv-avc.mpd HTTP/1.1" 200 797
DEBUG [1689598050.5321388] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/dash-avc.mpd'
DEBUG [1689598050.6222649] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/dash-avc.mpd HTTP/1.1" 200 918
DEBUG [1689598050.642335] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/hls-ts-avc.m3u8'
DEBUG [1689598052.6139941] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/hls-ts-avc.m3u8 HTTP/1.1" 200 704
DEBUG [1689598052.6228123] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/dash-avc.mpd'
DEBUG [1689598052.7237513] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/dash-avc.mpd HTTP/1.1" 200 918
DEBUG [1689598052.738594] svtplay-dl/svtplay_dl/utils/stream.py/format_prio: Format priority: ['h264', 'h264-51']
DEBUG [1689598052.73879] svtplay-dl/svtplay_dl/utils/stream.py/protocol_prio: Protocol priority scores (higher is better): {'dash': 3, 'hls': 2, 'http': 1}
DEBUG [1689598052.7391403] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/dash-full.mpd'
DEBUG [1689598052.7407427] $urllib3/connectionpool.py/_new_conn: Starting new HTTPS connection (1): svt-vod-7c.akamaized.net:443
DEBUG [1689598058.1654437] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/dash-full.mpd HTTP/1.1" 200 918
INFO [1689598058.1682267] svtplay-dl/svtplay_dl/utils/output.py/find_dupes: Outfile: svt.nyheter.kriget.i.ukraina.har.besoker.svt.en.skyttegrav.i.zaporizjzja-regionen-77e0cd8-svtplay.mp4
INFO [1689598058.1692932] svtplay-dl/svtplay_dl/utils/getmedia.py/get_one_media: Selected to download dash, bitrate: 3884 format: h264

[01/49][=..................................................................] ETA: 0:00:00DEBUG [1689598058.1715927] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-init.mp4'
DEBUG [1689598058.175341] $urllib3/connectionpool.py/_new_conn: Starting new HTTPS connection (1): svt-vod-7c.akamaized.net:443
DEBUG [1689598058.6021652] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-init.mp4 HTTP/1.1" 200 823

[02/49][==.................................................................] ETA: 0:00:10DEBUG [1689598058.6034865] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-1.mp4'
DEBUG [1689598058.6837819] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-1.mp4 HTTP/1.1" 200 94081

[03/49][====...............................................................] ETA: 0:00:22DEBUG [1689598059.656134] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-2.mp4'
DEBUG [1689598059.7667172] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-2.mp4 HTTP/1.1" 200 93060

[04/49][=====..............................................................] ETA: 0:00:20DEBUG [1689598060.0058322] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-3.mp4'
DEBUG [1689598060.3171017] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-3.mp4 HTTP/1.1" 200 93036

[05/49][======.............................................................] ETA: 0:00:19DEBUG [1689598060.4153004] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-4.mp4'
DEBUG [1689598060.524308] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-4.mp4 HTTP/1.1" 200 93055

[06/49][========...........................................................] ETA: 0:00:24DEBUG [1689598061.6401737] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-5.mp4'
DEBUG [1689598061.7451978] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-5.mp4 HTTP/1.1" 200 93056

[07/49][=========..........................................................] ETA: 0:00:22DEBUG [1689598061.9532626] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-6.mp4'
DEBUG [1689598062.423808] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-6.mp4 HTTP/1.1" 200 93050

[08/49][==========.........................................................] ETA: 0:00:21DEBUG [1689598062.425927] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-7.mp4'
DEBUG [1689598062.5105324] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-7.mp4 HTTP/1.1" 200 93069

[09/49][============.......................................................] ETA: 0:00:19DEBUG [1689598062.5715375] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-8.mp4'
DEBUG [1689598062.6526217] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-8.mp4 HTTP/1.1" 200 93046

[10/49][=============......................................................] ETA: 0:00:18DEBUG [1689598062.7956758] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-9.mp4'
DEBUG [1689598063.0027351] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-9.mp4 HTTP/1.1" 200 93228

[11/49][===============....................................................] ETA: 0:00:22DEBUG [1689598064.646315] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-10.mp4'
DEBUG [1689598064.770847] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-10.mp4 HTTP/1.1" 200 92832

[12/49][================...................................................] ETA: 0:00:20DEBUG [1689598064.847949] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-11.mp4'
DEBUG [1689598064.9408367] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-11.mp4 HTTP/1.1" 200 93048

[13/49][=================..................................................] ETA: 0:00:21DEBUG [1689598065.8986483] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-12.mp4'
DEBUG [1689598066.372316] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-12.mp4 HTTP/1.1" 200 93112

[14/49][===================................................................] ETA: 0:00:21DEBUG [1689598066.9145744] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-13.mp4'
DEBUG [1689598067.0727012] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-13.mp4 HTTP/1.1" 200 93009

[15/49][====================...............................................] ETA: 0:00:20DEBUG [1689598067.1774695] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-14.mp4'
DEBUG [1689598067.2648528] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-14.mp4 HTTP/1.1" 200 93023

[16/49][=====================..............................................] ETA: 0:00:19DEBUG [1689598067.4815724] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-15.mp4'
DEBUG [1689598067.5807397] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-15.mp4 HTTP/1.1" 200 93048

[17/49][=======================............................................] ETA: 0:00:17DEBUG [1689598067.631378] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-16.mp4'
DEBUG [1689598068.5485268] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-16.mp4 HTTP/1.1" 200 93089

[18/49][========================...........................................] ETA: 0:00:17DEBUG [1689598068.5501351] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-17.mp4'
DEBUG [1689598068.6540153] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-17.mp4 HTTP/1.1" 200 93007

[19/49][=========================..........................................] ETA: 0:00:16DEBUG [1689598068.7148623] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-18.mp4'
DEBUG [1689598068.807692] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-18.mp4 HTTP/1.1" 200 93054

[20/49][===========================........................................] ETA: 0:00:15DEBUG [1689598068.877555] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-19.mp4'
DEBUG [1689598069.2849655] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-19.mp4 HTTP/1.1" 200 93112

[21/49][============================.......................................] ETA: 0:00:14DEBUG [1689598069.3012028] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-20.mp4'
DEBUG [1689598069.3953052] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-20.mp4 HTTP/1.1" 200 93048

[22/49][==============================.....................................] ETA: 0:00:14DEBUG [1689598069.6585193] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-21.mp4'
DEBUG [1689598069.7596045] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-21.mp4 HTTP/1.1" 200 93094

[23/49][===============================....................................] ETA: 0:00:14DEBUG [1689598070.581385] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-22.mp4'
DEBUG [1689598070.686542] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-22.mp4 HTTP/1.1" 200 93135

[24/49][================================...................................] ETA: 0:00:13DEBUG [1689598070.746746] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-23.mp4'
DEBUG [1689598070.840801] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-23.mp4 HTTP/1.1" 200 92845

[25/49][==================================.................................] ETA: 0:00:12DEBUG [1689598070.9062548] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-24.mp4'
DEBUG [1689598071.000361] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-24.mp4 HTTP/1.1" 200 93063

[26/49][===================================................................] ETA: 0:00:11DEBUG [1689598071.0547264] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-25.mp4'
DEBUG [1689598071.146517] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-25.mp4 HTTP/1.1" 200 93130

[27/49][====================================...............................] ETA: 0:00:10DEBUG [1689598071.460835] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-26.mp4'
DEBUG [1689598071.5484307] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-26.mp4 HTTP/1.1" 200 92951

[28/49][======================================.............................] ETA: 0:00:10DEBUG [1689598071.9391866] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-27.mp4'
DEBUG [1689598072.0374548] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-27.mp4 HTTP/1.1" 200 93116

[29/49][=======================================............................] ETA: 0:00:10DEBUG [1689598072.8454108] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-28.mp4'
DEBUG [1689598072.987346] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-28.mp4 HTTP/1.1" 200 93093

[30/49][=========================================..........................] ETA: 0:00:09DEBUG [1689598073.3732889] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-29.mp4'
DEBUG [1689598073.4910274] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-29.mp4 HTTP/1.1" 200 92971

[31/49][==========================================.........................] ETA: 0:00:08DEBUG [1689598073.548514] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-30.mp4'
DEBUG [1689598073.6425724] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-30.mp4 HTTP/1.1" 200 93074

[32/49][===========================================........................] ETA: 0:00:08DEBUG [1689598073.929268] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-31.mp4'
DEBUG [1689598074.2849817] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-31.mp4 HTTP/1.1" 200 93054

[33/49][=============================================......................] ETA: 0:00:08DEBUG [1689598075.6548083] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-32.mp4'
DEBUG [1689598076.1407182] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-32.mp4 HTTP/1.1" 200 93104

[34/49][==============================================.....................] ETA: 0:00:07DEBUG [1689598076.1566544] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-33.mp4'
DEBUG [1689598076.2589457] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-33.mp4 HTTP/1.1" 200 92924

[35/49][===============================================....................] ETA: 0:00:07DEBUG [1689598077.722287] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-34.mp4'
DEBUG [1689598077.80748] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-34.mp4 HTTP/1.1" 200 93173

[36/49][=================================================..................] ETA: 0:00:07DEBUG [1689598079.6225972] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-35.mp4'
DEBUG [1689598079.7323072] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-35.mp4 HTTP/1.1" 200 93044

[37/49][==================================================.................] ETA: 0:00:07DEBUG [1689598079.7824552] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-36.mp4'
DEBUG [1689598079.891351] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-36.mp4 HTTP/1.1" 200 92927

[38/49][===================================================................] ETA: 0:00:06DEBUG [1689598079.9381828] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-37.mp4'
DEBUG [1689598080.0391352] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-37.mp4 HTTP/1.1" 200 93061

[39/49][=====================================================..............] ETA: 0:00:05DEBUG [1689598080.0939884] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-38.mp4'
DEBUG [1689598080.1749537] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-38.mp4 HTTP/1.1" 200 93068

[40/49][======================================================.............] ETA: 0:00:04DEBUG [1689598080.2233536] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-39.mp4'
DEBUG [1689598080.316767] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-39.mp4 HTTP/1.1" 200 93053

[41/49][========================================================...........] ETA: 0:00:04DEBUG [1689598080.3629138] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-40.mp4'
DEBUG [1689598080.4507878] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-40.mp4 HTTP/1.1" 200 93010

[42/49][=========================================================..........] ETA: 0:00:03DEBUG [1689598080.5025558] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-41.mp4'
DEBUG [1689598080.5950482] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-41.mp4 HTTP/1.1" 200 93066

[43/49][==========================================================.........] ETA: 0:00:03DEBUG [1689598080.6954007] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-42.mp4'
DEBUG [1689598080.7862132] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-42.mp4 HTTP/1.1" 200 93050

[44/49][============================================================.......] ETA: 0:00:02DEBUG [1689598080.8428986] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-43.mp4'
DEBUG [1689598080.9275541] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-43.mp4 HTTP/1.1" 200 93036

[45/49][=============================================================......] ETA: 0:00:02DEBUG [1689598081.2099252] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-44.mp4'
DEBUG [1689598081.5973666] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-44.mp4 HTTP/1.1" 200 93173

[46/49][==============================================================.....] ETA: 0:00:01DEBUG [1689598081.6195133] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-45.mp4'
DEBUG [1689598081.7092135] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-45.mp4 HTTP/1.1" 200 92997

[47/49][================================================================...] ETA: 0:00:01DEBUG [1689598081.8631806] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-46.mp4'
DEBUG [1689598081.9613426] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-46.mp4 HTTP/1.1" 200 92979

[48/49][=================================================================..] ETA: 0:00:00DEBUG [1689598082.0274587] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-47.mp4'
DEBUG [1689598083.654229] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-47.mp4 HTTP/1.1" 200 93035

[49/49][===================================================================] ETA: 0:00:00DEBUG [1689598083.6557472] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-48.mp4'
DEBUG [1689598085.637824] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-audio-sv-aac-2ch-192/cmaf-audio-sv-aac-2ch-192-48.mp4 HTTP/1.1" 200 62088


[01/49][=..................................................................] ETA: 0:00:00DEBUG [1689598085.6406555] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-init.mp4'
DEBUG [1689598086.092504] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-init.mp4 HTTP/1.1" 200 876

[02/49][==.................................................................] ETA: 0:00:10DEBUG [1689598086.0941553] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-1.mp4'
DEBUG [1689598086.2098386] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-1.mp4 HTTP/1.1" 200 1398759

[03/49][====...............................................................] ETA: 0:00:48DEBUG [1689598088.8318195] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-2.mp4'
DEBUG [1689598088.92192] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-2.mp4 HTTP/1.1" 200 1181448

[04/49][=====..............................................................] ETA: 0:00:52DEBUG [1689598090.318695] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-3.mp4'
DEBUG [1689598090.4027746] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-3.mp4 HTTP/1.1" 200 1770542

[05/49][======.............................................................] ETA: 0:01:19DEBUG [1689598094.666302] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-4.mp4'
DEBUG [1689598095.1485643] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-4.mp4 HTTP/1.1" 200 1659964

[06/49][========...........................................................] ETA: 0:01:24DEBUG [1689598097.422596] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-5.mp4'
DEBUG [1689598097.5337403] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-5.mp4 HTTP/1.1" 200 1536172

[07/49][=========..........................................................] ETA: 0:01:29DEBUG [1689598100.6192684] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-6.mp4'
DEBUG [1689598100.7231746] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-6.mp4 HTTP/1.1" 200 929400

[08/49][==========.........................................................] ETA: 0:01:28DEBUG [1689598102.9184568] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-7.mp4'
DEBUG [1689598103.492471] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-7.mp4 HTTP/1.1" 200 906689

[09/49][============.......................................................] ETA: 0:01:23DEBUG [1689598104.3162885] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-8.mp4'
DEBUG [1689598104.4097736] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-8.mp4 HTTP/1.1" 200 1181095

[10/49][=============......................................................] ETA: 0:01:17DEBUG [1689598105.3908846] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-9.mp4'
DEBUG [1689598105.4896786] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-9.mp4 HTTP/1.1" 200 1088409

[11/49][===============....................................................] ETA: 0:01:11DEBUG [1689598106.432384] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-10.mp4'
DEBUG [1689598106.6997688] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-10.mp4 HTTP/1.1" 200 1383190

[12/49][================...................................................] ETA: 0:01:07DEBUG [1689598107.5751252] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-11.mp4'
DEBUG [1689598107.7370071] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-11.mp4 HTTP/1.1" 200 1311983

[13/49][=================..................................................] ETA: 0:01:06DEBUG [1689598109.6412606] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-12.mp4'
DEBUG [1689598109.7536592] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-12.mp4 HTTP/1.1" 200 1446703

[14/49][===================................................................] ETA: 0:01:07DEBUG [1689598112.6487935] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-13.mp4'
DEBUG [1689598113.220485] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-13.mp4 HTTP/1.1" 200 1328392

[15/49][====================...............................................] ETA: 0:01:08DEBUG [1689598116.04748] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-14.mp4'
DEBUG [1689598116.1296723] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-14.mp4 HTTP/1.1" 200 1752852

[16/49][=====================..............................................] ETA: 0:01:08DEBUG [1689598118.6641595] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-15.mp4'
DEBUG [1689598118.7737522] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-15.mp4 HTTP/1.1" 200 1515618

[17/49][=======================............................................] ETA: 0:01:05DEBUG [1689598120.4261425] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-16.mp4'
DEBUG [1689598120.512587] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-16.mp4 HTTP/1.1" 200 1138763

[18/49][========================...........................................] ETA: 0:01:02DEBUG [1689598121.8006475] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-17.mp4'
DEBUG [1689598121.886215] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-17.mp4 HTTP/1.1" 200 918637

[19/49][=========================..........................................] ETA: 0:01:00DEBUG [1689598123.6551366] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-18.mp4'
DEBUG [1689598123.763014] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-18.mp4 HTTP/1.1" 200 1719571

[20/49][===========================........................................] ETA: 0:00:57DEBUG [1689598125.3603423] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-19.mp4'
DEBUG [1689598125.445588] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-19.mp4 HTTP/1.1" 200 1637671

[21/49][============================.......................................] ETA: 0:00:54DEBUG [1689598126.8738382] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-20.mp4'
DEBUG [1689598127.3665617] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-20.mp4 HTTP/1.1" 200 1472798

[22/49][==============================.....................................] ETA: 0:00:52DEBUG [1689598128.353812] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-21.mp4'
DEBUG [1689598128.443058] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-21.mp4 HTTP/1.1" 200 1101470

[23/49][===============================....................................] ETA: 0:00:50DEBUG [1689598130.6339083] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-22.mp4'
DEBUG [1689598130.7433841] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-22.mp4 HTTP/1.1" 200 1269613

[24/49][================================...................................] ETA: 0:00:48DEBUG [1689598131.768228] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-23.mp4'
DEBUG [1689598131.8500988] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-23.mp4 HTTP/1.1" 200 1303937

[25/49][==================================.................................] ETA: 0:00:46DEBUG [1689598133.57827] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-24.mp4'
DEBUG [1689598133.6824892] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-24.mp4 HTTP/1.1" 200 1693909

[26/49][===================================................................] ETA: 0:00:44DEBUG [1689598135.6219351] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-25.mp4'
DEBUG [1689598135.7277937] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-25.mp4 HTTP/1.1" 200 1572421

[27/49][====================================...............................] ETA: 0:00:42DEBUG [1689598137.350908] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-26.mp4'
DEBUG [1689598137.4475586] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-26.mp4 HTTP/1.1" 200 888913

[28/49][======================================.............................] ETA: 0:00:39DEBUG [1689598138.4172096] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-27.mp4'
DEBUG [1689598138.8929408] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-27.mp4 HTTP/1.1" 200 1446771

[29/49][=======================================............................] ETA: 0:00:39DEBUG [1689598142.667096] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-28.mp4'
DEBUG [1689598143.2510204] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-28.mp4 HTTP/1.1" 200 1360485

[30/49][=========================================..........................] ETA: 0:00:37DEBUG [1689598145.4330978] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-29.mp4'
DEBUG [1689598145.509804] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-29.mp4 HTTP/1.1" 200 1068073

[31/49][==========================================.........................] ETA: 0:00:35DEBUG [1689598146.8652449] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-30.mp4'
DEBUG [1689598146.9577265] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-30.mp4 HTTP/1.1" 200 866006

[32/49][===========================================........................] ETA: 0:00:32DEBUG [1689598147.751624] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-31.mp4'
DEBUG [1689598147.8514664] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-31.mp4 HTTP/1.1" 200 1467908

[33/49][=============================================......................] ETA: 0:00:30DEBUG [1689598149.1008482] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-32.mp4'
DEBUG [1689598149.7104385] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-32.mp4 HTTP/1.1" 200 1396252

[34/49][==============================================.....................] ETA: 0:00:28DEBUG [1689598150.5782263] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-33.mp4'
DEBUG [1689598150.6606176] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-33.mp4 HTTP/1.1" 200 1335964

[35/49][===============================================....................] ETA: 0:00:26DEBUG [1689598151.8770666] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-34.mp4'
DEBUG [1689598152.5167258] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-34.mp4 HTTP/1.1" 200 998646

[36/49][=================================================..................] ETA: 0:00:24DEBUG [1689598153.0208368] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-35.mp4'
DEBUG [1689598153.1099424] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-35.mp4 HTTP/1.1" 200 593796

[37/49][==================================================.................] ETA: 0:00:22DEBUG [1689598155.6541476] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-36.mp4'
DEBUG [1689598155.7643294] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-36.mp4 HTTP/1.1" 200 567776

[38/49][===================================================................] ETA: 0:00:20DEBUG [1689598156.3373108] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-37.mp4'
DEBUG [1689598156.4212053] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-37.mp4 HTTP/1.1" 200 984480

[39/49][=====================================================..............] ETA: 0:00:18DEBUG [1689598157.6617975] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-38.mp4'
DEBUG [1689598157.760823] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-38.mp4 HTTP/1.1" 200 887267

[40/49][======================================================.............] ETA: 0:00:16DEBUG [1689598158.4966218] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-39.mp4'
DEBUG [1689598158.602147] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-39.mp4 HTTP/1.1" 200 1142107

[41/49][========================================================...........] ETA: 0:00:14DEBUG [1689598159.4362843] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-40.mp4'
DEBUG [1689598159.5287344] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-40.mp4 HTTP/1.1" 200 1164316

[42/49][=========================================================..........] ETA: 0:00:12DEBUG [1689598161.268227] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-41.mp4'
DEBUG [1689598161.3823106] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-41.mp4 HTTP/1.1" 200 968690

[43/49][==========================================================.........] ETA: 0:00:10DEBUG [1689598162.6036167] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-42.mp4'
DEBUG [1689598162.6876347] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-42.mp4 HTTP/1.1" 200 919361

[44/49][============================================================.......] ETA: 0:00:08DEBUG [1689598164.2851846] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-43.mp4'
DEBUG [1689598164.3745215] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-43.mp4 HTTP/1.1" 200 1076472

[45/49][=============================================================......] ETA: 0:00:07DEBUG [1689598165.227249] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-44.mp4'
DEBUG [1689598165.3135946] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-44.mp4 HTTP/1.1" 200 1056256

[46/49][==============================================================.....] ETA: 0:00:05DEBUG [1689598166.0976236] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-45.mp4'
DEBUG [1689598166.2022464] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-45.mp4 HTTP/1.1" 200 1099634

[47/49][================================================================...] ETA: 0:00:03DEBUG [1689598168.6154706] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-46.mp4'
DEBUG [1689598168.7305484] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-46.mp4 HTTP/1.1" 200 797350

[48/49][=================================================================..] ETA: 0:00:01DEBUG [1689598169.9572248] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-47.mp4'
DEBUG [1689598170.0418084] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-47.mp4 HTTP/1.1" 200 697464

[49/49][===================================================================] ETA: 0:00:00DEBUG [1689598170.536723] svtplay-dl/svtplay_dl/utils/http.py/request: HTTP getting 'https://svt-vod-7c.akamaized.net/d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-48.mp4'
DEBUG [1689598170.6967847] $urllib3/connectionpool.py/_make_request: https://svt-vod-7c.akamaized.net:443 "GET /d0/world/20230710/4a34cbb5-b1e8-44e6-8cfc-132c25571ca8/cmaf-video-avc-1920x1080p25-2490/cmaf-video-avc-1920x1080p25-2490-48.mp4 HTTP/1.1" 200 32170

INFO [1689598170.796944] svtplay-dl/svtplay_dl/postprocess/__init__.py/merge: Merge audio and video into svt.nyheter.kriget.i.ukraina.har.besoker.svt.en.skyttegrav.i.zaporizjzja-regionen-77e0cd8-svtplay.mp4
INFO [1689598171.062669] svtplay-dl/svtplay_dl/postprocess/__init__.py/merge: Merging done, removing old files.
)R" ;
}

svtplay_dl::svtplay_dl( const engines& engs,const engines::engine& engine,QJsonObject& ) :
	engines::engine::baseEngine( engs.Settings(),engine,engs.processEnvironment() ),
	m_processEnvironment( engines::engine::baseEngine::processEnvironment() )
{
	m_processEnvironment.insert( "PYTHONUNBUFFERED","true" ) ;
}

void svtplay_dl::updateOutPutChannel( QProcess::ProcessChannel& s ) const
{
	s = QProcess::ProcessChannel::StandardError ;
}

void svtplay_dl::updateDownLoadCmdOptions( const engines::engine::baseEngine::updateOpts& e,
					   bool s,
					   const QStringList& m )
{
	e.ourOptions.append( "--verbose" ) ;

	engines::engine::baseEngine::updateDownLoadCmdOptions( e,s,m ) ;
}

QStringList svtplay_dl::horizontalHeaderLabels() const
{
	auto m = engines::engine::baseEngine::horizontalHeaderLabels() ;

	m[ 1 ] = QObject::tr( "Method" ) ;

	return m ;
}

engines::engine::baseEngine::optionsEnvironment svtplay_dl::setProxySetting( QStringList& e,const QString& s )
{
	e.append( "--proxy" ) ;
	e.append( s ) ;

	return {} ;
}

static bool _add( std::vector< engines::engine::baseEngine::mediaInfo >& s,const QString& e )
{
	for( const auto& m : s ){

		if( m.id() == e ){

			return false ;
		}
	}

	return true ;
}

std::vector<engines::engine::baseEngine::mediaInfo> svtplay_dl::mediaProperties( Logger&,const QByteArray& e )
{
	auto mm = util::split( e,'\n',true ) ;

	while( true ){

		if( mm.size() == 0 ){

			return {} ;

		}else if( mm[ 0 ].startsWith( "INFO: " ) ){

			break ;
		}else{
			mm.removeAt( 0 ) ;
		}
	}

	std::vector< engines::engine::baseEngine::mediaInfo > s ;

	for( const auto& it : mm ){

		if( it.startsWith( "INFO: Quality:" ) ){

			continue ;
		}

		auto a = util::split( it,' ',true ) ;

		auto n = a.size() ;

		if( n > 4 ){

			a.takeAt( 0 ) ;
			auto format = a.takeAt( 0 ) ;

			if( _add( s,format ) ){

				auto method     = "Method: " + a.takeAt( 0 ) ;
				auto codec      = a.takeAt( 0 ) ;
				auto resolution = a.takeAt( 0 ) ;
				auto notes      = method + "\n" + a.join( ", " ) ;

				s.emplace_back( format,codec,resolution,"NA","0",notes,"","" ) ;
			}

		}else if( n == 3 ){

			a.takeAt( 0 ) ;

			auto format = a.takeAt( 0 ) ;

			if( _add( s,format ) ){

				auto method     = "Method: " + a.takeAt( 0 ) ;
				auto codec      = a.takeAt( 0 ) ;
				auto resolution = "N/A" ;
				auto notes      = method ;

				s.emplace_back( format,codec,resolution,"NA","0",notes,"","" ) ;
			}
		}
	}

	return s ;
}

const QProcessEnvironment& svtplay_dl::processEnvironment() const
{
	return m_processEnvironment ;
}

svtplay_dl::~svtplay_dl()
{
}

engines::engine::baseEngine::DataFilter svtplay_dl::Filter( int id )
{
	auto& s = engines::engine::baseEngine::Settings() ;
	const auto& engine = engines::engine::baseEngine::engine() ;

	return { util::types::type_identity< svtplay_dl::svtplay_dlFilter >(),s,engine,id } ;
}

QString svtplay_dl::updateTextOnCompleteDownlod( const QString& uiText,
						 const QString& bkText,
						 const QString& dopts,
						 const QString& tabName,
						 const engines::engine::baseEngine::finishedState& f )
{
	if( f.success() ){

		return engines::engine::baseEngine::updateTextOnCompleteDownlod( uiText,dopts,tabName,f ) ;

	}else if( f.cancelled() ){

		return engines::engine::baseEngine::updateTextOnCompleteDownlod( bkText,dopts,tabName,f ) ;
	}else{
		using functions = engines::engine::baseEngine ;

		if( uiText.startsWith( "ERROR:" ) ){

			auto s = uiText.mid( 6 ) ;
			auto m = engines::engine::baseEngine::updateTextOnCompleteDownlod( s,dopts,tabName,f ) ;

			return m + "\n" + bkText ;

		}else if( uiText.contains( "Temporary failure in name resolution" ) ){

			return functions::errorString( f,functions::errors::noNetwork,bkText ) ;

		}else if( uiText.contains( "Name or service not known" ) ){

			return functions::errorString( f,functions::errors::unknownUrl,bkText ) ;
		}else{
			auto m = engines::engine::baseEngine::processCompleteStateText( f ) ;
			return m + "\n" + bkText ;
		}
	}
}

svtplay_dl::svtplay_dlFilter::svtplay_dlFilter( settings&,const engines::engine& engine,int id ) :
	engines::engine::baseEngine::filter( engine,id ){
}

class svtplayFilter : public engines::engine::baseEngine::filterOutPut
{
public:
	svtplayFilter( const engines::engine& engine ) :
		m_engine( engine ),
		m_callables( svtplayFilter::meetCondition,svtplayFilter::skipCondition )
	{
	}
	engines::engine::baseEngine::filterOutPut::result
	formatOutput( const engines::engine::baseEngine::filterOutPut::args& args ) const override
	{
		const auto& l = args.locale ;
		auto& d = args.data ;
		const auto& m = args.outPut ;

		auto notMainLogger = !d.mainLogger() ;

		if( notMainLogger && d.svtData().fileName().isEmpty() ){

			d.toStringList().forEach( [ & ]( const QByteArray& s ){

				if( s.startsWith( "Outfile: " ) ){

					auto m = s.mid( 9 ) ;
					d.svtData().setFileName( m ) ;

					args.data.addFileName( m ) ;
				}
			} ) ;
		}

		if( m.startsWith( "DEBUG " ) ){

			if( notMainLogger && m.contains( " 200 " ) ){

				auto q = m.lastIndexOf( ' ' ) ;

				if( q != -1 ){

					auto mm = m.mid( q + 1 ) ;

					d.svtData().addToSize( mm.toLongLong() ) ;
				}
			}

			if( d.lastText().contains( "ETA" ) ){

				return { d.lastText(),m_engine,m_callables } ;
			}else{
				auto s = m.indexOf( "HTTP getting" ) ;

				if( s != -1 ){

					m_tmp = "Getting: " + m.mid( s + 12 ) ;
				}else{
					m_tmp.clear() ;
				}

				return { m_tmp,m_engine,m_callables } ;
			}

		}else if( m.startsWith( "INFO" ) ){

			auto s = m.indexOf( "Outfile:" ) ;

			if( s != -1 ){

				m_tmp = m.mid( s ) ;
			}else{
				s = m.indexOf( "Selected to download" ) ;

				if( s != -1 ){

					m_tmp = m.mid( s ) ;
				}else{
					m_tmp.clear() ;
				}
			}

			return { m_tmp,m_engine,m_callables } ;
		}

		auto e = m.indexOf( "ETA" ) ;

		auto a = m.mid( e ) ;

		auto aa = a.indexOf( "DEBUG" ) ;

		if( aa != -1 ){

			a = a.mid( 0,aa ) ;
		}

		auto b = m.indexOf( ']' ) ;

		auto c = m.mid( 0,b ) ;

		auto cc = c.indexOf( '[' ) ;

		auto dd = util::split( c.mid( cc + 1 ),'/' ) ;

		if( dd.size() == 2 ){

			auto x = dd[ 0 ].toDouble() ;
			auto y = dd[ 1 ].toDouble() ;

			auto z = x / y ;

			auto zz = QString::number( z * 100,'f',2 ) ;

			auto ss = "[" + dd[ 0 ] + "/" + dd[ 1 ] + "] (" + zz + "%), " + a ;

			m_tmp = ss.toUtf8() ;

			if( !d.mainLogger() ){

				auto ss = d.svtData().size() ;

				auto ll = l.formattedDataSize( ss ).toUtf8() ;

				auto mm = static_cast< qint64 >( ss / z ) ;

				auto lll = l.formattedDataSize( mm ).toUtf8() ;

				m_tmp = ll + "/~" + lll + ", " + m_tmp ;
			}

			if( x == y ){

				d.svtData().reset() ;
			}
		}else{
			m_tmp = "[00/00] (NA), " + a ;
		}

		return { m_tmp,m_engine,m_callables } ;
	}
	bool meetCondition( const engines::engine::baseEngine::filterOutPut::args& args ) const override
	{
		const auto& e = args.outPut ;

		if( svtplayFilter::meetCondition( e ) ){

			return true ;
		}else{
			return utils::misc::startsWithAny( e,"DEBUG","Getting:","INFO" ) ;
		}
	}
	const engines::engine& engine() const override
	{
		return m_engine ;
	}
private:
	static bool startsWithCondition( const QByteArray& e )
	{
		if( e.startsWith( '[' ) && e.size() > 1 ){

			auto m = e[ 1 ] ;

			return m >= '0' && m <= '9' ;

		}else if( e.startsWith( "\r[" ) && e.size() > 2 ){

			auto m = e[ 2 ] ;

			return m >= '0' && m <= '9' ;
		}else{
			return false ;
		}
	}
	static bool meetCondition( const QByteArray& e )
	{
		return svtplayFilter::startsWithCondition( e ) && e.contains( " ETA:" ) ;
	}
	static bool meetCondition( const engines::engine&,const QByteArray& e )
	{
		return e.contains( "] (" ) && e.contains( " ETA:" ) ;
	}
	static bool skipCondition( const engines::engine&,const QByteArray& )
	{
		return false ;
	}

	const engines::engine& m_engine ;
	mutable QByteArray m_tmp ;
	engines::engine::baseEngine::filterOutPut::result::callables m_callables ;
} ;

engines::engine::baseEngine::FilterOutPut svtplay_dl::filterOutput( int )
{
	const engines::engine& engine = engines::engine::baseEngine::engine() ;

	return { util::types::type_identity< svtplayFilter >(),engine } ;
}

QString svtplay_dl::updateCmdPath( const QString& e )
{
	const auto& name = engines::engine::baseEngine::engine().name() ;

	if( utility::platformIsWindows() ){

		return e + "/" + name + "/" + name + ".exe" ;
	}else{
		return e ;
	}
}

engines::metadata svtplay_dl::parseJsonDataFromGitHub( const QJsonDocument& doc )
{
	auto array = doc.array() ;

	if( array.size() ){

		QString fileName ;
		QString url ;

		if( utility::platformIsWindows() ){

			if( utility::CPU().x86_32() ){

				fileName = "svtplay-dl-win32.zip" ;

				url = "https://svtplay-dl.se/download/%1/svtplay-dl-win32.zip" ;
			}else{
				fileName = "svtplay-dl-amd64.zip" ;

				url = "https://svtplay-dl.se/download/%1/svtplay-dl-amd64.zip" ;
			}
		}else{
			fileName = "svtplay-dl" ;

			url = "https://svtplay-dl.se/download/%1/svtplay-dl" ;
		}

		QJsonObject obj ;

		auto name = array[ 0 ].toObject().value( "name" ).toString() ;

		obj.insert( "browser_download_url",url.arg( name ) ) ;
		obj.insert( "name",fileName ) ;
		obj.insert( "digest","" ) ;
		obj.insert( "size",0 ) ;

		return obj ;
	}else{
		return {} ;
	}
}

engines::engine::baseEngine::onlineVersion svtplay_dl::versionInfoFromGithub( const QByteArray& e )
{
	QJsonParseError err ;
	auto doc = QJsonDocument::fromJson( e,&err ) ;

	if( err.error == QJsonParseError::NoError ){

		auto s = doc.array() ;

		if( s.size() ){

			auto m = s[ 0 ].toObject().value( "name" ).toString() ;

			return { m,m } ;
		}
	}

	return { {},{} } ;
}

QString svtplay_dl::downloadUrl()
{
	return "https://api.github.com/repos/spaam/svtplay-dl/tags" ;
}

const QByteArray& svtplay_dl::svtplay_dlFilter::operator()( Logger::Data& s )
{
	if( s.doneDownloading() ){

		auto lines = s.toStringList() ;

		for( auto it = lines.rbegin() ; it != lines.rend() ; it++ ){

			const QByteArray& m = *it ;

			if( m.startsWith( "ERROR" ) ){

				auto e = m.indexOf( ':' ) ;

				if( e == -1 ){

					return m ;
				}else{
					m_tmp = "ERROR:" + m.mid( e + 1 ) ;

					return m_tmp ;
				}

			}else if( m.contains( "media already exists" ) ){

				m_tmp = "Media already exists" ;

				return m_tmp ;

			}else if( m.contains( "Temporary failure in name resolution" ) ){

				m_tmp = "Temporary failure in name resolution" ;

				return m_tmp ;

			}else if( m.contains( "Name or service not known" ) ){

				m_tmp = "Name or service not known" ;

				return m_tmp ;
			}
		}

		if( m_fileName.isEmpty() ){

			return m_tmp ;
		}else{
			s.addFileName( m_fileName ) ;

			return m_fileName ;
		}

	}else if( s.lastLineIsProgressLine() ){

		const auto& fileName = s.svtData().fileName() ;

		if( fileName.isEmpty() ){

			return s.lastText() ;
		}else{
			m_tmp = fileName + "\n" + s.lastText() ;

			return m_tmp ;
		}
	}else{
		const auto& m = s.lastText() ;

		if( m.startsWith( "WARNING" ) && m.contains( "already exists" ) ){

			auto a = m.indexOf( '(' ) ;
			auto b = m.lastIndexOf( ')' ) ;

			if( a != -1 && b != -1 && b > a ){

				m_tmp = "\"" + m.mid( a + 1,b - a - 1 ) + "\" already exists" ;
			}else{
				m_tmp = "media already exists" ;
			}

			return m_tmp ;

		}else if( m.contains( "Merge audio, video and subtitle into " ) ){

			auto e = m.indexOf( "Merge audio, video and subtitle into " ) ;

			if( e != -1 ){

				m_fileName = m.mid( e + 37 ) ;
			}

			return m_fileName ;

		}else if( m.contains( "Merge audio and video into " ) ){

			auto e = m.indexOf( "Merge audio and video into " ) ;

			if( e != -1 ){

				m_fileName = m.mid( e + 27 ) ;
			}

			return m_fileName ;

		}else if( m.contains( "INFO: " ) ){

			auto e = m.indexOf( "INFO: " ) ;

			if( e != -1 ){

				m_tmp = m.mid( e + 6 ) ;
			}

			return m_tmp ;

		}else if( m.startsWith( "ERROR" ) ){

			auto e = m.indexOf( ":" ) ;

			if( e != -1 ){

				m_tmp = m.mid( e + 1 ) ;
			}

			return m_tmp ;
		}else{
			return m_preProcessing.text() ;
		}
	}
}

svtplay_dl::svtplay_dlFilter::~svtplay_dlFilter()
{
}
