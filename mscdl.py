import  requests as rq, sys, time
AGENT =  "Mozilla/5.0 (X11; Linux x86_64; rv:145.0) Gecko/20100101 Firefox/145.0"
QUERYBASE = "https://triton.squid.wtf/search/?s="
REFERER = "https://tidal.squid.wtf/"
DOWNLOAD = "https://vogel.qqdl.site/track/?"

def getSongUrl(id, audioquality):
    final = DOWNLOAD + f"id={id}&quality={audioquality}"
    r = rq.get(final, headers={'user-agent':AGENT, "referer":REFERER})
    while r.status_code != 200:
        time.sleep(2)
        r = rq.get(final, headers={'user-agent':AGENT, "referer":REFERER})
    return r.json()

def writeToFile(url, name, artist):
    file = str(name) + ' ' + str(artist) + ".flac"
    r = rq.get(url, headers={'user-agent':AGENT, "referer":REFERER})
    if r.status_code != 200:
        return False
    with open(file, 'wb') as f:
        f.write(r.content)
        return True

def getSearchResult(query):
    final = QUERYBASE + "%20".join(query.split(" "))
    r = rq.get(final, headers = {"user-agent":AGENT, "Referer" : REFERER})
    while r.status_code != 200:
        time.sleep(2)
        r = rq.get(final, headers = {"user-agent":AGENT, "Referer" : REFERER})
    return r.json()

def main():
    if len(sys.argv) != 2:
        print("Usage: mscdl <search query>")
        return
    name = sys.argv[1]
    data = getSearchResult(name)
    for i in range(min(len(data['items']),5)):
        print(f"{i+1}.", data['items'][i]['title'], data['items'][i]['artists'][0]['name'])
    choice = input(">>> ")
    while not choice.isdigit() or not (1 <= int(choice) <= 5):
        choice = input(">>> ")
    song = getSongUrl(data['items'][int(choice)-1]['id'], data['items'][int(choice)-1]['audioQuality'])
    writeToFile(song[2]['OriginalTrackUrl'], song[0]['title'], song[0]['artist']['name'])


main()
