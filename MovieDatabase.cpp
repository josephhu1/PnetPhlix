/*
Author: Joseph Hu
*/

#include "MovieDatabase.h"
#include "Movie.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

MovieDatabase::MovieDatabase()
{}

bool MovieDatabase::load(const string& filename)
{
    ifstream infile(filename);
    if ( ! infile )
    {
        cerr << "Cannot open the file!" << endl;
        return false;
    }
    
    string movieID;
    while (getline(infile, movieID))
    {
        string movie_name;
        getline(infile, movie_name);
        
        string year;
        getline(infile, year);
        
        vector<string> directors;
        vector<string> actors;
        vector<string> genres;
        
        string all_directors;
        getline(infile, all_directors);
        directors = get_items_helper(all_directors);
        
        string all_actors;
        getline(infile, all_actors);
        actors = get_items_helper(all_actors);
        
        string all_genres;
        getline(infile, all_genres);
        genres = get_items_helper(all_genres);
        
        float rating;
        infile >> rating;
        infile.ignore(10000, '\n');
        
        //When end is reached
        infile.ignore(10000, '\n');
        
        //Insert into the trees
        m_movie_ids.insert(get_lower(movieID), Movie(movieID, movie_name, year, directors, actors, genres, rating));
        
        for (int i = 0; i < actors.size(); i++)
            m_movie_actors.insert(get_lower(actors[i]), Movie(movieID, movie_name, year, directors, actors, genres, rating));
        
        for (int i = 0; i < genres.size(); i++)
            m_movie_genres.insert(get_lower(genres[i]), Movie(movieID, movie_name, year, directors, actors, genres, rating));
        
        for (int i = 0; i < directors.size(); i++)
            m_movie_directors.insert(get_lower(directors[i]), Movie(movieID, movie_name, year, directors, actors, genres, rating));
    }
    return true;
}

Movie* MovieDatabase::get_movie_from_id(const string& id) const
{
    TreeMultimap<string, Movie>::Iterator it = m_movie_ids.find(get_lower(id));
    if (it.is_valid())
        return &(it.get_value());
    else
        return nullptr;
}

vector<Movie*> MovieDatabase::get_movies_with_director(const string& director) const
{
    return get_movies_helper(m_movie_directors, director);
}

vector<Movie*> MovieDatabase::get_movies_with_actor(const string& actor) const
{
    return get_movies_helper(m_movie_actors, actor);
}

vector<Movie*> MovieDatabase::get_movies_with_genre(const string& genre) const
{
    return get_movies_helper(m_movie_genres, genre);
}

/*
    Auxiliary Functions:
*/
string MovieDatabase::get_lower(string update) const
{
    for (size_t i = 0; i < update.length(); i++)
        update[i] = tolower(update[i]);
    return update;
}

vector<string> MovieDatabase::get_items_helper(string& line) const
{
    vector<string> collection;
    size_t index = 0;
    //separate separate words from commas
    for(;;)
    {
        if (line.size() <= 0)
            break;
        else if (index == line.size() - 1)
        {
            collection.push_back(line);
            break;
        }
        else if (line[index] == ',')
        {
            collection.push_back(line.substr(0, index));
            line = line.substr(index + 1);
            index = 0;
        }
        else
            index++;
    }
    return collection;
}

vector<Movie*> MovieDatabase::get_movies_helper(const TreeMultimap<string, Movie>& keys, const string& type) const
{
    vector<Movie*> movie_ptrs;
    TreeMultimap<string, Movie>::Iterator it = keys.find(get_lower(type));
    while (it.is_valid())
    {
        movie_ptrs.push_back(&(it.get_value()));
        it.advance();
    }
    return movie_ptrs;
}
