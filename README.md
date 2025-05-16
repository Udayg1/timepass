Nothing just a college student bored in first summer vacation. Continuously bothered to download songs, sometimes failure to find the song. 

`Usage` - Run the C binary. Enter the song name and choice of song to download when prompted. If successful, song will be downloaded in the current directory. 

`Intallation` - 
    Build the C binary file. 
    Build the ytmusic.py file into an exe file. I used pyinstaller by this command 
    `pyinstaller -F ytmusic.py --add-file=~/path/to/locales/folder:requests/locales`. 

Find the locales path with 
      `import requests`
      `import os`
      `print(os.path.dirname(requests.__file__))`
Place the binary file in the same folder as the C binary file. 

The downloaded songs will be in opus format (default for youtube music), 133kbps bitrate and 48kHz sample rate. 
