#include <iostream>
#include <algorithm>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <vector>

typedef std::vector<std::string> stringvec;
 
void read_directory(const std::string& name, stringvec& v)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        v.push_back(dp->d_name);
    }
    closedir(dirp);
}

int main()
{
    stringvec v;
    read_directory("images", v);
    std::copy(v.begin(), v.end(),
         std::ostream_iterator<std::string>(std::cout, "\n"));
}