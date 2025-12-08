import requests as rq
import time
import mpv
import os
import base64
import tempfile

AGENT =  "Mozilla/5.0 (X11; Linux x86_64; rv:145.0) Gecko/20100101 Firefox/145.0"
QUERYBASE = "https://maus.qqdl.site/search/?s="
REFERER = "https://tidal.squid.wtf/"
DOWNLOAD = "https://tidal.kinoplus.online/track/?"
AUDIO = '/tmp/segmentedaudio.fifo'

# ---- GLOBAL MPV PLAYER ----
player = mpv.MPV(
    ytdl=True,
    input_default_bindings=True,
    input_vo_keyboard=True,
    osc=True,
    hwdec="auto",
    keep_open="yes",
    msg_level="all=v",
    demuxer_lavf_o="protocol_whitelist=[file,https,http,tls,tcp,crypto,data]"
)



def decode_base64_manifest(s: str) -> str:
    if not isinstance(s, str):
        return ""

    t = s.strip()
    if not t:
        return ""

    # Replace URL-safe Base64 characters
    t = t.replace("-", "+").replace("_", "/")

    # Fix padding
    missing = len(t) % 4
    if missing == 2:
        t += "=="
    elif missing == 3:
        t += "="
    elif missing == 1:
        # Invalid base64 length
        return s

    try:
        decoded = base64.b64decode(t).decode("utf-8")
        return decoded
    except Exception:
        return s


def queue_mpd_xml_as_file(mpd_xml, title, artist):
    fd, path = tempfile.mkstemp(suffix=".mpd")
    os.close(fd)
    with open(path, "w", encoding="utf-8") as f:
        f.write(mpd_xml)
    queueSong(path, title, artist)

def getSongUrl(id, audioquality):
    final = DOWNLOAD + f"id={id}&quality={audioquality}"
    r = rq.get(final, headers={'user-agent': AGENT, "referer": REFERER})
    while r.status_code != 200:
        time.sleep(2)
        r = rq.get(final, headers={'user-agent': AGENT, "referer": REFERER})
    return r.json()

def getSearchResult(query):
    final = QUERYBASE + "%20".join(query.split(" "))
    r = rq.get(final, headers={"user-agent": AGENT, "Referer": REFERER})
    while r.status_code != 200:
        time.sleep(2)
        r = rq.get(final, headers={"user-agent": AGENT, "Referer": REFERER})
    return r.json()

def queueSong(url, title, artist):
    """Append a song to MPV playlist."""
    print(f"\nQueued: {title} - {artist}")
    player.playlist_append(url)

    if player.playlist_count == 1:
        player.playlist_play_index(0)

def main():
    print("MPV Music Client (queue + autoplay)")
    print("-----------------------------------")

    while True:
        name = input("\nEnter song name (or q to quit): ")
        if not name.strip():
            continue
        if  name.lower().strip() == "q":
            break

        data = getSearchResult(name)

        # show up to 5 matches
        print("\nResults:")
        for i in range(min(len(data['data']["items"]), 5)):
            item = data['data']["items"][i]
            print(f"{i+1}. {item['title']} — {item['artists'][0]['name']}")

        choice = input("Pick number, or press Enter to cancel: ")
        if not (choice.isdigit() and 1 <= int(choice) <= 5):
            continue
        i = int(choice) - 1
        item = data['data']["items"][i]
        title = data['data']['items'][i]['title']
        artist = data['data']['items'][i]['artist']['name']

        song = getSongUrl(item['id'],"HI_RES_LOSSLESS")
        decoded64 = decode_base64_manifest(song['data']['manifest'])
        # print(decoded64[:200])
        # url = decoded64[24].split(r'$Number$')
        # segnum = int(decoded64[27][decoded64[27].find("\"")+1: decoded64[27].find("\"",decoded64[27].find("\"")+1)])+2
        queue_mpd_xml_as_file(decoded64, title, artist)
        # for i in range(segnum):
        #     r = str(i).join(url)
        #     queueSong(r, title, artist)

try:
    main()
except KeyboardInterrupt:
    print("\nExiting...")
