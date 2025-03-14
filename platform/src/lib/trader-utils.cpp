#include "models/Trader.hpp"
#include "trader-utils.hpp"

#include <string>
#include <cstdlib>

const std::string generateTraderName()
{
    std::string gender = (rand() % 2) ? "M" : "F";

    static const char *lastNames[] = {
        "Smith", "Johnson", "Brown", "Taylor", "Anderson", "Harris", "Clark",
        "Allen", "Scott", "Carter", "Mitchell", "Roberts", "Garcia", "Martinez",
        "Young", "King", "Wright", "Lopez", "Hill", "Green"};

    std::string firstName;
    if (gender == std::string("M"))
    {
        static const char *maleNames[] = {
            "James", "Alexander", "Robert", "Michael", "William", "David", "Richard",
            "Joseph", "Thomas", "Charles", "Daniel", "Matthew", "Anthony", "Benjamin"};
        firstName = maleNames[rand() % 14];
    }
    else
    {
        static const char *femaleNames[] = {
            "Mary", "Emily", "Georgia", "Sarah", "Elizabeth", "Jennifer", "Maria",
            "Chloe", "Natalie", "Eve", "Martina", "Nancy", "Isabelle", "Julia"};
        firstName = femaleNames[rand() % 14];
    }

    return firstName + " " + lastNames[rand() % 20];
}