from ytmusicapi import YTMusic
def search_song(query):
    yt = YTMusic()
    result = yt.search(query,filter = 'songs')  # get the songs from the given query
    return result

def main():
    print("Enter the song you want to search: ", end='')
    query = input().strip()
    songs = search_song(query)
    while not songs:
        print("\nSearch returned nothing. Try again (0 to exit): ",end='')
        query = input().strip()
        if query == '0':
            return 0  # no real use, thought i could read it in the C code but couldn't figure it out
        songs = search_song(query)
    results = []
    for i in range(min(5,len(songs))):  # get the top 5 song results and get user input 
        print(str(i+1)+'.')
        print("\t",songs[i]['title'])
        print("\t",songs[i]['artists'][0]['name'])
        print("\t",songs[i]['duration'])
        results.append([songs[i]['title'],songs[i]["videoId"]])
    print("Enter the song you want to download(1-5, 0 to abort): ",end='')
    choice = int(input()) or 0
    if choice == 0:
        return 0  # again no real use
    with open(".temp",'w') as f:  # communicate the song name and video-id to C code 
        f.write(results[choice-1][0] + '\n')
        f.write(results[choice-1][1])

main()
