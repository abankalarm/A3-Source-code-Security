#include "OfuscateWorker.h"

int _add_ofuscation(int interval, int o_length, int &junk_counter, string file_name)
{
    
    string line;
    int cursive_present = 0;
    ifstream outfile(file_name);
    string buff;
    if (outfile.is_open())
    { 
        int i = 0;
        getline(outfile, line, '{');
        buff.append(line);
        if(!outfile.eof())
            buff.append("{");
        while (getline(outfile, line, ';')) {
            
                i++;
                if (!outfile.eof()) {
                if (i == interval) {
                    //buff.append("\n");
                    string o_code = get_junk(o_length, junk_counter);
                    buff.append(o_code);
                    buff.append("\n");
                    //outfile << teststr; 
                    i = 0;
                }   
                }
                //buff.append("\n");
                buff.append(line);
                buff.append(";");
           
        }
    }

    else
    {
        cout << "The file you specified could not be opened!  Please contact the source code administrator for further assistance.";
        return 1;
    };

    outfile.close();
    std::ofstream out(file_name);
    out << buff;
    out.close();
    return 0;
}
