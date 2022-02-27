#define _CRT_SECURE_NO_WARNINGS
#include "FileWorker.h"

bool contain(string str, string target)
{
    if (str.find(target) != string::npos)
        return true;
    return false;
}

////////////////////////////////////function to check wheather file is .cpp or .h
bool checkFileExtension(string fileName)
{
    if (contain(fileName, ".cpp"))
    {
        return true;
    }
    if (contain(fileName, ".h"))
    {
        return true;
    }
    if (contain(fileName, ".hpp"))
    {
        return true;
    }

    return false;
}

//////////////////////////////////////get a list of all the files in a directory
vector<string> getFilesInFolder(string folder)
{
    vector<string> names;
    string search_path = folder + "/*.*";
    WIN32_FIND_DATAA fd;
    HANDLE hFind = ::FindFirstFileA(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                names.push_back(folder + "\\" + fd.cFileName);
            }
        } while (::FindNextFileA(hFind, &fd));
        ::FindClose(hFind);
    }
    return names;
}

//////////////////////Function to add #include "encryptstring" in every file at the top
void addStringToTop(string fileName, string Line0, int& count)
{
    string temp,     //to read lines one by one
        FileContent; //that will be the new content for the file
    fstream File(fileName);
    FileContent = Line0;
    while (getline(File, temp))
    {
        FileContent += "\n" + temp;
    } //add the content of the file after line0
    File.close();
    File.open(fileName, std::ios::out); //reopen the file for output operation
    File << FileContent;                //print the new content
    File.close();
    count++;
}

//////////////////////////Function to store string of a file
vector<string> addTextToVector(string fileName, int &count)
{
    vector<string> vec;
    string item_name;
    ifstream nameFileout;
    // ofstream nameFileoutput;
    // nameFileoutput.open(fileName);
    nameFileout.open(fileName);
    string line;
    bool init = true;
    regex reg("\"([^\"]*)\"");
    string fileData = "";

    while (getline(nameFileout, line))
    {

        int i = 0;

        if (contain(line, "{"))
        {
            init = false;
        }
        if (!init)
        {
            smatch temp;
            regex_search(line, temp, reg);

            for (int i = 1; i < temp.size(); i++) {
                string x = temp.str(i);

                cout << x;
                string s = "_c(\"";
                s.append(x);
                s.append("\")");

                line = regex_replace(line, reg, s);
                count++;
            }
        }
        fileData += line + "\n";
    }
    cout << fileData;
    nameFileout.close();
    ofstream File(fileName);
    File << fileData;
    File.close();

    return vec;
}

void _add_antidebug(string o_code, bool& anti_debug_included, string file_name)
{

    string line;

    ifstream outfile(file_name);
    string buff;
    if (outfile.is_open())
    {
        int i = 0;
        getline(outfile, line, '{');
        buff.append(line);
        buff.append("{");

        getline(outfile, line, ';');
        buff.append("\n");
        buff.append(o_code);
        buff.append(line);
        buff.append(";");

        while (getline(outfile, line, ';')) {
            //buff.append("\n");
            buff.append(line);
            buff.append(";");
        }
        anti_debug_included = true;
    }

    else
    {
        cout << "The file you specified could not be opened!  Please contact the source code administrator for further assistance.";
        
    };

    outfile.close();
    std::ofstream out(file_name);
    out << buff;
    out.close();
    
}

#define COPY_STR "copy %s %s"

int copy_file(string src, string dst, bool delete_after_move) {
    
    string copy_str;
    copy_str = "copy \"" + src +"\" \"" + dst + "\"";
    std::cout << "\n" << copy_str << "\n";
    system(copy_str.c_str());
    
    return 0;
}