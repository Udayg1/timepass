#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Python.h>

#define MAX_LINE 1024

void ytdlp(const char *name, const char *vidId);
int pyParser(char *nameOut, size_t nameSize, char *idOut, size_t idSize);

int main() {
    while (1) {
        char choice[4];
        printf("Start a new search? (y/n): ");
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = 0;

        if (strcmp(choice, "y") == 0) {
            char name[100], vidId[100];
            int status = pyParser(name, sizeof(name), vidId, sizeof(vidId));
            if (status == 0) {
                ytdlp(name, vidId);
            } else {
                printf("pyParser didn't return anything.\n");
            }
        } else if (strcmp(choice, "n") == 0) {
            break;
        } else {
            printf("Invalid choice. Please enter 'y' or 'n'.\n");
        }
    }

    return 0;
}

void ytdlp(const char *name, const char *vidId) {
    char command[MAX_LINE];
    snprintf(command, sizeof(command),
             "yt-dlp -f bestaudio -q --add-metadata -x -o '%s.%%(ext)s' https://music.youtube.com/watch?v=%s",
             name, vidId);
    int status = system(command);
    if (status != 0) {
        printf("An error occurred\n");
    } else {
        printf("Song successfully downloaded\n");
    }
}

int pyParser(char *nameOut, size_t nameSize, char *idOut, size_t idSize) {
    Py_Initialize();

    const char *pyCode =
        "import requests as re\n"
        "def search_song(query):\n"
        "    query = '+'.join(query.split(' '))\n"
        "    head = {'User-Agent': 'Mozilla/5.0 Firefox/117.0'}\n"
        "    resp = re.get(f'https://music.youtube.com/search?q={query}', headers=head)\n"
        "    if resp.status_code != 200:\n"
        "        return []\n"
        "    read = resp.text.splitlines()[-1]\n"
        "    read = read[read.find('YT')+2:]\n"
        "    name = ''\n"
        "    vidId = ''\n"
        "    read = read.split(r'\\x22')\n"
        "    for i in range(len(read)):\n"
        "        if read[i].strip() == 'title' and not name:\n"
        "            for j in range(i, i + 20):\n"
        "                if read[j].strip() == ':':\n"
        "                    name = read[j+1].strip()\n"
        "                    break\n"
        "        elif read[i].strip() == 'videoId' and not vidId:\n"
        "            vidId = read[i+2].strip()\n"
        "        if name and vidId:\n"
        "            break\n"
        "    return [name, vidId]\n"
        "def main():\n"
        "    print('Enter the song and artist name: ', end='')\n"
        "    query = input().strip()\n"
        "    songs = search_song(query)\n"
        "    while not songs:\n"
        "        print('Search returned nothing. Try again (0 to exit): ', end='')\n"
        "        query = input().strip()\n"
        "        if query == '0':\n"
        "            return []\n"
        "        songs = search_song(query)\n"
        "    return songs\n";

    PyRun_SimpleString("import sys; sys.path.append('.')");  // Just in case
    PyRun_SimpleString(pyCode);

    PyObject *main_module = PyImport_AddModule("__main__");
    PyObject *main_dict = PyModule_GetDict(main_module);
    PyObject *main_func = PyDict_GetItemString(main_dict, "main");

    if (main_func && PyCallable_Check(main_func)) {
        PyObject *result = PyObject_CallObject(main_func, NULL);
        if (result && PyList_Check(result) && PyList_Size(result) == 2) {
            PyObject *pyName = PyList_GetItem(result, 0);
            PyObject *pyId = PyList_GetItem(result, 1);
            if (PyUnicode_Check(pyName) && PyUnicode_Check(pyId)) {
                strncpy(nameOut, PyUnicode_AsUTF8(pyName), nameSize - 1);
                strncpy(idOut, PyUnicode_AsUTF8(pyId), idSize - 1);
                nameOut[nameSize - 1] = '\0';
                idOut[idSize - 1] = '\0';
                Py_DECREF(result);
                Py_Finalize();
                return 0;
            }
        }
        Py_XDECREF(result);
    }

    PyErr_Print();
    Py_Finalize();
    return 1;
}
