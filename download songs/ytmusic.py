import requests as re
def search_song(query):
    query = "+".join(query.split(" "))
    head = {"User-Agent": "Mozilla/5.0 Firefox/117.0"}
    resp = re.get(f"https://music.youtube.com/search?q={query}", headers = head)
    if resp.status_code != 200:
        return []
    read = resp.text.splitlines()[-1]
    read = read[read.find("value")+1:]
    read = read[read.find("YT")+2:]
    name = ''
    vidId = ''
    read = read.split(r'\x22')
    for i in range(len(read)):
        if read[i].strip() == 'title' and not name:
            for j in range(i, i + 20):
                if read[j].strip() == ':':
                    name = read[j+1].strip()
                    break
        elif read[i].strip() == 'videoId' and not vidId:
            vidId = read[i+2].strip()
        if name and vidId:
            break
    return [name, vidId]

def main():
    print("Enter the song and artist name: ", end='')
    query = input().strip()
    songs = search_song(query)
    while not songs:
        print("\nSearch returned nothing. Try again (0 to exit): ",end='')
        query = input().strip()
        if query == '0':
            return 0
        songs = search_song(query)
    with open(".temp",'w') as f:
        f.write(songs[0] + '\n')
        f.write(songs[1])


main()
