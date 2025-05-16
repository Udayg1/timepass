""" Extract the first song according to the one entered by the user by fetching the webpage from ytmusic and extracting useful info. """
import requests as re
def search_song(query):
    query = "+".join(query.split(" "))  # structure the query according to the url
    head = {"User-Agent": "Mozilla/5.0 Firefox/117.0"}
    resp = re.get(f"https://music.youtube.com/search?q={query}", headers = head)  # get the webpage for the song
    if resp.status_code != 200:
        return []
    read = resp.text.splitlines()[-1]
    # read = read[read.find("value")+1:]
    read = read[read.find("YT")+2:]  # clean the useless part
    name = ''
    vidId = ''
    read = read.split(r'\x22')
    for i in range(len(read)):  # extract name and videoId
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
        print("\nSearch returned nothing. Try again (0 to exit): ",end='')  # some error handling
        query = input().strip()
        if query == '0':
            return 0
        songs = search_song(query)
    with open(".temp",'w') as f:
        f.write(songs[0] + '\n')
        f.write(songs[1])

main()
