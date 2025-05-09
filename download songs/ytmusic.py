from ytmusicapi import YTMusic
def search_song(query):
    yt = YTMusic()
    result = yt.search(query,filter = 'songs')
    return result

def main():
    print("Enter the song you want to search: ", end='')
    query = input().strip()
    songs = search_song(query)
    while not songs:
        print("\nSearch returned nothing. Try again (0 to exit): ",end='')
        query = input().strip()
        if query == '0':
            return 0
        songs = search_song(query)
    results = []
    for i in range(min(5,len(songs))):
        print(str(i+1)+'.')
        print("\t",songs[i]['title'])
        print("\t",songs[i]['artists'][0]['name'])
        print("\t",songs[i]['duration'])
        results.append([songs[i]['title'],songs[i]["videoId"]])
    print("Enter the song you want to download(1-5, 0 to abort): ",end='')
    choice = int(input()) or 0
    if choice == 0:
        return 0
    with open(".temp",'w') as f:
        f.write(results[choice-1][0] + '\n')
        f.write(results[choice-1][1])


main()
