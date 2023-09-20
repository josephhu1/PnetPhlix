/*
Author: Joseph Hu
*/

#include "UserDatabase.h"
#include "User.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

UserDatabase::UserDatabase()
{}

bool UserDatabase::load(const string& filename)
{
    ifstream infile(filename);
    if ( ! infile )
    {
        cerr << "Cannot open the file!" << endl;
        return false;
    }
    
    string name;
    while (getline(infile, name))
    {
        string email;
        getline(infile, email);
        
        vector<string> movies;
        
        int n_movies;
        infile >> n_movies;
        infile.ignore(10000, '\n');
        
        for (int i = 0; i < n_movies; i++)
        {
            string movieID;
            getline(infile, movieID);
            movies.push_back(movieID);
        }
        //When end is reached
        infile.ignore(10000, '\n');
        
        //Insert USER
        m_users.insert(email, User(name, email, movies));
    }
    return true;
}
    
User* UserDatabase::get_user_from_email(const string& email) const
{
    TreeMultimap<string, User>::Iterator it = m_users.find(email);
    if (it.is_valid())
        return &(it.get_value());
    else
        return nullptr;
}
