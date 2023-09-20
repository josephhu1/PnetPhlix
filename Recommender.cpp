/*
Author: Joseph Hu
*/

#include "Recommender.h"
#include "UserDatabase.h"
#include "MovieDatabase.h"
#include "User.h"
#include "Movie.h"
#include <unordered_map>
#include <algorithm>
#include <string>
#include <vector>
using namespace std;

Recommender::Recommender(const UserDatabase& user_database,
                         const MovieDatabase& movie_database)
: m_all_users(&user_database), m_all_movies(&movie_database){}

vector<MovieAndRank> Recommender::recommend_movies(const string& user_email, int movie_count) const
{
    //maps:
    unordered_map<string, vector<Movie*>> director_map;
    unordered_map<string, vector<Movie*>> actor_map;
    unordered_map<string, vector<Movie*>> genre_map;
    
    //get each watched movie
    vector<string> watched_movies = m_all_users->get_user_from_email(user_email)->get_watch_history();
    for (int i = 0; i < watched_movies.size(); i++)
    {
        //for each recommended director, store movie recommendations
        vector<string> recommended_directors = m_all_movies->get_movie_from_id(watched_movies[i])->get_directors();
        for (int i = 0; i < recommended_directors.size(); i++)
        {
            if (director_map[recommended_directors[i]].size() == 0)
                director_map[recommended_directors[i]] = m_all_movies->get_movies_with_director(recommended_directors[i]);
            else
            {
                vector<Movie*> temp = m_all_movies->get_movies_with_director(recommended_directors[i]);
                director_map[recommended_directors[i]].insert(director_map[recommended_directors[i]].end(), temp.begin(), temp.end());
            }
        }
        //for each recommended actor, store movie recommendations
        vector<string> recommended_actors = m_all_movies->get_movie_from_id(watched_movies[i])->get_actors();
        for (int i = 0; i < recommended_actors.size(); i++)
        {
            if (actor_map[recommended_actors[i]].size() == 0)
                actor_map[recommended_actors[i]] = m_all_movies->get_movies_with_actor(recommended_actors[i]);
            else
            {
                vector<Movie*> temp = m_all_movies->get_movies_with_actor(recommended_actors[i]);
                actor_map[recommended_actors[i]].insert(actor_map[recommended_actors[i]].end(), temp.begin(), temp.end());
            }
        }
        //for each recommended genre, store movie recommendations
        vector<string> recommended_genres = m_all_movies->get_movie_from_id(watched_movies[i])->get_genres();
        for (int i = 0; i < recommended_genres.size(); i++)
        {
            if (genre_map[recommended_genres[i]].size() == 0)
                genre_map[recommended_genres[i]] = m_all_movies->get_movies_with_genre(recommended_genres[i]);
            else
            {
                vector<Movie*> temp = m_all_movies->get_movies_with_genre(recommended_genres[i]);
                genre_map[recommended_genres[i]].insert(genre_map[recommended_genres[i]].end(), temp.begin(), temp.end());
            }
        }
    }
    
    //store movieID as key and score as data in map
    unordered_map<string, int> movie_recommendation_map;
    
    //decide when to increment the scores
    for (unordered_map<string, vector<Movie*>>::iterator p = director_map.begin(); p != director_map.end(); p++)
        //p->second is vector for each iteration
        for (int i = 0; i < p->second.size(); i++)
            movie_recommendation_map[(p->second)[i]->get_id()] += 20;
    
    for (unordered_map<string, vector<Movie*>>::iterator p = actor_map.begin(); p != actor_map.end(); p++)
        for (int i = 0; i < p->second.size(); i++)
            movie_recommendation_map[(p->second)[i]->get_id()] += 30;

    for (unordered_map<string, vector<Movie*>>::iterator p = genre_map.begin(); p != genre_map.end(); p++)
        for (int i = 0; i < p->second.size(); i++)
            movie_recommendation_map[(p->second)[i]->get_id()] += 1;
    
    //get rid of movies user already watched
    for (int i = 0; i < watched_movies.size(); i++)
        if (movie_recommendation_map.count(watched_movies[i]))
            if (movie_recommendation_map[watched_movies[i]] >= 1)
                movie_recommendation_map.erase(watched_movies[i]);
    
    //move from unsorted map to vector
    vector<MovieAndRank> final_recommender;
    for (unordered_map<string, int>::iterator p = movie_recommendation_map.begin(); p != movie_recommendation_map.end(); p++)
        final_recommender.push_back(MovieAndRank(p->first, p->second));
    
    //lambda function to sort vector
    sort(final_recommender.begin(), final_recommender.end(), [this](const MovieAndRank& m1, const MovieAndRank& m2) -> bool
    {
        if (m1.compatibility_score > m2.compatibility_score)
            return true;
        else if (m1.compatibility_score == m2.compatibility_score)
        {
            if (m_all_movies->get_movie_from_id(m1.movie_id)->get_rating() > m_all_movies->get_movie_from_id(m2.movie_id)->get_rating())
                return true;
            else if (m_all_movies->get_movie_from_id(m1.movie_id)->get_rating() == m_all_movies->get_movie_from_id(m2.movie_id)->get_rating())
            {
                if (m_all_movies->get_movie_from_id(m1.movie_id)->get_title() < m_all_movies->get_movie_from_id(m2.movie_id)->get_title())
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        else
            return false;
    });
    
    //erase part of vector for certain # of recommendations
    if (movie_count <= 0)
        final_recommender.erase(final_recommender.begin(), final_recommender.end());
    else if (!(final_recommender.size() < movie_count))
        final_recommender.erase(final_recommender.begin() + movie_count, final_recommender.end());
    
    return final_recommender;
}
